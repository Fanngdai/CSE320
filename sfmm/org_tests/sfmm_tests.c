#include <criterion/criterion.h>
#include <errno.h>
#include <signal.h>
#include "debug.h"
#include "sfmm.h"

#define MIN_BLOCK_SIZE (32)

sf_free_list_node *find_free_list_for_size(size_t size) {
    sf_free_list_node *fnp = sf_free_list_head.next;
    while(fnp != &sf_free_list_head && fnp->size < size)
	fnp = fnp->next;
    if(fnp == &sf_free_list_head || fnp->size != size)
	return NULL;
    return fnp;
}

int free_list_count(sf_header *ahp) {
    int count = 0;
    sf_header *hp = ahp->links.next;
    while(hp != ahp) {
	count++;
	hp = hp->links.next;
    }
    return count;
}

void assert_free_list_count(size_t size, int count) {
    sf_free_list_node *fnp = sf_free_list_head.next;
    while(fnp != &sf_free_list_head) {
	if(fnp->size == size)
	    break;
	fnp = fnp->next;
    }
    cr_assert(fnp != &sf_free_list_head && fnp->size == size,
	      "No free list of size %lu was found", size);
    int flc = free_list_count(&fnp->head);
    cr_assert_eq(flc, count,
		 "Wrong number of blocks in free list for size %lu (exp=%d, found=%d)",
		 size, flc);
}

void assert_free_block_count(int count) {
    int n = 0;
    sf_free_list_node *fnp = sf_free_list_head.next;
    while(fnp != &sf_free_list_head) {
		n += free_list_count(&fnp->head);
		fnp = fnp->next;
    }
    cr_assert_eq(n, count, "Wrong number of free blocks (exp=%d, found=%d)", count, n);
}

Test(sf_memsuite_student, malloc_an_Integer_check_freelist, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
	int *x = sf_malloc(sizeof(int));

	cr_assert_not_null(x, "x is NULL!");

	*x = 4;

	cr_assert(*x == 4, "sf_malloc failed to give proper space for an int!");

	assert_free_block_count(1);
	assert_free_list_count(PAGE_SZ - sizeof(sf_prologue) - sizeof(sf_epilogue) - MIN_BLOCK_SIZE, 1);

	cr_assert(sf_errno == 0, "sf_errno is not zero!");
	cr_assert(sf_mem_start() + PAGE_SZ == sf_mem_end(), "Allocated more than necessary!");
}

Test(sf_memsuite_student, malloc_three_pages, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
	void *x = sf_malloc(3 * PAGE_SZ - sizeof(sf_prologue) - sizeof(sf_epilogue) - MIN_BLOCK_SIZE);

	cr_assert_not_null(x, "x is NULL!");
	assert_free_block_count(0);
	cr_assert(sf_errno == 0, "sf_errno is not 0!");
}

Test(sf_memsuite_student, malloc_over_four_pages, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
	void *x = sf_malloc(PAGE_SZ << 2);

	cr_assert_null(x, "x is not NULL!");
	assert_free_block_count(1);
	cr_assert(sf_errno == ENOMEM, "sf_errno is not ENOMEM!");
}

Test(sf_memsuite_student, free_double_free, .init = sf_mem_init, .fini = sf_mem_fini, .signal = SIGABRT) {
	sf_errno = 0;
	void *x = sf_malloc(sizeof(int));
	sf_free(x);
	sf_free(x);
}

Test(sf_memsuite_student, free_no_coalesce, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
	/* void *x = */ sf_malloc(sizeof(long));
	void *y = sf_malloc(sizeof(double) * 10);
	/* void *z = */ sf_malloc(sizeof(char));

	sf_free(y);

	assert_free_block_count(2);
	assert_free_list_count(96, 1);
	assert_free_list_count(3888, 1);
	cr_assert(sf_errno == 0, "sf_errno is not zero!");
}

