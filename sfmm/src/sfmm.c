/**
 * All functions you make for the assignment must be implemented in this file.
 * Do not submit your assignment with a main function in this file.
 * If you submit with a main function in this file, you will get a zero.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "debug.h"
#include "sfmm.h"

/*
 * Ensures that the size given is a divisble of 16. If not, increase it to the next integer
 * which is divisible by 16.
 *
 * @param size The number of bytes requested to be allocated
 */
size_t padding(size_t size) {
    size_t temp_size = size+8;
    // size + header must always be greater than 32
    if(temp_size<32)
        return 32;
    else if(temp_size%16==0)
        return temp_size;
    else
        return temp_size+(16-(temp_size%16));
}

/*
 * This function is called when there is no memory allocated in the heap. This will make room
 * for 4048 heap size.
 *
 * @return 0 if not valid (not successful)
 *         1 if successful
 */
int init_mem() {
    // extend heap by one page and returns pointer to the new page
    if(sf_mem_grow() == NULL) {
        return 0;
    }

    // PROLOGUE
    sf_prologue *prologue = sf_mem_start();
    memset(prologue,0,sizeof(sf_prologue));

    // PROLOGUE HEADER
    sf_header *prologue_header = sf_mem_start();
    memset(prologue_header,0,sizeof(*prologue_header));
    prologue_header->info.allocated = 0x1;
    // prologue_header->info.requested_size = sizeof(*prologue);
    // prologue_header->info.block_size = padding(sizeof(*prologue)) >> 4;
    prologue->header = *prologue_header;

    // PROLOGUE FOOTER
    sf_footer *prologue_footer = sf_mem_start()+sizeof(*prologue)-8;
    memset(prologue_footer,0,sizeof(*prologue_footer));
    prologue_footer->info.allocated = 0x1;
    // prologue_footer->info.requested_size = sizeof(*prologue);
    // prologue_footer->info.block_size= padding(sizeof(*prologue)) >> 4;
    prologue->footer = *prologue_footer;

    // EPILOGUE
    sf_epilogue *epilogue = sf_mem_end()-8;
    memset(epilogue,0,sizeof(sf_epilogue));

    // EPILOGUE FOOTER
    sf_footer *epilogue_footer = sf_mem_end()-8;
    memset(epilogue_footer,0,sizeof(*epilogue_footer));
    epilogue_footer->info.allocated = 0x1;
    // epilogue_footer->info.prev_allocated = 0x1;
    // epilogue_footer->info.requested_size = sizeof(*epilogue);
    // epilogue_footer->info.block_size = padding(sizeof(*epilogue)) >> 4;
    epilogue->footer = *epilogue_footer;

    size_t free_block_size = PAGE_SZ-sizeof(*prologue)-sizeof(epilogue);

    // FREE BLOCK HEADER
    sf_header *free_block_header = sf_mem_start()+sizeof(*prologue);
    memset(free_block_header,0,sizeof(*free_block_header));
    free_block_header->info.block_size = free_block_size >> 4;

    // FREE BLOCK FOOTER
    sf_footer *free_block_footer = sf_mem_end()-sizeof(epilogue)-sizeof(sf_footer);
    memset(free_block_footer,0,sizeof(*free_block_footer));
    free_block_footer->info.block_size = free_block_size >> 4;

    // FREE LIST
    sf_free_list_node *free_list_node = sf_add_free_list(free_block_size, sf_free_list_head.next);
    if(free_list_node==NULL) {
        return 0;
    }
    free_list_node->head.info.block_size = free_block_size >> 4;

    // Circular Doubly Linked List
    free_list_node->head.links.next = free_block_header;
    free_list_node->head.links.prev = free_block_header;
    free_block_header->links.prev = &free_list_node->head;
    free_block_header->links.next = &free_list_node->head;

    return 1;
}

int exact_size(size_t size, size_t padd_size) {
    // Extend until size is appropriate or error
    while(size<padd_size){
        sf_header *temp_header;
        if((temp_header = sf_mem_grow()) == NULL) {
            sf_errno = ENOMEM;
            return -1;
        }
        size += sf_mem_end() - (void *)temp_header;
    }
    return size;
}

