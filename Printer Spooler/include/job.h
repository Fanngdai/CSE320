// typedef struct job {
//     int jobid;                      /* Job ID of job. */

//     JOB_STATUS status;              /* Status of job. */
//     int pgid;                       /* Process group ID of conversion pipeline. */
//     char *file_name;                /* Pathname of file to be printed. */
//     char *file_type;                /* Type name of file to be printed. */
//     PRINTER_SET eligible_printers;  /* Set of eligible printers for the job. */
//     PRINTER *chosen_printer;        /* Printer chosen for this job. */
//     struct timeval creation_time;   /* Time job was queued. */
//     struct timeval change_time;     /* Time of last status change. */
//     void *other_info;               /* You may store other info in this field. */
// } JOB;


typedef struct Job_list {
    JOB *job;
    struct Job_list *next;
} Job_list;

Job_list* add_job(Job_list **head, char *file, char *ext, PRINTER_SET set);
Job_list* search_jobid(Job_list *head,int id);
void remove_job(Job_list **head, int id);
void print_job_status(JOB *job);
void print_job(Job_list *head, FILE *fp);
void change_job_status(JOB *job, JOB_STATUS status);
void free_job(Job_list *head);

extern int job_id;