# OOPlloc
This repository is an archive of a course project for developing a custom memory manager for object-oriented languages. It uses block allocation of the same length.

## Project Hierarchy
1. The `benchmarks` folder contains benchmarks described and used in the project report. The `new_super_puper_benchmark.cpp` is the main benchmark and is used as the primary one in the project. It was used to evaluate the performance of five existing memory managers, three implementations of existing algorithms, as well as OOPlloc itself. The folder also contains micro-benchmarks for different parts of the project.
2. The `implementations` folder contains existing implementations of memory allocator structures and algorithms, such as SSS (Simple Segregated Storage), Slab allocator, and Segmented allocator. Their descriptions were taken from scientific articles and books, for example, "Memory Management Algorithms and Implementation in C/C++ by Bill Blunden".
3. The `OOPAllocator` folder contains the project's implementation itself.

## Compilation
1. To compile the benchmark, use the following command:

```
g++ benchmarks/new_super_puper_benchmark.cpp -std=c++20 -O3 -I. -L. -ljemalloc -lumalloc -ltlsf -o main  && ./main.
```

2. To compile the Google benchmark:

```
g++ test.cpp -std=c++11 -isystem benchmark/include \
  -Lbenchmark/build/src -lbenchmark -lpthread ooplloc.cpp atomicStructs.cpp mutexLock.cpp -DTHREAD_SAFE -o mybenchmark && ./mybenchmark
```

3. To compile the allocator:
The main.cpp file for testing the allocator is in the allocator folder, along with the Makefile.

```
make && ./main
```

## Usage
```
OOPLloc_Allocator all; // Initializes the allocator with standard sizes
OOPLloc_Allocator all1(4096); // Initializes the allocator with 4096B pages
OOPLloc_Allocator all2(4096, 16); // Initializes the allocator with 4096B pages and 16B blocks

void* ptr = all.alloc(); // Allocates a block of the size specified during initialization
all.free(ptr); // Frees a block of the size specified during initialization

int a = 10;
void* ptr1 = all.ialloc(a); // Allocates a block with a specific parameter

std::vector<void*> allocated_pool;
all.arrlloc(&allocated_pool, 10); // Fills the vector with 10 allocated blocks
```

To compile the benchmark for testing the structure implementations, use the flag **-DSTRUCTSTEST**.
To compile the allocator with THREAD_SAFE mode disabled, use the flag **-DTHREAD_SAFE**.

*The project report can be provided upon request.*
