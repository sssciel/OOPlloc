#ifndef ATOMICSTRUCT_H
#define ATOMICSTRUCT_H

#pragma once
#include <atomic>
#include <stdexcept>

struct Node {
    void* value;
    Node* next;
};

struct Block {
    Block* next;
};

class AtomicStack {
public:
    AtomicStack();

    void push(void* value);
    void* pop();

    bool is_empty() const;

private:
    std::atomic<Node*> head;
};

class AtomicFreeList {
public:
    AtomicFreeList();

    void push(void* block);
    void* pop();
    bool is_empty() const;

private:
    std::atomic<Block*> head;
};

#endif /* ATOMICSTRUCT_H */