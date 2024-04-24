#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <chrono>
#include <string>
#include <jemalloc/jemalloc.h>
#include <umalloc.h>
#include "tlsf.h"

class Allocator {
public:
    bool needInit = false;
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* ptr) = 0;
    virtual void initialization(size_t size) = 0;
    virtual void determination() = 0;
    virtual std::string name() const = 0;
    virtual ~Allocator() {}
};

class MallocAllocator : public Allocator {
public:
    void* allocate(size_t size) override {
        return malloc(size);
    }

    void deallocate(void* ptr) override {
        free(ptr);
    }

    void initialization(size_t size) override {
        return;
    }

    void determination() override {
        return;
    }

    std::string name() const override {
        return "Malloc()";
    }
};

class JemallocAllocator : public Allocator {
public:

    void* allocate(size_t size) override {
        return mallocx(size, 0);
    }

    void deallocate(void* ptr) override {
        dallocx(ptr, 0);
    }

    void initialization(size_t size) override {
        return;
    }

    void determination() override {
        return;
    }

    std::string name() const override {
        return "Jemalloc()";
    }
};

class UmallocAllocator : public Allocator {
public:

    void* allocate(size_t size) override {
        return umalloc(size);
    }

    void deallocate(void* ptr) override {
        ufree(ptr);
    }

    void initialization(size_t size) override {
        return;
    }

    void determination() override {
        return;
    }

    std::string name() const override {
        return "Umalloc()";
    }
};

class TLSFAllocator : public Allocator {
public:
    void* heapptr;
    tlsf_t tlsf;
    bool needInit = true;

    void* allocate(size_t size) override {
        return tlsf_malloc(tlsf, 8);
    }

    void deallocate(void* ptr) override {
        tlsf_free(tlsf, ptr);
    }

    void initialization(size_t size) override {
        heapptr = malloc(size * 8);
        tlsf = tlsf_create_with_pool(heapptr, size * 8);
    }

    void determination() override {
        free(heapptr);
    }

    std::string name() const override {
        return "TLSF()";
    }
};

const std::vector<int64_t> operations_count = {10000000, 100000000, 500000000, 1000000000};

enum {
    OPERATIONS_COUNT = 100000000,
    THREADS_COUNT = 2,
    BLOCK_SIZE = 8,
    TEST_REPEATS = 5
};

void test(Allocator& allocator, int64_t test_times) {
    for (int i = 0; i < test_times; ++i) {
        void* memory = allocator.allocate(BLOCK_SIZE);
        allocator.deallocate(memory);
    }
}

void* thread_func(void* arg, Allocator& allocator, int64_t test_times) {
    test(allocator, test_times);
    return NULL;
}

double get_time(void (*func)(Allocator& allocator, int64_t test_times), Allocator& allocator, int64_t test_times) {
    if (allocator.needInit) {
        allocator.initialization(test_times);
    }
    auto start = std::chrono::high_resolution_clock::now();
    
    func(allocator, test_times);

    auto end = std::chrono::high_resolution_clock::now();
    auto time_taken = std::chrono::duration<double>(end-start).count();
    if (allocator.needInit) {
        allocator.determination();
    }
    return (double)(time_taken);
}

void single_proc_test(Allocator& allocator) {
    double elapsed_time;
    std::cout << allocator.name() << " single process with one thread:\n";
    for (int j = 0; j < operations_count.size(); ++j) {
        double single_time_sum = 0.0f;
        single_time_sum += get_time(test, allocator, operations_count[j]);
        std::cout << "For " << operations_count[j] << " repeats: Elapsed time: " << single_time_sum <<" seconds\n";
    }
    
    for (int j = 0; j < operations_count.size(); ++j) {
        std::cout << allocator.name() << " two processes on dual processor:\n";
        pid_t pid = fork();
        if (pid == 0) {
            elapsed_time = get_time(test, allocator, operations_count[j]);
            std::cout << "For " << operations_count[j] << " repeats: " << allocator.name() << " child process elapsed time: " << elapsed_time << " seconds\n";
            exit(0);
        } else if (pid > 0) {
            elapsed_time = get_time(test, allocator, operations_count[j]);
            std::cout << "For " << operations_count[j] << " repeats: " << allocator.name() << " parent process elapsed time: " << elapsed_time << " seconds\n";
            wait(NULL);
        } else {
            perror("Failed to fork");
        }
    }
}

int main() {

    MallocAllocator mallocAllocator;
    JemallocAllocator jemallocAllocator;
    UmallocAllocator umallocAllocator;
    TLSFAllocator tlsfallocator;

    single_proc_test(jemallocAllocator);
    single_proc_test(mallocAllocator);
    single_proc_test(umallocAllocator);
    single_proc_test(tlsfallocator);

    return 0;
}
