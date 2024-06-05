#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


typedef struct Chunk{
  size_t header;
  struct Chunk* next;
} Chunk;

#define CHUNK_SIZE 4096

//these macros are for freelist initializing function
#define FLS 128 //freelistsize == block_index(104)
#define ISC 32
#define FLII 7 //index of leftover chunks of 128B from 128 B freelist allocation from sbrk(CHUNK_SIZE)

extern void *bulk_alloc(size_t size);

/*
 * This function is also defined in bulk.c, and it frees an allocation
 * created with bulk_alloc().  Note that the pointer passed to this
 * function MUST have been returned by bulk_alloc(), and the size MUST
 * be the same as the size passed to bulk_alloc() when that memory was
 * allocated.  Any other usage is likely to fail, and may crash your
 * program.
 */
extern void bulk_free(void *ptr, size_t size);


static Chunk** freelist;
static int checkfreeinit = 0;

static void* returnhead(int index);
int block_index(size_t x); //should block_index be a static function?
static void addchunk(Chunk* head, void* memory);
static void populateindex(Chunk* head, int numchunks);
static void initializefreelist();
void* malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);




static void* returnhead(int index){
  Chunk* head = freelist[index];
  if (head != NULL){
    Chunk* next = head->next;
    freelist[index] = next;
    //head->header = head->header | 0x0000000000000001; //after taking out of freelist and before giving to user, mark last bit of size_t header as 1 to indicate that it is an allocated chunk (1 == allocated, 0 == free by default)
    head->next = NULL;
    //void* returnmem = (void*)head;
    //return (returnmem+8); // return mem location 8 bytes more in order not to write over size_t header, then when accessing in free/realloc, head-8 is start of size_t header
    return head;
  }else{
    return NULL;
  }
}


int block_index(size_t x){
  if (x <= 8){
    return 5;
  }else{
    return 32 - __builtin_clz((unsigned int)x + 7);
  }
}

static void addchunk(Chunk* head,void* memory){
  Chunk* copy = head;
  for (int i = 0; copy->next != NULL; i++){
    copy = copy->next;
  }
  copy->next = (Chunk*)memory;
  copy->next->header = head->header;
  copy->next->next = NULL;


}

static void populateindex(Chunk* head, int numchunks){
  void* memory = (void*)head;
  size_t size = CHUNK_SIZE/numchunks;
  if (head->header == FLS){
    size = 128;
  }
  for (int i = 0; i < numchunks-1; i++){ //head counts as 1
    memory += size;
    addchunk(head,memory);
  }
}

static void initializefreelist(){
  if (freelist == NULL){
    void* rawmemory = sbrk(CHUNK_SIZE); //return address of start of chunk of memory fo size CHUNK_SIZE
    freelist = (Chunk**)rawmemory;
    for (int i = 0; i < 13; i++){
      freelist[i] = NULL; //initalize all indicies to NULL (0-4 will be unused as chunk size starts at 2^5)
    }
    void* startusablemem = rawmemory+128; //first 128B dedicated to freelist structure
    freelist[FLII] = (Chunk*)startusablemem;
    freelist[FLII]->header = (size_t)FLS;
    freelist[FLII]->next = NULL;
    populateindex(freelist[FLII], (CHUNK_SIZE-128)/128);
    //printf("freelist initialized with size: %d\n", validatefreelist(104));
  }
}

void* malloc(size_t size){
  //printf("%zu\n", size);
  //fprintf("THIS IS THE SIZE: %zu\n", size);
  void* requestedChunk = NULL;
  if (size == 0){
    return NULL;
  }
  if (checkfreeinit == 0){
    freelist = NULL;
    initializefreelist();
    checkfreeinit = 1;
  }
  int index = block_index(size);
  int roundedsize = (1<<index);
  if (size <= 4088){
    if (freelist[index] == NULL){
      void* rawmemory = sbrk(CHUNK_SIZE);
      freelist[index] = (Chunk*)rawmemory;
      freelist[index]->header = (size_t)roundedsize;
      freelist[index]->next = NULL;
      int numchunks = CHUNK_SIZE/roundedsize;
      populateindex(freelist[index],numchunks);
      requestedChunk = (returnhead(index)+8);
      
      // printf("freelist[%d] == NULL\n", index);
    }else if (freelist[index] != NULL){
      if (roundedsize == FLS){
	//printf("freelist[%d] != NULL and roundedsize == 128\n",index);
	
	requestedChunk = (returnhead(FLII)+8);
      }else{
	//printf("freelist[%d] != NULL and roundedsize != 128\n",index);
	
	requestedChunk = (returnhead(index)+8);
      }
    } 
  }else{
    //printf("true\n");
    return bulk_alloc((size_t)roundedsize);
  }
  
  return requestedChunk;
}


void *calloc(size_t nmemb, size_t size) {
    void *ptr = malloc(nmemb * size);
    memset(ptr, 0, nmemb * size);
    return ptr;
}

/*
 * You must also implement realloc().  It should create allocations
 * compatible with those created by malloc(), honoring the pool
 * alocation and bulk allocation rules.  It must move data from the
 * previously-allocated block to the newly-allocated block if it cannot
 * resize the given block directly.  See man 3 realloc for more
 * information on what this means.
 *
 * It is not possible to implement realloc() using bulk_alloc() without
 * additional metadata, so the given code is NOT a working
 * implementation!
 */
void *realloc(void *ptr, size_t size) {
  void* rlcmem = NULL;
  size_t sizemem = *((size_t*)(ptr-8));
  if (size == 0 || ptr == NULL){
    return NULL;
  }
  if (size <= sizemem-8){
    return ptr;
  }else if (size > sizemem){
    void* new = malloc(size);
    rlcmem = memcpy(new,ptr,sizemem);
    free(ptr);
  }
  return rlcmem;;
}

/*
 * You should implement a free() that can successfully free a region of
 * memory allocated by any of the above allocation routines, whether it
 * is a pool- or bulk-allocated region.
 *
 * The given implementation does nothing.
 */
void free(void *ptr) {
  if (ptr != NULL){
    void* startmem = (ptr-8);
    //*((size_t*)(startmem)) = *((size_t*)(startmem)) & 0x1111111111111110;
    size_t sizemem = *((size_t*)(startmem));
    int index = block_index(sizemem-8);
    addchunk(freelist[index],startmem); //find way to zero all memory could use calloc possibly?
  }
}




//I FORGOT TO USE BULK_FREE WHEN MEM IS > 4096 IN FREE, ALSO FORGOT TO CAST BULK MEM (MEM > 4088) TO CHUNK THEN ASSIGN HEADER THE MEM, THIS MIGHT FUCK ME OUT OF POINTS
