#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sys/mman.h>
#include <chrono>

#define ITERATIONS 1000
#define MEM_SIZE (1 << 5)

std::vector<int> times = {1, 10000};
std::vector<double> results;

int main() {
    for (int j = 0; j < times.size(); ++j) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < times[j]; ++i) {
            void* ptr = mmap(NULL, (1 << 10) * times[1 - j], PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
            if (ptr == MAP_FAILED) {
                perror("mmap");
                exit(EXIT_FAILURE);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto time_taken = std::chrono::duration<double>(end-start).count();
        std::cout << "MMap with " << times[j] << " times: " << time_taken << "s.\n";
        results.emplace_back(time_taken);
    }
    std::cout << "Total diff: " << (results[1] - results[0]) / results[0] * 100 << "\n";
}