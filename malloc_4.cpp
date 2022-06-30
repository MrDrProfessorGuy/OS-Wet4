
#include "unistd.h"
#include "string.h"
#include "sys/mman.h"
#include "algorithm"
//#include "assert.h"
//#include "iostream"
//#include "iomanip"
using namespace std;

#define MAX_SIZE 100000000
#define DEFAULT_MMAP_THRESHOLD_MAX (4 * 1024 * 1024 * sizeof(long))
static size_t MMAP_THRESHOLD (128*1024);

#define HUGEPAGE_MALLOC_THRESHOLD (4 * 1024 * 1024)
#define HUGEPAGE_CALLOC_THRESHOLD (2 * 1024 * 1024)

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

#define AllocViaMalloc 0
typedef size_t AllocMethod;

typedef struct MallocMetadata {
    size_t size;
    bool is_free;
    MallocMetadata* next;
    MallocMetadata* prev;
    MallocMetadata* next_free;
    MallocMetadata* prev_free;
    AllocMethod allocation_method;
}BlockMetadata;

inline size_t MUL_SIZE(size_t size){
    if (size%8 == 0){
        return size;
    }
    return (size + (8-size%8));
}

#define METADATA_SIZE_UNALLINED sizeof(BlockMetadata)
#define METADATA_SIZE (MUL_SIZE(METADATA_SIZE_UNALLINED))

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


enum ListType{BlockList, FreeList};

struct List {
    BlockMetadata head;
    BlockMetadata tail;
};
static struct List list {.head = {.size = 0, .is_free = false, .next = &(list.tail), .prev = NULL, .next_free=&list.tail, .prev_free=NULL, .allocation_method=AllocViaMalloc},
        .tail = {.size = 0, .is_free = false, .next = NULL, .prev = &(list.head), .next_free=NULL, .prev_free=&list.head, .allocation_method=AllocViaMalloc}};

static struct List mmap_list {.head = {.size = 0, .is_free = false, .next = &(mmap_list.tail), .prev = NULL, .next_free=&mmap_list.tail, .prev_free=NULL, .allocation_method=1},
        .tail = {.size = 0, .is_free = false, .next = NULL, .prev = &(mmap_list.head), .next_free=NULL, .prev_free=&mmap_list.head, .allocation_method=1}};

/// ====================================================================================================== ///
/// ========================================== Helper Functions ========================================== ///
/// ====================================================================================================== ///


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
        /*    if (meta_data->is_free == false){
                cout << "ERROR::ListRemove:: address=" << meta_data << " is NOT free" << endl;
                printHeap();
                assert(meta_data->is_free == true);
            }*/
        
        
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
    if (size >= (128)){
        return true;
    }
    return false;
}



/*
 * Get an address to start of MetaData of a free block and required data size,
 * split if largeEnough
 */
void splitBlock(BlockMetadata* block, size_t first_blk_size, bool blockIsFree=true){
    //assert(block->is_free);
    size_t new_size = (block->size) - (first_blk_size + METADATA_SIZE);
    
    //cout << "splitBlock:: first_blk_size: " << first_blk_size << "      new_size: " << new_size << endl;
    if(block->size >= (first_blk_size+METADATA_SIZE) && largeEnough(new_size)){
        BlockMetadata* new_block = (BlockMetadata*)((char*)block + METADATA_SIZE + first_blk_size);
        //Update MetaData
        block->size = first_blk_size;
        //ListRemove(block, false, true);
        
        new_block->size = new_size;
        
        
        BlockMetadata* block_next = block->next;
        linkBlocks(block, new_block, BlockList);
        linkBlocks(new_block, block_next, BlockList);
        
        new_block->is_free = false; /// so assertion in FreeListInsertBlock isn't triggered
        FreeListInsertBlock(new_block);
        
        
        stats.allocated_blocks++;
        stats.allocated_bytes -= METADATA_SIZE;
        stats.free_blocks++;
        if (blockIsFree){
            stats.free_bytes -= METADATA_SIZE;
        }
        else{
            stats.free_bytes += new_size;
        }
        
        
        
    }
}

/*
 * Get an address to start of MetaData of a free block and combine with neighbours if possible
 */
