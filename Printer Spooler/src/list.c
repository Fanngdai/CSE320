#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "msg.h"

Node* add_list(Node **head, char *new_data) {
    // Value is already in the linked list
    if(search_list(*head, new_data)!=NULL) {
        error_msg("type already exists");
        return NULL;
    }

    char *temp = malloc(strlen(new_data)+1);
    memcpy(temp, new_data, strlen(new_data)+1);

    Node* new_node = malloc(sizeof(Node));
    new_node->data = temp;
    new_node->visited = 0;
    new_node->conv = NULL;
    new_node->next = NULL;

    if(*head == NULL) {
        *head = new_node;
    } else {
        Node *cursor = *head;
        while(cursor->next) {
            cursor = cursor->next;
        }
        cursor->next = new_node;
    }
    return new_node;
}

Node* search_list(Node *head, char *node) {
    while (head != NULL) {
        // If value is found
        if(!strcmp(head->data, node)) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}


// testing file. DO NOT SUBMIT WITH THIS RUNNING
void print_list(Node *node)  {
    if(node == NULL) {
        error_msg("node is NULL");
        return;
    }

    Node *cursor = node;
    while (cursor != NULL) {
        printf("%s\n", cursor->data);
        cursor = cursor->next;
    }
}










int add_conv(Node **head, char *data1, char *data2, char *conv_prog, char *args) {
    if(!strcmp(data1, data2)) {
        // fprintf(stderr, "files types identical\n");
        return 1;
    }

    Node *d1 = search_list(*head, data1);
    Node *d2 = search_list(*head, data2);
    if(!d1) {
        error_msg_note("file type \'%s\' not compatible", data1);
        return 0;
    } else if(!d2) {
        error_msg_note("file type \'%s\' not compatible", data2);
        return 0;
    }

    if(search_conv(d1->conv, data2)) {
        return 1;
    } else {
        char *tdest = malloc(strlen(data2)+1);
        memcpy(tdest, data2, strlen(data2)+1);

        char *tconv = malloc(strlen(conv_prog)+1);
        memcpy(tconv, conv_prog, strlen(conv_prog)+1);

        Conv *node = malloc(sizeof(Conv));
        node->dest = tdest;
        node->conv_prog = tconv;
        node-> next = NULL;

        if(args) {
            char *targ = malloc(strlen(args)+1);
            memcpy(targ, args, strlen(args)+1);
            node->args = targ;
        } else {
            node->args = NULL;
        }

        if(!d1->conv) {
            d1->conv = node;
        } else {
            Conv *cursor = d1->conv;
            while(cursor->next) {
                cursor = cursor->next;
            }
            cursor->next = node;
        }
    }
    return 1;
}

Conv* search_conv(Conv *head, char *node) {
    while (head != NULL) {
        // If value is found
        if(!strcmp(head->dest, node)) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}


// testing file. DO NOT SUBMIT WITH THIS RUNNING
void print_conv(Node *node){
    if(node == NULL) {
        error_msg("node is NULL");
        return;
    }

    Node *cursor = node;
    while (cursor != NULL) {
        printf("%s:\t", cursor->data);
        Conv *c = cursor->conv;
        while(c != NULL) {
            printf("%s\t", c->dest);
            printf("%s\t", c->conv_prog);
            printf("%s\t", c->args);
            c = c->next;
        }
        printf("\n");
        cursor = cursor->next;
    }
}

void reset_visited(Node *head) {
    while (head != NULL) {
        head->visited = 0;
        head = head->next;
    }
}

char *queue[128] = {};
int loc = -1;

void look(Node *head, Node *node, char *dest) {
    if(node == NULL || node->visited) {
        return;
    }

    queue[++loc] = node->data;
    node->visited = 1;

    if(strcmp(node->data,dest) == 0) {
        return;
    } else if(search_conv(node->conv, dest)) {
        look(head, search_list(head, dest), dest);
    } else {
        Conv *temp = node->conv;
        while(temp != NULL) {
            look(head, search_list(node, temp->dest), dest);
            temp = temp->next;
        }
    }
}

char *result[128] = {};
int res_loc = -1;
void backtrack(Node *head) {
    Node *node;
    Conv *conv;
    for(; loc>-1; loc--) {
        node = search_list(head, queue[loc]);
        conv = search_conv(node->conv, result[res_loc]);
        if(conv){
            result[++res_loc] = queue[loc];
        }
    }
    loc = -1;
}

void bfs(Node *head, char *src, char *dest) {
    // src and dest is the same
    if(!strcmp(src, dest)) {
        result[++res_loc] = src;
        return;
    }
    res_loc = -1;
    reset_visited(head);

    look(head, search_list(head, src), dest);
    if(loc!=-1) {
        // look for the dest
        for(; loc>-1; --loc) {
            if(!strcmp(queue[loc], dest)) {
                result[++res_loc] = dest;
                break;
            }
        }

        // if there is a way to get from src to dest
        if(loc>0) {
            backtrack(head);

            // backtrack gives you the value in reverse
            int j = res_loc;
            for(int i=0; i<j ; i++, j--) {
                char *temp = result[i];
                result[i] = result[j];
                result[j] = temp;
            }
        }
    }
    // for(int i=0; i<=res_loc; i++) {
    //     printf("%s\n", result[i]);
    // }
}










void free_list(Node *head) {
    if(head == NULL)
        return;

    head = NULL;
    Node *prev = NULL;
    while(head != NULL) {
        if(head->data != NULL) {
            free(head->data);
            head->data = NULL;
        }
        Conv *temp;
        while(head->conv != NULL) {
            free(head->conv->dest);
            head->conv->dest = NULL;

            free(head->conv->conv_prog);
            head->conv->conv_prog = NULL;

            if(!head->conv->args) {
                free(head->conv->args);
                head->conv->args = NULL;
            }

            temp = head->conv;
            head->conv = temp->next;

            free(temp);
            temp = NULL;
        }

        prev = head;
        head = head->next;

        if(prev!=NULL) {
            free(prev);
            prev = NULL;
        }
    }
}
