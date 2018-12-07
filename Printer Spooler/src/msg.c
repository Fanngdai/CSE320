#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "imprimer.h"
#include "msg.h"

// #define TRY_HELP fprintf(stderr, )
// #define REQ_ARG(s) fprintf(stderr, "option requires an argument -- \'%s\'\n", s)
// #define INVALID_OPT(s) fprintf(stderr, "invalid option -- \'%s\'\n", s)
// #define TOO_MANY_ARG fprintf(stderr, "")
// #define ERROR_BASH(s) fprintf(stderr, "Usage: %s [-i <cmd_file>] [-o <out_file>]\n", s)

FILE *out;

void TRY_HELP() {
    char buf[512] = {};
    imp_format_error_message("try \'help\' for more information\n", buf, 512);
    fprintf(out, "%s", buf);
}

void REQ_ARG(char *s) {
    char dest[512] = {};
    char buf[512] = {};
    sprintf(dest,"option requires an argument -- \'%s\'", s);
    imp_format_error_message(dest, buf, 512);
    fprintf(out, "%s\n", buf);
}

void INVALID_OPT(char *s) {
    char dest[512] = {};
    char buf[512] = {};
    sprintf(dest,"invalid option -- \'%s\'", s);
    imp_format_error_message(dest, buf, 512);
    fprintf(out, "%s\n", buf);
}

void TOO_MANY_ARG() {
    char buf[512] = {};
    imp_format_error_message("too many argument", buf, 512);
    fprintf(out, "%s\n", buf);
}

void ERROR_BASH(char *s) {
    char dest[512] = {};
    char buf[512] = {};
    sprintf(dest,"option requires an argument -- \'%s\'", s);
    imp_format_error_message(dest, buf, 512);
    fprintf(stderr, "%s\n", buf);
}

void error_msg(char *s) {
    char buf[512] = {};
    imp_format_error_message(s, buf, 512);
    fprintf(out, "%s\n", buf);
}

void error_msg_note(char *format, char *s) {
    char dest[512] = {};
    char buf[512] = {};
    sprintf(dest,format, s);
    imp_format_error_message(dest, buf, 512);
    fprintf(stderr, "%s\n", buf);
}