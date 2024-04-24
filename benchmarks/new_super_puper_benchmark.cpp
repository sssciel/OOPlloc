#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <stack>

#ifdef STRUCTSTEST
#include "../implementations/sss.cpp"
#include "../implementations/segmented.cpp"
#include "../implementations/slab.cpp"
#else
#include <jemalloc/jemalloc.h>
#include <umalloc.h>
#include <tlsf.h>
#endif

/*
    Shrotcuts
*/
#define str std::string
#define U1 unsigned long
#define U2 unsigned long long

/*
    Allocator's Names
*/
#ifdef STRUCTSTEST
#define sssName "SSS()"
#define slabName "Slab()"
#define segmentedName "Segmented()"
#else
#define mallocName "Malloc()"
#define jemallocName "jeMalloc()"
#define umallocName "uMalloc()"
#define tlsfName "TLSF()"
#endif

/*
    Variables
*/
#define BLOCK_SIZE 8
#define ITERATIONS_TIMES_LEVER 1e8
#define ITERATIONS_TIMES_RANDOM 1e6
#define RANDOM_TEST_TIMES 3

#ifdef STRUCTSTEST
std::vector<U1> iterationsLever = {10000, 1000000, 10000000, 100000000};
#else
std::vector<U1> iterationsLever = {10000, 1000000, 10000000, 100000000, 200000000};
#endif
std::vector<U1> iterationsRandom = {1000, 10000, 100000, 1000000};

str leverSingleStr =  "LEVER BOHERAM SINGLE PROC:\n";
str leverDoubleStr = "LEVER BOHERAM DOUBLE PROC:\n";
str randomDeallocStr = "RANDOM DEALLOCATION TEST:\n";

/*
    Base Class
*/
class Allocator {
public:
    bool needInit = false;
    str name = "";

    Allocator(const std::string& a, const bool b) : name(a), needInit(b) {}

    virtual void* allocate(size_t size) { return 0; };
    virtual void deallocate(void* ptr) {};
    virtual void initialization(size_t size) {};
    virtual void determination() {};
    virtual ~Allocator() {}

    str getName() {
        return name;
    };
    bool isInitialized() {
        return needInit;
    }
};
#ifdef STRUCTSTEST
class SSSAllocator : public Allocator {
public:
    void* heapptr;
    free_class* freeclass;

    SSSAllocator(const std::string& name, const bool needInit) : Allocator(name, needInit) {}

    void* allocate(size_t size) override {
        return freeclass->allocate();
    }

    void deallocate(void* ptr) override {
        freeclass->deallocate(ptr);
    }

    void initialization(size_t size) override {
        auto sbr = new free_class(BLOCK_SIZE, size);
        freeclass = sbr;
    }
};

class SlabAllocator : public Allocator {
public:
    void* heapptr;
    slabs_class* freeclass;

    SlabAllocator(const std::string& name, const bool needInit) : Allocator(name, needInit) {}

    void* allocate(size_t size) override {
        return freeclass->allocate();
    }

    void deallocate(void* ptr) override {
        freeclass->deallocate(ptr);
    }

    void initialization(size_t size) override {
        auto sbr = new slabs_class(BLOCK_SIZE, size);
        freeclass = sbr;
    }
};

class SegmentedAllocator : public Allocator {
public:
    void* heapptr;
    block_class* freeclass;

    SegmentedAllocator(const std::string& name, const bool needInit) : Allocator(name, needInit) {}

    void* allocate(size_t size) override {
        return freeclass->allocate(size);
    }

    void deallocate(void* ptr) override {
        freeclass->deallocate(ptr);
    }

    void initialization(size_t size) override {
        auto sbr = new block_class(BLOCK_SIZE * size);
        freeclass = sbr;
    }
};

#else
class MallocAllocator : public Allocator {
public:

    MallocAllocator(const std::string& name, const bool needInit) : Allocator(name, needInit) {}

    void* allocate(size_t size) override {
        return malloc(size);
    }

    void deallocate(void* ptr) override {
        free(ptr);
    }
};

class JemallocAllocator : public Allocator {
public:

    JemallocAllocator(const std::string& name, const bool needInit) : Allocator(name, needInit) {}

    void* allocate(size_t size) override {
        return mallocx(size, 0);
    }

    void deallocate(void* ptr) override {
        dallocx(ptr, 0);
    }
};

class UmallocAllocator : public Allocator {
public:

    UmallocAllocator(const std::string& name, const bool needInit) : Allocator(name, needInit) {}

    void* allocate(size_t size) override {
        return umalloc(size);
    }

    void deallocate(void* ptr) override {
        ufree(ptr);
    }
};

class TLSFAllocator : public Allocator {
public:
    void* heapptr;
    tlsf_t tlsf;

    TLSFAllocator(const std::string& name, const bool needInit) : Allocator(name, needInit) {}

    void* allocate(size_t size) override {
        return tlsf_malloc(tlsf, 8);
    }

    void deallocate(void* ptr) override {
        tlsf_free(tlsf, ptr);
    }

    void initialization(size_t size) override {
        heapptr = malloc(size);
        tlsf = tlsf_create_with_pool(heapptr, size);
    }

    void determination() override {
        free(heapptr);
    }
};

#endif

