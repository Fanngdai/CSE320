#include "server.h"
#include "csapp.h"
#include "debug.h"

#include "protocol.h"
#include "transaction.h"
#include "data.h"
#include "store.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void xacto_put_pkt(int fd, TRANSACTION *tp, int *end);
void xacto_get_pkt(int fd, TRANSACTION *tp, XACTO_PACKET *pkt, int *end);
int reply(int fd, XACTO_PACKET *pkt, int type, int status, BLOB *bp);
void abort_end(int fd,TRANSACTION *tp, XACTO_PACKET *pkt, int *end);

CLIENT_REGISTRY *client_registry;

/*
 * Thread function for the thread that handles client requests.
 * Invoked as the thread function for a thread that is created to
 * service a client connection.
 *
 * @param       Pointer to a variable that holds the file descriptor for
 *              the client connection.  This pointer must be freed once
 *              the file descriptor has been retrieved.
 */
void *xacto_client_service(void *arg) {
    int fd = *((int *)arg);
    Pthread_detach(pthread_self());
    Free(arg);
    arg = NULL;

    debug("[%d] Starting client service", fd);

    creg_register(client_registry, fd);
    TRANSACTION *tp = trans_create();

    int endloop = 1;

    while(endloop) {
        // receive request packet sent by client
        XACTO_PACKET *pkt = Calloc(sizeof(XACTO_PACKET), sizeof(char));
        // carries out the request
        if(proto_recv_packet(fd, pkt, NULL) == -1) {
            abort_end(fd, tp, pkt, &endloop);
        } else {
            switch(pkt->type) {
                case XACTO_PUT_PKT:
                    debug("[%d] PUT packet received", fd);
                    xacto_put_pkt(fd, tp, &endloop);

                    // Make sure xacto_put_pkt did not call terminate
                    if(endloop) {
                        store_show();
                        trans_show_all();
                    }
                    break;
                case XACTO_GET_PKT:
                    // data packet that contains the result
                    debug("[%d] GET packet received", fd);
                    xacto_get_pkt(fd, tp, pkt, &endloop);

                    if(endloop) {
                        store_show();
                        trans_show_all();
                    }
                    break;
                case XACTO_COMMIT_PKT:
                    debug("[%d] COMMIT packet received", fd);

                    int status = trans_commit(tp);
                    // int fd, XACTO_PACKET *pkt, int type, int status, BLOB *bp
                    if(status == TRANS_ABORTED) {
                        reply(fd, pkt, XACTO_REPLY_PKT, TRANS_ABORTED, NULL);
                    } else if(reply(fd, pkt, XACTO_REPLY_PKT, status, NULL) == -1){
                        abort_end(fd, tp, pkt, &endloop);
                    } else {
                        store_show();
                        trans_show_all();
                    }


                    endloop = 0;
                    break;
                default:
                    debug("[%d] Ending client service", fd);
                    abort_end(fd, tp, pkt, &endloop);
                    // service loop should end
                    break;
            }
        }
        if(pkt){
            Free(pkt);
            pkt = NULL;
        }
    }

    creg_unregister(client_registry, fd);
    // close client connection
    Close(fd);
    // client service thread terminate
    return NULL;
}

int reply(int fd, XACTO_PACKET *pkt, int type, int status, BLOB *bp) {
    pkt->type = type;
    pkt->status = status;
    pkt->size = (bp&&type!=XACTO_REPLY_PKT)?bp->size:0;
    pkt->null = (bp||type==XACTO_REPLY_PKT)?0:1;
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    pkt->timestamp_sec = time.tv_sec;
    pkt->timestamp_nsec = time.tv_nsec;
    return proto_send_packet(fd, pkt, bp?bp->content:NULL);
}



void abort_end(int fd,TRANSACTION *tp, XACTO_PACKET *pkt, int *end) {
    reply(fd, pkt, XACTO_REPLY_PKT, TRANS_ABORTED, NULL);
    *end = 0;
    if(tp) trans_abort(tp);
}