/*
 * This is your implementation of sf_malloc. It acquires uninitialized memory that
 * is aligned and padded properly for the underlying system.
 *
 * @param size The number of bytes requested to be allocated.
 *
 * @return If size is 0, then NULL is returned without setting sf_errno.
 * If size is nonzero, then if the allocation is successful a pointer to a valid region of
 * memory of the requested size is returned.
 * If the allocation is not successful, then NULL is returned and sf_errno is set to ENOMEM.
 */
void *sf_malloc(size_t size) {
    // If size is 0,
    if(size == 0) {
        return NULL;
    }

    // Initialize the size of the heap if not made already. If error with init, error
    if(sf_mem_start()==sf_mem_end() && !init_mem()) {
        sf_errno = ENOMEM;
        return NULL;
    }

    sf_free_list_node *cursor_free_list = sf_free_list_head.next;
    sf_free_list_node *successor = NULL;
    size_t padd_size = padding(size);

    // SIZE MATCH. Checks to make sure you are not back at the head which is size 0
    while(cursor_free_list->size != 0) {
        // Exact size is found
        if(cursor_free_list->size == padd_size) {
            sf_header *temp_node = &cursor_free_list->head;

            // No free block available. Head points to itself
            if(temp_node->links.next==temp_node && temp_node==temp_node->links.prev) {
                temp_node->links.next = temp_node;
                temp_node->links.prev = temp_node;
                cursor_free_list = cursor_free_list->next;
                continue;       // because looking for successor as well
            }

            // WARNING Check to make sure when you add a value before epilogue, it will set epilogue prev_alloc to true
            // try malloc32 malloc64 free32 malloc64. Make sure epilogue is correct

            // Modify this value only if there is a prev value not only head. Change next value prev_alloc to true
            if(temp_node->links.next != temp_node->links.prev) {
                temp_node->links.next->links.next->info.prev_allocated = 0x1;
                // ((sf_footer *)(&temp_node->links.next->links.next)+(temp_node->links.next->info.block_size<<4))->info.prev_allocated = 0x1;
            } else {        // at the end. go to epilogue
                ((sf_footer *)(sf_mem_end()-sizeof(sf_footer)))->info.prev_allocated = 0x1;
            }

            // Take the first available (free) node
            temp_node = cursor_free_list->head.links.next;

            temp_node->info.requested_size = size;
            temp_node->info.block_size = padd_size>>4;
            temp_node->info.allocated = 0x1;

            // Remove this free block
            temp_node->links.prev->links.next = temp_node->links.next;
            temp_node->links.next->links.prev = temp_node->links.prev;

            return (void *)temp_node+8;
        }

        // BEST FIT. Find the next largest value. Will not be used if exact size is found.
        else if(cursor_free_list->size > padd_size) {
            sf_header *temp_node = &cursor_free_list->head;
            // Make sure there is a free block. header points to itself
            if(temp_node->links.next==temp_node && temp_node==temp_node->links.prev) {
                // If no free block found, do nothing
            } else if(successor==NULL) {
                successor = cursor_free_list;
            } else if(successor->size > cursor_free_list->size){
                successor = cursor_free_list;
            }
        }

        // next pointer
        cursor_free_list = cursor_free_list->next;
    }


    // NO SIZE AVAILABLE - need to extend heap
    if(successor==NULL) {
        sf_footer *old_epilogue = sf_mem_end() - 8;
        sf_footer *prev_footer = sf_mem_end() - 16;
        sf_header *new_header;
        // new size of block
        size_t new_size;

        // Prev is allocated
        if(old_epilogue->info.prev_allocated) {
            //set the header to the previous bc it is free
            new_header = sf_mem_end() - (prev_footer->info.block_size<<4) - 8;
            new_size = ((sf_footer *)(sf_mem_end()-16))->info.block_size<<4;
            if((new_size = exact_size(new_size, padd_size)) == -1){
                return NULL;
            }
            // Remove this free block
            new_header->links.prev->links.next = new_header->links.next;
            new_header->links.next->links.prev = new_header->links.prev;
        } else {
            // the prev was not free
            new_header = sf_mem_end();
            if((new_size = exact_size(0, padd_size)) == -1){
                return NULL;
            }
        }

        // header
        memset(new_header,0,sizeof(*new_header));
        new_header->info.requested_size = size;
        new_header->info.block_size = new_size>>4;
        new_header->info.allocated = 0x1;
        new_header->info.prev_allocated = 0x1;

        // footer before the epilogue
        ((sf_footer *)(sf_mem_end()-16))->info.block_size = new_size>>4;
        ((sf_footer *)(sf_mem_end()-16))->info.allocated = 0x1;
        ((sf_footer *)(sf_mem_end()-16))->info.prev_allocated = 0x1;
        ((sf_footer *)(sf_mem_end()-16))->info.requested_size = size;


        // new epilgoue
        memset(sf_mem_end()-8,0,sizeof(sf_epilogue));
        ((sf_epilogue *)(sf_mem_end()-8))->footer.info.allocated = 0x1;

        if(new_size==padd_size) {
            return new_header+8;
        } else {
            // Put the remainder back in the linked list
            sf_free_list_node cursor = *sf_free_list_head.next;
            int flag = 1;

            while(cursor.size!=0) {
                if(cursor.size == new_size) {
                    new_header->links.next = cursor.head.links.next;
                    new_header->links.prev = &cursor.head;
                    cursor.head.links.next->links.prev = new_header;
                    cursor.head.links.next = new_header;
                    // bc we will be breaking it up if neccessary
                    successor = &cursor;
                    flag = 0;
                    break;
                }
                cursor = *cursor.next;
            }

            if(flag) {
                sf_free_list_node cursor = *sf_free_list_head.next;
                successor = sf_free_list_head.prev;
                while(cursor.size != 0) {
                    if(cursor.size > new_size) {
                        if(successor==sf_free_list_head.prev) {
                            successor = cursor.prev;
                            break;
                        }
                    }
                    // next pointer
                    cursor = *cursor.next;
                }

                // There was no size list found.
                successor = sf_add_free_list(new_size, successor->next);
                if(successor==NULL) {
                    sf_errno = ENOMEM;
                    return NULL;
                }

                successor->head.info.block_size = new_size >> 4;
                ((sf_footer *)(sf_mem_end()-16))->info.block_size = new_size>>4;

                successor->head.links.next = new_header;
                successor->head.links.prev = new_header;
                new_header->links.prev = &successor->head;
                new_header->links.next = &successor->head;
            }
        }
    }


    // BEST FIT. SIZE GREATER THAN IS AVAILABLE
    sf_header *temp_node = successor->head.links.next;

    // Remove this ENTIRE free block so temp_node+split_node
    temp_node->links.prev->links.next = temp_node->links.next;
    temp_node->links.next->links.prev = temp_node->links.prev;

    // remainder
    sf_header *split_node_header = (void *)temp_node + padd_size;
    memset(split_node_header,0,sizeof(*split_node_header));
    size_t split_node_size = ((temp_node->info.block_size<<4)-padd_size) >> 4;

    split_node_header->info.block_size = split_node_size;
    split_node_header->info.prev_allocated = 0x1;

    sf_footer *split_node_footer = (void *)temp_node+(temp_node->info.block_size<<4)-8;
    split_node_footer->info.block_size = split_node_size;
    split_node_footer->info.prev_allocated = 0x1;

    // Modifying the new space we got
    temp_node->info.block_size = padd_size>>4;
    temp_node->info.requested_size = size;
    temp_node->info.allocated = 0x1;

    // if previous is prologue, the prev_alloc should be 1
    if(sf_mem_start()+sizeof(sf_prologue)==temp_node) {
        temp_node->info.prev_allocated = 0x1;
    }

    // Put the remainder back in the linked list
    sf_free_list_node *cursor = sf_free_list_head.next;
    size_t free_size = (successor->size)-padd_size;
    cursor = sf_free_list_head.next;
        while(cursor->size!=0) {
        // Value is larger than. The insertion should be between this and another
        if(cursor->size == free_size) {
            split_node_header->links.next = cursor->head.links.next;
            split_node_header->links.prev = &cursor->head;
            cursor->head.links.next->links.prev = split_node_header;
            cursor->head.links.next = split_node_header;
            return (void *)temp_node+8;
        }
        cursor = cursor->next;
    }

    successor = sf_free_list_head.prev;
    while(cursor->size != 0) {
        if(cursor->size > free_size) {
            if(successor==sf_free_list_head.prev) {
                successor = cursor->prev;
                break;
            }
        }
        // next pointer
        cursor = cursor->next;
    }

    // There was no size list found.
    sf_free_list_node *free_list_node = sf_add_free_list(free_size, successor->next);
    if(free_list_node==NULL) {
        sf_errno = ENOMEM;
        return NULL;
    }
    free_list_node->head.links.next = split_node_header;
    free_list_node->head.links.prev = split_node_header;
    split_node_header->links.prev = &free_list_node->head;
    split_node_header->links.next = &free_list_node->head;

    free_list_node->head.info.block_size = free_size >> 4;

    while(cursor->size!=0) {
        // Value is larger than. The insertion should be between this and another
        if(cursor->size == free_size) {
            split_node_header->links.next = cursor->head.links.next;
            split_node_header->links.prev = &cursor->head;
            cursor->head.links.next->links.prev = split_node_header;
            cursor->head.links.next = split_node_header;
            return (void *)temp_node+8;
        }
        cursor = cursor->next;
    }

    return (void *)temp_node+8;
}