// leverSingle() checks big allocations and dealocations times in one proc
double leverSingle(Allocator& t, U1 iterationTimes) {
    auto start = std::chrono::high_resolution_clock::now();

    std::stack<void*> allocations;
    if (t.isInitialized()) {
        t.initialization(iterationTimes * BLOCK_SIZE);
    }
    for (U1 i = 0; i < iterationTimes; ++i) {
        auto p = t.allocate(BLOCK_SIZE);
        allocations.push(p);
    }
    for (U1 i = 0; i < iterationTimes; ++i) {
        void* p = allocations.top();
        t.deallocate(p);
        allocations.pop();
    }
    if (t.isInitialized()) {
        t.determination();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto testRunTime = std::chrono::duration<double>(end-start).count();
    
    return testRunTime;
}

// leverMulti() checks big allocations and dealocations times in two proc
double leverMulti(Allocator& t, U1 iterationTimes) {
    auto start = std::chrono::high_resolution_clock::now();

    pid_t pid = fork();

    if (pid == 0) {
        double runTime = leverSingle(t, iterationTimes);
        exit(0);
    } else if (pid > 0) {
        double runTime = leverSingle(t, iterationTimes);
        wait(NULL);
    } else {
        perror("Failed to fork");
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto testRunTime = std::chrono::duration<double>(end-start).count();
    
    return testRunTime;
}

// randomAllocations() is a fragmentation test
double randomAllocations(Allocator& t, U1 iterationTimes) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> genDealloc(0, 99);

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<void*> allocations;
    if (t.isInitialized()) {
        t.initialization(iterationTimes * BLOCK_SIZE * RANDOM_TEST_TIMES);
    }

    for (U1 j = 0; j < RANDOM_TEST_TIMES; ++j) {
        for (U1 i = 0; i < iterationTimes; ++i) {
            auto p = t.allocate(BLOCK_SIZE);
            int rnd = genDealloc(rng);
            if (rnd % 2) {
                t.deallocate(p);
            } else {
                allocations.emplace_back(p);
            }
        }

        for (U1 i = 0; i < allocations.size() / 2; ++i) {
            std::uniform_int_distribution<int> rndDealloc(0, allocations.size() - 1);
            int rnd = rndDealloc(rng);
            t.deallocate(allocations[rnd]);
            allocations.erase(allocations.begin() + rnd);
        }
    }

    for (U1 i = 0; i < allocations.size(); ++i) {
        t.deallocate(allocations[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto testRunTime = std::chrono::duration<double>(end-start).count();
    
    return testRunTime;
}

void printStars(str& s) {
    for (U1 i = 0; i < s.size() - 2; ++i) {
        std::cout << "*";
    }
    std::cout << "\n";
    std::cout << s << "\n";
}

void leverSingleTest(Allocator& t) {
    std::cout << t.getName() << "\n";
    for (U1 j = 0; j < iterationsLever.size(); ++j) {
        auto p = iterationsLever[j];
        double runTime = leverSingle(t, iterationsLever[j]);
        std::cout << t.getName() << " exec time of Lever test for one proc with " << p << " allocations is\n\t" << runTime << "s.\n\n";
    }    
}

void leverDoubleTest(Allocator& t) {
    std::cout << t.getName() << "\n";
    for (U1 j = 0; j < iterationsLever.size(); ++j) {
        auto p = iterationsLever[j];
        double runTime = leverMulti(t, iterationsLever[j]);
        std::cout << t.getName() << " exec time of Lever test for two procs with " << p << " allocations is\n\t" << runTime << "s.\n\n";
    }  
}

void randomDeallocTest(Allocator& t) {
    std::cout << t.getName() << "\n";
    for (U1 j = 0; j < iterationsRandom.size(); ++j) {
        auto p = iterationsRandom[j];
        double runTime = randomAllocations(t, iterationsRandom[j]);
        std::cout << t.getName() << " exec time of Random deallocation test with " << p << " tries is\n\t" << runTime << "s.\n\n";
    }    
}

int main() {
    #ifdef STRUCTSTEST
    SSSAllocator sss(sssName, true);
    SlabAllocator slab(slabName, true);
    SegmentedAllocator segmented(segmentedName, true);

    printStars(leverSingleStr);

    leverSingleTest(sss);
    leverSingleTest(slab);
    leverSingleTest(segmented);

    printStars(leverDoubleStr);

    leverDoubleTest(sss);
    leverDoubleTest(slab);
    leverDoubleTest(segmented);

    #else
    MallocAllocator malloc(mallocName, false);
    JemallocAllocator jemalloc(jemallocName, false);
    UmallocAllocator umalloc(umallocName, false);
    TLSFAllocator tlsf(tlsfName, true);

    printStars(leverSingleStr);

    leverSingleTest(malloc);
    leverSingleTest(umalloc);
    leverSingleTest(jemalloc);
    leverSingleTest(tlsf);

    printStars(leverDoubleStr);

    leverDoubleTest(malloc);
    leverDoubleTest(umalloc);
    leverDoubleTest(jemalloc);
    leverDoubleTest(tlsf);

    printStars(randomDeallocStr);

    randomDeallocTest(malloc);
    randomDeallocTest(umalloc);
    randomDeallocTest(jemalloc);
    randomDeallocTest(tlsf);
    #endif
}