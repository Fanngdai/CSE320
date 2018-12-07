# Homework 3 Dynamic Memory Allocator - CSE 320 - Fall 2018
#### Professor Eugene Stark

### **Due Date: Friday 10/26/2018 @ 11:59pm**


We **HIGHLY** suggest that you read this entire document, the book chapter,
and examine the base code prior to beginning. If you do not read the entire
document before beginning, you may find yourself doing extra work.

> :scream: Start early so that you have an adequate amount of time to test
your program!

> :scream: The functions `malloc`, `free`, `realloc`, `memalign`, `calloc`,
> etc., are **NOT ALLOWED** in your implementation. If any of these functions,
> or any other function with similar functionality is found in your program,
> you **will receive a <span style="color:red;">ZERO</span>**.

**NOTE:** In this document, we refer to a word as 2 bytes (16 bits) and a memory
row as 4 words (64 bits). We consider a page of memory to be 4096 bytes (4 KB)

# Introduction

You must read **Chapter 9.9 Dynamic Memory Allocation Page 839** before
starting this assignment. This chapter contains all the theoretical
information needed to complete this assignment. Since the textbook has
sufficient information about the different design strategies and
implementation details of an allocator, this document will not cover this
information. Instead, it will refer you to the necessary sections and pages in
the textbook.

## Takeaways

