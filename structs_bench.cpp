#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <memory>
#include <chrono>
#include "sss.cpp"
#include "segmented.cpp"
#include "slab.cpp"

void benchmarkAllocatorSSS(size_t iterations, size_t numAllocations) {
    auto start = std::chrono::high_resolution_clock::now();
    free_class allocator(8, 1024);

    std::vector<std::pair<void*, size_t>> allocatedBlocks;
    allocatedBlocks.reserve(numAllocations);

    srand(static_cast<unsigned>(time(nullptr)));

    for (size_t i = 0; i < iterations; ++i) {
        if (allocatedBlocks.size() < numAllocations) {
            void* block = allocator.allocate();
            allocatedBlocks.push_back({block, 8});
        }

        if (!allocatedBlocks.empty() && (rand() % 2 == 0)) {
            size_t index = rand() % allocatedBlocks.size();
            allocator.deallocate(allocatedBlocks[index].first);
            allocatedBlocks.erase(allocatedBlocks.begin() + index);
        }
    }

    for (auto& block : allocatedBlocks) {
        allocator.deallocate(block.first);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto time_taken = std::chrono::duration<double>(end-start).count();
    std::cout << "SSS():\n" << iterations << " iterations and " << numAllocations <<  " allocations:\n\tTotal time " << time_taken  << "\n";
}

void benchmarkAllocatorSegment(size_t iterations, size_t numAllocations) {
    auto start = std::chrono::high_resolution_clock::now();
    block_class allocator(1024 * 8);

    std::vector<std::pair<void*, size_t>> allocatedBlocks;
    allocatedBlocks.reserve(numAllocations);

    srand(static_cast<unsigned>(time(nullptr)));

    for (size_t i = 0; i < iterations; ++i) {
        if (allocatedBlocks.size() < numAllocations) {
            void* block = allocator.allocate(8);
            allocatedBlocks.push_back({block, 8});
        }

        if (!allocatedBlocks.empty() && (rand() % 2 == 0)) {
            size_t index = rand() % allocatedBlocks.size();
            allocator.deallocate(allocatedBlocks[index].first);
            allocatedBlocks.erase(allocatedBlocks.begin() + index);
        }
    }

    for (auto& block : allocatedBlocks) {
        allocator.deallocate(block.first);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto time_taken = std::chrono::duration<double>(end-start).count();
    std::cout << "Segment():\n" << iterations << " iterations and " << numAllocations <<  " allocations:\n\tTotal time " << time_taken  << "\n";
}

void benchmarkAllocatorSlab(size_t iterations, size_t numAllocations) {
    auto start = std::chrono::high_resolution_clock::now();
    slabs_class allocator(8);

    std::vector<std::pair<void*, size_t>> allocatedBlocks;
    allocatedBlocks.reserve(numAllocations);

    srand(static_cast<unsigned>(time(nullptr)));

    for (size_t i = 0; i < iterations; ++i) {
        if (allocatedBlocks.size() < numAllocations) {
            void* block = allocator.allocate();
            allocatedBlocks.push_back({block, 8});
        }

        if (!allocatedBlocks.empty() && (rand() % 2 == 0)) {
            size_t index = rand() % allocatedBlocks.size();
            allocator.deallocate(allocatedBlocks[index].first);
            allocatedBlocks.erase(allocatedBlocks.begin() + index);
        }
    }

    for (auto& block : allocatedBlocks) {
        allocator.deallocate(block.first);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto time_taken = std::chrono::duration<double>(end-start).count();
    std::cout << "Slab():\n" << iterations << " iterations and " << numAllocations <<  " allocations:\n\tTotal time " << time_taken  << "\n";
}

std::vector<int> iterations = {10000, 100000, 1000000};
std::vector<int> numAllocations = {5000, 50000, 50000};
int main() {

    for (int i = 0; i < iterations.size(); ++i) {
        benchmarkAllocatorSSS(iterations[i], numAllocations[i]);
        benchmarkAllocatorSegment(iterations[i], numAllocations[i]);
        benchmarkAllocatorSlab(iterations[i], numAllocations[i]);
    }
    return 0;
}