void free_pkt_void(XACTO_PACKET *pkt, void **key) {
    if(pkt) {
        Free(pkt);
        pkt = NULL;
    }
    if(*key) {
        Free(*key);
        *key = NULL;
    }
    if(key) {
        Free(key);
        key = NULL;
    }
}

void xacto_put_pkt(int fd, TRANSACTION *tp, int *end) {
    XACTO_PACKET *pkt1 = Calloc(sizeof(XACTO_PACKET), sizeof(char));
    void **key = Calloc(sizeof(void**), sizeof(char));
    if(proto_recv_packet(fd, pkt1, key) == -1) {
        abort_end(fd, tp, pkt1, end);
        free_pkt_void(pkt1, key);
        return;
    }
    debug("[%d] Received key, size %d", fd, pkt1->size);

    XACTO_PACKET *pkt2 = Calloc(sizeof(XACTO_PACKET), sizeof(char));
    void **value = Calloc(sizeof(void**), sizeof(char));
    if(proto_recv_packet(fd, pkt2, value) == -1) {
        abort_end(fd, tp, pkt1, end);
        free_pkt_void(pkt1, key);
        free_pkt_void(pkt2, value);
        return;
    }
    debug("[%d] Received value, size %d", fd, pkt2->size);

    BLOB *bp1 = blob_create(*key, pkt1->size);
    KEY *kp = key_create(bp1);
    BLOB *bp2 = blob_create(*value, pkt2->size);

    if(store_put(tp, kp, bp2) == TRANS_ABORTED) {
        abort_end(fd, tp, pkt1, end);
        free_pkt_void(pkt1, key);
        free_pkt_void(pkt2, value);
        return;
    }

    // sends a reply packet
    if(reply(fd, pkt1, XACTO_REPLY_PKT, TRANS_PENDING, NULL) == -1) {
        abort_end(fd, tp, pkt1, end);
        free_pkt_void(pkt1, key);
        free_pkt_void(pkt2, value);
        return;
    }

    free_pkt_void(pkt1, key);
    free_pkt_void(pkt2, value);
}

void xacto_get_pkt(int fd, TRANSACTION *tp, XACTO_PACKET *pkt, int *end) {
    XACTO_PACKET *pkt1 = Calloc(sizeof(XACTO_PACKET), sizeof(char));
    void **key = Calloc(sizeof(void**), sizeof(char));
    if(proto_recv_packet(fd, pkt1, key) == -1) {
        abort_end(fd, tp, pkt1, end);
        free_pkt_void(pkt1, key);
        return;
    }
    debug("[%d] Received key, size %d", fd, pkt1->size);

    BLOB *bp1 = blob_create(*key, pkt1->size);
    KEY *kp = key_create(bp1);

    if(store_get(tp, kp, &bp1) == TRANS_ABORTED) {
        abort_end(fd, tp, pkt1, end);
        free_pkt_void(pkt1, key);
        return;
    }

    if(reply(fd, pkt, XACTO_REPLY_PKT, TRANS_PENDING, bp1) == -1) {
        abort_end(fd, tp, pkt1, end);
        free_pkt_void(pkt1, key);
        return;
    }

    // if there is a value associated with the key
    if(bp1 && bp1->content) {
        debug("[%d] Value is %p [%d]", fd, pkt1, pkt1->size);
        blob_unref(bp1, "obtained from store_get");

        // sends a reply packet
         if(reply(fd, pkt1, XACTO_DATA_PKT, TRANS_PENDING, bp1) == -1) {
            abort_end(fd, tp, pkt1, end);
            free_pkt_void(pkt1, key);
            return;
        }
    } else {
        debug("[%d] Value is NULL", fd);
        // sends a reply packet
        if(reply(fd, pkt, XACTO_DATA_PKT, TRANS_PENDING, bp1) == -1) {
            abort_end(fd, tp, pkt1, end);
            free_pkt_void(pkt1, key);
            return;
        }
    }

    free_pkt_void(pkt1, key);
    return;
}
