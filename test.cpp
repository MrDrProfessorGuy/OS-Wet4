

#include "malloc_3.cpp"
#include "iostream"
#include "iomanip"
#include "vector"
using namespace std;

#define BLOCK_STRT_ADDR(block) (((BlockMetadata*)(block))-1)

class test{
public:
    size_t allocated_blocks;
    size_t allocated_bytes;
    size_t free_blocks;
    size_t free_bytes;
    size_t meta_data_size;
    size_t num_meta_bytes;
    
    void clear_stats(){
        allocated_blocks = 0;
        allocated_bytes = 0;
        free_blocks = 0;
        free_bytes = 0;
        num_meta_bytes = 0;
        meta_data_size = _size_meta_data();
    }
    
    
    void update_stats(){
        clear_stats();
        
        for (BlockMetadata* iter = &list.head; iter != nullptr ; iter = iter->next) {
            if (iter != &list.head && iter != &list.tail){
                allocated_blocks++;
                allocated_bytes += iter->size;
                num_meta_bytes = allocated_blocks*meta_data_size;
                if (iter->is_free){
                    free_blocks++;
                    free_bytes += iter->size;
                }
            
            }
        }
        
    }
    void printDataLine(int line_width, string str1, size_t data1, string str2, size_t data2){
        int h = 8;
        cout <<string(h, ' ');
        cout <<left<<setfill(' ')<<setw(line_width)<< str1 << data1;
        cout <<string(h, ' ')<< " || "<<string(h, ' ');
        cout <<left<<setfill(' ')<<setw(line_width)<<str2 << data2 << std::endl;
    }
    void print_stats(){
        cout << "======================= print_stats =======================" << std::endl;
        std::cout << "num_allocated_blocks =    Expected: " << allocated_blocks << "        Got: "<< _num_allocated_blocks()  << std::endl;
        std::cout << "num_allocated_bytes =     Expected: " << allocated_bytes << "        Got: "<< _num_allocated_bytes() << std::endl;
        std::cout << "num_free_blocks:          Expected: " << free_blocks << "        Got: "<< _num_free_blocks() << std::endl;
        std::cout << "num_free_bytes =          Expected: " << free_bytes << "        Got: "<< _num_free_bytes() << std::endl;
        std::cout << "num_meta_bytes =          Expected: " << num_meta_bytes << "        Got: "<< _num_meta_data_bytes() << std::endl;
        std::cout << "size_meta_data =          Expected: " << meta_data_size << "        Got: "<< _size_meta_data() << std::endl;
        cout << "======================= End =======================" << std::endl;
        
    }
    bool validStats(){
        update_stats();
        bool allBlocks = allocated_blocks == _num_allocated_blocks();
        bool allBytes = allocated_bytes == _num_allocated_bytes();
        bool freeBlocks = free_blocks == _num_free_blocks();
        bool freeBytes = free_bytes == _num_free_bytes();
        bool metaBytes = num_meta_bytes == _num_meta_data_bytes();
        
        if (!allBlocks || !allBytes || !freeBlocks || !freeBytes || !metaBytes){
            cout << "======================= ERROR:: Invalid Stats =======================" << endl;
            printHeap();
            print_stats();
            return false;
        }
        return true;
    }
    