BlockMetadata* combine(BlockMetadata* block, bool prev=true, bool next=true, bool blockIsFree=true){
    //assert(block->is_free);
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
        if (blockIsFree){
            stats.free_bytes += METADATA_SIZE;
        }
        else{
            stats.free_bytes -= next->size;
        }
    }
    
    //prev + current
    if(prev && block->prev->is_free){
        total_size+= block->prev->size + METADATA_SIZE;
        new_block = block->prev;
        //BlockMetadata* next = block->next;
        //linkBlocks(new_block, next, BlockList);
        //linkBlocks(new_block, next, FreeList);
        
        
        stats.allocated_blocks--;
        stats.allocated_bytes+= METADATA_SIZE;
        stats.free_blocks--;
        if (blockIsFree){
            ListRemove(block, true, true);
            stats.free_bytes += METADATA_SIZE;
        }
        else{
            ListRemove(block, true, false);
            ListRemove(new_block, false, true);
            stats.free_bytes -= new_block->size;
        }
        
        
    }
    
    new_block->size = total_size;
    if (blockIsFree){
        ListRemove(new_block, false, true);
        FreeListInsertBlock(new_block);
    }
    return new_block;
}

/// allocate more memory to wilderness if needed and is free
BlockMetadata* initWilde(size_t data_size, bool blockIsFree=true){
    BlockMetadata* wilderness = list.tail.prev;
    //assert(wilderness->is_free);
    //assert(wilderness != &list.head);
    /*
    if (data_size <= wilderness->size){
        cout << "ERROR::initWilde:: address=" << wilderness << " is already big enough" << endl;
        printHeap();
        assert(data_size <= wilderness->size);
    }*/
    BlockMetadata* new_block = (BlockMetadata*) sbrk((intptr_t) (data_size - wilderness->size));
    //cout <<"initWilde:: new size= "<< size - list.tail.prev->size << "    new_block= "<< new_block << endl;
    if(new_block == (void*)-1){
        return NULL;
    }
    
    stats.allocated_bytes+= (data_size - wilderness->size);
    if (blockIsFree){
        stats.free_blocks--;
        stats.free_bytes -= wilderness->size;
        ListRemove(wilderness, false, true);
    }
    
    wilderness->is_free = false;
    wilderness->size = data_size;
    
    return wilderness;
}
static bool first_alloc = true;

void* initial_allignment(){
    void* a = sbrk(0);
    if(a == (void*)-1){
        return NULL;
    }
    size_t diff = (size_t)a%8;
    if (diff != 0){
        int alloc = 8 - (int)diff;
        if(sbrk(alloc) == (void*)-1){
            return NULL;
        }
    }
    return a;
}

/// ====================================================================================================== ///
/// ========================================== Malloc Functions ========================================== ///
/// ====================================================================================================== ///


