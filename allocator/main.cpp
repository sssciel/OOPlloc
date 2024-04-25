#include <iostream>
#include <chrono>
#include <stack>
#include <vector>
#include <memory>
#include <stack>
#include "ooplloc.h"

#define U1 unsigned long
#define N1 nullptr

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ declarations
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

class Slab {
public:
    Slab(size_t blockSize, size_t blockCount)
        : blockSize(blockSize), blockCount(blockCount), data(new char[blockSize * blockCount]) {
        // Инициализируем стек свободных блоков
        for (size_t i = 0; i < blockCount; ++i) {
            freeList.push(data.get() + i * blockSize);
        }
    }

    void* allocate() {
        if (freeList.empty()) {
            return nullptr; // Если нет свободных блоков, возвращаем nullptr
        }
        void* block = freeList.top();
        freeList.pop();
        return block;
    }

    void deallocate(void* block) {
        freeList.push(block); // Возвращаем блок обратно в стек
    }

    bool isEmpty() const {
        return freeList.size() == blockCount; // Проверяем, все ли блоки свободны
    }

private:
    size_t blockSize;
    size_t blockCount;
    std::unique_ptr<char[]> data; // Храним данные в массиве char
    std::stack<void*> freeList; // Стек свободных блоков
};

class SlabAllocator {
public:
    SlabAllocator(size_t blockSize, size_t blockCount, size_t slabCount)
        : blockSize(blockSize), blockCount(blockCount) {
        // Инициализируем набор "слабов"
        for (size_t i = 0; i < slabCount; ++i) {
            slabs.push_back(std::make_unique<Slab>(blockSize, blockCount));
        }
    }

    void* allocate() {
        for (auto& slab : slabs) {
            void* block = slab->allocate();
            if (block != nullptr) {
                return block; // Возвращаем блок, если он есть
            }
        }

        // Если нет свободных блоков, добавляем новый "слаб"
        auto newSlab = std::make_unique<Slab>(blockSize, blockCount);
        void* block = newSlab->allocate();
        slabs.push_back(std::move(newSlab)); // Добавляем новый "слаб" в список
        return block;
    }

    void deallocate(void* block) {
        // Находим первый "слаб", который содержит блок
        for (auto& slab : slabs) {
            if (slab->isEmpty()) {
                slab->deallocate(block);
                break;
            }
        }
    }

private:
    size_t blockSize;
    size_t blockCount;
    std::vector<std::unique_ptr<Slab>> slabs; // Набор "слабов"
};

double leverSingleSlab(UI1 iterationTimes) {
    SlabAllocator all(8, iterationTimes, 10);
    auto start = std::chrono::high_resolution_clock::now();

    std::stack<void*> allocations;

    for (UI1 i = 0; i < iterationTimes / 2; ++i) {
        void *p = all.allocate();
        allocations.push(p);
    }

    //std::cout << allocations.size() << "\n";

    for (UI1 i = 0; i < iterationTimes / 2; ++i) {
        void* p = allocations.top();
        all.deallocate(p);
        allocations.pop();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto testRunTime = std::chrono::duration<double>(end-start).count();
    
    return testRunTime;
}

// double leverSingleSSS(UI1 iterationTimes) {
//     free_class asdjad(8, iterationTimes * 8);
//     auto start = std::chrono::high_resolution_clock::now();

//     std::stack<void*> allocations;

//     for (UI1 i = 0; i < iterationTimes; ++i) {
//         auto p = free_class.allocate();
//         free(p);
//         //allocations.push(p);
//     }
//     // for (UI1 i = 0; i < iterationTimes; ++i) {
//     //     void* p = allocations.top();
//     //     free(p);
//     //     allocations.pop();
//     // }

//     auto end = std::chrono::high_resolution_clock::now();
//     auto testRunTime = std::chrono::duration<double>(end-start).count();
    
//     return testRunTime;
// }


double leverSingleMalloc(UI1 iterationTimes) {
    auto start = std::chrono::high_resolution_clock::now();

    std::stack<void*> allocations;

    for (UI1 i = 0; i < iterationTimes / 2; ++i) {
        auto p = malloc(DEFAULT_BLOCK_SIZE);
        //free(p);
        allocations.push(p);
    }
    for (UI1 i = 0; i < iterationTimes / 2; ++i) {
        void* p = allocations.top();
        free(p);
        allocations.pop();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto testRunTime = std::chrono::duration<double>(end-start).count();
    
    return testRunTime;
}

double leverSingleOOPlloc(UI1 iterationTimes) {
    OOPLloc_Allocator all(iterationTimes * 8, 8);
    auto start = std::chrono::high_resolution_clock::now();

    std::stack<void*> allocations;

    for (UI1 i = 0; i < iterationTimes / 2; ++i) {
        void *p = all.alloc();
        allocations.push(p);
    }

    //std::cout << allocations.size() << "\n";

    for (UI1 i = 0; i < iterationTimes / 2; ++i) {
        void* p = allocations.top();
        all.free(p);
        allocations.pop();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto testRunTime = std::chrono::duration<double>(end-start).count();
    
    return testRunTime;
}

std::vector<UI1> times = {10000000};

int main() {
    for (UI1 i = 0; times.size(); ++i) {
        auto p1 = leverSingleMalloc(times[i]);
        auto p2 = leverSingleOOPlloc(times[i]);
        auto p3 = leverSingleSlab(times[i]);
        std::cout << "Malloc time for " << times[i] << " is " << p1 << "\n";
        std::cout << "OOPlloc time for " << times[i] << " is " << p2 << "\n";
        std::cout << "Slab time for " << times[i] << " is " << p3 << "\n";
        std::cout << "Total diff: " << (p1 - p2) / p2 * 100 << "\n";
    }
    // auto p = all.alloc();
    // all.free(p);

    // OOPLloc_Allocator all(8,8);
    // std::cout << all.mainAdress << " " << static_cast<void*>(static_cast<char*>(all.mainAdress) + 8) << " " << all.getPageSize() << "\n";
    // auto p1 = all.alloc();
    // int* i1 = static_cast<int*>(p1);
    // std::cout << i1 << " " << *i1 << "\n";
    // *i1 = 10;
    // std::cout << *i1 << "\n";

    // auto p2 = all.alloc();
    // int* i2 = static_cast<int*>(p2);
    // std::cout << i2 << " " << *i2 << "\n";
    // *i2 = 10;
    // std::cout << *i2 << "\n";
    //std::cout << all.allocatedPages << "\n";
    return 0;
}