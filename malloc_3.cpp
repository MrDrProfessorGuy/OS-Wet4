
#include "unistd.h"
#include "string.h"
#include "sys/mman.h"
#include "assert.h"
#include "iostream"
#include "iomanip"
using namespace std;

#define MAX_SIZE 100000000
#define MMAP_THRESHOLD (128*1024)

#define IS_FREE(block) (block->is_free == true)
#define IS_WILDERNESS(block) (block->next == &list.tail)

// ############################### API ###############################
void* srealloc(void* oldp, size_t size);
void sfree(void* p);
void* scalloc(size_t num, size_t size);
void* smalloc(size_t data_size);
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

#define METADATA_SIZE_UNALLINED sizeof(BlockMetadata)
#define METADATA_SIZE (METADATA_SIZE_UNALLINED)


BlockMetadata* findFreeBlock(size_t size);
void FreeListInsertBlock(BlockMetadata* free_block);

typedef struct Stats {
    size_t free_blocks;
    size_t free_bytes;
    size_t allocated_blocks;
    size_t allocated_bytes;
    const size_t meta_data;
}Stats;

inline size_t MUL_SIZE(size_t size){
    if (size%8 == 0){
        return size;
    }
    return (size + (8-size%8));
}
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
static struct List list {.head = {.size = 0, .is_free = false, .next = &(list.tail), .prev = NULL, .next_free=&list.tail, .prev_free=NULL},
                         .tail = {.size = 0, .is_free = false, .next = NULL, .prev = &(list.head), .next_free=NULL, .prev_free=&list.head}};

static struct List mmap_list {.head = {.size = 0, .is_free = false, .next = &(mmap_list.tail), .prev = NULL, .next_free=&mmap_list.tail, .prev_free=NULL},
                              .tail = {.size = 0, .is_free = false, .next = NULL, .prev = &(mmap_list.head), .next_free=NULL, .prev_free=&mmap_list.head}};

/// ====================================================================================================== ///
/// ========================================== Helper Functions ========================================== ///
/// ====================================================================================================== ///


void printHeap(){
    std::cout << "======================= PrintHeap =======================" << std::endl;
    
    int counter = 0;
    int width = 20;
    for (BlockMetadata* iter = &list.head; iter != nullptr ; iter = iter->next) {
        std::cout << "----------- BlockMetaData["<< counter <<"] "<< iter << " -----------" << std::endl;
        std::cout <<setw(width)<< "size= " << iter->size << " || ";
        cout <<setw(width)<<"is_free= " << iter->is_free << std::endl;
        
        std::cout <<setw(width)<< "prev= " << iter->prev << " || ";
        cout <<setw(width)<<"next= " << iter->next << std::endl;
        
        std::cout <<setw(width)<< "prev_free= " << iter->prev_free << " || ";
        cout <<setw(width)<<"next_free= " << iter->next_free << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        counter++;
    }
}
void printHeapFree(){
    std::cout << "======================= PrintHeapFree =======================" << std::endl;
    
    int counter = 0;
    for (BlockMetadata* iter = &list.head; iter != nullptr ; iter = iter->next_free) {
        std::cout << "----------- BlockMetaData["<< counter <<"] "<< iter << " -----------" << std::endl;
        std::cout << "      -size= " << iter->size << "     ||      -is_free= " << iter->is_free << std::endl;
        std::cout << "      -prev= " << iter->prev << "     ||      -next= " << iter->next << std::endl;
        std::cout << "      -prev_free= " << iter->prev_free << "       ||      -next_free= " << iter->next_free << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        counter++;
    }
    
}

