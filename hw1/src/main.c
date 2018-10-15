#include <stdlib.h>

#include "hw1.h"
#include "debug.h"

#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv)
{
    if(!validargs(argc, argv)) {
        debug("EXIT_FAILURE on validargs");
        USAGE(*argv, EXIT_FAILURE);
    }

    debug("Options: 0x%lX", global_options);
    debug("EXIT_SUCCESS on validargs");

    if(global_options & (0x1L << 63)) {
        USAGE(*argv, EXIT_SUCCESS);
    }

    if(recode(argv))
        return EXIT_SUCCESS;

    return EXIT_FAILURE;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
