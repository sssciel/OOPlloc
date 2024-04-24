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
#include <atomic>

#define TEST_TIMES 1e8
#define DEFAULT_VALUE 102030

double stackTotalTime;
double freelistTotalTime;

void stack_test() {
    auto start = std::chrono::high_resolution_clock::now();

    std::stack<int> testStack;

    for (size_t i = 0; i < TEST_TIMES; ++i) {
        testStack.push(DEFAULT_VALUE);
    }

    auto end = std::chrono::high_resolution_clock::now();
    stackTotalTime = std::chrono::duration<double>(end-start).count();
    std::cout << "Stack, total time: " << stackTotalTime << "s.\n";
}

void freelist_test() {
    auto start = std::chrono::high_resolution_clock::now();

    struct listfree {
        int value = 0;
        listfree* next = nullptr;
    };

    listfree* testList = new listfree;

    for (size_t i = 0; i < TEST_TIMES; ++i) {
        listfree* newList = new listfree;
        newList->value = DEFAULT_VALUE;
        newList->next = testList;
    }

    auto end = std::chrono::high_resolution_clock::now();
    freelistTotalTime = std::chrono::duration<double>(end-start).count();
    std::cout << "Freelist, total time: " << freelistTotalTime << "s.\n";
}

int main() {
    stack_test();
    freelist_test();
    std::cout << "Total diff: " << (stackTotalTime - freelistTotalTime) / freelistTotalTime * 100 << "\n";
}