#include <atomic>
#include <iostream>
#include <stdexcept>
#include "atomicStructs.h"

AtomicStack::AtomicStack() : head(nullptr) {}

void AtomicStack::push(void* value) {
    Node* newNode = new Node{value, nullptr};
    Node* oldHead;

    do {
        oldHead = head.load(std::memory_order_relaxed);
        newNode->next = oldHead; 
    } while (!head.compare_exchange_weak(oldHead, newNode, std::memory_order_release, std::memory_order_relaxed));
}

void* AtomicStack::pop() {
    Node* oldHead;

    do {
        oldHead = head.load(std::memory_order_relaxed);
        if (oldHead == nullptr) {
            throw std::out_of_range("Stack is empty");
        }
    } while (!head.compare_exchange_weak(oldHead, oldHead->next, std::memory_order_release, std::memory_order_relaxed));

    void* value = oldHead->value;
    delete oldHead;
    return value;
}

bool AtomicStack::is_empty() const {
    return head.load(std::memory_order_relaxed) == nullptr;
}

AtomicFreeList::AtomicFreeList() : head(nullptr) {}

void AtomicFreeList::push(void* ptr) {
    Block* block = static_cast<Block*>(ptr);
    Block* oldHead;
    do {
        oldHead = head.load(std::memory_order_relaxed);
        block->next = oldHead;
    } while (!head.compare_exchange_weak(
        oldHead, block, std::memory_order_release, std::memory_order_relaxed
    ));
}

void* AtomicFreeList::pop() {
    Block* oldHead;
    do {
        oldHead = head.load(std::memory_order_relaxed);
        if (oldHead == nullptr) {
            return nullptr;
        }
    } while (!head.compare_exchange_weak(
        oldHead, oldHead->next, std::memory_order_release, std::memory_order_relaxed
    ));

    void* ptr = static_cast<void*>(oldHead);
    return ptr;
}

bool AtomicFreeList::is_empty() const {
    return head.load(std::memory_order_relaxed) == nullptr;
}