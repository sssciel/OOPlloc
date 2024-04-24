#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sys/mman.h>
#include <chrono>
#include <stack>

#define POOL_SIZE (1 << 10) * 100000
#define BLOCK_SIZE 8

std::vector<int> times = {(POOL_SIZE / BLOCK_SIZE) / 2, POOL_SIZE / BLOCK_SIZE};
std::vector<double> results;

int main() {
    for (int j = 0; j < times.size(); ++j) {
        auto start = std::chrono::high_resolution_clock::now();

        void* ptr = mmap(NULL, POOL_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == MAP_FAILED) {
            perror("mmap");
            exit(EXIT_FAILURE);
        }

        std::stack<char*> stack_of_free;

        for (int i = 0; i < times[j]; ++ i) {
            stack_of_free.push(static_cast<char*>(ptr) + i * BLOCK_SIZE);
        }

        munmap(ptr, POOL_SIZE);

        auto end = std::chrono::high_resolution_clock::now();
        auto time_taken = std::chrono::duration<double>(end-start).count();
        std::cout << "For " << times[j] << " times, total time: " << time_taken << "s.\n";
        results.emplace_back(time_taken);
    }
    std::cout << "Total diff: " << (results[1] - results[0]) / results[0] * 100 << "\n";
}