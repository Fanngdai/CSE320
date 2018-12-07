typedef struct Printer_list {
    PRINTER *printer;
    struct Printer_list *next;
} Printer_list;

Printer_list* add_printer(Printer_list **head, char *name, char *type);
Printer_list* search_printer(Printer_list *head, char *name);
Printer_list* get_printer_by_id(Printer_list *head, int id);
void print_printer_status(PRINTER *printer);
void print_printer(Printer_list *head, FILE *out);
void free_printer(Printer_list *head);

extern int printer_id;