#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <fstream>
#include <algorithm>
#include <memory>
#include <cstdlib>
#include <tuple>
#include <jemalloc/jemalloc.h>

class IAllocator {
public:
    virtual void* allocate(size_t size) = 0;
    virtual void deallocate(void* ptr, size_t size) = 0;
    virtual ~IAllocator() {}
};

// class CustomAllocator : 
// public IAllocator {
//     void* allocate(size_t size) override {
//         return malloc(size);
//     }

//     void deallocate(void* ptr, size_t size) override {
//         free(ptr);
//     }
// };

class JemallocAllocator : 
public IAllocator {
    void* allocate(size_t size) override {
        return mallocx(size, 0);
    }

    void deallocate(void* ptr, size_t size) override {
        dallocx(ptr, 0);
    }
};

class Timer {
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    double stop() {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end_time - start_time;
        return diff.count();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

void testAllocationTime(std::string allocatorName, IAllocator& allocator, size_t numBlocks, size_t blockSize, std::vector<std::tuple<std::string, std::string, size_t, double>>& results, std::vector<void*>& blocks) {
    Timer timer;
    timer.start();

    for (size_t i = 0; i < numBlocks; ++i) {
        allocator.allocate(blockSize);
        for (size_t i = 0; i < numBlocks; ++i) {
            blocks.push_back(allocator.allocate(8));
        }
    }

    double time_elapsed = timer.stop();

    results.push_back(std::make_tuple(allocatorName, "AllocationTime", numBlocks, time_elapsed));
}

void testDeallocationTime(std::string allocatorName, IAllocator& allocator, std::vector<void*>& blocks, std::vector<std::tuple<std::string, std::string, size_t, double>>& results) {
    Timer timer;
    timer.start();

    for (auto block : blocks) {
        allocator.deallocate(block, 8);
    }

    double time_elapsed = timer.stop();

    results.push_back(std::make_tuple(allocatorName, "DeallocationTime", blocks.size(), time_elapsed));
    blocks.clear();
}

int main() {
    //CustomAllocator allocator;
    JemallocAllocator jeallocator;
    std::vector<std::tuple<std::string, std::string, size_t, double>> results;
    std::vector<void*> allocatedBlocks;

    // Example block sizes to test
    std::vector<size_t> blockSizes = {1000, 2000, 5000, 10000, 20000};

    for (auto numBlocks : blockSizes) {
        // Allocation
        allocatedBlocks.reserve(numBlocks);
        //testAllocationTime("Custom", allocator, numBlocks, 8, results);
        testAllocationTime("Jemalloc", jeallocator, numBlocks, 8, results);

        // Storing allocated blocks for deallocation
        for (size_t i = 0; i < numBlocks; ++i) {
            allocatedBlocks.push_back(allocator.allocate(8));
        }

        // Deallocation
        //testDeallocationTime("Custom", allocator, allocatedBlocks, results);
        testDeallocationTime("Jemalloc", jeallocator, allocatedBlocks, results);
    }

    // Output results to a file
    std::ofstream outFile("benchmark_results.csv");
    outFile << "TestName,NumBlocks,Duration\n";
    for (const auto& result : results) {
        outFile << std::get<0>(result) << "," << std::get<1>(result) << "," << std::get<2>(result) << "\n";
    }
    outFile.close();

    return 0;
}