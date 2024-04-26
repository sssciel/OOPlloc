#include <iostream>
#include <chrono>
#include <stack>
#include <vector>
#include <memory>
#include <stack>
#include "ooplloc.h"

double leverSingleMalloc(UI1 iterationTimes) {
    auto start = std::chrono::high_resolution_clock::now();

    std::stack<void*> allocations;

    for (UI1 i = 0; i < iterationTimes; ++i) {
        auto p = malloc(DEFAULT_BLOCK_SIZE);
        allocations.push(p);
    }
    for (UI1 i = 0; i < iterationTimes; ++i) {
        void* p = allocations.top();
        free(p);
        allocations.pop();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto testRunTime = std::chrono::duration<double>(end-start).count();
    
    return testRunTime;
}

double leverSingleOOPlloc(UI1 iterationTimes) {
    OOPLloc_Allocator all(iterationTimes * 8, 8);
    auto start = std::chrono::high_resolution_clock::now();

    std::stack<void*> allocations;

    for (UI1 i = 0; i < iterationTimes; ++i) {
        void *p = all.alloc();
        allocations.push(p);
    }

    for (UI1 i = 0; i < iterationTimes; ++i) {
        void* p = allocations.top();
        all.free(p);
        allocations.pop();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto testRunTime = std::chrono::duration<double>(end-start).count();
    
    return testRunTime;
}

std::vector<UI1> times = {10000000};

int main() {
    for (UI1 i = 0; times.size(); ++i) {
        auto p1 = leverSingleMalloc(times[i]);
        auto p2 = leverSingleOOPlloc(times[i]);
        std::cout << "Malloc time for " << times[i] << " is " << p1 << "\n";
        std::cout << "OOPlloc time for " << times[i] << " is " << p2 << "\n";
        std::cout << "Total diff: " << (p1 - p2) / p2 * 100 << "\n";
    }
    return 0;
}