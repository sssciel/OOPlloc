#include <iostream>
#include <vector>
#include "allocator/ooplloc.h"

#define objCount 3

class obj {
public:
    virtual void update() = 0;
    virtual void render() = 0;
    virtual ~obj() = default;
};

class objOne : public obj {
public:
    objOne(int x, int y) : x(x), y(y) {}
    
    void update() override {
        x++;
        y++;
        std::cout << "objOne changed to (" << x << ", " << y << ")" << std::endl;
    }

    void render() override {
        std::cout << "Print objOne at (" << x << ", " << y << ")" << std::endl << std::endl;
    }

private:
    int x, y;
};

class objTwo : public obj {
public:
    objTwo(int x, int y) : x(x), y(y) {}
    
    void update() override {
        x--;
        y--;
        std::cout << "objTwo changed to (" << x << ", " << y << ")" << std::endl;
    }

    void render() override {
        std::cout << "Print objTwo at (" << x << ", " << y << ")" << std::endl << std::endl;
    }

private:
    int x, y;
};

int main() {
    const size_t blockSize = std::max(sizeof(objOne), sizeof(objTwo));
    const size_t blockCount = 6;
    OOPLloc_Allocator memoryManager(blockSize * blockCount, blockSize);

    std::vector<obj*> objects;

    for (int i = 0; i < objCount; ++i) {
        void* block = memoryManager.alloc();

        int u;

        std::cin >> u;

        if (block) {
            objOne* player = new(block) objOne(u, u);
            objects.push_back(player);
        }
    }

    for (int i = 0; i < objCount; ++i) {
        void* block = memoryManager.alloc();

        int u;

        std::cin >> u;

        if (block) {
            objTwo* enemy = new(block) objTwo(u * 2, u * 2);
            objects.push_back(enemy);
        }
    }

    for (obj* i : objects) {
        i->update();
        i->render();
    }

    for (obj* obj : objects) {
        if ((objOne*)(obj)) {
            obj->~obj();
            memoryManager.free(obj);
        } else if ((objTwo*)(obj)) {
            obj->~obj();
            memoryManager.free(obj);
        }
    }

    std::cout << "Used blocks at the end: " << memoryManager.getUsedBlocks() << std::endl;

    return 0;
}