/*
 * Checks to ensure that the ptr are valid
 */
void valid_ptr(void *ptr) {
    sf_header *header = (void *)ptr-8;
    if(ptr == NULL) {
        abort();
    } else if(sf_mem_start()+sizeof(sf_prologue) > (void *)header) {
        abort();
    } else if(sf_mem_end()+sizeof(sf_epilogue) < (void *)header) {
        abort();
    } else if(header->info.allocated == 0x0) {
        abort();
    } else if(header->info.block_size<<4 < 32) {
        abort();
    } else if(header->info.requested_size+8 > header->info.block_size<<4) {
        abort();
    } else if(header->info.prev_allocated == 0){
        sf_footer *a = (((void *)header)-sizeof(sf_footer));
        if(a->info.allocated!=0) {
            abort();
        }
    }
}

void put_into_free(sf_header *header) {
    sf_header *head = (void *)header - 8;

    sf_header *head_of_next = ((void *)header-8) + (head->info.block_size << 4);
    sf_footer *foot = (void *)head_of_next-8;
    sf_footer *foot_of_next;
    if(head_of_next->info.block_size<<4) {
        foot_of_next = ((void *)head_of_next) + (head_of_next->info.block_size << 4)-8;
    } else {
        foot_of_next = sf_mem_end()-8;
    }

    head->info.requested_size = 0;
    head->info.allocated = 0;

    foot->info.requested_size = head->info.requested_size;
    foot->info.block_size = head->info.block_size;
    foot->info.two_zeroes = 0x0;
    foot->info.prev_allocated = head->info.prev_allocated;
    foot->info.allocated = head->info.allocated;

    // change the next list pa to 0
    head_of_next->info.prev_allocated = 0x0;
    foot_of_next->info.prev_allocated = 0x0;

    // put into free
    sf_free_list_node *cursor = sf_free_list_head.next;
    sf_free_list_node *free_list_node;
    while(1) {
        if(cursor->size == head->info.block_size<<4) {
            head->links.prev = &cursor->head;
            head->links.next = cursor->head.links.prev;
            cursor->head.links.next->links.prev = head;
            cursor->head.links.next = head;
            return;
        } else if(cursor->size > head->info.block_size<<4) {
            free_list_node = sf_add_free_list(head->info.block_size<<4, cursor);
            if(free_list_node==NULL) {
                abort();
            }
            break;
        } else if(cursor->size == 0) {
            // value was not found a greater size is not found. Put into end of list
            free_list_node = sf_add_free_list(head->info.block_size<<4, sf_free_list_head.next->prev);
            if(free_list_node==NULL) {
                abort();
            }
            break;
        }
        cursor = cursor->next;
    }

    // add into free
    head->links.next = free_list_node->head.links.next;
    head->links.prev = &free_list_node->head;
    free_list_node->head.links.next->links.prev = head;
    free_list_node->head.links.next = head;
}
/*
 *
 */
