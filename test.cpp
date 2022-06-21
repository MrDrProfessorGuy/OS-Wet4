

#include "malloc_3.cpp"
#include "iostream"


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
        for (BlockMetadata* iter = &list.head; iter != nullptr ; iter = (BlockMetadata*)(((char*)iter )+ iter->size + METADATA_SIZE)) {
            std::cout << "----------- BlockMetaData ["<< counter <<"] -----------" << std::endl;
            std::cout << "      -size= " << iter->size << "     ||      -is_free= " << iter->is_free << std::endl;
            std::cout << "      -next= " << iter->next << "     ||      -prev= " << iter->prev << std::endl;
            std::cout << "      -next_free= " << iter->next_free << "       ||      -prev= " << iter->prev_free << std::endl;
            std::cout << "---------------------------------------" << std::endl;
            counter++;
        }
        
        
    }
    
    void test1(){
        print();
        printHeap();
        void* a = smalloc(10);
        print();
        printHeap();
        
    }
    
};




int main(int argc,char* argv[]) {
    test a;
    a.test1();

    return 0;
}

























