#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <sys/mman.h>
#include "ooplloc.h"
#ifndef THREAD_PROTECTION
#include "atomicStructs.h"
#endif

//------------------------------------------------------------------------------
// Definitions
//------------------------------------------------------------------------------
UI1 OOPLloc_Allocator::getBlockSize() {
    return blockSize;
}

UI1 OOPLloc_Allocator::getPageSize() {
    return pageSize;
}

UI1 OOPLloc_Allocator::getUsedBlocks() {
    return usedBlocks;
}

bool OOPLloc_Allocator::isEnoughMem() {
    return usedBlocks <= pageCapacity;
}

/*
Functions for working with the freelist stack of pages and blocks. 
They use simple pointers and their movements. 
Mutex is used for atomicity
*/
#ifdef THREAD_PROTECTION
// blockPush() add new free block to list.
void OOPLloc_Allocator::blockPush(void* child) {
    if (child == nullptr) return;
    oBlock* block = static_cast<oBlock*>(child);
    block->block = freeStack;
    freeStack = block;
}

// blockPop() return address of free block or return NULLPTR when it is impossible.
void* OOPLloc_Allocator::blockPop() {
    if (freeStack == nullptr) {
        return nullptr;
    }
    void* newAddress = static_cast<void*>(freeStack);
    freeStack = freeStack->block;
    return newAddress;
}

// pagePush() add new free page to list.
void OOPLloc_Allocator::pagePush(void* address) {
    if (!address) {
        #ifdef DEBUG
            assert(child)
        #endif
        return;
    }

    oPage* page = static_cast<oPage*>(address);
    page->page = pageStack;
    page->usedBlocks = 0;
    pageStack = page;
}
#endif

// allocateNewPage() allocates the memory available for the allocator.
void OOPLloc_Allocator::allocateNewPage() {
    void* ptr = mmap(NULL, pageSize, PROT_ACCESS, MAP_ACCESS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    mainAdress = ptr;
    pageCapacity = pageSize / blockSize - 1;
    usedBlocks = 1;
    #ifdef THREAD_PROTECTION
    blockPush(ptr);
    #else
    freeStack.push(ptr);
    pageStack.push(ptr);
    #endif
    ++allocatedPages;
}

void* OOPLloc_Allocator::getNewBlock() {
    if (isEnoughMem()) {
        mainAdress = static_cast<void*>(static_cast<char*>(mainAdress) + blockSize);
        ++usedBlocks;
        return mainAdress;
    } else {
        allocateNewPage();
        #ifdef THREAD_PROTECTION
        return blockPop();
        #else
        return freeStack.pop();
        #endif
    }
}
/*
Without arguments, the constructor takes the default values declared in the header file. 
Memory allocation takes place via mmap, addresses are stored on the stack, 
for possible cleaning, if there is not enough memory.
*/
OOPLloc_Allocator::OOPLloc_Allocator() {
    pageSize = DEFAULT_PAGE_SIZE;
    blockSize = DEFAULT_BLOCK_SIZE;
    allocatedPages = 0;
    allocateNewPage();
}

OOPLloc_Allocator::~OOPLloc_Allocator() {
    // #ifdef THREAD_PROTECTION
    // #else
    // while(allocatedPages--) {
    //     if (munmap(pageStack.pop(), pageSize) == -1) {
    // #endif
    //         std::cerr << "munmap failed" << std::endl;
    //     }
    // } 
}

OOPLloc_Allocator::OOPLloc_Allocator(UI1 pSize) {
    pageSize = pSize;
    blockSize = DEFAULT_BLOCK_SIZE;
    allocatedPages = 0;
    allocateNewPage();
}

OOPLloc_Allocator::OOPLloc_Allocator(UI1 pSize, UI1 bSize) {
    pageSize = pSize;
    blockSize = bSize;
    allocatedPages = 0;
    allocateNewPage();
} 
/*end cunstructors*/

void* OOPLloc_Allocator::alloc() {
    #ifdef THREAD_PROTECTION
    if (freeStack == nullptr) {
    #else
    if (freeStack.is_empty()) {
    #endif
        return getNewBlock();
    }
    #ifdef THREAD_PROTECTION
    return blockPop();
    #else
    return freeStack.pop();
    #endif
}

void OOPLloc_Allocator::free(void* p) {
    #ifdef THREAD_PROTECTION
    blockPush(p);
    #else
    freeStack.push(p);
    #endif
}