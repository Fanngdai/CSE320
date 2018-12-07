#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <readline/readline.h>

#include "imprimer.h"
#include "printer.h"
#include "msg.h"

// the id
int printer_id = 0;

Printer_list* add_printer(Printer_list **head, char *name, char *type) {
    if(printer_id+1==MAX_PRINTERS) {
        error_msg("max printer (32)");
        return NULL;
    // Value is already in the linked list
    } else if(search_printer(*head, name)!=NULL) {
        error_msg("printer already exists");
        return NULL;
    }

    char *tname = malloc(strlen(name)+1);
    memcpy(tname, name, strlen(name)+1);
    char *ttype = malloc(strlen(type)+1);
    memcpy(ttype, type, strlen(type)+1);

    Printer_list *new_node = malloc(sizeof(Printer_list));
    PRINTER *p = malloc(sizeof(PRINTER));
    p->id = printer_id++;
    p->name = tname;
    p->type = ttype;
    p->enabled = 0;
    p->busy = 0;
    p->other_info = NULL;
    new_node->printer = p;
    new_node->next = NULL;

    if(*head == NULL) {
        *head = new_node;
    } else {
        Printer_list *cursor = *head;
        while(cursor->next != NULL) {
            cursor = cursor->next;
        }
        cursor->next = new_node;
    }

    return new_node;
}

Printer_list* search_printer(Printer_list *head, char *name) {
    while (head != NULL) {
        // If value is found
        if(!strcmp(head->printer->name, name)) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

Printer_list* get_printer_by_id(Printer_list *head, int id) {
    if(id<0 && id<31) {
        return NULL;
    }
    while (head != NULL) {
        // If value is found
        if(head->printer->id == id) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

// prints one
void print_printer_status(PRINTER *printer) {
    char buf[512] = {};
    imp_format_printer_status(printer, buf, 512);
    fprintf(out, "%s\n", buf);
}

// prints all (printers) cmd
void print_printer(Printer_list *head, FILE *fp) {
    if(head == NULL) {
        error_msg("printer is NULL");
        return;
    }

    Printer_list *cursor = head;
    char buf[512] = {};
    while (cursor != NULL) {
        imp_format_printer_status(cursor->printer, buf, 512);
        fprintf(fp, "%s\n", buf);
        cursor = cursor->next;
    }
}

void free_printer(Printer_list *head) {
    if(head == NULL)
        return;

    head = NULL;
    Printer_list *prev = NULL;
    while (head != NULL) {
        if(head->printer != NULL) {
            if(head->printer->name != NULL) {
                free(head->printer->name);
                head->printer->name = NULL;
            }
            if(head->printer->type != NULL) {
                free(head->printer->type);
                head->printer->type = NULL;
            }
            free(head->printer);
            head->printer = NULL;
        }

        prev = head;
        head = head->next;

        free(prev);
        prev = NULL;
    }
}
