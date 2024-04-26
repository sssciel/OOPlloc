#include <benchmark/benchmark.h>
#include "ooplloc.h"

static void BM_Allocate(benchmark::State& state) {
    for (auto _ : state) {
        void* arr = malloc(state.range(0));
        free(arr);
    }
}

BENCHMARK(BM_Allocate)->Arg(8);

static void BM_Allocate_OOPlloc(benchmark::State& state) {
    OOPLloc_Allocator all(8 * 1024 * 1024 * 8);
    for (auto _ : state) {
        auto arr = all.alloc();
        all.free(arr);
    }
}

BENCHMARK(BM_Allocate_OOPlloc);

BENCHMARK_MAIN();