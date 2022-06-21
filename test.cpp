

#include "malloc_3.cpp"
#include "iostream"
#include "iomanip"
#include "vector"
using namespace std;


class test{
public:
    size_t allocated_blocks;
    size_t allocated_bytes;
    size_t free_blocks;
    size_t free_bytes;
    size_t meta_data_size;
    size_t num_beta_bytes;
    
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
        std::cout << "======================= Print =======================" << std::endl;
    
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
    bool validSize(BlockMetadata* iter, bool print=true){
        if (iter == nullptr){
            return true;
        }
        if (iter->next != &list.tail){
            if ((iter->size + METADATA_SIZE + (char*)iter) != (char*)iter->next){
                if (print){
                    MemView();
                }
                return false;
            }
        }
        return true;
    }
    void test1(){
        print();
        printHeap();
        void* a = smalloc(1000);
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

    void test2(){
        vector<void*> blocks;
        int test_num = 100;
        enum OP {Malloc, Calloc, Free, Realloc, OPs_NUM};
        
        for (int a = 0; a < test_num; a++){
            int op = rand()%OPs_NUM;
            size_t size = rand()%MMAP_THRESHOLD;
            
            if (op == Malloc){
                blocks.push_back(smalloc(size));
                assert(validSize((BlockMetadata*)blocks.back()));
            }
            else if (op == Calloc){
                size_t items = rand()%10;
                blocks.push_back(scalloc(items, size));
                assert(validSize((BlockMetadata*)blocks.back()));
            }
            else if (op == Free){
                if (blocks.empty()){
                    continue;
                }
                int pos = rand()%blocks.size();
                auto iter = blocks.begin();
                for (int a = 0; a <= pos; a++){
                    iter++;
                }
               
                sfree(*iter);
                assert(validSize((BlockMetadata*)*iter));
                blocks.erase(iter);
            }
            else if (op == Realloc){
            
            }
        }
        
        
    }
};




int main(int argc,char* argv[]) {
    test a;
    a.test2();
    
    return 0;
}

























