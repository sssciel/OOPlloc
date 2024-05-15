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
#define DEFAULT_PAGE_SIZE (1 << 16)

//------------------------------------------------------------------------------
// Declarations
//------------------------------------------------------------------------------
/*
Block struct
*/
struct oBlock {
    oBlock* block;
};

/*
Allocator struct
*/
struct OOPLloc_Allocator {
    oBlock* freeStack = nullptr; // Список свободных блоков
    AtomicStack pageStack; // Стэк инициализированных страниц

    void* mainAdress = nullptr; // Адресс блока, на котором остановлено аллоцирование
    UI1 blockSize; // Размер блока
    UI1 pageSize; // Размер страницы
    UI1 usedBlocks; // Количество использованных блоков
    UI1 allocatedPages; // Количество аллоцированных страниц
    UI1 pageCapacity; // Вместимость страницы

    /* Constructors */
    OOPLloc_Allocator();
    OOPLloc_Allocator(UI1 bSize);
    OOPLloc_Allocator(UI1 bSize, UI1 pSize);

    /* Destructors */
    ~OOPLloc_Allocator();

    /* Service functions */
    inline UI1 getBlockSize();
    inline UI1 getPageSize();
    UI1 getUsedBlocks();
    inline bool isEnoughMem();

    void allocateNewPage(); // Инициализировать новую страницу
    inline void blockPush(void* child); // Вставить блок в список
    inline void* blockPop(); // Получить блок из списка

    inline void* getNewBlock(); // Получить новый блок

    /* Main functions */
    void* alloc(); // Аллоцирование
    void free(void* p); // Освобождение

    template <typename T>
    void* ialloc(const T& source); // Аллоцирование с имплементацией

    void arrlloc(std::vector<void*>& vec); // Заполняет вектор необходимым количеством аллоцированных адресов
    void arrlloc(std::vector<void*>& vec, UI1 count); // Заполняет вектор необходимым количеством аллоцированных адресов

    void arrfree(std::vector<void*>& vec); // Освобождает вектор адресов
};

#endif /* OOPLLOC_H */