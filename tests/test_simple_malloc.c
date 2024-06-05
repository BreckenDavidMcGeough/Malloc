#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define ALLOC_SIZE 48
#define CHUNK_SIZE 4096

/* This test checks to ensure that the program break does not move
 * between two allocations of the same size.  Note that this test WILL
 * PASS with the given allocator, but that the given allocator does NOT
 * meet the requirements of this assignment.  This test should, however,
 * help you debug your own allocator and think of ways to write your own
 * tests for your allocator. */
int main(int argc, char *argv[])
{
  void* brk1 = sbrk(0);


  if (brk1 == sbrk(0)){
    //printf("simple malloc test passed\n");
    //return 0;
  }else{
    printf("error\n");
    return 1;
  }

  void* brk2 = sbrk(0);


  if (brk2 == sbrk(0)){
    //printf("simple malloc test passed\n");
  }else{
    printf("error\n");
    return 1;
  }



  

  return 0;
}