Test(sf_memsuite_student, free_coalesce, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
	/* void *w = */ sf_malloc(sizeof(long));
	void *x = sf_malloc(sizeof(double) * 11);
	void *y = sf_malloc(sizeof(char));
	/* void *z = */ sf_malloc(sizeof(int));

	sf_free(y);
	sf_free(x);

	assert_free_block_count(2);
	assert_free_list_count(128, 1);
	assert_free_list_count(3856, 1);
	cr_assert(sf_errno == 0, "sf_errno is not zero!");
}

Test(sf_memsuite_student, freelist, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *u = sf_malloc(1); //32
	/* void *v = */ sf_malloc(40); //48
	void *w = sf_malloc(152); //160
	/* void *x = */ sf_malloc(536); //544
	void *y = sf_malloc(1); // 32
	/* void *z = */ sf_malloc(2072); //2080

	sf_free(u);
	sf_free(w);
	sf_free(y);

	assert_free_block_count(4);
	assert_free_list_count(32, 2);
	assert_free_list_count(160, 1);
	assert_free_list_count(1152, 1);

	// First block in list should be the most recently freed block.
	sf_free_list_node *fnp = find_free_list_for_size(32);
	cr_assert_eq(fnp->head.links.next, (sf_header *)((char *)y - sizeof(sf_footer)),
		     "Wrong first block in free list (32): (found=%p, exp=%p)",
                     fnp->head.links.next, (sf_header *)((char *)y - sizeof(sf_footer)));
}

Test(sf_memsuite_student, realloc_larger_block, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(sizeof(int));
	/* void *y = */ sf_malloc(10);
	x = sf_realloc(x, sizeof(int) * 10);

	cr_assert_not_null(x, "x is NULL!");
	sf_header *hp = (sf_header *)((char *)x - sizeof(sf_footer));
	cr_assert(hp->info.allocated == 1, "Allocated bit is not set!");
	cr_assert(hp->info.block_size << 4 == 48, "Realloc'ed block size not what was expected!");

	assert_free_block_count(2);
	assert_free_list_count(32, 1);
	assert_free_list_count(3936, 1);
}

Test(sf_memsuite_student, realloc_smaller_block_splinter, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(sizeof(int) * 8);
	void *y = sf_realloc(x, sizeof(char));

	cr_assert_not_null(y, "y is NULL!");
	cr_assert(x == y, "Payload addresses are different!");

	sf_header *hp = (sf_header *)((char*)y - sizeof(sf_footer));
	cr_assert(hp->info.allocated == 1, "Allocated bit is not set!");
	cr_assert(hp->info.block_size << 4 == 48, "Block size not what was expected!");
	cr_assert(hp->info.requested_size == 1, "Requested size not what was expected!");

	// There should be only one free block of size 4000.
	assert_free_block_count(1);
	assert_free_list_count(4000, 1);
}

Test(sf_memsuite_student, realloc_smaller_block_free_block, .init = sf_mem_init, .fini = sf_mem_fini) {
	void *x = sf_malloc(sizeof(double) * 8);
	void *y = sf_realloc(x, sizeof(int));

	cr_assert_not_null(y, "y is NULL!");

	sf_header *hp = (sf_header *)((char*)y - sizeof(sf_footer));

	cr_assert(hp->info.allocated == 1, "Allocated bit is not set!");
	cr_assert(hp->info.block_size << 4 == 32, "Realloc'ed block size not what was expected!");
	cr_assert(hp->info.requested_size == 4, "Requested size not what was expected!");

	// After realloc'ing x, we can return a block of size 48 to the freelist.
	// This block will coalesce with the block of size 3968.
	assert_free_block_count(1);
	assert_free_list_count(4016, 1);
}

//############################################
//STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
//DO NOT DELETE THESE COMMENTS
//############################################

/*
 * Adds up to a perfect size 4048 which is 1 complete grow
 */