void coalesce(sf_header *top, sf_header *bottom, sf_footer *new_footer) {
    // Check to make sure bottom prev_alloc = 1
    // Check to make sure both alloc are 0
    // if(bottom->info.prev_allocated == 0) {
    //     abort();
    // } else if(top->info.allocated || bottom->info.allocated) {
    //     abort();
    // }

    size_t new_size = (top->info.block_size<<4) + (bottom->info.block_size<<4);

    // Take out of free linked list
    top->links.prev->links.next = top->links.next;
    top->links.next->links.prev = top->links.prev;
    bottom->links.prev->links.next = bottom->links.next;
    bottom->links.next->links.prev = bottom->links.prev;

    // Make new block first
    top->info.requested_size = 0x0;
    top->info.block_size = new_size >> 4;
    top->info.two_zeroes = 0x0;

    new_footer->info.requested_size = 0x0;
    new_footer->info.block_size = new_size >> 4;
    new_footer->info.two_zeroes = 0x0;
    new_footer->info.prev_allocated = top->info.prev_allocated;
    new_footer->info.allocated = 0x0;

    sf_footer *abc = new_footer+1;
    abc->info.prev_allocated = 0;

    // add into linked list of sizes
    sf_free_list_node *cursor = sf_free_list_head.next;
    while(1) {
        if(cursor->size == new_size) {
            top->links.prev = &cursor->head;
            top->links.next = cursor->head.links.prev;
            cursor->head.links.next->links.prev = top;
            cursor->head.links.next = top;
            return;
        } else if(cursor->size > new_size) {
            sf_free_list_node *free_list_node = sf_add_free_list(new_size, cursor);
            if(free_list_node==NULL) {
                abort();
            }
            top->links.prev = &free_list_node->head;
            top->links.next = free_list_node->head.links.prev;
            free_list_node->head.links.next->links.prev = top;
            free_list_node->head.links.next = top;
            return;
        } else if(cursor->size == 0) {
            sf_free_list_node *free_list_node = sf_add_free_list(new_size, sf_free_list_head.next);
            if(free_list_node==NULL) {
                abort();
            }
            top->links.prev = &free_list_node->head;
            top->links.next = free_list_node->head.links.prev;
            free_list_node->head.links.next->links.prev = top;
            free_list_node->head.links.next = top;
            return;
        }
        cursor = cursor->next;
    }
}

