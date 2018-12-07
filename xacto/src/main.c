#include "csapp.h"
#include "server.h"
#include "debug.h"

#include "client_registry.h"
#include "transaction.h"
#include "store.h"

#include <string.h>
#include <signal.h>

static void terminate(int status);
int valid_port(char* str);
void sighup_handler();

CLIENT_REGISTRY *client_registry;

int main(int argc, char* argv[]){
    // Option processing should be performed here.
    // Option '-p <port>' is required in order to specify the port number
    // on which the server should listen.
    char *port = argv[2];
    if(argc!=3 || strcmp(argv[1], "-p") || valid_port(port) == -1) {
        fprintf(stderr, "Invalid arguments. USAGE: bin/xacto -p <port>\n");
        exit(EXIT_FAILURE);
    }

    // Perform required initializations of the client_registry,
    // transaction manager, and object store.
    client_registry = creg_init();
    trans_init();
    store_init();

    // TODO: Set up the server socket and enter a loop to accept connections
    // on this socket.  For each connection, a thread should be started to
    // run function xacto_client_service().  In addition, you should install
    // a SIGHUP handler, so that receipt of SIGHUP will perform a clean
    // shutdown of the server.

    Signal(SIGHUP, &sighup_handler);

    int listenfd, *connfdp;
    socklen_t clientlen = sizeof(struct sockaddr_storage);
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    if((listenfd = Open_listenfd(argv[2])) < 0) { terminate(EXIT_FAILURE); }

    while(1) {
        connfdp = Calloc(sizeof(int), sizeof(char));
        *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);

        if(*connfdp < 0) {
            Free(connfdp);
            terminate(EXIT_FAILURE);
        }

        Pthread_create(&tid, NULL, xacto_client_service, connfdp);
    }

    fprintf(stderr, "You have to finish implementing main() "
	    "before the Xacto server will function.\n");

    terminate(EXIT_FAILURE);
}

/*
 * Function called to cleanly shut down the server.
 */
void terminate(int status) {
    // Shutdown all client connections.
    // This will trigger the eventual termination of service threads.
    creg_shutdown_all(client_registry);

    debug("Waiting for service threads to terminate...");
    creg_wait_for_empty(client_registry);
    debug("All service threads terminated.");

    // Finalize modules.
    creg_fini(client_registry);
    trans_fini();
    store_fini();

    debug("Xacto server terminating");
    exit(status);
}

/*
 * Make sure that port is actually a number and it is btw 0-65535
 */
int valid_port(char* str) {
    int port = 0;
    while(*str != '\0') {
        if(*str<48 || *str>57)
            return -1;
        else
            port = port * 10 + (*str-48);
        str++;
    }
    return (port<0||port>65535)?-1:1;
}

void sighup_handler(){
    terminate(EXIT_SUCCESS);
}