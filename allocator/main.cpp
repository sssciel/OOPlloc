#include <iostream>
#include <chrono>
#include <stack>
#include "ooplloc.h"

int main() {
    OOPLloc_Allocator all;
    void* p = all.alloc();
    all.free(p);
    std::cout << "Done :)";
    return 0;
}