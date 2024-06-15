#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H
#include <libc.h>
#include <stdint.h>

#define BUDDY

typedef struct MemoryManagerCDT *MemoryManagerADT;

MemoryManagerADT createMemoryManager(void *const restrict managedMemory, uint64_t memAmount);
void *allocMemory(const uint64_t size);
void getMemoryInfo(MemoryInfo *mminfo);
uint8_t freeMemory(void *ptrAllocatedMemory);

#endif