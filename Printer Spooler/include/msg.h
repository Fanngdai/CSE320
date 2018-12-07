void TRY_HELP();
void REQ_ARG(char *s);
void INVALID_OPT(char *s);
void TOO_MANY_ARG();
void ERROR_BASH(char *s);
void error_msg(char *s);
void error_msg_note(char *format, char *s);

extern FILE *out;