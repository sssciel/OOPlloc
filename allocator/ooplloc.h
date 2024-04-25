#ifndef OOPLLOC_H
#define OOPLLOC_H

#pragma once
#include "atomicStructs.h"

//------------------------------------------------------------------------------
// ShortCuts
//------------------------------------------------------------------------------
#define UI1 unsigned long
#define UI2 unsigned long long
#define str std::string
#define PROT_ACCESS PROT_READ | PROT_WRITE
#define MAP_ACCESS MAP_PRIVATE | MAP_ANONYMOUS

//------------------------------------------------------------------------------
// MagicNumbers
//------------------------------------------------------------------------------
#define DEFAULT_BLOCK_SIZE 8
#define DEFAULT_PAGE_SIZE (1 << 14)

//------------------------------------------------------------------------------
// Declarations
//------------------------------------------------------------------------------
#ifdef THREAD_PROTECTION
struct oBlock {
    oBlock* block;
};

/*
Freelist of pages
*/
struct oPage {
    oPage* page;
    UI1 usedBlocks;
};
#endif
/*
Allocator struct
*/
struct OOPLloc_Allocator {
    #ifdef THREAD_PROTECTION
    oBlock* freeStack = nullptr;
    oPage* pageStack = nullptr;
    #else
    AtomicFreeList freeStack; // stack of free blocks
    AtomicStack pageStack;
    #endif
    void* mainAdress = nullptr; // adress of last allocated block
    UI1 blockSize;
    UI1 pageSize;
    UI1 usedBlocks;
    UI1 allocatedPages;
    UI1 pageCapacity;

    /* Constructors */
    OOPLloc_Allocator();
    OOPLloc_Allocator(UI1 bSize);
    OOPLloc_Allocator(UI1 bSize, UI1 pSize);
    ~OOPLloc_Allocator();

    /* Service functions */
    UI1 getBlockSize(); // inline for optimization
    UI1 getPageSize();
    UI1 getUsedBlocks();
    inline bool isEnoughMem();

    void allocateNewPage();
    #ifdef THREAD_PROTECTION
    inline void blockPush(void* child);
    inline void* blockPop();

    void pagePush(void* address);
    void* pageDelete(void* address);
    #endif

    inline void* getNewBlock();

    /* Main functions */
    void* alloc();
    void free(void* p);
    void* calloc();
};

#endif /* OOPLLOC_H */