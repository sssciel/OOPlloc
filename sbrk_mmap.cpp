#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <sys/mman.h>

#define ITERATIONS 1000
#define MEM_SIZE (1 << 5)

void _sbrk() {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; ++i) {
        void* ptr = sbrk(MEM_SIZE);
        if (ptr == (void*)-1) {
            perror("sbrk");
            exit(EXIT_FAILURE);
        }
        sbrk(-MEM_SIZE);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto time_taken = std::chrono::duration<double>(end-start).count();

    std::cout << "sbrk() took " << static_cast<double>(time_taken) << "microseconds\n";
}

void _mmap() {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; ++i) {
        void* ptr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }
        munmap(ptr, MEM_SIZE);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto time_taken = std::chrono::duration<double>(end-start).count();

    std::cout << "mmap() took " << static_cast<double>(time_taken) << "microseconds\n";
}

int main() {
    std::cout << "Benchmarking sbrk()...\n";
    _sbrk();

    std::cout << "\nBenchmarking mmap()...\n";
    _mmap();

    return 0;
}
