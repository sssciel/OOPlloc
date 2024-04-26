#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <assert.h>
#include <sys/mman.h>
#include "ooplloc.h"
#include "atomicStructs.h"

/*
Since we have two modes, in order not to use  
#ifdef preprocessor inserts inside the code, 
we declare the functions as empty if 
THREAD_SAFE mode is not used
*/

#ifdef THREAD_SAFE
    #include "mutexLock.h"
#else
    #define MuteON(a)
    #define MuteOFF(a)
    #define MuteCreate() (nullptr)
    #define MuteDeterm(a)
#endif

#define NORMILIZE_POINTER(ptr) \
    (ptr ? ((void*)((char*)ptr)) : NULL)

#define POINTER_ADD(ptr, a) \
    ((char*)ptr + a)

static void* mutexPtr;

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
Functions for working with the list of blocks.
They use simple pointers and their movements.
Mutex is used for atomicity
*/

// blockPush() add new free block to list.
void OOPLloc_Allocator::blockPush(void* ptr) 
{
    if (ptr == nullptr)
    {
        std::cerr << "Tried to add empty block to list";
        return;
    }

    // Get pointer to oBlock struct
    oBlock* block = (oBlock*)NORMILIZE_POINTER(ptr);

    MuteON(mutexPtr);

    // Set next block in list
    block->block = freeStack;

    // Set new first element in list
    freeStack = block;

    MuteOFF(mutexPtr);
}

// blockPop() return address of free block or return NULLPTR 
// when it is impossible.
void* OOPLloc_Allocator::blockPop() 
{
    if (freeStack == nullptr) 
        return nullptr;

    MuteON(mutexPtr);

    // Get a normilized adress as void*
    void* newAddress = NORMILIZE_POINTER(freeStack);

    // Change first block
    freeStack = freeStack->block;

    MuteOFF(mutexPtr);

    return newAddress;
}

// allocateNewPage() allocates the memory available for the allocator.
void OOPLloc_Allocator::allocateNewPage() 
{
    void* ptr;

    // Пробуем уменьшить размер страницы до тех пор, пока не сможем аллоцировать ее
    while((ptr = mmap(NULL, pageSize, PROT_ACCESS, MAP_ACCESS, -1, 0)) == MAP_FAILED)
    {
        std::cerr << "Can't mmap " << pageSize << " bytes page";
        pageSize /= 2;
    };

    // Сохраняем адрес страницы, с которого начнется разметка
    mainAdress = ptr;

    // Поскольку мы могли уменьшить размер страницы, мы изменяем вместимость
    pageCapacity = pageSize / blockSize - 1;

    // Изменяем количество используемых блоков и страниц
    usedBlocks = 1;
    ++allocatedPages;

    // Добавляем только что инициализированный блок, а также страницу в список и стэк соответственно
    blockPush(ptr);
    pageStack.push(ptr);
}

// getNewBlock() returns new free block
void* OOPLloc_Allocator::getNewBlock() 
{
    if (isEnoughMem()) 
    {
        // Берем следующий из неразлинованных блоков
        mainAdress = NORMILIZE_POINTER(POINTER_ADD(mainAdress, blockSize));

        ++usedBlocks;

        return mainAdress;

    } else 
    {
        // Если не хватает памяти, инициализируем новую страницу
        allocateNewPage();

        return blockPop();
    }
}

/*
Without arguments, the constructor takes the default values declared in the header file. 
Memory allocation takes place via mmap, addresses are stored on the stack, 
for possible cleaning, if there is not enough memory.
*/
OOPLloc_Allocator::OOPLloc_Allocator() : pageSize(DEFAULT_PAGE_SIZE), blockSize(DEFAULT_BLOCK_SIZE)
{
    allocatedPages = 0;

    // Создаем мьютекс для THREAD_SAFE
    mutexPtr = MuteCreate();

    // Инициализируем первую страницу
    allocateNewPage();
}

OOPLloc_Allocator::~OOPLloc_Allocator() 
{
    // Пока все еще есть инициализированные страницы, удаляем их
    while(allocatedPages--) 
    {
        if (munmap(pageStack.pop(), pageSize) == -1) 
        {
            std::cerr << "munmap failed" << std::endl;
        }
    }

    // Удаляем мьютекс
    MuteDeterm(mutexPtr);
}

OOPLloc_Allocator::OOPLloc_Allocator(UI1 pSize) : pageSize(pSize), blockSize(DEFAULT_BLOCK_SIZE)
{
    allocatedPages = 0;

    // Создаем мьютекс для THREAD_SAFE
    mutexPtr = MuteCreate();

    // Инициализируем первую страницу
    allocateNewPage();
}

OOPLloc_Allocator::OOPLloc_Allocator(UI1 pSize, UI1 bSize) : pageSize(pSize), blockSize(bSize)
{
    allocatedPages = 0;

    // Создаем мьютекс для THREAD_SAFE
    mutexPtr = MuteCreate();

    // Инициализируем первую страницу
    allocateNewPage();
} /*end cunstructors*/

/*
Основные функции основанны на сервисных, по сути
они почти ничего не делают, кроме как красиво оборачивают их
*/

// alloc() аллоцирует блок памяти и возвращает адрес
void* OOPLloc_Allocator::alloc() 
{
    // Проверяем то, что блок влезает в страницу
    assert(pageSize >= blockSize);

    // Если список не свободен, выделяем новый блок
    if (freeStack == nullptr) {
        return getNewBlock();
    }

    // Если же свободен, просто берем первый
    return blockPop();
}

void OOPLloc_Allocator::free(void* p) 
{
    // Просто добавляем блок в список
    blockPush(p);
}

// Функция, которая записывает данные в ячейку памяти
template <typename T>
void* OOPLloc_Allocator::ialloc(const T& source) {

    // Ограничиваем размер до 8 байт
    assert(sizeof(T) <= blockSize);

    // Получаем новый блок
    auto ptr = this->alloc();

    // Копируем данные из source в destination
    std::memmove(ptr, &source, sizeof(T));
}

void OOPLloc_Allocator::arrlloc(std::vector<void*>& vec) {

    // Проверяем, что вектор не пустой
    assert(vec.empty());

    // Заполняем массив ссылками на элементы вектора
    for (UI1 i = 0; i < vec.size(); ++i) {
        vec[i] = this->alloc();
    }
}

void OOPLloc_Allocator::arrlloc(std::vector<void*>& vec, UI1 count) {

    assert(count);

    // Заполняем массив ссылками на элементы вектора
    for (UI1 i = 0; i < count; ++i) {
        vec.push_back(this->alloc());
    }
}

void OOPLloc_Allocator::arrfree(std::vector<void*>& vec) {

    assert(vec.size());

    // Освобождаем массив ссылками на элементы вектора
    for (UI1 i = 0; i < vec.size(); ++i) {
        this->free(vec[i]);
    }
}