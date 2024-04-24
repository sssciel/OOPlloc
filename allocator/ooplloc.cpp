#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>
#include "ooplloc.h"

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
    return getUsedBlocks() + 1 <= (getPageSize() / getBlockSize());
}

/*
Functions for working with the freelist stack of pages and blocks. 
They use simple pointers and their movements. 
Mutex is used for atomicity
*/
// allocateNewPage() allocates the memory available for the allocator.
void OOPLloc_Allocator::allocateNewPage() {
    void* ptr = mmap(NULL, getPageSize(), PROT_ACCESS, MAP_ACCESS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    mainAdress = ptr;
    usedBlocks = 0;
    blockPush(ptr);
    pagePush(ptr);
}

// blockPush() add new free block to list.
void OOPLloc_Allocator::blockPush(void* child) {
    if (!child) {
        #ifdef DEBUG
            assert(child)
        #endif
        return;
    }

    oBlock* block = static_cast<oBlock*>(child);
    block->block = freeStack;
    freeStack = block;
}

// blockPop() return address of free block or return NULLPTR when it is impossible.
void* OOPLloc_Allocator::blockPop() {
    if (!freeStack) {
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

void* OOPLloc_Allocator::getNewBlock() {
    if (isEnoughMem()) {
        mainAdress = static_cast<void*>(static_cast<char*>(mainAdress) + getBlockSize());
        ++usedBlocks;
        return mainAdress;
    } else {
        allocateNewPage();
        return blockPop();
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
}

OOPLloc_Allocator::~OOPLloc_Allocator() {
}

OOPLloc_Allocator::OOPLloc_Allocator(UI1 pSize) {
    pageSize = pSize;
    blockSize = DEFAULT_BLOCK_SIZE;
}

OOPLloc_Allocator::OOPLloc_Allocator(UI1 pSize, UI1 bSize) {
    pageSize = pSize;
    blockSize = bSize;
} 
/*end cunstructors*/

void* OOPLloc_Allocator::alloc() {
    void* freeBlock = blockPop();
    if (!freeBlock) {
        return freeBlock;
    }

    return getNewBlock();
}

void OOPLloc_Allocator::free(void* p) {
    blockPush(p);
}