/*
 * Marks a dynamically allocated region as no longer in use.
 * Adds the newly freed block to the free list.
 *
 * @param ptr Address of memory returned by the function sf_malloc.
 *
 * If ptr is invalid, the function calls abort() to exit the program.
 */
void sf_free(void *ptr) {
    valid_ptr(ptr);

    // Setting up the values
    sf_header *header = (sf_header *)ptr;
    put_into_free(header);

    sf_header *head = (void *)header - 8;
    sf_footer *foot_of_prev = (((void *)header)-8);
    sf_header *head_of_prev;
    if(foot_of_prev->info.block_size<<4) {
        head_of_prev = ((void *)foot_of_prev) - (foot_of_prev->info.block_size << 4);
    } else {
        head_of_prev = sf_mem_start();
    }
    sf_header *head_of_next = ((void *)header-8) + (head->info.block_size << 4);
    sf_footer *foot = (void *)head_of_next-8;
    sf_footer *foot_of_next;
    if(head_of_next->info.block_size<<4) {
        foot_of_next = ((void *)head_of_next) + (head_of_next->info.block_size << 4)-8;
    } else {
        foot_of_next = sf_mem_end()-8;
    }

    // if prev was allocated
    if(header->info.prev_allocated && head_of_prev != sf_mem_start()){
        coalesce(head_of_prev, head, foot);
    }
    // if next is free
    if(!head_of_next->info.allocated && foot_of_next != sf_mem_end()-8){
        coalesce(head, head_of_next, foot_of_next);
    }
    return;
}
int valid_ptr_realloc(void *ptr) {
    sf_header *header = (void *)ptr-8;
    if(ptr == NULL) {
        sf_errno = EINVAL;
        return 0;
    } else if(sf_mem_start()+sizeof(sf_prologue) > (void *)header) {
        sf_errno = EINVAL;
        return 0;
    } else if(sf_mem_end()+sizeof(sf_epilogue) < (void *)header) {
        sf_errno = EINVAL;
        return 0;
    } else if(header->info.allocated == 0x0) {
        sf_errno = EINVAL;
        return 0;
    } else if(header->info.block_size<<4 < 32) {
        sf_errno = EINVAL;
        return 0;
    } else if(header->info.requested_size+8 > header->info.block_size<<4) {
        abort();
    } else if(header->info.prev_allocated == 0){
        sf_footer *a = (((void *)header)-sizeof(sf_footer));
        if(a->info.allocated!=0) {
            sf_errno = EINVAL;
        return 0;
        }
    }
    return 1;
}

