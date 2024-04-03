
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <random>
#include <stack>
#include <queue>

const int BLOCK_SIZES[] = {16, 64, 256, 1024, 4096};
const int NUM_ITERATIONS = 10000;
const int LONG_DURATION = 3600;
const int FIXED_BLOCK_SIZE = 8; 
const double TIME_LIMIT = 1.0;
const int NUM_THREADS = 4;

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

void perform_stress_test(int size, std::ofstream& file) {
    Timer timer;
    timer.start();

    for (long i = 0; i < LONG_DURATION * 1000; ++i) {
        char* memory = new char[size];
        delete[] memory;
    }

    double time_elapsed = timer.stop();
    file << "Stress Test, " << size << ", " << time_elapsed << "\n";
}

void perform_long_term_test(std::ofstream& file) {
    Timer timer;
    timer.start();

    std::vector<char*> allocations;
    for (int i = 0; i < LONG_DURATION; ++i) {
        allocations.push_back(new char[256]);
        if (i % 1000 == 0) {
            for (char* ptr : allocations) {
                delete[] ptr;
            }
            allocations.clear();
        }
    }

    double time_elapsed = timer.stop();
    file << "Long Term Test, 256, " << time_elapsed << "\n";
}

void perform_alternate_allocation_test(std::ofstream& file) {
    std::vector<char*> allocations;
    Timer timer;
    timer.start();
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        char* memory = new char[FIXED_BLOCK_SIZE];
        delete[] memory;
    }
    double alternate_time = timer.stop();
    file << "Alternate Allocation/Deallocation, " << alternate_time << "\n";
}

std::vector<char*> allocations_for_mass;

void perform_delayed_allocation_test(std::ofstream& file) {
    Timer timer;
    timer.start();
    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        allocations_for_mass.push_back(new char[FIXED_BLOCK_SIZE]);
    }
    double allocation_time = timer.stop();
    file << "Delayed Deallocation, " << allocation_time << "\n";
}

void perform_delayed_deallocation_test(std::ofstream& file) {
    Timer timer;
    timer.start();
    for (char* ptr : allocations_for_mass) {
        delete[] ptr;
    }
    double mass_deallocation_time = timer.stop();
    file << "Mass Deallocation, " << mass_deallocation_time << "\n";
}


void allocate_memory(int size, int iterations, std::ofstream& file) {
    std::vector<char*> allocations;
    Timer timer;
    timer.start();

    for (int i = 0; i < iterations; ++i) {
        allocations.push_back(new char[size]);
    }

    double allocation_time = timer.stop();
    file << "Allocation, " << size << ", " << allocation_time << "\n";

    timer.start();
    for (char* ptr : allocations) {
        delete[] ptr;
    }

    double deallocation_time = timer.stop();
    file << "Deallocation, " << size << ", " << deallocation_time << "\n";
}

void max_blocks_in_time(int size, std::ofstream& file) {
    Timer timer;
    timer.start();
    int count = 0;

    while (true) {
        char* memory = new char[size];
        delete[] memory;
        count++;

        if (timer.stop() > TIME_LIMIT) {
            break;
        }
    }

    file << "Max Blocks, " << size << ", " << count << "\n";
}

void random_allocation(std::ofstream& file) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 4);

    std::vector<char*> allocations;
    Timer timer;
    timer.start();

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        int index = dis(gen);
        int size = BLOCK_SIZES[index];
        allocations.push_back(new char[size]);
    }

    double elapsed = timer.stop();
    file << "Random Allocation, " << elapsed << "\n";

    timer.start();
    for (char* ptr : allocations) {
        delete[] ptr;
    }

    elapsed = timer.stop();
    file << "Random Deallocation, " << elapsed << "\n";
}

void thread_test(int size, std::ofstream& file) {
    allocate_memory(size, NUM_ITERATIONS, file);
    max_blocks_in_time(size, file);
}

void perform_overlapping_allocations_test(std::ofstream& file) {
    // to do
}

void perform_various_deallocation_orders_test(std::ofstream& file) {
    // to do
}

void perform_fragmentation_resistance_test(std::ofstream& file) {
    // to do
}

int main() {
    std::ofstream results_file("results.csv");

    for (int size : BLOCK_SIZES) {
        perform_stress_test(size, results_file);
    }

    // std::vector<std::thread> threads;

    // for (int size : BLOCK_SIZES) {
    //     for (int i = 0; i < NUM_THREADS; ++i) {
    //         threads.emplace_back(thread_test, size, std::ref(results_file));
    //     }
    // }

    // for (auto& th : threads) {
    //     th.join();
    // }

    perform_long_term_test(results_file);
    perform_alternate_allocation_test(results_file);
    perform_delayed_allocation_test(results_file);
    perform_delayed_deallocation_test(results_file);
    perform_overlapping_allocations_test(results_file);
    perform_various_deallocation_orders_test(results_file);
    perform_fragmentation_resistance_test(results_file);

    results_file.close();
    return 0;
}
