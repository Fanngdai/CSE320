#include <stdio.h>

#include "sfmm.h"

#include <errno.h>
#include <signal.h>
#include "debug.h"
#include "sfmm.h"

int main(int argc, char const *argv[]) {
    sf_mem_init();

    // sf_malloc(PAGE_SZ << 2);
    // sf_malloc(sizeof(double));
    // sf_malloc(234);
    // sf_malloc(546);
    // sf_malloc(1);
    // sf_malloc(324);
    // sf_malloc(76);
    // sf_malloc(412);
    // sf_malloc(65);
    // sf_malloc(145);
    // sf_malloc(43);
    // sf_malloc(1992);
    // sf_malloc(4040);
    // sf_malloc(4088);

    // ERROR CHECKING
    // sf_malloc(-1);

    // double* ptr = sf_malloc(sizeof(double));

    // *ptr = 320320320e-320;

    // printf("%p\n", ptr);

    // sf_free(ptr);

    // sf_mem_fini();

    sf_malloc(3 * PAGE_SZ - sizeof(sf_prologue) - sizeof(sf_epilogue) - 16);
    sf_show_heap();

    return EXIT_SUCCESS;
}

