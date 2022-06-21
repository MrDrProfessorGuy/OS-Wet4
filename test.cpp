

#include "malloc_3.cpp"
#include "iostream"
#include "iomanip"
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
        
        int counter = 0;
        for (BlockMetadata* iter = &list.head; iter != nullptr ; iter = iter->next) {
            cout <<setfill('_')<<setw(18)<< iter;
            cout <<setfill('=')<<"|"<<std::setw(2*width)<<"|"<<endl;
            
            size_t step = iter->size/12;
            for (size_t a = 0; a < iter->size; a += step){
                cout<<setfill('_')<<setw(18)<< a << "|| ";
                if(a == 6*step){
                    cout <<setfill('_')<<setw(width)<< "size= " << iter->size <<setw(width-3);
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
    
    void test1(){
        print();
        printHeap();
        void* a = smalloc(10);
        print();
        printList();
        void* b = smalloc(20);
        print();
        printList();
        sfree(a);
        print();
        printList();
        sfree(b);
        print();
        printList();
        MemView();
    }
    
};




int main(int argc,char* argv[]) {
    test a;
    a.test1();

    return 0;
}

























