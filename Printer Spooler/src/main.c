#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <readline/readline.h>
#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "msg.h"
#include "imprimer.h"
#include "list.h"
#include "printer.h"
#include "job.h"


#define _GNU_SOURCE

FILE *out;

/*
 * Prints the valid commands
 */
void help() {
    printf( "\e[4mMiscellaneous Commands\e[0m\n"
            "\e[1mhelp\e[0m\n"
            "\e[1mquit\e[0m\n\n"

            "\e[4mConfiguration Commands\e[0m\n"
            "\e[1mtype\e[0m\t\t\e[3m<file_type>\e[0m\n"
            "\e[1mprinter\e[0m\t\t\e[3m<printer_name> <file_type>\e[0m\n"
            "\e[1mconversion\e[0m\t\e[3m<file_type1> <file_type2> <conversion_program> [arg1 arg2...]\e[0m\n\n"

            "\e[4mInformational Commands\e[0m\n"
            "\e[1mprinters\e[0m\n"
            "\e[1mjobs\e[0m\n\n"

            "\e[4mSpooling Commands\e[0m\n"
            "\e[1mprint\e[0m\t\t\e[3m<file_name> [printer1 printer2...]\e[0m\n"
            "\e[1mcancel\e[0m\t\t\e[3m<job_number>\e[0m\n"
            "\e[1mpause\e[0m\t\t\e[3m<job_number>\e[0m\n"
            "\e[1mresume\e[0m\t\t\e[3m<job_number>\e[0m\n"
            "\e[1mdisable\e[0m\t\t\e[3m<printer_name>\e[0m\n"
            "\e[1menable\e[0m\t\t\e[3m<printer_name>\e[0m\n"
        );
}


/**
 * @brief Same as atoi (Converts string to integer)
 *
 * @param str is an array of chars which is consisted of decimal digits
 *
 * @return decimal value if string given is a decimal
 *          -1 if str is not a decimal value
 */
int strtoi(char* str) {
    int num = 0;
    int sign = 1;
    if(*str == '-') {
        sign = -1;
        str++;
    }
    while(*str != '\0' && *str>47 && *str<58) {
        num = num * 10 + (*str-48);
        str++;
    }
    if(*str=='\0') {
        return num * sign;
    }
    return -1;
}

char *command_file;
char *output_file;
void batch_mode(int argc, char *argv[]) {
    char optval;
    for (int i = 0; optind < argc; i++) {
        // + means all following commands with : (has param) are required.
        if((optval = getopt(argc, argv, "+i:o:")) != -1) {

            // No args given
            if(!optarg) {
                ERROR_BASH(argv[0]);
                exit(EXIT_FAILURE);
                // Make sure the following arg is not a command.
            } else if(strcmp(optarg, "-i")==0 || strcmp(optarg, "-o")==0) {
                ERROR_BASH(argv[0]);
                exit(EXIT_FAILURE);
                // There must be a space btw the command and argument
            } else if(*(optarg-1) != '\0') {
                ERROR_BASH(argv[0]);
                exit(EXIT_FAILURE);
            }

            switch(optval) {
                case 'i':
                    command_file = optarg;
                    break;
                case 'o':
                    output_file = optarg;
                    break;
                case '?':
                default:
                    ERROR_BASH(argv[0]);
                    exit(EXIT_FAILURE);
            }
        }
    }
}



Node *head = NULL;
Printer_list *phead = NULL;
Job_list *jhead = NULL;



/*
 * Searches for the printer and returns the id in the set format
 */
int printer_set_value(char *name) {
    Printer_list *plist = search_printer(phead, name);
    if(!plist) {
        error_msg_note("\'%s\' not valid printer", name);
        return 0;
    }
    return (0x1 << plist->printer->id);
}