After completing this assignment, you will have a better understanding of:
* The inner workings of a dynamic memory allocator
* Memory padding and alignment
* Structs and linked lists in C
* [errno](https://linux.die.net/man/3/errno) numbers in C
* Unit testing in C

# Overview

You will create a segregated free list allocator for the x86-64 architecture
with the following features:

- Best-fit placement policy.
- One free list for each block size.  The collection of free lists will
    itself be organized as a "list of lists", maintained in increasing order
	of block size.
- Immediate coalescing on free with adjacent free blocks.
- Boundary tags with footer optimization that allows footers to be omitted
    from allocated blocks.
- Block splitting without creating splinters.
- Allocated blocks aligned to "double memory row" (16-byte) boundaries.
- Free lists maintained using **last in first out (LIFO)** discipline.

You will implement your own versions of the **malloc**, **realloc**, and
**free** functions.

You will use existing Criterion unit tests and write your own to help debug
your implementation.

## Segregated Free List Management Policy

You **MUST** use a **segregated free list** as described in **Chapter 9.9.14
Page 863** to manage free blocks.  Your segregated list will consist of a
"list of free lists", one for each size block that has ever been created.
This list of free lists will be maintained in increasing order of block size.
This will permit you to implement a best fit placement policy, by scanning
the free lists in increasing order of block size until a large enough free block
has been found.
Within each free list, the blocks are to be maintained using a LIFO (last-in, first-out)
discipline.  This means that each time a block is allocated, it is taken from
the front of the appropriate free list, and each time a block is freed, it is added
to the front of the appropriate free list.
If a block is to be freed, but there is currently no free list for that size block,
a new free list is to be created and inserted into the "list of free lists"
at the proper position.

## Splitting Blocks & Splinters

Your allocator must split blocks at allocation time to reduce the amount of
internal fragmentation.  Details about this feature can be found in
**Chapter 9.9.8 Page 849**.
Note that the minimum useful block size is 32 bytes.  No "splinters" of smaller
size than this are ever to be created.  If splitting a block to be allocated would
result in a splinter, then the block should not be split; rather, the block should
be used as-is to satisfy the allocation request (*i.e.*, you will "over-allocate"
by issuing a block slightly larger than that required).

> :thinking: Why is the minimum block size 32?  As you read more details about
> the format of a block header, block footer, and alignment requirements,
> think about how these constrain the minimum block size.

## Block Placement Policy

When allocating memory, use **best fit placement** (discussed in **Chapter 9.9.7 Page 849**).
In brief, you should search the list of free lists in increasing order of block size
o find the first nonempty free list for a block size that is large enough to accommodate
the request, taking into account the additional space required to store the block header,
footer, and free list links.  You should then use the first block in this free list,
splitting it if it is too large and doing so would not create a splinter.  If the block is
split, the remainder should be inserted at the beginning of the correct free list for
its size.

If there is no nonempty free list for a block size large enough to satisfy
the allocation request, then `sf_mem_grow` should be called to extend the heap
by an additional page of memory.  After coalescing this page with any free block
that immediately precedes it, you should attempt to use the resulting block of memory
to satisfy the allocation request; splitting it as usual if it is too large and
no splinter would result.  If the block of memory is still not large enough,
another call to `sf_mem_grow` should be made; continuing to grow the heap until
either a large enough block is obtained or the return value from `sf_mem_grow`
indicates that there is no more memory.

## Immediate Coalescing

Your implementation must perform **immediate coalescing** upon freeing a block.
This essentially amounts to the procedure described in **Chapter 9.9.10 Page 850**.
Coalescing must **only** be done when `sf_free` is called, for some cases of
calls to `sf_realloc` described below, and when `sf_mem_grow` is called.

## Block Headers & Footers

In **Chapter 9.9.6 Page 847 Figure 9.35**, the header is defined as 2 words (32
bits) to hold the block size and allocated bit. In this assignment, the header
will be 4 words (i.e. 64 bits or 1 memory row). The header fields will be similar
to those in the textbook but you will keep an extra field for recording whether
or not the previous block is allocated (*i.e.* for the the optimization that permits
footers to be omitted from allocated blocks).

**Block Header Format (allocated block):**
<pre>
+--------------------------------------------+------------------+-------+-------+---------+ &lt;- header
|              requested_size                |   block_size     |       | prev  |  alloc  |
|                                            |                  |  00   | alloc |    1    |
|                 32 bits                    |     28 bits      |       | 1 bit |   bit   |
+--------------------------------------------+------------------+-------+-------+---------+ &lt;- (double-row
                                                                                                aligned)
</pre>

**Block Header Format (free block):**
<pre>
+--------------------------------------------+------------------+-------+-------+---------+
|                 unused                     |   block_size     |       | prev  |  alloc  |
|                                            |                  |  00   | alloc |    1    |
|                 32 bits                    |     28 bits      |       | 1 bit |   bit   |
+--------------------------------------------+------------------+-------+-------+---------+ &lt;- (double-row
|                                                                                         |     aligned)
|                                Pointer to next free block                               |
|                                                                                         |
+--------------------------------------------+------------------+-------+-------+---------+
|                                                                                         |
|                               Pointer to previous free block                            |
|                                                                                         |
+--------------------------------------------+------------------+-------+-------+---------+
</pre>

**Block Footer Format (free block only):**
<pre>
+--------------------------------------------+------------------+-------+-------+---------+
|              requested_size                |   block_size     |       |       |  alloc  |
|                                            |                  |  00   |   0   |    1    |
|                 32 bits                    |     28 bits      |       |       |   bit   |
+--------------------------------------------+------------------+-------+-------+---------+
</pre>

- The `block_size` field is 28 bits (but really 32 bits - see the note below).
It gives the number of bytes for the **entire** block (including header/footer,
payload, and padding)
- The `requested_size` field is 32 bits. It is the number of bytes that the
client requested.
- The `alloc` bit is a boolean. It is 1 if the block is allocated and 0 if it
is free.
- The `prev_alloc` bit is also a boolean. It is 1 if the **immediately preceding** block
in the heap is allocated and 0 if it is not.

> :thinking: The block size field represents a 32-bit block size, but requires
> only 28 bits to do so because the four low-order bits of a block size are always
> zero due to the alignment requirement.  The definition of the C structure `sf_block_info`
> (see below) defines the `block_size` field to be a "bit field" of 28 bits in width.
> If you extract the value of this field using the normal way of referencing a field
> in a structure, the value you obtain should be regarded as the block size shifted
> right by four bits.  To obtain the actual block size, you need to shift this value
> left by four bits.  Similarly, when storing a 32-bit block size into the `block_size`
> field, you first need to shift it right by four bits to obtain a 28-bit value.

> :thinking:  Here is an example of determining the block size required to satisfy
> a particular requested payload size.  Suppose the requested size is 25 bytes.
> An additional 8 bytes will be required to store the block header, which must always
> be present.  That means a block of at least 33 bytes must be used, however due to
> alignment requirements this has to be rounded up to the next multiple of 16,
> which is 48.  As a result, there will be 15 bytes of "padding" at the end of
> the payload area, which contributes to internal fragmentation.
> Besides the header, when the block is freed, it will also be necessary
> to store a footer, as well and next and previous links for the freelist.
> These will take an additional 24 bytes of space, however when the block is free there
> is no payload so the payload area can be used to store this information, assuming that
> the payload area is big enough in the first place.  But the payload area is 40 bytes
> (25 bytes plus 15 bytes of padding), which is certainly bigger than 24 bytes,
> so a block of total size 48 will be fine.
> Note that if a block is smaller than 32 bytes, there would not be enough space to
> store the header, footer, and freelist links when the block is free.
> This is why the minimum block size is 32 bytes.

# Getting Started

**Remember to use the `--strategy-option theirs` flag with the `git merge`
command as described in the `hw1` doc to avoid merge conflicts in the Gitlab
CI file.**

Fetch and merge the base code for `hw3` as described in `hw0` from the
following link: https://gitlab02.cs.stonybrook.edu/cse320/hw3

## Directory Structure

<pre>
.
├── .gitlab-ci.yml
└── hw3
    ├── include
    │   ├── debug.h
    │   └── sfmm.h
    ├── lib
    │   └── sfutil.o
    ├── Makefile
    ├── src
    │   ├── main.c
    │   └── sfmm.c
    └── tests
        └── sfmm_tests.c
</pre>

The `lib` folder contains the object file for the `sfutil` library. This
library provides you with several functions to aid you with the implementation
of your allocator. <span style="color:red">**Do NOT delete this file as it
is an essential part of your homework assignment.**</span>

The provided `Makefile` creates object files from the `.c` files in the `src`
directory, places the object files inside the `build` directory, and then links
the object files together, including `lib/sfutil.o`, to make executables that
are stored to the `bin` directory.

**Note:** `make clean` will not delete `sfutil.o` or the `lib` folder, but it
will delete all other contained `.o` files.

The `sfmm.h` header file contains function prototypes and defines the format
of the various data structures that you are to use.

> **DO NOT modify `sfmm.h` or the Makefile.** Both will be replaced when we run
> tests for grading. If you wish to add things to a header file, please create
> a new header file in the `include` folder

All functions for your allocator (`sf_malloc`, `sf_realloc`, and `sf_free`)
**must be** implemented in `src/sfmm.c`.

The program in `src/main.c` contains a basic example of using the initialization
and allocation functions together. Running `make` will create a `sfmm`
executable in the `bin` directory. This can be run using the command `bin/sfmm`.

> Any functions other than `sf_malloc`, `sf_free`, and `sf_realloc` **WILL NOT**
> be graded.

# Allocation Functions

You will implement the following three functions in the file `src/sfmm.c`.
The file `include/sfmm.h` contains the prototypes and documentation found here.

Standard C library functions set `errno` when there is an error. To avoid
conflicts with these functions, your allocation functions will set `sf_errno`, a
variable declared as `extern` in `sfmm.h`.

```c
/*
 * This is your implementation of sf_malloc. It acquires uninitialized memory that
 * is aligned and padded properly for the underlying system.
 *
 * @param size The number of bytes requested to be allocated.
 *
 * @return If size is 0, then NULL is returned without setting sf_errno.
 * If size is nonzero, then if the allocation is successful a pointer to a valid region of
 * memory of the requested size is returned.  If the allocation is not successful, then
 * NULL is returned and sf_errno is set to ENOMEM.
 */
void *sf_malloc(size_t size);

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
void* sf_realloc(void *ptr, size_t size);

/*
 * Marks a dynamically allocated region as no longer in use.
 * Adds the newly freed block to the free list.
 *
 * @param ptr Address of memory returned by the function sf_malloc.
 *
 * If ptr is invalid, the function calls abort() to exit the program.
 */
void sf_free(void *ptr);
```

> :scream: <span style="color:red;">Make sure these functions have these exact names
> and arguments. They must also appear in the correct file. If you do not name
> the functions correctly with the correct arguments, your program will not
> compile when we test it. **YOU WILL GET A ZERO**</span>

# Initialization Functions

In the `build` directory, we have provided you with the `sfutil.o` object file.
When linked with your program, this object file allows you to access the
`sfutil` library, which contains the following functions:

This library contains the following functions:

```c
/*
 * Any program using the sfmm library must call this function ONCE
 * before issuing any allocation requests. This function DOES NOT
 * allocate any space to your allocator.
 */
void sf_mem_init();

/*
 * Any program using the sfmm library must call this function ONCE
 * after all allocation requests are complete. If implemented cleanly,
 * your program should have no memory leaks in valgrind after this function
 * is called.
 */
void sf_mem_fini();

/*
 * This function increases the size of your heap by adding one page of
 * memory to the end.
 *
 * @return On success, this function returns a pointer to the start of the
 * additional page, which is the same as the value that would have been returned
 * by sf_mem_heap_end() before the size increase.  On error, NULL is returned
 * and sf_errno is set to ENOMEM.
 */
void *sf_mem_grow();

/* The size of a page of memory returned by sf_mem_grow(). */
#define PAGE_SZ 4096

/*
 * @return The starting address of the heap for your allocator.
 */
void *sf_mem_start();

/*
 * @return The ending address of the heap for your allocator.
 */
void *sf_mem_end();
```

The function `sf_mem_init` **MUST** be used to initialize memory.  It is to be
called once **BEFORE** using any of the other functions from `sfutil.o`.
The function `sf_mem_grow` is to be invoked by `sf_malloc`, at the time of the
first allocation request to set up the heap prologue and epilogue and obtain
an initial free block, and on subsequent allocations when a large enough block
to satisfy the request is not found.

> :scream: As these functions are provided in a pre-built .o file, the source
> is not available to you. You will not be able to debug these using gdb.
> You must treat them as black boxes.

# sf_mem_grow

For this assignment, your implementation **MUST ONLY** use `sf_mem_grow` to
extend the heap.  **DO NOT** use any system calls such as **brk** or **sbrk**
to do this.

Function `sf_mem_grow` returns memory to your allocator in pages.
Each page is 4096 bytes (4 KB) and there are a limited, small number of pages
available (the actual number may vary, so do not hard-code any particular limit
into your program).  Each call to `sf_mem_grow` extends the heap by one page and
returns a pointer to the new page (this will be the same pointer as would have
been obtained from `sf_mem_end` before the call to `sf_mem_grow`.

The `sf_mem_grow` function also keeps track of the starting and ending addresses
of the heap for you. You can get these addresses through the `sf_mem_start` and
`sf_mem_end` functions.

> :smile: A real allocator would typically use the **brk**/**sbrk** system calls
> calls for small memory allocations and the **mmap**/**munmap** system calls
> for large allocations.  To allow your program to use other functions provided by
> glibc, which rely on glibc's allocator (*i.e.* `malloc`), we have provided
> `sf_mem_grow` as a safe wrapper around **sbrk**.  This makes it so your heap and
> the one managed by glibc do not interfere with each other.

# Implementation Details

## Memory Row Size

The table below lists the sizes of data types (following Intel standard terminlogy)
on x86-64 Linux Mint:

| C declaration | Data type | x86-64 Size (Bytes) |
| :--------------: | :----------------: | :----------------------: |
| char  | Byte | 1 |
| short | Word | 2 |
| int   | Double word | 4 |
| long int | Quadword | 8 |
| unsigned long | Quadword | 8 |
| pointer | Quadword | 8 |
| float | Single precision | 4 |
| double | Double precision | 8 |
| long double | Extended precision | 16

> :nerd: You can find these sizes yourself using the sizeof operator. For
example, `printf("%lu\n", sizeof(int))` prints 4

In this assignment we will assume that each "memory row" is 8 bytes (64 bits) in size.
All pointers returned by `sf_malloc` are to be "double memory row" aligned;
that is, they will be addresses that are multiples of 16.
This will permit such pointers to be used to store values of the largest data type
(`long double`) supported by the hardware.

## Free List Heads

In the file `include/sfmm.h`, you will see the following declarations:

```c
typedef struct sf_free_list_node {
    size_t size;
    sf_header head;             /* Head of list of free nodes of same size. */
    struct sf_free_list_node *next;
    struct sf_free_list_node *prev;
} sf_free_list_node;

sf_free_list_node sf_free_list_head;
```

The variable `sf_free_list_head` is the head of the "list of free lists",
which contains a `sf_free_list_node` for each size block that has ever been
inserted.  Each `sf_free_list_node` contains a `head` field, which gives
access to a list of headers of free blocks, and a `size` field, which
records the size of the blocks in that particular list (all blocks in
a given list have the same size).  The nodes in the "list of free lists"
are maintained in increasing order of the value of their size field.

The "list of free lists" is maintained as a **circular, doubly linked list**.
Each node in the list contains a `next` pointer that points to the next
node in the list, and a `prev` pointer that points the previous node.
The variable `sf_free_list_head` is a dummy, "sentinel" node, which is
used to connect the beginning and the end of the list.  This node is always
present and (aside from its `next` and `free` pointers) does **not** contain
any other data.  If the list is empty, then the fields `sf_freelist_head.next`
and `sf_freelist_head.prev` both contain `&sf_freelist_head`
(*i.e.* the sentinel node points back to itself).  If the list is nonempty,
then `sf_freelist_head.next` points to the first node in the list and
`sf_freelist_head.prev` points to the last node in the list.
Inserting into and deleting from a circular doubly linked list is done
in the usual way, except that, owing to the use of the sentinel, there
are no edge cases for inserting or removing at the beginning or the end
of the list.
If you need a further introduction to this data structure, you can readily
find information on it by googling ("circular doubly linked lists with sentinel").

As stated above the `head` field of each `sf_free_list_node` serves as
the head of a list that consists of the headers of free blocks of a single size.
These free lists are also maintained as circular, doubly linked lists,
with the `head` field of `sf_free_list_node` as the sentinel.
Insertion into and deletion from these individual free lists is entirely
analogous to the same operations on the "list of free lists".
For our purposes, all insertions and deletions into an individual free list
will be done at the beginning of the list (resulting in a LIFO discipline).
So inserting into a list would set the `next` field of the sentinel to be
the inserted node, and removing from a list would likewise modify the
`next` field of the sentinel.

> :scream:  You **MUST** use the `sf_free_list_head` variable as the head
> of your "list of free lists" and you **MUST** maintain this as a
> circular, doubly linked list.  You **MUST** also maintain the individual
> free lists as circular, doubly linked lists and you must access them
> using a LIFO discipline.
> The helper functions discussed later, as well as the unit tests,
> will assume that you have done this when accessing your free lists.

The `sf_mem_init` function in the `sfutil.o` library takes care of initializing
the `sf_free_list_head` sentinel with the proper self-referential links, so you
don't have to worry about that.
The `sfutil.o` library also contains a function `sf_add_free_list`, which you
should use when you need to create a new free list for blocks of a specified size.
The `sf_add_free_list` function handles the tasks of allocating an `sf_free_list_node`,
initializing the `head` field as an empty list, and inserting the node before
a specified existing node in the "list of free lists".  A pointer to the newly added
node is returned.  Although the mechanics of adding a new node the the list of
free lists is handled for you, you are responsible for determining the proper
position to add the new node, so as to maintain the list in increasing order of
block size.

## Block Header & Footer Fields

The various header and footer formats are specified in `include/sfmm.h`:

```c
                                      Format of an allocated memory block
    +-----------------------------------------------------------------------------------------+
    |                                       64-bits wide                                      |
    +-----------------------------------------------------------------------------------------+

    +--------------------------------------------+------------------+-------+-------+---------+ <- header
    |              requested_size                |   block_size     |       | prev  |  alloc  |
    |                                            |                  |  00   | alloc |    1    |
    |                 32 bits                    |     28 bits      |       | 1 bit |   bit   |
    +--------------------------------------------+------------------+-------+-------+---------+ <- (double-row
    |                                                                                         |     aligned)
    |                                   Payload and Padding                                   |
    |                                     (N Memory Rows)                                     |
    |                                                                                         |
    |                                                                                         |
    +--------------------------------------------+------------------+-------+-------+---------+



                                      Format of a free memory block
    +--------------------------------------------+------------------+-------+-------+---------+ <- header
    |                 unused                     |   block_size     |       | prev  |  alloc  |
    |                                            |                  |  00   | alloc |    1    |
    |                 32 bits                    |     28 bits      |       | 1 bit |   bit   |
    +--------------------------------------------+------------------+-------+-------+---------+ <- (double-row
    |                                                                                         |     aligned)
    |                                Pointer to next free block                               |
    |                                                                                         |
    +--------------------------------------------+------------------+-------+-------+---------+
    |                                                                                         |
    |                               Pointer to previous free block                            |
    |                                                                                         |
    +--------------------------------------------+------------------+-------+-------+---------+
    |                                                                                         | 
    |                                         Unused                                          | 
    |                                     (N Memory Rows)                                     |
    |                                                                                         |
    |                                                                                         |
    +--------------------------------------------+------------------+-------+-------+---------+ <- footer
    |                 unused                     |   block_size     |       | prev  |  alloc  |
    |                                            |                  |  00   | alloc |    1    |
    |                 32 bits                    |     28 bits      |       | 1 bit |   bit   |
    +--------------------------------------------+------------------+-------+-------+---------+
```

The `sfmm.h` header file contains C structure definitions corresponding to the above diagrams:

```c
/* Struct for the common part of block header and block footer. */
typedef struct {
    unsigned      allocated :  1;
    unsigned prev_allocated :  1;
    unsigned     two_zeroes :  2;
    unsigned     block_size : 28;  // Note: value is size>>4
    unsigned requested_size : 32;
} __attribute__((packed)) sf_block_info;
```

> :smile: The  `__attribute__((__packed__))` annotation tells gcc to leave out
> all padding between members of the struct. In this way, the fields are forcibly
> placed next to each other.

```c
/* Struct for a block header */
typedef struct sf_header {
    sf_block_info info;
    /*
     * A free block has pointers to the next and previous free block of the same size.
     * An allocated block has a payload, and since the next and previous pointers are
     * only needed for a free block, the payload can use this space when the block is
     * allocated.  We use a union to reflect this idea.
     */
    union {
        uint64_t payload;  /* First word of payload (aligned). */
        struct {
            struct sf_header *next;
            struct sf_header *prev;
        } links;           /* Pointers to next and previous free blocks. */
    };
} sf_header;

/* Struct for a block footer (footers are only present in free blocks) */
typedef struct {
    sf_block_info info;
} sf_footer;
```

> :thumbsup:  You can use casts to convert a generic pointer value to one
> of type `sf_header` or `sf_footer`, in order to make use of the above
> structure definitions to easily access the various fields.

## Overall Structure of the Heap: Prologue and Epilogue

Your heap should use a prologue and epilogue (as described in the book) to
arrange for the proper block alignment and to avoid edge cases when coalescing blocks.
The overall organization of the heap is as shown below:

```c
                                         Format of the heap
    +-----------------------------------------------------------------------------------------+
    |                                       64-bits wide                                      |
    +-----------------------------------------------------------------------------------------+

                                                                                                   heap start
    +-----------------------------------------------------------------------------------------+ <- (aligned)
    |                                                                                         |
    |                                            0                                            | padding
    |                                         64 bits                                         |
    +-----------------------------------------------------------------------------------------+
    |                                            |                  |       |       |         |
    |                    0                       |        0         |  00   |   0   |    1    | prologue 
    |                 32 bits                    |     28 bits      |       |       |         | header
    +--------------------------------------------+------------------+-------+-------+---------+ <- (aligned)
    |                                            |                  |       |       |         |
    |                    0                       |        0         |  00   |   0   |    1    | prologue
    |                 32 bits                    |     28 bits      |       |       |         | footer
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                                                                                         |
    |                                                                                         |
    |                                                                                         |
    |                                 Allocated and free blocks                               |
    |                                                                                         |
    |                                                                                         |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                            |                  |       | prev  |         |
    |                    0                       |        0         |  00   | alloc |    1    | epilogue
    |                 32 bits                    |     28 bits      |       | 1 bit |         |
    +-----------------------------------------------------------------------------------------+ <- heap end
                                                                                                   (aligned)
```

The "prologue" consists of padding (to achieve the desired alignment)
and an allocated block with just a header and a footer and no payload area.
The "epilogue" consists only of an allocated footer.
The prologue and epilogue are never freed.
When the heap is extended, a new epilogue is created at the end of the
newly added region and the old epilogue becomes the header of the new block.
This is as described in the book.

As your heap is initially empty, at the time of the first call to `sf_malloc`
you will need to make one call to `sf_mem_grow` to obtain a page of memory
within which to set up the prologue and initial epilogue.
The remainder of the memory in this first page should then be inserted into
the free list as a single block.

## Notes on sf_malloc

When implementing your `sf_malloc` function, first determine if the request size
is 0.  If so, then return `NULL` without setting `sf_errno`.
If the request size is non-zero, then you should determine the size of the
block to be allocated by adding the header size and the size of any necessary
padding to reach a size that is a multiple of 16 to maintain proper alignment.
Remember also that the block has to be big enough to store the footer and
`next` and `prev` pointers when it is free, though as these fields are not
present in an allocated block this space can (and should) be overlapped with
the payload area.  These constraints lead to a minimum block size of 32 bytes,
so you should not attempt to allocate any block smaller than this.
After having determined the required block size, you should search the free lists
to obtain the first block on the first freelist whose blocks are at least
that big.  If there is no such block, then you must use `sf_mem_grow` to
request more memory.  (For requests larger than a page, more than one such
call might be required).  If your allocator ultimately cannot satisfy the
request, your `sf_malloc` function must set `sf_errno` to `ENOMEM` and
return `NULL`.

### Notes on sf_mem_grow

After each call to `sf_mem_grow`, you must attempt to coalesce the newly
allocated page with any free block immediately preceding it, in order to build
blocks larger than one page.  After coalescing, add the entire new block to the
**beginning** of the correct list.

**Note:** Do not coalesce with the prologue or past the beginning of the heap.

### Notes on sf_free

When implementing `sf_free`, you must first verify that the pointer being
passed to your function belongs to an allocated block. This can be done by
examining the fields in the block header and footer.  In this assignment,
we will consider the following cases for invalid pointers:

- The pointer is `NULL`.
- The header of the block is before the end of the prologue, or after the
  beginning of the epilogue.
- The `allocated` bit in the header or footer is 0
- The `block_size` field is not a multiple of 16 or is less than the
  minimum block size of 32 bytes.
  ***NOTE: It is always a multiple of 16***
- The `requested_size` field, plus the size required for the block header,
is greater than the `block_size` field.
- If the `prev_alloc` field is 0, indicating that the previous block is free,
  then the `alloc` fields of the previous block header and footer should also be 0.

If an invalid pointer is passed to your function, you must call `abort` to exit
the program.  Use the man page for the `abort` function to learn more about this.

After confirming that a valid pointer was given, you must free the block.
First, determine if it is possible to coalesce the block with either one or
both of the adjacent blocks.  If it can, remove any blocks to be coalesced
from their free lists and combine the blocks.  Then, insert the newly freed
block into the proper free list.

**Note that the only other times you should coalesce a block is after calling
`sf_mem_grow` or in some cases of `sf_realloc`**

### Notes on sf_realloc

When implementing your `sf_realloc` function, you must first verify that the
pointer and size parameters passed to your function are valid. The criteria for
pointer validity are the same as those described in the 'Notes on sf_free'
section above.  If the pointer is valid but the size parameter is 0,
free the block and return `NULL`.

After verifying both parameters, consider the cases described below.
Note that in some cases, `sf_realloc` is more complicated than calling `sf_malloc`
to allocate more memory, `memcpy` to move the old memory to the new memory, and
`sf_free` to free the old memory.

## Reallocating to a Larger Size

When reallocating to a larger size, always follow these three steps:

1. Call `sf_malloc` to obtain a larger block.

2. Call `memcpy` to copy the data in the block given by the client to the block
returned by `sf_malloc`.

3. Call `sf_free` on the block given by the client (coalescing if necessary).

4. Return the block given to you by `sf_malloc` to the client.

If `sf_malloc` returns `NULL`, `sf_realloc` must also return `NULL`. Note that
you do not need to set `sf_errno` in `sf_realloc` because `sf_malloc` should
take care of this.

## Reallocating to a Smaller Size

When reallocating to a smaller size, your allocator must use the block that was
passed by the caller.  You must attempt to split the returned block. There are
two cases for splitting:

- Splitting the returned block results in a splinter. In this case, do not
split the block. Leave the splinter in the block, update the header field
if necessary, and return the same block back to the caller.

**Example:**

<pre>
            b                                        b
+----------------------+                       +------------------------+
| allocated            |                       |   allocated.           |
| Blocksize: 64 bytes  |   sf_realloc(b, 32)   |   Block size: 64 bytes |
| payload: 48 bytes    |                       |   payload: 32 bytes    |
|                      |                       |                        |
|                      |                       |                        |
+----------------------+                       +------------------------+
</pre>

In the example above, splitting the block would have caused a 16-byte splinter.
Therefore, the block is not split. The `requested_size` field in the footer
is set to 32.

- The block can be split without creating a splinter. In this case, split the
block and update the block size fields in both headers.  Free the remaining block
(i.e. coalesce if possible and insert the block into the head of the correct
free list).  Return a pointer to the payload of the smaller block to the caller.

Note that in both of these sub-cases, you return a pointer to the same block
that was given to you.

**Example:**

<pre>
            b                                        b
+----------------------+                       +------------------------+
| allocated            |                       | allocated |  free      |
| Blocksize: 64 bytes  |   sf_realloc(b, 16)   | 32 bytes  |  32 bytes. |
| payload: 48 bytes    |                       | payload:  |            |
|                      |                       | 16 bytes  | goes into  |
|                      |                       |           | free list  |
+----------------------+                       +------------------------+
</pre>

# Helper Functions

The `sfutil` library additionally contains the following helper functions,
which should be self explanatory.  They all output to `stderr`.

```c
void sf_show_block_info(sf_block_info *ip);
void sf_show_blocks();
void sf_show_free_list();
void sf_show_heap();
```

We have provided these functions to help you visualize your free lists and
allocated blocks. We have also provided you with additional unit tests which
will check certain properties of each free block when a snapshot is being
performed and the snapshot verbose value is set to **true**.

# Things to Remember

- Make sure that memory returned to the client is aligned and padded correctly for
the system we use (64-bit Linux Mint).
- We will not grade using Valgrind. However, you are encouraged to use it to
detect alignment errors.

# Unit Testing

For this assignment, we will use Criterion to test your allocator. We have
provided a basic set of test cases and you will have to write your own as well.

You will use the Criterion framework alongside the provided helper functions to
ensure your allocator works exactly as specified.

In the `tests/sfmm_tests.c` file, there are ten unit test examples. These tests
check for the correctness of `sf_malloc`, `sf_realloc`, and `sf_free`.
We provide some basic assertions, but by no means are they exhaustive.  It is your
job to ensure that your header/footer bits are set correctly and that blocks are
allocated/freed as specified.

## Compiling and Running Tests

When you compile your program with `make`, a `sfmm_tests` executable will be
created in the `bin` folder alongside the `main` executable. This can be run
with `bin/sfmm_tests`. To obtain more information about each test run, you can
use the verbose print option: `bin/sfmm_tests --verbose=0`.

## Writing Criterion Tests

The first test `malloc_an_Integer` tests `sf_malloc`. It allocates space for an
integer and assigns a value to that space. It then runs an assertion to make
sure that the space returned by `sf_malloc` was properly assigned.

```c
cr_assert(*x == 4, "sf_malloc failed to give proper space for an int!");
```

The string after the assertion only gets printed to the screen if the assertion
failed (i.e. `*x != 4`). However, if there is a problem before the assertion,
such as a SEGFAULT, the unit test will print the error to the screen and
continue to run the rest of the unit tests.

For this assignment **<span style="color:red;">you must write 5 additional unit tests
which test new functionality and add them to `sfmm_tests.c` below the following
comment:</span>**

```
//############################################
//STUDENT UNIT TESTS SHOULD BE WRITTEN BELOW
//DO NOT DELETE THESE COMMENTS
//############################################
```

> For additional information on Criterion library, take a look at the official
> documentation located [here](http://criterion.readthedocs.io/en/master/)! This
> documentation is VERY GOOD.

# Hand-in instructions
Make sure your directory tree looks like this and that your homework compiles.

<pre>
.
├── .gitlab-ci.yml
└── hw3
    ├── include
    │   ├── debug.h
    │   └── sfmm.h
    ├── lib
    │   └── sfutil.o
    ├── Makefile
    ├── src
    │   ├── main.c
    │   └── sfmm.c
    └── tests
        └── sfmm_tests.c
</pre>

This homework's tag is: `hw3`

<pre>
$ git submit hw3
</pre>

> :nerd: This program will be very difficult to get working unless you are
> extremely disciplined about your coding style.  Think carefully about how
> to modularize your code in a way that makes it easier to understand and
> avoid mistakes.  Verbose, repetitive code is error-prone and **evil!**
> When writing your program try to comment as much as possible.
> Format the code consistently.  It is much easier for your TA and the
> professor to help you if we can quickly figure out what your code does.