/*
 * Resizes the memory pointed to by ptr to size bytes.
 *
 * @param ptr Address of the memory region to resize.
 * @param size The minimum size to resize the memory to.
 *
 * @return If successful, the pointer to a valid region of memory is
 * returned, else NULL is returned and sf_errno is set appropriately.
 *
 *   If sf_realloc is called with an invalid pointer sf_errno should be set to EINVAL.
 *   If there is no memory available sf_realloc should set sf_errno to ENOMEM.
 *
 * If sf_realloc is called with a valid pointer and a size of 0 it should free
 * the allocated block and return NULL without setting sf_errno.
 */
void* sf_realloc(void *ptr, size_t size) {
    if(!valid_ptr_realloc(ptr)) {
        return NULL;
    }

    // Setting up the values
    sf_header *header = (sf_header *)ptr;
    sf_header *head = (void *)header - 8;

    int old_size = head->info.block_size<<4;
    int padd_size = padding(size);
    int remaining_size = old_size-padd_size;

    if(padd_size > head->info.block_size<<4) {
        // call malloc
        int *new_block = malloc(size);
        // memcpy
        memcpy(new_block, ptr, old_size);
        // free
        free(ptr);
        // return malloc ptr
        return new_block+8;
    } else if(padd_size < head->info.block_size<<4){
        // else {        // split the block
            // break up the block
            sf_header *remainder_head = ((void *)header-8) + padd_size;
            remainder_head->info.requested_size = 0x0;
            remainder_head->info.block_size = remaining_size>>4;
            remainder_head->info.two_zeroes = 0x0;
            remainder_head->info.prev_allocated = 0x1;
            remainder_head->info.allocated = 0x0;

            sf_footer *remainder_foot = ((void *)header-8) + old_size - 8;
            remainder_foot->info.requested_size = 0x0;
            remainder_foot->info.block_size = remaining_size>>4;
            remainder_foot->info.two_zeroes = 0x0;
            remainder_foot->info.prev_allocated = 0x1;
            remainder_foot->info.allocated = 0x0;

            head->info.requested_size = size;
            head->info.block_size = padd_size>>4;
            head->info.allocated = 0x1;

            sf_footer *foot = ((void *)header-8) + (padd_size - 8);
            foot->info.requested_size = size;
            foot->info.block_size = padd_size>>4;
            foot->info.two_zeroes = 0x0;
            foot->info.prev_allocated = 0x1;
            foot->info.allocated = 0x1;

            sf_header *next_block = (void *)remainder_foot + 8;
            next_block->info.prev_allocated = 0x0;

            sf_footer *feet = (void *)next_block + (next_block->info.block_size<<4) - 8;
            feet->info.prev_allocated = 0x0;

            // link
            sf_free_list_node *cursor = sf_free_list_head.next;
            while(1) {
                if(cursor->size == remaining_size) {
                    remainder_head->links.prev = &cursor->head;
                    remainder_head->links.next = cursor->head.links.prev;
                    cursor->head.links.next->links.prev = remainder_head;
                    cursor->head.links.next = remainder_head;
                    break;
                } else if(cursor->size > remaining_size) {
                    sf_free_list_node *free_list_node = sf_add_free_list(remaining_size, cursor);
                    if(free_list_node==NULL) {
                        abort();
                    }
                    remainder_head->links.next = free_list_node->head.links.next;
                    remainder_head->links.prev = &free_list_node->head;
                    free_list_node->head.links.next->links.prev = remainder_head;
                    free_list_node->head.links.next = remainder_head;
                    break;
                } else if(cursor->size==0){
                    sf_free_list_node *free_list_node = sf_add_free_list(remaining_size, sf_free_list_head.next);
                    if(free_list_node==NULL) {
                        abort();
                    }
                    remainder_head->links.next = free_list_node->head.links.next;
                    remainder_head->links.prev = &free_list_node->head;
                    free_list_node->head.links.next->links.prev = remainder_head;
                    free_list_node->head.links.next = remainder_head;
                    break;
                }
                cursor = cursor->next;
            }

            if(!next_block->info.allocated && (next_block->info.block_size<<4)!=0){
                coalesce(remainder_head, next_block, feet);
            }
        // }
            // the remainder is not greater than size no splitting is needed
    }
    else if(remaining_size<32) {
        head->info.requested_size = size;
    }
    return ptr ;
}