Job_list* print(char *arg1, char *arg2, char *arg3) {
    char *token = NULL;
    // make sure there is a .
    char *ext = strchr(arg1, '.');
    Job_list *new_job;

    // file exists
    if(ext && ++ext != '\0') {
        // get the extension
        if(!search_list(head, ext)) {
            error_msg("invalid file type");
            return NULL;
        } else if(arg3) {      // has optional
            PRINTER_SET set = 0;

            int temp_set = printer_set_value(arg2);
            if(temp_set) {
                set ^= temp_set;
            } else {
                return NULL;
            }

            temp_set = printer_set_value(arg3);
            if(temp_set) {
                set ^= temp_set;
            } else {
                return NULL;
            }

            token = strtok(NULL, " ");
            while(token) {
                // sets up the printer_set
                temp_set = printer_set_value(token);
                if(temp_set) {
                    set ^= temp_set;
                } else {
                    return NULL;
                }
                token = strtok(NULL, " ");
            }

            if(set == 0) {
                error_msg("printer not compatible");
                return NULL;
            }
            new_job = add_job(&jhead, arg1, ext, set);
        } else if (arg2) {
            PRINTER_SET set = 0;

            int temp_set = printer_set_value(arg2);
            if(temp_set) {
                set ^= temp_set;
            }

            if(set == 0) {
                error_msg("printer not compatible");
                return NULL;
            }
            new_job = add_job(&jhead, arg1, ext, set);
        } else {
            new_job = add_job(&jhead, arg1, ext, ANY_PRINTER);
        }

        print_job_status(new_job->job);
        return new_job;
    } else {
        // If file extension d.n.e
        error_msg("invalid file type");
        return NULL;
    }
    return NULL;
}


/*
 * Done. Finds the path from one to another
 */
PRINTER *find_bfs(JOB *job) {
    PRINTER_SET track = job->eligible_printers;
    for(int i=0; i<32; i++) {
        if(track&(0x1<<i)) {
            Printer_list *p;
            // get the printer by the id - only if it exists
            if((p = get_printer_by_id(phead, i)) && p->printer->enabled && p->printer->busy==0) {
                bfs(head, job->file_type, p->printer->type);
                // path available
                if(res_loc != -1) {
                    return p->printer;
                }
                // path is found. This prints the result
                // for(int i=0; i<=res_loc && res_loc!=-1; i++)
                //     printf("###%s\n", result[i]);
            }
        }
    }
    return NULL;
}

// split in python with a space
int split(char *conv_prog, char *arg, char *args[]) {
    int arr_size = 1;
    args[0] = conv_prog;

    char *token = strtok(arg, " ");
    while(token) {
        args[arr_size++] = token;
    }
    return arr_size;
}

pid_t Fork(void) {
    pid_t pid;
    if((pid=fork()) < 0) {
        error_msg_note("fork error: %s", strerror(errno));
    }
    return pid;
}

void process_print();

pid_t child;
void cleanup(int signal) {
  int status;
  waitpid(child, &status, 0);
  // write(1,"cleanup!\n",9);
}

// cancel
void sigterm(int signal) {

}

// pause
void sigstop(int signal) {

}

// resume
void sigcont(int signal) {

}

// void cleanup(int signal) {
//   while (waitpid((pid_t) (-1), 0, WNOHANG) > 0) {}
// }


// forks the shit
void fork_conv(Job_list *j, PRINTER *printer) {
    printer->busy = 1;
    print_printer_status(printer);
    j->job->chosen_printer = printer;
    change_job_status(j->job, RUNNING);

    pid_t parent;
    signal(SIGCHLD, cleanup);
    if((parent=Fork())==0) { //master
        signal(SIGTERM, sigterm);
        signal(SIGSTOP, sigstop);
        signal(SIGCONT, sigcont);

        setpgid(getpgid(getpid()), getpid());
        pid_t child;

        // fork master process and one child process - for if the shit is the same
        // path was found. Path is in result array with size res_loc
        if((child=Fork())==0) { // child
            int in = open(j->job->file_name, O_RDONLY);
            int out = imp_connect_to_printer(printer, PRINTER_NORMAL);

            if(out!=-1){
                char *tst[] = {"/bin/cat", NULL};
                dup2(in, 0);
                dup2(out, 1);

                if(execv("/bin/cat", tst)<0) {
                    exit(EXIT_FAILURE);
                } else {
                    exit(EXIT_SUCCESS);
                }
            }

            // do some conversion from one file to the next here
            // for(int i=1; i<=res_loc; i++) {
                // Node *from = search_list(head, result[i-1]);
                // Conv *to = search_conv(from->conv,result[i]);

                // char *temp_args[128];
                // int arg_size = split(to->conv_prog, to->args, temp_args);
                // char **args = malloc(arg_size*sizeof(char*));
                // for(int i=0; i<arg_size; i++)
                //     *(args+i) = temp_args[i];



                // free(args);
            // }
        } else { // back in master
            int random = 0;
            waitpid(child, &random, 0);
            // sleep(60);
            random = 0;
            if(WIFEXITED(random)==EXIT_SUCCESS){
                exit(EXIT_SUCCESS);
            } else {
                exit(EXIT_FAILURE);
            }
        }
    } else { // back in main
        printer->busy = 0;
        print_printer_status(printer);
        // j->job->chosen_printer = NULL;

        int random = 0;
        if(WIFEXITED(random)==EXIT_SUCCESS){
            change_job_status(j->job, COMPLETED);
        } else {
            change_job_status(j->job, ABORTED);
        }

        // waitpid(parent, &random, 0);
        // if (WIFEXITED(random))
        // remove_job(&jhead, j->job->jobid);
        process_print();
    }
}



