#include <iostream>

#define U1 unsigned long
#define N1 nullptr

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declarations
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct listfree {
    listfree* next; // next free block
};

class free_class {
private:
    listfree* list;
    U1 blockSize;
    U1 sizeOfPool; // number of blocks in pool
    void* mainadress; // adress of pool

    void allocatePool() {
        mainadress = ::operator new(blockSize * sizeOfPool); // memmory init
        char* currentBlock = static_cast<char*>(mainadress);
        for (size_t i = 0; i < sizeOfPool; ++i) {
            deallocate(currentBlock);
            currentBlock += blockSize;
        }
    }

public:

    free_class() {}
    
    free_class(U1 a, U1 b) : blockSize(a), sizeOfPool(b), list(N1), mainadress(N1) {
        allocatePool(); // init
    }

    void* allocate() {
        if (!list)
            allocatePool(); // expand free mainadress
        listfree* block = list;
        list = list->next;
        return block;
    }

    void deallocate(void* ptr) {
        listfree* block = static_cast<listfree*>(ptr);
        block->next = list;
        list = block;
    }
};