BlockMetadata* findFreeBlock(size_t size){
    BlockMetadata* iter = list.head.next_free;
    while(iter != &list.tail){
        if(size <= iter->size){
            if (!iter->is_free){
                cout << "ERROR::findFreeBlock:: address=" << iter << " is NOT free" << endl;
                printHeap();
            }
            assert(iter->is_free);
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

void ListRemove(BlockMetadata* meta_data, bool blockList, bool freeList){
    if (meta_data == NULL){
        return;
    }
    
    if (blockList){
        BlockMetadata* prev = meta_data->prev;
        BlockMetadata* next = meta_data->next;
        linkBlocks(prev, next, BlockList);
        meta_data->next = NULL;
        meta_data->prev = NULL;
    }
    if (freeList){
        if (meta_data->is_free == false){
            cout << "ERROR::ListRemove:: address=" << meta_data << " is NOT free" << endl;
            printHeap();
            assert(meta_data->is_free == true);
        }
        
        
        meta_data->is_free = false;
        BlockMetadata* prev = meta_data->prev_free;
        BlockMetadata* next = meta_data->next_free;
        linkBlocks(prev, next, FreeList);
        meta_data->next_free = NULL;
        meta_data->prev_free = NULL;
    }
}


BlockMetadata* initBlock(size_t size){
    BlockMetadata* allocated_block = (BlockMetadata*) sbrk((intptr_t) (size + METADATA_SIZE));
    if(allocated_block == (void*)-1){
        return NULL;
    }
    allocated_block->is_free = false;
    allocated_block->size = size;
    
    allocated_block->next_free = NULL;
    allocated_block->prev_free = NULL;
    
    stats.allocated_blocks++;
    stats.allocated_bytes+= size;
    return allocated_block;
}

bool largeEnough(size_t size){
    if (size >= (METADATA_SIZE + 128)){
        return true;
    }
    return false;
}



/*
 * Get an address to start of MetaData of a free block and required data size,
 * split if largeEnough
 */
void splitBlock(BlockMetadata* block, size_t first_blk_size){
    assert(block->is_free);
    size_t new_size = (block->size) - (first_blk_size + METADATA_SIZE);
    
    //cout << "splitBlock:: first_blk_size: " << first_blk_size << "      new_size: " << new_size << endl;
    if(block->size > first_blk_size+METADATA_SIZE && largeEnough(new_size)){
        BlockMetadata* new_block = (BlockMetadata*)((char*)block + METADATA_SIZE + first_blk_size);
        //Update MetaData
        block->size = first_blk_size;
        //ListRemove(block, false, true);
        
        new_block->size = new_size;
        //new_block->is_free = true;
        
        BlockMetadata* block_next = block->next;
        linkBlocks(block, new_block, BlockList);
        linkBlocks(new_block, block_next, BlockList);
        
        FreeListInsertBlock(new_block);
        
        stats.allocated_blocks++;
        stats.allocated_bytes -= METADATA_SIZE;
        stats.free_blocks++;
        stats.free_bytes -= METADATA_SIZE;
    }
}

/*
 * Get an address to start of MetaData of a free block and combine with neighbours if possible
 */
BlockMetadata* combine(BlockMetadata* block, bool prev=true, bool next=true){
    assert(block->is_free);
    size_t total_size = block->size;
    BlockMetadata* new_block = block;
    
    //current + next
    if (next && block->next->is_free){
        total_size += block->next->size + METADATA_SIZE;
        BlockMetadata* next = block->next;///->next;
        //linkBlocks(block, next, BlockList);
        //linkBlocks(block, next, FreeList);
        ListRemove(next, true, true);
        
        stats.allocated_blocks--;
        stats.allocated_bytes+= METADATA_SIZE;
        stats.free_blocks--;
        stats.free_bytes += METADATA_SIZE;
    }
    
    //prev + current
    if(prev && block->prev->is_free){
        total_size+= block->prev->size + METADATA_SIZE;
        new_block = block->prev;
        //BlockMetadata* next = block->next;
        //linkBlocks(new_block, next, BlockList);
        //linkBlocks(new_block, next, FreeList);
        ListRemove(block, true, true);
        
        
        stats.allocated_blocks--;
        stats.allocated_bytes+= METADATA_SIZE;
        stats.free_blocks--;
        stats.free_bytes += METADATA_SIZE;
    }
    
    new_block->size = total_size;
    ListRemove(new_block, false, true);
    FreeListInsertBlock(new_block);
    return new_block;
}

/// allocate more memory to wilderness if needed and is free
BlockMetadata* initWilde(size_t data_size){
    BlockMetadata* wilderness = list.tail.prev;
    assert(wilderness->is_free);
    assert(wilderness != &list.head);
    
    if (data_size <= wilderness->size){
        cout << "ERROR::initWilde:: address=" << wilderness << " is already big enough" << endl;
        printHeap();
        assert(data_size <= wilderness->size);
    }
    BlockMetadata* new_block = (BlockMetadata*) sbrk((intptr_t) (data_size - wilderness->size));
    //cout <<"initWilde:: new size= "<< size - list.tail.prev->size << "    new_block= "<< new_block << endl;
    if(new_block == (void*)-1){
        return NULL;
    }
    
    ListRemove(wilderness, false, true);
    wilderness->is_free = false;
    
    stats.allocated_bytes+= (data_size - wilderness->size);
    stats.free_blocks--;
    stats.free_bytes -= wilderness->size;
    
    wilderness->size = data_size;
    
    return wilderness;
}

/// ====================================================================================================== ///
/// ========================================== Malloc Functions ========================================== ///
/// ====================================================================================================== ///
void* smalloc(size_t data_size){
    if(data_size == 0 || MUL_SIZE(data_size) > MAX_SIZE){
        return NULL;
    }
    data_size = MUL_SIZE(data_size);
    
    if(data_size >= MMAP_THRESHOLD){
        BlockMetadata* new_region = (BlockMetadata*) mmap(NULL, data_size + METADATA_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS , -1, 0);
        if(new_region == MAP_FAILED){
            return NULL;
        }
        
        new_region->is_free = false;
        new_region->size = data_size;
        BlockMetadata* next = mmap_list.head.next;
        linkBlocks(&(mmap_list.head), new_region, BlockList);
        linkBlocks(new_region, next, BlockList);
        //linkBlocks(new_region, new_region, FreeList);
        
        stats.allocated_blocks++;
        stats.allocated_bytes+= data_size;
        return (new_region + 1);
    }
    
    BlockMetadata* new_block = findFreeBlock(data_size);
    if(new_block == NULL){
        
        if(false && list.head.next == &list.tail){ //Empty List
            new_block = initBlock(data_size);
            if(new_block == NULL){
                return NULL;
            }
            //assert(tail.prev == NULL);
            linkBlocks(&list.head, new_block, BlockList);
            linkBlocks(new_block, &list.tail, BlockList);
            
        }
        if (list.tail.prev->is_free){
            //int size_prev = list.tail.prev->size;
            if (initWilde(data_size) == NULL){
                return NULL;
            }
            new_block = list.tail.prev;
            //ListRemove(new_block, false, true);
            
            //stats.free_blocks--;
            //stats.free_bytes-= new_block->size;
        }
        else{ //Insert Last
            new_block = initBlock(data_size);
            if(new_block == NULL){
                return NULL;
            }
            linkBlocks((list.tail.prev), new_block, BlockList);
            linkBlocks(new_block, &list.tail, BlockList);
        }
    }
    else{
        
        //cout << "##########################################################" << endl;
        //cout << "smalloc:: using existing block " << new_block << endl;
        //printHeap();
        splitBlock(new_block, data_size);
        ListRemove(new_block, false, true);
        //printHeap();
        //cout << "##########################################################" << endl;
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
    BlockMetadata* meta_data = (BlockMetadata*)res -1;
    memset(res, 0, meta_data->size);
    return res;
}

void FreeListInsertBlock(BlockMetadata* free_block){
    assert(!free_block->is_free);
    free_block->is_free = true;
    //cout << "============= FreeListInsertBlock =============" << endl;
    //cout << "free_block= " << free_block << endl;
    
    BlockMetadata* iter = list.head.next_free;
    int count = 0;
    
    while(iter != &list.tail &&  iter->size < free_block->size){
        iter = iter->next_free;
        assert(count < 30);
        count++;
    }
    BlockMetadata* prev = iter->prev_free;
    linkBlocks(free_block, iter, FreeList);
    linkBlocks(prev, free_block, FreeList);
    
    //cout << "iter= " << iter << endl;
    //cout << "iter.prev= " << prev << endl;
    //cout << "============= FreeListInsertBlock END =============" << endl;
    
}

void sfree(void* p){
    if (p == NULL) {
        return;
    }
    
    BlockMetadata* block_meta_data = (BlockMetadata*)p - 1;
    if (block_meta_data->is_free){
        return;
    }
    //block_meta_data->is_free = true;
    if (block_meta_data->size >= MMAP_THRESHOLD){
        ListRemove(block_meta_data, true, false);
        munmap(block_meta_data, block_meta_data->size + METADATA_SIZE);
        
        stats.allocated_blocks--;
        stats.allocated_bytes-=block_meta_data->size;
    }
    else{
        FreeListInsertBlock(block_meta_data);
        stats.free_blocks++;
        stats.free_bytes+= block_meta_data->size;
        //printHeap();
        //cout << "sfree:: block_meta_data= "<< block_meta_data << endl;
        combine(block_meta_data);
    }
   
}


void* srealloc(void* oldp, size_t size){
    
    if (MUL_SIZE(size) >= MMAP_THRESHOLD){
        sfree(oldp);
        oldp = smalloc(MUL_SIZE(size));
    }
    if (oldp == NULL){
        void* res = smalloc(size);
        if(res == NULL){
            return NULL;
        }
        
        return res;
    }
    BlockMetadata* block = (BlockMetadata*)oldp - 1;
    if (block->size >= MUL_SIZE(size)){ /// a
        splitBlock(block, MUL_SIZE(size));
        return block+1;
    }
    
    
    bool merge_prev = (IS_FREE(block->prev) && (block->prev->size + block->size >= MUL_SIZE(size)));
    bool merge_next = (IS_FREE(block->next) && (block->next->size + block->size >= MUL_SIZE(size)));
    bool merge_all = (IS_FREE(block->prev)) && IS_FREE(block->next) && (block->prev->size + block->size +
                                                                        block->next->size >= MUL_SIZE(size));
    
    BlockMetadata* tmp_data = (BlockMetadata*) mmap(NULL, MUL_SIZE(size), PROT_READ | PROT_WRITE, MAP_ANONYMOUS , -1, 0);
    if(tmp_data == MAP_FAILED){
        return NULL;
    }
    else{
        memmove(tmp_data, block+1, block->size);
    }
    
    if(merge_prev){/// b
        stats.free_blocks--;
        stats.free_bytes -= block->prev->size;
        block->is_free = true;
        
        block = combine(block, true, false);
        block->is_free = false;
        splitBlock(block, MUL_SIZE(size));
        /// unmap tmp
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
    }
    else if(IS_WILDERNESS(block)){/// c
        if (IS_FREE(block->prev)){ /// b_note
            stats.free_blocks--;
            stats.free_bytes -= block->prev->size;
            block->is_free = true;
    
            block = combine(block, true, false);
            block->is_free = false;
            
        }
        initWilde(MUL_SIZE(size));
        /// unmap tmp
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
    }
    else if(merge_next){/// d
        stats.free_blocks--;
        stats.free_bytes -= block->prev->size;
        block->is_free = true;
    
        combine(block, false, true);
        block->is_free = false;
        splitBlock(block, MUL_SIZE(size));
        /// unmap tmp
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
    }
    else if(merge_all){/// e + f.1
        stats.free_blocks -= 2;
        stats.free_bytes -= block->prev->size + block->next->size;
        block->is_free = true;
        block = combine(block, true, true);
        block->is_free = false;
        splitBlock(block, MUL_SIZE(size));
        
        /// unmap tmp
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
    }
    else if(IS_WILDERNESS(block->next)){/// f.2
        if (IS_FREE(block->prev) && IS_FREE(block->next)){
            stats.free_blocks -= 2;
            stats.free_bytes -= block->prev->size + block->next->size;
            block->is_free = true;
            block = combine(block, true, true);
            block->is_free = false;
        }
        initWilde(MUL_SIZE(size));
        /// unmap tmp
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
    }
    else{/// g + h
       
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
        sfree(oldp);
        oldp = smalloc(MUL_SIZE(size));
    }
    
    
    return oldp;
}

/// ====================================================================================================== ///
/// ========================================== Stats ===================================================== ///
/// ====================================================================================================== ///

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



