#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <readline/readline.h>
#include <sys/time.h>

#include "imprimer.h"
#include "printer.h"
#include "job.h"
#include "msg.h"

// the id
int job_id = 0;

Job_list* add_job(Job_list **head, char *file, char *ext, PRINTER_SET set) {
    char *file_name = malloc(strlen(file)+1);
    memcpy(file_name, file, strlen(file)+1);

    char *file_ext = malloc(strlen(ext)+1);
    memcpy(file_ext, ext, strlen(ext)+1);

    Job_list *new_node = malloc(sizeof(Job_list));
    JOB *p = malloc(sizeof(JOB));

    p->jobid = job_id++;
    // Default status is queue
    p->status = QUEUED;
    p->pgid = 0;
    p->file_name = file_name;
    p->file_type = file_ext;
    p->eligible_printers = set;
    p->chosen_printer = NULL;

    struct timeval t;
    gettimeofday(&t, NULL);
    p->creation_time = t;
    p->change_time = t;
    p->other_info = NULL;

    new_node->job = p;
    new_node->next = NULL;

    if(*head == NULL) {
        *head = new_node;
    } else {
        Job_list *cursor = *head;
        while(cursor->next != NULL) {
            cursor = cursor->next;
        }
        cursor->next = new_node;
    }

    return new_node;
}

Job_list* search_jobid(Job_list *head, int id) {
    while (head != NULL) {
        // If value is found
        if(head->job->jobid == id) {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

void print_job(Job_list *head, FILE *fp) {
    if(head == NULL) {
        error_msg("job is NULL");
        return;
    }

    Job_list *cursor = head;
    char buf[512] = {};
    while (cursor != NULL) {
        imp_format_job_status(cursor->job, buf, 512);
        fprintf(fp, "%s\n", buf);
        cursor = cursor->next;
    }
}

void print_job_status(JOB *job) {
    char buf[512] = {};
    imp_format_job_status(job, buf, 512);
    fprintf(out, "%s\n", buf);
}

void change_job_status(JOB *job, JOB_STATUS status) {
    job->status = status;
    if(job->status == QUEUED || job->status == COMPLETED || job->status == ABORTED)
        job->chosen_printer = NULL;
    struct timeval t;
    gettimeofday(&t, NULL);
    job->change_time = t;
    print_job_status(job);
}

Job_list* free_one_job(Job_list *list) {
    Job_list *next;
    if(list->next == NULL)
        next = NULL;
    else
        next = list->next;

    if(list != NULL) {
        if(list->job != NULL) {
            if(list->job->file_name != NULL) {
                free(list->job->file_name);
                list->job->file_name = NULL;
            }
            if(list->job->file_type != NULL) {
                free(list->job->file_type);
                list->job->file_type = NULL;
            }
            if(list->job) {
                free(list->job);
                list->job = NULL;
            }
        }
        free(list);
        list = NULL;
    }
    return next;
}

void remove_job(Job_list **h, int id) {
    Job_list *head = *h;
    if(head->job->chosen_printer!=NULL) {
        head->job->chosen_printer->busy = 0;
        print_printer_status(head->job->chosen_printer);
    }

    // the first value. remove head
    if(head->job->jobid == id) {
        *h = free_one_job(head);
        return;
    }

    while (head->next != NULL) {
        // If value is found
        if(head->next->job->jobid == id) {
            Job_list *temp = free_one_job(head->next);
            if(temp)
                *head->next = *temp;
            return;
        }
        head = head->next;
    }

    return;
}


void free_job(Job_list *head) {
    if(head == NULL)
        return;

    // head = NULL;
    // Job_list *prev = NULL;
    while (head != NULL) {
        // if(head->job != NULL) {
        //     if(head->job->file_name != NULL) {
        //         free(head->job->file_name);
        //         head->job->file_name = NULL;
        //     }
        //     if(head->job->file_type != NULL) {
        //         free(head->job->file_type);
        //         head->job->file_type = NULL;
        //     }

        //     free(head->job);
        //     head->job = NULL;
        // }

        // prev = head;
        // head = head->next;

        // free(prev);
        // prev = NULL;
        head = free_one_job(head);
    }
}
