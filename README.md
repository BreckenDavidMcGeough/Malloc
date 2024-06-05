# Dynamic Memory Allocator Implementation From Scratch In C

## A fully functional dynamic memory allocator written in native C from scratch

This project was from CSE220:Systems Programming and tested our knowledge and understanding of implicit free lists, C system calls, void pointers, and dynamic memory allocation. It includes the following functionality:

* Malloc chunks of memory of a specified size

* Calloc chunks of memory of a specified size

* Realloc chunks of memory of a specified size

* Free a chunk of memory which was manually allocated

## Overview of functionality

The program keeps track of allocated chunks of memory in the form of a linked list. It uses the sbrk() C command to request a page of memory to divide up and allocate. Stores chunks of memory using an implicit free list architecture. Will return a void pointer with the memory a user requests, rounded to the nearest chunk size with respect to the amount of memory they request. Links to Wiki pages for free lists and dynamic memory allocation are provided below for a deeper understanding.

* Free lists: [link
](https://en.wikipedia.org/wiki/Free_list)

* Dynamic memory allocation: [link
](https://en.wikipedia.org/wiki/C_dynamic_memory_allocation)




Dynamic Memory Allocator: Multi-Pool
===

You should have received a detailed handout for this project.  It
contains the project requirements and guidelines for implementing the
project.

Repository Layout
---

The `src/` directory contains the source code for your allocator, as
well as some helper code.  As given, it is a barely-working allocator
that uses `sbrk()` to directly allocate memory from the operating system
for every request.  It does not implement `realloc()`, so many
applications will not run.

You should implement your entire allocator in `src/mm.c`.  The bulk
allocator is also in `src/bulk.c`, but you should not modify the bulk
allocator for your implementation; the Autograder will test your
implementation using the given bulk allocator.

The `tests/` directory contains two simple tests; one of them tests the
bulk allocator (which, as stated above, you should not modify; this test
is just a sanity check to make sure the bulk allocator is working, and
to give you an example of a test), and the other tests to make sure that
`sbrk()` is not called between two requests for the same size
allocation.  Note that this second test _will_ pass with the given
allocator, but that the given allocator is **NOT** a correct allocator;
however, if _your_ allocator has bugs, it may well fail this test!
These two tests should help you see how to design and run your own
tests.

Building the Project
---

The default make target (built by invoking `make`) builds the _shared
object_ (library) file `libcsemalloc.so`.  This object contains your
allocator and can be used to run your standard Unix utilities using your
allocator.  Instructions for how to do this are in the Makefile.


Submission
---

Use `make submission` to build the file `malloc.tar`, which you will
upload to Autograder.
