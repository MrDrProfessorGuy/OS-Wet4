
#include "unistd.h"
#include "string.h"

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

typedef struct MallocMetadata {
    size_t size;
    bool is_free;
    MallocMetadata* next;
    MallocMetadata* prev;
    MallocMetadata* next_free;
    MallocMetadata* prev_free;
}BlockMetadata;

#define METADATA_SIZE sizeof(BlockMetadata)

BlockMetadata* findFreeBlock(size_t size);
void FreeListInsertBlock(BlockMetadata* free_block);

typedef struct Stats {
    size_t free_blocks;
    size_t free_bytes;
    size_t allocated_blocks;
    size_t allocated_bytes;
    const size_t meta_data;
}Stats;

static Stats stats = {.free_blocks = 0, .free_bytes = 0, .allocated_blocks = 0, .allocated_bytes = 0, .meta_data = sizeof(BlockMetadata)};

void Stats_update(size_t freeBlocks=0, size_t freeBytes=0, size_t allocatedBlocks=0, size_t allocatrdBytes=0){
    stats.free_blocks+= freeBlocks;
    stats.free_bytes+= freeBytes;
    stats.allocated_blocks+= allocatedBlocks;
    stats.allocated_bytes+= allocatrdBytes;
}

enum ListType{BlockList, FreeList};

struct List {
    BlockMetadata head;
    BlockMetadata tail;
};
static struct List list {.head = {.size = 0, .is_free = false, .next = &(list.tail), .prev = NULL},
        .tail = {.size = 0, .is_free = false, .next = NULL, .prev = &(list.head)}};


BlockMetadata* findFreeBlock(size_t size){
    BlockMetadata* iter = list.head.next_free;
    while(iter != &list.tail){
        if(size <= iter->size){
            return iter;
        }
        iter = iter->next_free;
    }
    return NULL;
    
}



void linkBlocks(BlockMetadata* first, BlockMetadata* second, ListType list_type){
    if(list_type == BlockList){
        first->next = second;
        second->prev = first;
    }
    else{
        first->next_free = second;
        second->prev_free = first;
    }
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

bool largeEnough(size_t size){
    if (size < (METADATA_SIZE + 128)){
        return false;
    }
    return true;
}

void splitBlock(BlockMetadata* block, size_t first_blk_size){
    assert(block->is_free);
    size_t new_size = (block->size + METADATA_SIZE) - (first_blk_size + METADATA_SIZE);
    
    if(block->size > first_blk_size && largeEnough(new_size)){
        BlockMetadata* new_block = (BlockMetadata*)((char*)block + METADATA_SIZE + first_blk_size);
        //Update MetaData
        block->size = first_blk_size;
        block->is_free = false;
        new_block->size = new_size;
        new_block->is_free = true;

        linkBlocks(block->prev_free, block->next_free, FreeList);

        BlockMetadata* block_next = block->next;
        linkBlocks(block, new_block, BlockList);
        linkBlocks(new_block,block_next, BlockList);
        FreeListInsertBlock(new_block);
        
        stats.allocated_blocks++;
        stats.allocated_bytes -= METADATA_SIZE;
    }
}
BlockMetadata* combine(BlockMetadata* block){
    assert(block->is_free);
    size_t total_size = block->size;
    BlockMetadata* new_block = block;
    
    //current + next
    if (block->next->is_free){
        total_size += block->next->size;
        linkBlocks(block, block->next->next, BlockList);
        linkBlocks(block, block->next->next, FreeList);
        stats.allocated_blocks--;
        stats.allocated_bytes+= METADATA_SIZE;
    }
    
    //prev + current
    if(block->prev->is_free){
        total_size+=block->prev->size;
        new_block = block->prev;
        linkBlocks(block->prev, block->next, BlockList);
        linkBlocks(block->prev, block->next, FreeList);
        
        stats.allocated_blocks--;
        stats.allocated_bytes+= METADATA_SIZE;
    }
    new_block->size = total_size;
    return new_block;
}
BlockMetadata* initWilde(size_t size){
    BlockMetadata* new_block = (BlockMetadata*) sbrk((intptr_t) (size - list.tail.prev->size));
    if(new_block == (void*)-1){
        return NULL;
    }
    new_block->is_free = false;
    new_block->size = size;
    stats.allocated_bytes+= size;
    
    return new_block;
}
void* smalloc(size_t size){
    if(size == 0 || size > MAX_SIZE){
        return NULL;
    }
    BlockMetadata* new_block = findFreeBlock(size);
    if(new_block == NULL){
        
        if(list.head.next == &list.tail){ //Empty List
            new_block = initBlock(size);
            if(new_block == NULL){
                return NULL;
            }
            //assert(tail.prev == NULL);
            linkBlocks(&list.head, new_block, BlockList);
            linkBlocks(new_block, &list.tail, BlockList);
        }
        else if (list.tail.prev->is_free){
            new_block = initWilde(size);
            linkBlocks(new_block->prev_free, new_block->next_free, FreeList);
        }
        else{ //Insert Last
            new_block = initBlock(size);
            if(new_block == NULL){
                return NULL;
            }
            linkBlocks((list.tail.prev), new_block, BlockList);
            linkBlocks(new_block, &list.tail, BlockList);
        }
    }
    else{
        new_block->is_free = false;
        linkBlocks(new_block->prev_free, new_block->next_free, FreeList);
        stats.free_blocks--;
        stats.free_bytes-= new_block->size;
        splitBlock(new_block, size);
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

void FreeListInsertBlock(BlockMetadata* free_block){
    BlockMetadata* iter = list.head.next_free;
    while(iter != &list.tail &&  iter->size < free_block->size){
        iter = iter->next_free;
    }
    BlockMetadata* prev = iter->prev_free;
    linkBlocks(free_block, iter, FreeList);
    linkBlocks(prev, free_block, FreeList);
    
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
    FreeListInsertBlock(block_meta_data);
    stats.free_blocks++;
    stats.free_bytes+= block_meta_data->size;
    
    combine(block_meta_data);
}


void* srealloc(void* oldp, size_t size){
    BlockMetadata* block_meta_data = (BlockMetadata*)oldp - 1;
    if(oldp == NULL || block_meta_data->size < size){
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



