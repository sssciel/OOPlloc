#include "mutexLock.h"
#include <mutex>

void* MuteCreate(void)
{
    std::mutex* lock = new std::mutex;
    return lock;
}

void MuteDeterm(void* ptr)
{
    std::mutex* lock = (std::mutex*)(ptr);
    delete lock;
}

void MuteON(void* ptr)
{
    std::mutex* lock = (std::mutex*)(ptr);
	lock->lock();
}

void MuteOFF(void* ptr)
{
    std::mutex* lock = (std::mutex*)(ptr);
    lock->unlock();
}