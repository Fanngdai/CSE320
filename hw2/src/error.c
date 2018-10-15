/*
 * Error handling routines
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>                     // used for va_arg

#include "error.h"

int errors;
int warnings;
int dbflag = 1;

void fatal(char* fmt, ...) {
    fprintf(stderr, "\nFatal error: ");

    // char *s = 0;
    // va_list arg;
    // va_start(arg, fmt);
    // while((s = va_arg(arg, char *)) != NULL)
    //     fprintf(stderr, "%s",s);
    // va_end(arg);

    va_list arg;
    va_start(arg, fmt);
    // if you vfprintf va_list it will print the entire list
    vfprintf(stderr, fmt, arg);
    va_arg(arg, char*);
    va_end(arg);

    fprintf(stderr, "\n");
    exit(1);
}

void error(const char* fmt, ...) {
    fprintf(stderr, "\nError: ");

    va_list arg;
    va_start(arg, fmt);
    // if you vfprintf va_list it will print the entire list
    vfprintf(stderr, fmt, arg);
    va_arg(arg, char*);
    va_end(arg);

    fprintf(stderr, "\n");
    errors++;
}

void warning(const char* fmt, ...) {
    fprintf(stderr, "\nWarning: ");

    va_list arg;
    va_start(arg, fmt);
    // if you vfprintf va_list it will print the entire list
    vfprintf(stderr, fmt, arg);
    va_arg(arg, char*);
    va_end(arg);

    fprintf(stderr, "\n");
    warnings++;
}

void debug(const char* fmt, ...) {
    if(!dbflag)
        return;
    fprintf(stderr, "\nDebug: ");

    va_list arg;
    va_start(arg, fmt);
    // if you vfprintf va_list it will print the entire list
    vfprintf(stderr, fmt, arg);
    va_arg(arg, char*);
    va_end(arg);

    fprintf(stderr, "\n");
}
