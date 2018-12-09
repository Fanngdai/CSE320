# CSE320
Programming assignments for programming and essential concepts of operating systems, compilers, concurrency, and performance analysis course.

Taken **Fall 2018** at **Stony Brook University** with Professor **Eugene Stark**..

* **Intro**
    * Intro to _linux_
    * Install _vmware_ or _virtual box_
* **MIPS Convertor**
  * command line utility to translate MIPS machine code between binary and human-readable mnemonic form
* **Audible**
  * Perform certain transformations on _audio files_ encoded in Sun audio (.au) format
  * Intro to C
  * Check for valid arguments passed from the terminal
  * Ensure that all _flags_ are valid and store their definitions accordingly
  * Write a program which _slows down_, _speeds up_, and _encodes_ the _audio file_
* **Snarf**
  * Functions like the curl command on the terminal
  * a simple HTTP client, which can contact an HTTP server over the Internet and retrieve a document
* **Grades**
  * Debugging and Fixing
  * Given an ancient code in C, fix it up so that it will run with no _seg fault_ nor _mem-leaks_ using _gdb_ and _valgrind_
  * Although _still reachable mem leaks_ are not considered
  * Ensure that _opt_long_ works correctly
  * Add an additional feature (the _-o flag_) for output file
* **sfmm**
  * Dynamic Allocator
  * Best-fit placement policy
  * One free list for each block size.  The collection of free lists will itself be organized as a "list of lists", maintained in increasing order of block size.
  * Immediate coalescing on free with adjacent free blocks.
  * Boundary tags with footer optimization that allows footers to be omitted
      from allocated blocks.
  * Block splitting without creating splinters.
  * Allocated blocks aligned to "double memory row" (16-byte) boundaries.
  * Free lists maintained using **last in first out (LIFO)** discipline.
* **Printer Spooler**
  * processes, signal handling, files, and I/O redirection
  * forking, executing, and reaping
  * Signal handling
  * Understand the use of "dup" to perform I/O redirection.
  * Implemented breadth first search
* **Xacto**
  * socket programming
  * Understand thread execution, locks, and semaphores
  * Have an advanced understanding of POSIX threads
  * Have some insight into the design of concurrent data structures