/*
 * LOOKS THROUGH ALL JOBS AND CHECK IF THEY CAN BE PROCESSED.
 * runs job if available. (Eligible printer is enabled and not busy)
 * Make sure you use eligible_printers
 */
void process_print() {
    Job_list *cursor = jhead;
    PRINTER *p;
    while(cursor && cursor->job) {
        // if the status is queued and there is a path
        if(cursor->job->status == QUEUED && (p = find_bfs(cursor->job))) {
            // then convert by forking
            fork_conv(cursor, p);
        }
        cursor = cursor->next;
    }
}



/*
 * Used for when there are no command line arguments and the inputs are manually inserted through stdin.
 */
int interactive_mode(char *input) {
    char *token = strtok(input, "\n");
    token = strtok(token, "\r\n");
    token = strtok(token, " ");

    char *arg1 = NULL;
    char *arg2 = NULL;
    char *arg3 = NULL;
    if(token) {
        if((arg1 = strtok(NULL, " "))) {
            if((arg2 = strtok(NULL, " "))) {
                arg3 = strtok(NULL, " ");
            }
        }
    }

    if(token == NULL) {

    } else if(!strcmp(token, "help")) {
        if(arg1) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            help();
        }
    } else if(!strcmp(token, "quit")) {
        if(arg1) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            return 0;
        }
    } else if(!strcmp(token, "type")) {
        if(!arg1) {
            REQ_ARG("type");
            TRY_HELP();
        } else if(arg2) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            add_list(&head, arg1);
        }
    } else if(!strcmp(token, "printer")) {
        if(!arg2) {
            REQ_ARG("printer");
            TRY_HELP();
        } else if(arg3) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            // arg1 printer name
            // arg2 file type
            if(!search_printer(phead, arg1) && search_list(head, arg2)) {
                Printer_list *status = add_printer(&phead, arg1, arg2);
                // Inform the user of the status
                print_printer_status(status->printer);
            } else {
                error_msg("file type/printer not compatible");
            }
        }
    } else if(!strcmp(token, "conversion")) {
        if(!arg3) {
            REQ_ARG("conversion");
            TRY_HELP();
        } else {
            char *temp = strtok(NULL, "\r\n");
            add_conv(&head, arg1, arg2, arg3, temp);
            // runs job if available
            process_print();
        }
    } else if(!strcmp(token, "printers")) {
        if(arg1) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            print_printer(phead, out);
        }
    } else if(!strcmp(token, "jobs")) {
        if(arg1) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            print_job(jhead, out);
        }
    } else if(!strcmp(token, "print")) {
        // QUEUED OR RUNNING
        Job_list *new_job = NULL;
        // if file_name d.n.e
        if(!arg1) {
            REQ_ARG("print");
            TRY_HELP();
        // print makes a new job. If there is a new_job
        } else if((new_job = print(arg1, arg2, arg3))) {
            // print update
            // print_job_status(new_job->job);
            PRINTER *p;
            if((p=find_bfs(new_job->job))) {
                fork_conv(new_job, p);
            }
        }
    } else if(!strcmp(token, "cancel")) {
        // USE SIGTERM TO PROCESSGRP TO STATUS ABORTED
        if(!arg1) {
            REQ_ARG("cancel");
            TRY_HELP();
        } else if(arg2) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            int num;
            // If it is a valid value
            if((num = strtoi(arg1))>=0) {
                Job_list *job;
                if((job = search_jobid(jhead,num))) {
                    change_job_status(job->job, ABORTED);
                    // signal(SIGTERM, cleanup);
                    // killpg(job->job->pgid, SIGTERM);
                    // remove_job(&jhead, job->job->jobid);
                } else {
                    error_msg("job_number not found");
                }
            } else {
                error_msg("invalid job_number");
            }
        }
    } else if(!strcmp(token, "pause")) {
        // USE SIGSTOP TO PROCESSGRP TO STATUS PAUSED
        if(!arg1) {
            REQ_ARG("pause");
            TRY_HELP();
        } else if(arg2) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            int num;
            // If it is a valid value
            if((num = strtoi(arg1))>=0) {
                Job_list *job;
                if((job = search_jobid(jhead,num))) {
                    if(job->job->status == RUNNING) {
                        change_job_status(job->job, PAUSED);
                    } else {
                        error_msg("job not running");
                    }
                    // signal(SIGSTOP, cleanup);
                    // killpg(job->job->pgid, SIGSTOP);
                    // pause();
                } else {
                    error_msg("job_number not found");
                }
            } else {
                error_msg("invalid job_number");
            }
        }
    } else if(!strcmp(token, "resume")) {
        // USE SIGCONT TO PROCESSGRP TO STATUS RUNNING
        if(!arg1) {
            REQ_ARG("resume");
            TRY_HELP();
        } else if(arg2) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            int num;
            // If it is a valid value
            if((num = strtoi(arg1))>=0) {
                Job_list *job;
                if((job = search_jobid(jhead,num))) {
                    if(job->job->status == PAUSED) {
                        change_job_status(job->job, RUNNING);
                    } else {
                        error_msg("job not paused");
                    }
                    // signal(SIGSTOP, cleanup);
                    // killpg(job->job->pgid, SIGCONT);
                } else {
                    error_msg("job_number not found");
                }
            } else {
                error_msg("invalid job_number");
            }
        }
    } else if(!strcmp(token, "disable")) {
        if(!arg1) {
            REQ_ARG("disable");
            TRY_HELP();
        } else if(arg2) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            Printer_list *p = search_printer(phead, arg1);
            if(p) {
                if(p->printer->enabled) {
                    p->printer->enabled = 0;
                    print_printer_status(p->printer);
                } else {
                    error_msg("unable to disable printer");
                }
            } else {
                error_msg_note("printer \'%s\' not compatible", arg1);
            }
        }
    } else if(!strcmp(token, "enable")) {
        if(!arg1) {
            REQ_ARG("enable");
            TRY_HELP();
        } else if(arg2) {
            TOO_MANY_ARG();
            TRY_HELP();
        } else {
            Printer_list *p = search_printer(phead, arg1);
            if(p) {
                if(!p->printer->enabled) {
                    p->printer->enabled = 1;
                    print_printer_status(p->printer);
                    // runs job if available
                    process_print();
                } else {
                    error_msg("unable to enable printer");
                }
            } else {
                error_msg_note("printer \'%s\' not compatible", arg1);
            }
        }
    } else {
        INVALID_OPT(token);
        TRY_HELP();
    }

    return 1;
}

