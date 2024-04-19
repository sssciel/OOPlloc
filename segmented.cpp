#include <iostream>
#include <vector>

#define U1 unsigned long
#define N1 nullptr

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declarations
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct block {
    void* mainadress;
    U1 sizeOfPool;
    U1 countOfUsed = 0;

    block(U1 a) : sizeOfPool(a) {
        mainadress = ::operator new(a);
    }

    void* allocate(U1 a) {
        if (sizeOfPool - a < countOfUsed) 
            return N1;
            
        void* block = static_cast<char*>(mainadress) + countOfUsed;
        countOfUsed += a;

        return block;
    }
};

class block_class {
    std::vector<block> blocks;
    U1 sizeOfBlock;

public:
    block_class(U1 a) : sizeOfBlock(a) {
    }

    void* allocate(U1 a) {
        for (auto& segment : blocks) {
            void* block = segment.allocate(a);
            if (block != N1) 
                return block;
        }

        blocks.emplace_back(sizeOfBlock);
        auto p = blocks.back().allocate(a);
        return p;
    }

    void deallocate(void* a) {
        for (auto& segment : blocks) {
            if (!(a >= static_cast<char*>(segment.mainadress))) 
                return;
            if (!(a < static_cast<char*>(segment.mainadress) + segment.countOfUsed))
                return;
            segment.countOfUsed = 0;
            return;
        }
    }
};