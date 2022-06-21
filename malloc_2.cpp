
#include "unistd.h"
#include "string.h"

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
typedef struct MallocMetadata {
    size_t size;
    bool is_free;
    MallocMetadata* next;
    MallocMetadata* prev;
}BlockMetadata;

BlockMetadata* findFreeBlock(size_t size);

typedef struct Stats {
    size_t free_blocks;
    size_t free_bytes;
    size_t allocated_blocks;
    size_t allocated_bytes;
    const size_t meta_data;
}Stats;

static Stats stats = {.free_blocks = 0, .free_bytes = 0, .allocated_blocks = 0, .allocated_bytes = 0, .meta_data = sizeof(BlockMetadata)};


struct a {
    BlockMetadata head;
    BlockMetadata tail;
};
static struct a list = {.head = {.size = 0, .is_free = false, .next = &(list.tail), .prev = NULL},
                        .tail = {.size = 0, .is_free = false, .next = NULL, .prev = &(list.head)}};



BlockMetadata* findFreeBlock(size_t size){
    BlockMetadata* iter = list.head.next;
    while(iter != &list.tail){
        if(iter->is_free && size <= iter->size){
            return iter;
        }
        iter = iter->next;
    }
    return NULL;
    
}

void linkBlocks(BlockMetadata* first, BlockMetadata* second){
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
    
    stats.allocated_blocks++;
    stats.allocated_bytes+= size;
    return allocated_block;
}

void* smalloc(size_t size){
    if(size == 0 || size > MAX_SIZE){
        return NULL;
    }
    BlockMetadata* new_block = findFreeBlock(size);
    if(new_block == NULL){
        new_block = initBlock(size);
        if(new_block == NULL){
            return NULL;
        }
        
        if(list.head.next == &list.tail){ //Empty List
            //assert(tail.prev == NULL);
            linkBlocks(&list.head, new_block);
            linkBlocks(new_block, &list.tail);
        }
        else{ //Insert Last
            linkBlocks((list.tail.prev), new_block);
            linkBlocks(new_block, &list.tail);
        }
    }
    else{
        new_block->is_free = false;
        
        stats.free_blocks--;
        stats.free_bytes-= new_block->size;
    }
    
    return (new_block + 1);
}


void* scalloc(size_t num, size_t size){
    void* res = smalloc(num * size);
    
    if(res == NULL){
        return NULL;
    }
    memset(res, 0, num * size);
    return res;
}


void sfree(void* p){
    if (p == NULL) {
        return;
    }
    
    BlockMetadata* block_meta_data = (BlockMetadata*)p - 1;
    if (block_meta_data->is_free){
        return;
    }
    block_meta_data->is_free = true;
    stats.free_blocks++;
    stats.free_bytes+= block_meta_data->size;
}


void* srealloc(void* oldp, size_t size){
    if(oldp == NULL){
        void* res = smalloc(size);
        if(res == NULL){
            return NULL;
        }
        
        return res;
    }
    BlockMetadata* block_meta_data = (BlockMetadata*)oldp - 1;
    if(block_meta_data->size < size){
        void* res = smalloc(size);
        if(res == NULL){
            return NULL;
        }
    
        memmove(res, oldp, block_meta_data->size);
        sfree(oldp);
        return res;
    }
    return oldp;
}

size_t _num_free_blocks(){
    return stats.free_blocks;
}

size_t _num_free_bytes(){
    return stats.free_bytes;
}

size_t _num_allocated_blocks(){
    return stats.allocated_blocks;
}

size_t _num_allocated_bytes(){
    return stats.allocated_bytes;
}

size_t _size_meta_data(){
    return stats.meta_data;
}

size_t _num_meta_data_bytes(){
    return _size_meta_data() * _num_allocated_blocks();
}