void* _smalloc(size_t data_size, AllocMethod hugepage_threshold=HUGEPAGE_MALLOC_THRESHOLD){
    if(data_size <= 0 || MUL_SIZE(data_size) > MAX_SIZE){
        return NULL;
    }
    if (first_alloc == true){
        if(initial_allignment() == NULL){
            return NULL;
        }
        first_alloc = false;
    }
    data_size = MUL_SIZE(data_size);
    if (data_size >= hugepage_threshold){
        BlockMetadata* new_region = (BlockMetadata*) mmap(NULL, data_size + METADATA_SIZE, PROT_READ | PROT_WRITE, MAP_HUGETLB | MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
        if(new_region == MAP_FAILED){
            return NULL;
        }
    
        new_region->is_free = false;
        new_region->size = data_size;
        new_region->allocation_method = AllocViaMalloc;
        BlockMetadata* next = mmap_list.head.next;
        linkBlocks(&(mmap_list.head), new_region, BlockList);
        linkBlocks(new_region, next, BlockList);
        //linkBlocks(new_region, new_region, FreeList);
    
        stats.allocated_blocks++;
        stats.allocated_bytes+= data_size;
        return (new_region + 1);
    }
    if(data_size >= MMAP_THRESHOLD){

        BlockMetadata* new_region = (BlockMetadata*) mmap(NULL, data_size + METADATA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
        if(new_region == MAP_FAILED){
            return NULL;
        }
        
        new_region->is_free = false;
        new_region->size = data_size;
        new_region->allocation_method = AllocViaMalloc;
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
    new_block->allocation_method = AllocViaMalloc;
    return (new_block + 1);
}

void* smalloc(size_t data_size) {
    return _smalloc(data_size);
}


void* scalloc(size_t num, size_t size){
    void* res = _smalloc(num * size, HUGEPAGE_CALLOC_THRESHOLD);
    if(res == NULL){
        return NULL;
    }
    BlockMetadata* meta_data = (BlockMetadata*)res -1;
    memset(res, 0, meta_data->size);
    meta_data->allocation_method = size;
    return res;
}

void FreeListInsertBlock(BlockMetadata* free_block){
    //assert(!free_block->is_free);
    free_block->is_free = true;
    //cout << "============= FreeListInsertBlock =============" << endl;
    //cout << "free_block= " << free_block << endl;
    
    BlockMetadata* iter = list.head.next_free;
    int count = 0;
    
    while(iter != &list.tail &&  iter->size < free_block->size){
        iter = iter->next_free;
        //assert(count < 30);
        count++;
    }
    while(iter != &list.tail && free_block->size == iter->size && free_block > iter){
        iter = iter->next_free;
    }
    
    
    BlockMetadata* prev = iter->prev_free;
    linkBlocks(free_block, iter, FreeList);
    linkBlocks(prev, free_block, FreeList);
    
    //cout << "iter= " << iter << endl;
    //cout << "iter.prev= " << prev << endl;
    //cout << "============= FreeListInsertBlock END =============" << endl;
    
}

bool isMmaped(BlockMetadata* block){
    if (block == NULL || block == &list.tail || block == &list.head){
        return false;
    }
    bool malloc_mmaped = (block->size >= HUGEPAGE_MALLOC_THRESHOLD) && (block->allocation_method == AllocViaMalloc);
    bool calloc_mmaped = (block->allocation_method >= HUGEPAGE_CALLOC_THRESHOLD);
    if (block->size >= MMAP_THRESHOLD || malloc_mmaped || calloc_mmaped){
        return true;
    }
    return false;
}

void sfree(void* p){
    if (p == NULL) {
        return;
    }
    
    BlockMetadata* block_meta_data = (BlockMetadata*)p - 1;
    if (block_meta_data->is_free){
        return;
    }
    if (block_meta_data->size > MMAP_THRESHOLD && block_meta_data->size < DEFAULT_MMAP_THRESHOLD_MAX){
        MMAP_THRESHOLD = block_meta_data->size;
    }
    //block_meta_data->is_free = true;
    if (isMmaped(block_meta_data)){
        stats.allocated_blocks--;
        stats.allocated_bytes-=block_meta_data->size;
        ListRemove(block_meta_data, true, false);
        munmap(block_meta_data, block_meta_data->size + METADATA_SIZE);
    }
    else{
        FreeListInsertBlock(block_meta_data);
        stats.free_blocks++;
        stats.free_bytes+= block_meta_data->size;
        //printHeap();
        //cout << "sfree:: block_meta_data= "<< block_meta_data << endl;
        combine(block_meta_data, true, true, true);
    }
    
}

AllocMethod block_hugepage_threshold(BlockMetadata* block){
    
    if (block == NULL || block->allocation_method == AllocViaMalloc){
        return HUGEPAGE_MALLOC_THRESHOLD;
    }
    return HUGEPAGE_CALLOC_THRESHOLD;
}

void* srealloc(void* oldp, size_t size){
    //cout << string(30, '=') <<" Realloc "<< string(30, '=') << endl;
    //cout<< string(8, ' ') << "oldp: " << oldp << "   size: " << size << endl;
    if (size <= 0){
        return NULL;
    }
    
    if (oldp == NULL){
        //cout<< string(8, ' ') << "oldp == NULL " << endl;
        void* res = _smalloc(size, HUGEPAGE_MALLOC_THRESHOLD);
        if(res == NULL){
            return NULL;
        }
        
        return res;
    }
    
    
    
    BlockMetadata* block = (BlockMetadata*)oldp - 1;
    BlockMetadata* tmp_data = (BlockMetadata*) mmap(NULL, MUL_SIZE(size), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS , -1, 0);
    if(tmp_data == MAP_FAILED){
        return NULL;
    }
    memmove(tmp_data, block+1, min(block->size, MUL_SIZE(size)));
    
    bool calloc_mmap = (MUL_SIZE(size) >= HUGEPAGE_CALLOC_THRESHOLD) && (block->allocation_method > AllocViaMalloc);
    bool malloc_mmap = (MUL_SIZE(size) >= HUGEPAGE_MALLOC_THRESHOLD) && (block->allocation_method == AllocViaMalloc);
    
    if (malloc_mmap){
        block = (BlockMetadata*) _smalloc(MUL_SIZE(size), HUGEPAGE_MALLOC_THRESHOLD);
        if (block == NULL){
            return NULL;
        }
        block->allocation_method = AllocViaMalloc;
        memmove(block, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
        sfree(oldp);
        return block;
    }
    else if (calloc_mmap){
        AllocMethod entry_size = block->allocation_method;
        block = (BlockMetadata*) _smalloc(MUL_SIZE(size), HUGEPAGE_CALLOC_THRESHOLD);
        if (block == NULL){
            return NULL;
        }
        block->allocation_method = entry_size; // calloc
        memmove(block, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
        sfree(oldp);
        return block;
    }
    if (MUL_SIZE(size) >= MMAP_THRESHOLD){
        //cout<< string(8, ' ') << "MUL_SIZE(size) >= MMAP_THRESHOLD " << endl;
        if (block->size == MUL_SIZE(size)){
            munmap(tmp_data, MUL_SIZE(size));
            return oldp;
        }
        block = (BlockMetadata*) smalloc(MUL_SIZE(size));
        if(block == NULL){
            return NULL;
        }
        memmove(block, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
        
        sfree(oldp);
        return block;
    }
    if (block->size >= MUL_SIZE(size)){ /// a
        //cout<< string(8, ' ') << "block->size >= MUL_SIZE(size) " << endl;
        //FreeListInsertBlock(block);
        splitBlock(block, MUL_SIZE(size), false);
        if (block->next != &list.tail && block->next->is_free){
            combine(block->next, false, true, true);
        }
        
        return block+1;
    }
    
    
    bool merge_prev = (IS_FREE(block->prev) && (block->prev->size + block->size + METADATA_SIZE >= MUL_SIZE(size)));
    bool merge_next = (IS_FREE(block->next) && (block->next->size + block->size + METADATA_SIZE >= MUL_SIZE(size)));
    bool merge_all = (IS_FREE(block->prev)) && IS_FREE(block->next) && (block->prev->size + block->size +
                                                                        block->next->size + 2*METADATA_SIZE >= MUL_SIZE(size));
    
    
    
    if(merge_prev){/// b
        //cout << string(8, '~') <<" Realloc::B " << endl;
        
        //stats.free_bytes -= block->prev->size + METADATA_SIZE;
        //block->is_free = true;
        //FreeListInsertBlock(block);
        block = combine(block, true, false, false);
        //ListRemove(block, false, true);
        //block->is_free = false;
        splitBlock(block, MUL_SIZE(size), false);
        if (block->next != &list.tail && block->next->is_free){
            combine(block->next, false, true, true);
        }
        /// unmap tmp
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
    }
    else if(IS_WILDERNESS(block)){/// c
        //cout << string(8, '~') <<" Realloc::C " << endl;
        //stats.free_blocks++;
        //stats.free_bytes += block->size;
        
        if (IS_FREE(block->prev)){ /// b_note
            //cout << string(8, '~') <<" Realloc::C::Note " << endl;
            //stats.free_bytes -= block->prev->size + METADATA_SIZE;
            //block->is_free = true;
            //FreeListInsertBlock(block);
            block = combine(block, true, false, false);
            //ListRemove(block, false, true);
            //stats.free_blocks--;
            
        }
        
        //FreeListInsertBlock(block);
        initWilde(MUL_SIZE(size), false);
        /// unmap tmp
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
    }
    else if(merge_next){/// d
        //cout << string(8, '~') <<" Realloc::D " << endl;
        
        //stats.free_bytes -= block->next->size + METADATA_SIZE;
        //block->is_free = true;
        //FreeListInsertBlock(block);
        block = combine(block, false, true, false);
        //ListRemove(block, false, true);
        //block->is_free = false;
        splitBlock(block, MUL_SIZE(size), false);
        if (block->next != &list.tail && block->next->is_free){
            combine(block->next, false, true, true);
        }
        /// unmap tmp
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
        
        
    }
    
    else if(merge_all){/// e
        //cout << string(8, '~') <<" Realloc::E+F.1 " << endl;
        //stats.free_blocks -= 2;
        //stats.free_bytes -= block->prev->size + block->next->size;
        //block->is_free = true;
        block = combine(block, true, true, false);
        splitBlock(block, MUL_SIZE(size), false);
        if (block->next != &list.tail && block->next->is_free){
            combine(block->next, false, true, true);
        }
        //block->is_free = false;
        //splitBlock(block, MUL_SIZE(size), false);
        
        /// unmap tmp
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
    }
    else if(IS_WILDERNESS(block->next)){/// f.1 + f.2
        //cout << string(8, '~') <<" Realloc::F.2 " << endl;
        if (IS_FREE(block->prev) && IS_FREE(block->next)){ /// f.1
            //stats.free_blocks -= 2;
            //stats.free_bytes -= block->prev->size + block->next->size;
            //block->is_free = true;
            block = combine(block, true, true, false);
            //block->is_free = false;
        }
        else if (IS_FREE(block->next)){ /// f.2
            block = combine(block, false, true, false);
        }
        initWilde(MUL_SIZE(size), false);
        /// unmap tmp
        memmove(block+1, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
    }
    else{/// g + h
        //cout << string(8, '~') <<" Realloc::G + H " << endl;
        
        block = (BlockMetadata*)smalloc(MUL_SIZE(size));
        sfree(oldp);
        if (block == NULL){
            return NULL;
        }
        memmove(block, tmp_data, MUL_SIZE(size));
        munmap(tmp_data, MUL_SIZE(size));
        return block;
    }
    
    //cout << string(30, '=') <<" Realloc End "<< string(30, '=') << endl;
    return block+1;
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
