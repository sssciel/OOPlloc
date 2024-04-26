#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H

void* MuteCreate(void);
void MuteDeterm(void* ptr);
void MuteON(void* ptr);
void MuteOFF(void* ptr);

#endif 
