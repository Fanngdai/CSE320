typedef struct Node {
    char *data;
    int visited;
    struct Conv *conv;
    struct Node *next;
} Node;

Node* add_list(Node **head, char *new_data);
Node* search_list(Node *head, char *node);
void print_list(struct Node *node);



typedef struct Conv {
    char *dest;
    char *conv_prog;
    char *args;
    struct Conv *next;
} Conv;

int add_conv(Node **head, char *data1, char *data2, char *conv_prog, char *args);
Conv* search_conv(Conv *head, char *node);
void print_conv(Node *node);
void bfs(Node *head, char *src, char *dest);



void free_list(Node *head);



extern char *result[128];
extern int res_loc;