Test(sf_many_perfect_size, many_perfect_size, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
    sf_malloc(sizeof(double));
    sf_malloc(234);
    sf_malloc(546);
    sf_malloc(1);
    sf_malloc(324);
    sf_malloc(76);
    sf_malloc(412);
    sf_malloc(65);
    sf_malloc(145);
    sf_malloc(43);
    sf_malloc(1992);

	assert_free_block_count(0);
	assert_free_list_count(2000, 0);
	assert_free_list_count(2064, 0);
	assert_free_list_count(2224, 0);
	assert_free_list_count(2304, 0);
	assert_free_list_count(2736, 0);
	assert_free_list_count(2832, 0);
	assert_free_list_count(3168, 0);
	assert_free_list_count(3200, 0);
	assert_free_list_count(3760, 0);
	assert_free_list_count(4016, 0);
	assert_free_list_count(4048, 0);

	cr_assert(sf_errno == 0, "sf_errno is not zero!");
}

/*
 *	4048 is an edge case for malloc
 */
Test(sf_one_greater_than, one_greater_than, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;
    sf_malloc(4088);

	assert_free_block_count(1);
	assert_free_list_count(4048, 1);
	assert_free_list_count(8144, 0);

	cr_assert(sf_errno == 0, "sf_errno is not zero!");
}

/*
 * Makes so many small values that it will be forced to mem_grow
 */
Test(sf_many_small, many_small, .init = sf_mem_init, .fini = sf_mem_fini) {
	sf_errno = 0;

	for(int i = 0; i<1000; i++) {
		malloc(2);
		malloc(3);
		malloc(4);
		malloc(4);
		malloc(5);
	}

	cr_assert(sf_errno == 0, "sf_errno is not zero!");
}

/*
 * Returns NULL malloc
 */
Test(sf_malloc_NULL, malloc_NULL, .init = sf_mem_init, .fini = sf_mem_fini) {
	int *x = sf_malloc(0);
	cr_assert_null(x, "x is not NULL!");
}

Test(sf_realloc_NULL, realloc_NULL, .init = sf_mem_init, .fini = sf_mem_fini) {
	int *x = sf_realloc(NULL,0);
	cr_assert_null(x, "x is not NULL!");
}

Test(sf_free_NULL, free_NULL, .init = sf_mem_init, .fini = sf_mem_fini, .signal = SIGABRT) {
	sf_free(NULL);
}

Test(sf_many_malloc, many_malloc_with_abort, .init = sf_mem_init, .fini = sf_mem_fini, .signal = SIGABRT) {
	sf_errno = 0;
	void *a = sf_malloc(sizeof(char));
	void *b = sf_malloc(sizeof(int));
	void *c = sf_malloc(sizeof(double));
	void *d = sf_malloc(sizeof(long double));
	void *g = sf_malloc(555);
	sf_free(a);
	sf_free(b);
	sf_free(c);
	void *e = sf_malloc(sizeof(long int));
	void *f = sf_malloc(sizeof(unsigned int));
	sf_free(g);
	sf_free(d);
	sf_free(e);
	sf_free(f);
	sf_free(g);
}

// //Makes sure that the memory from new pointer is actually copied over to old pointer in a realloc call.
// Test(sf_realloc_mem_copy, realloc_mem_copy, .init = sf_mem_init, .fini = sf_mem_fini, .timeout = 5) {
//     int *i = (int *) sf_malloc(sizeof(int)); // 4 bytes -> block of ORDER 5 (32 bytes)
//     *i = 999;
//     i = sf_realloc(i, sizeof(int) * 10); // 40 bytes -> block of ORDER 6 (64 bytes)

//     cr_assert_eq(*i, 999, "integer not copied.");
//     sf_free(i);

//     char *s = (char *) sf_malloc(sizeof(char) * 5); // 7 bytes -> block of ORDER 5 (32 bytes)
//     s[0] = 't';
//     s[1] = 'e';
//     s[2] = 's';
//     s[3] = 't';
//     s[6] = '\0';
//     s = sf_realloc(s, sizeof(char) * 25); // 25 bytes -> block of ORDER 6 (64 bytes)

//     cr_assert_arr_eq(s, "test", 7, "string not copied.");
// }


// Test(bud_custom_suite, null_pointer_and_empty_rsize, .init = bud_mem_init, .fini = bud_mem_fini, .timeout = 5) {
//     int *ptr = bud_realloc(NULL, 0);

//     cr_assert_null(ptr);
//     expect_errno_value(22); // ERROR STATUS 22 = EINVAL
// }