#include <iostream>
#include <vector>

#define U1 unsigned long
#define N1 nullptr
#define DECLARED_BLOCKS 32

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declarations
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

struct slab_class {
    void* mainadress;
    U1 sizeOfBlock;
    U1 sizeOfPool; // number of blocks in Slab
    U1 usedCount;
    std::vector<bool> bitPool; // used vector

    void allocatePool() {
        mainadress = ::operator new(sizeOfBlock * sizeOfPool);
        bitPool.assign(sizeOfPool, 1);    
    }

    slab_class(U1 a, U1 b) : sizeOfBlock(a), sizeOfPool(b), usedCount() {
        allocatePool();
    }

    void* allocate() {
        for (size_t i = 0; i < sizeOfPool; ++i) {
            if (bitPool[i] == 1) {
                bitPool[i] = 0;
                ++usedCount;
                return static_cast<char*>(mainadress) + i * sizeOfBlock; // get new adress
            }
        }
        return nullptr;
    }

    void deallocate(void* ptr) {
        U1 index = (static_cast<char*>(ptr) - static_cast<char*>(mainadress)) / sizeOfBlock; // get adress
        --usedCount;
        bitPool[index] = 1;
    }
};

class slabs_class {
private:
    std::vector<slab_class> pools;
    U1 sizeOfBlock;

public:
    slabs_class(size_t a) : sizeOfBlock(a) {}

    void* allocate() {
        for (auto& slab : pools) {
            if (!(slab.usedCount == slab.sizeOfPool)) {
                return slab.allocate();
            }
        }
        pools.emplace_back(sizeOfBlock, DECLARED_BLOCKS);
        return pools.back().allocate();
    }

    void deallocate(void* ptr) {
        for (auto& slab : pools) {
            if (ptr >= slab.mainadress && ptr < static_cast<char*>(slab.mainadress) + (slab.sizeOfPool * slab.sizeOfBlock)) {
                slab.deallocate(ptr);
                return;
            }
        }
    }
};