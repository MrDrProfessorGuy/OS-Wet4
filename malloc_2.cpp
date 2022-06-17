
#include "unistd.h"

#include "assert.h"

#define MAX_SIZE 100000000
// ############################### API ###############################
void* srealloc(void* oldp, size_t size);
void sfree(void* p);
void* scalloc(size_t num, size_t size);
void* smalloc(size_t size);
// ############################### Helper Functions ###############################
size_t _num_free_blocks();
size_t _num_free_bytes();
size_t _num_allocated_blocks();
size_t _num_allocated_bytes();
size_t _num_meta_data_bytes();
size_t _num_meta_data();
// ############################### BlockMetadata ###############################
BlockMetadata* findFreeBlock(size_t size);

typedef struct MallocMetadata {
    size_t size;
    bool is_free;
    MallocMetadata* next;
    MallocMetadata* prev;
}BlockMetadata;

static BlockMetadata head;
static BlockMetadata tail;

void init_list(){
    head.size = 0;
    head.is_free = false;
    head.next = tail;
    head.prev = NULL;
    
    tail.size = 0;
    tail.is_free = false;
    tail.next = NULL;
    tail.prev = head;
    
}

BlockMetadata* findFreeBlock(size_t size){
    BlockMetadata* iter = head.next;
    while(iter != &tail){
        if(iter->is_free && size <= iter->size){
            return iter;
        }
        iter = iter->next;
    }
    return NULL;
    
}

bool linkBlocks(BlockMetadata* first, BlockMetadata* second){
    first->next = second;
    second->prev = first;
}

BlockMetadata* initBlock(size_t size){
    BlockMetadata* allocated_block = (BlockMetadata*) sbrk((intptr_t) (size + sizeof(BlockMetadata)));
    if(allocated_block == (void*)-1){
        return NULL;
    }
    allocated_block->is_free = false;
    allocated_block->size = size;
    return allocated_block;
}

void* smalloc(size_t size){
    if(size == 0 || size > MAX_SIZE){
        return NULL;
    }
    BlockMetadata* new_block = findFreeBlock(size);
    if(new_block == NULL){
        BlockMetadata* allocated_block = initBlock(size);
        if(allocated_block == NULL{
            return NULL;
        }
        
        if(new_block == head){
            assert(tail.prev == NULL);
            linkBlocks(&head, (BlockMetadata*)allocated_block);
            linkBlocks((BlockMetadata*)allocated_block, &tail);
        }
        else{
            linkBlocks((tail.prev), (BlockMetadata*)allocated_block);
            linkBlocks((BlockMetadata*)allocated_block, &tail);
        }
    }
    
}


void* scalloc(size_t num, size_t size){

}


void sfree(void* p){

}


void* srealloc(void* oldp, size_t size){

}

size_t _num_free_blocks(){

}

size_t _num_free_bytes(){

}

size_t _num_allocated_blocks(){

}

size_t _num_allocated_bytes(){

}

size_t _num_meta_data_bytes(){

}

size_t _size_meta_data(){

}