    void print(){
        std::cout << "======================= Print =======================" << std::endl;
        std::cout << "head= " << &list.head << std::endl;
        std::cout << "tail= " << &list.tail << std::endl;
    
        std::cout << "num_free_blocks = " << _num_free_blocks() << std::endl;
        std::cout << "num_free_bytes = " <<  _num_free_bytes() << std::endl;
        std::cout << "num_allocated_blocks = " << _num_allocated_blocks()  << std::endl;
        std::cout << "num_allocated_bytes = " <<  _num_allocated_bytes() << std::endl;
        std::cout << "num_beta_bytes = " <<  _num_meta_data_bytes() << std::endl;
        std::cout << "size_meta_data = " <<  _size_meta_data() << std::endl;
        std::cout << std::endl;
    }
    void printHeap(){
        std::cout << "======================= PrintHeap =======================" << std::endl;
    
        int counter = 0;
        for (BlockMetadata* iter = &list.head; iter != nullptr ; iter = iter->next) {
            std::cout << "----------- BlockMetaData["<< counter <<"] "<< iter << " -----------" << std::endl;
            std::cout << "      -size= " << iter->size << "     ||      -is_free= " << iter->is_free << std::endl;
            std::cout << "      -prev= " << iter->prev << "     ||      -next= " << iter->next << std::endl;
            std::cout << "      -prev_free= " << iter->prev_free << "       ||      -next_free= " << iter->next_free << std::endl;
            std::cout << "------------------------------------------------------------" << std::endl;
            counter++;
        }
        
    }
    void print_mmap(){
        std::cout << "======================= PrintHeap =======================" << std::endl;
        
        int counter = 0;
        for (BlockMetadata* iter = &mmap_list.head; iter != nullptr ; iter = iter->next) {
            std::cout << "----------- BlockMetaData["<< counter <<"] "<< iter << " -----------" << std::endl;
            std::cout << "      -size= " << iter->size << "     ||      -is_free= " << iter->is_free << std::endl;
            std::cout << "      -prev= " << iter->prev << "     ||      -next= " << iter->next << std::endl;
            std::cout << "      -prev_free= " << iter->prev_free << "       ||      -next_free= " << iter->next_free << std::endl;
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
    
    void printList(){
        int width = 15;
        std::cout << "======================= Print List =======================" << std::endl;
        
        int counter = 0;
        for (BlockMetadata* iter = &list.head; iter != nullptr ; iter = iter->next) {
            std::cout << "----------- BlockMetaData["<< counter <<"] "<< iter << " -----------" << std::endl;
            std::cout << "      -size= " << iter->size << "     ||      -is_free= " << iter->is_free << std::endl;
            counter++;
        }
        std::cout << "------------------------------------------------------------" << std::endl;
    }
    
    void MemView(){
        int width = 20;
        std::cout << "======================= Memory View =======================" << std::endl;
        enum printT {MetaData, Size, IsFree, Fill};
        
        int counter = 0;
        for (BlockMetadata* iter = &list.head; iter != nullptr ; iter = iter->next) {
            cout <<setfill('_')<<setw(18)<< iter;

            cout <<setfill('=')<<"||"<<std::setw(2*width)<<"||"<<endl;
            printT print = MetaData;
            
            size_t step = max((iter->size+METADATA_SIZE)/8,(size_t)1);
    
            
            for (size_t a = 0; a < iter->size+METADATA_SIZE; a += step){
                cout<<setfill('_')<<setw(18)<< a << "|| ";
               
                if(print == MetaData && a+step >= METADATA_SIZE-1){
                    cout <<setfill('_')<<setw(width)<< "MetaData END" <<setw(width);
                    print = Size;
                }
                else if(print == Size){
                    cout <<setfill('_')<<setw(width)<< "size= " << iter->size <<setw(width-3);
                    
                    print = IsFree;
                }
                else if(print == IsFree){
                    cout <<setfill('_')<<setw(width)<< "free= " << iter->is_free <<setw(width-1);
                    print = Fill;
                }
                else{
                    cout <<setfill('_')<<setw(2*width);
                }
                
                cout <<" ||"<<endl;
            }
            counter++;
        }
        std::cout << "------------------------------------------------------------" << std::endl;
    }
    
    bool validFreePointer(BlockMetadata* iter){
        if (iter->is_free){
            if (iter->next_free == iter || iter->prev_free == iter){
                printHeapFree();
                return false;
            }
        }
        return true;
    }
    bool validPointer(BlockMetadata* iter){
        if (iter->next == iter || iter->prev == iter){
            printHeap();
            return false;
        }
        return true;
    }
    
    bool validSize(BlockMetadata* iter, bool print=true){
        if (iter == nullptr){
            return true;
        }
        assert(validPointer(iter));
        assert(validFreePointer(iter));
        
        if (iter->next != &list.tail){
            char* next = (iter->size + METADATA_SIZE + (char*)iter);
            if ((iter->size + METADATA_SIZE + (char*)iter) != (char*)iter->next){
                if (print){
                    MemView();
                    cout << "Expected: " << (void*)next << endl;
                    cout << "Got: " << (iter->next) << endl;
                    cout << "Diff: " << (char*)next-(char*)(iter->next) << endl;
                }
                return false;
            }
        }
        return true;
    }
    
    void printBlock(BlockMetadata* iter, int counter){
        int width = 14;
        int h = 4;
        ///Line 1 - Title
        cout <<string(2*width, '=')<< "BlockMetaData["<< counter <<"] "<< iter << string(2*width, '=')<<endl;
        
        ///Line 2
        cout <<left<<setfill(' ')<<string(h, ' ')<<setw(width)<< "size= " <<left<< iter->size ;
        cout <<right<<string(h, ' ')<< " || "<<string(h, ' ');
        cout <<left<<setfill(' ')<<setw(width)<<"is_free= " << iter->is_free << std::endl;
        ///Line 3 - BlockList
        cout <<left<<setfill(' ')<<string(h, ' ')<<setw(width)<< "prev= " << iter->prev ;
        cout <<right<<string(h, ' ')<< " || "<<string(h, ' ');
        cout <<left<<setfill(' ')<<setw(width)<<"next= " << iter->next << std::endl;
        ///Line 4 - FreeList
        cout <<string(h, ' ')<<left<<setw(width)<< "prev_free= " << iter->prev_free;
        cout <<right<<string(h, ' ')<< " || "<<string(h, ' ');
        cout <<left<<setw(width)<<"next_free= " << iter->next_free << std::endl;
        ///Line 4 - End
        cout <<string(4*width + 32, '-')<< endl;
    }
    
    
    void validateHeap(){
        int width = 10;
        int counter = 0;
        
        clear_stats();
        
    
        cout <<string(2*width, '=')<< " Validate Heap " <<string(2*width, '=')<< endl;
        
        for (BlockMetadata* iter = &list.head; iter != nullptr ; iter = iter->next) {
            printBlock(iter, counter);
            counter++;
        }
        cout <<string(2*width, '=')<< " Validation Ended " << string(2*width, '=')<< endl;
    }
    
    
    void test1(){
        print();
        printHeap();
        void* a = smalloc(MMAP_THRESHOLD);
        MemView();
        void* b = smalloc(200);
        MemView();
        sfree(b);
        MemView();
        
        void* c = smalloc(400);
        MemView();
        sfree(a);
        MemView();
        sfree(c);
        MemView();
    }
    
    void test_mmap(){
        void* a = smalloc(MMAP_THRESHOLD + 100);
        print_mmap();
        printHeap();
        return;
        void* b = smalloc(MMAP_THRESHOLD);
        print_mmap();
    
        sfree(a);
        print_mmap();
    
        sfree(b);
        print_mmap();
    }

    void test2(int seed){
        vector<void*> blocks;
        int test_num = 1000;
        //srand(872);
        srand(seed);
        enum OP {Malloc, Calloc, Free, Realloc, OPs_NUM};
        
        int num_of_errors = 0;
        
        for (int a = 0; a < test_num; a++){
            assert(validStats());
            
            int op = rand()%OPs_NUM;
            int size = rand()%(MMAP_THRESHOLD/12);
            cout << "test_2[" << a <<"]::";
            if (op == Malloc){
                cout << "malloc " << size;
                cout.flush();
                //printHeap();
                void* block = smalloc(size);
                if (block == nullptr){
                    num_of_errors++;
                    continue;
                }
                blocks.push_back(block);
                cout << "   address= "<< BLOCK_STRT_ADDR(block) << endl;
                assert(validSize(((BlockMetadata*)block)-1));
            }
            else if (op == Calloc){
                size_t items = 1 + rand()%10;
                cout << "calloc, items= "<< items <<",    size= "<<size;
                cout.flush();
                void* block = scalloc(items, size);
                if (block == nullptr){
                    num_of_errors++;
                    cout << " ERROR  address= null" << endl;
                    continue;
                }
                blocks.push_back(block);
                cout << "   address= "<< BLOCK_STRT_ADDR(block) << endl;
                assert(validSize(BLOCK_STRT_ADDR(block)));
            }
            else if (op == Free){
                if (blocks.empty()){
                    continue;
                }
                int pos = rand()%(blocks.size());
                auto iter = blocks.begin();
                for (int a = 0; a < pos; a++){
                    iter++;
                }
                cout << "free address=" << BLOCK_STRT_ADDR(*iter) << endl;
                //validateHeap();
                sfree(*iter);
                //assert(validSize(((BlockMetadata*)(*iter))-1));
                blocks.erase(iter);
            }
            else if (op == Realloc){
            
            }
            cout << endl;
        }
        
        
        cout << string(30, '=') << endl;
        cout << string(9, '=') << " Test Ended " << string(9, '=') << endl<<endl;
        cout << "Total Errors: " << num_of_errors << endl;
        cout <<endl<< string(30, '=') << endl;
        
    }
};




int main(int argc,char* argv[]) {
    test a;
    
    a.test_mmap();
    
    int seed = 1;
    if (argc > 1){
        seed = atoi(argv[1]);
        a.test2(seed);
    }
    
    
    
    return 0;
}

























