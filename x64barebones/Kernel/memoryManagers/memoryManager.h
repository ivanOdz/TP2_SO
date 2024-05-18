#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H
#include <stdint.h>

#define BUDDY
typedef struct MemoryManagerCDT *MemoryManagerADT;

typedef struct MemoryInfo {
	void *startAddress;
	uint64_t totalMemory;
	uint64_t freeMemory;
	uint64_t occupiedMemory;
	double fragmentedMemoryPercentage;
	double minGlobalFreeMemoryPercentage;
	void *endAddress;
} MemoryInfo;

MemoryManagerADT createMemoryManager(void *const restrict managedMemory, uint64_t memAmount);
void *allocMemory(const uint64_t size);
void getMemoryInfo(MemoryInfo *mminfo);
void free(void *ptrAllocatedMemory);
void printNodes();
void printList();
void setDebug(uint8_t);

#endif