void print_cmd(char *input) {
    if(out != stdout) {
        fprintf(out, "imp> ");
        if(input)
            fprintf(out, "%s\n", input);
        else
            fprintf(out, "\n");
    }
}

/*
 * "Imprimer" printer spooler.
 */
int main(int argc, char *argv[]) {
    out = stdout;
    if(argc > 1) batch_mode(argc, argv);

    if(output_file) {
        out = fopen(output_file,"w+");
        if(!out) exit(EXIT_FAILURE);
    }

    char *input = NULL;
    size_t t = 1024;
    int end_loop = 1;


    // printf("###########################\n");
    // printf("Parent pid: %d\n", getpid());
    // printf("Parent grpid: %d\n", getpgid(getpid()));

    // pid_t pid;
    // if(!(pid = fork())) {
    //     printf("Master pid: %d\n", getpid());
    //     printf("Master groupid: %d\n", getpgid(getpid()));
    //     setpgid(getpid(), getpid());
    //     printf("New Master groupid: %d\n", getpgid(getpid()));
    // }
    // printf("###########################\n");

    // ReturnsPIDofcurrentprocess
    // pid_t getpid(void);


    if(command_file) {
        FILE *in = fopen(command_file,"r");
        // Error opening file
        if(!in) exit(EXIT_FAILURE);
        // getline must be freed regardless
        // fgets(input, t, in);
        while(getline(&input, &t, in) != EOF) {
            print_cmd(strtok(input, "\r\n"));
            if(strcmp(input, "\r\n") && strcmp(input, "\n"))
                end_loop = interactive_mode(input);
            free(input);
            input = NULL;

            if(!end_loop)
                break;
        }

        fclose(in);
    } else {
        do {
            input = readline("imp> ");
            print_cmd(input);
            if(input) {
                end_loop = interactive_mode(input);
                free(input);
                input = NULL;
            }
        } while(end_loop);
    }

    if(out != stdout)
        fclose(out);

    // print_list(head);
    // print_conv(head);
    free_list(head);
    free_printer(phead);
    free_job(jhead);
    exit(EXIT_SUCCESS);
}

// valgrind --leak-check=full --show-leak-kinds=all bin/imprimer

// TO DO:
// done - Make sure to do output to file
//
