#include <libc.h>
#include <memoryManager.h>
#include <stddef.h>

#define BLOCK_SIZE	        8 /// 8 chars = 64 bits
#define LIST_MEM_SIZE       8192
#define MIN_MEM_SIZE        8
#define MAX_LEVELS          32

BlockNode list[LIST_MEM_SIZE] = {0};
BlockNode route[MAX_LEVELS] = {0};

static MemoryManagerCDT memMan = {0,0,0};

typedef struct {
    void *addr;
    uint64_t size;
    BlockNode *left;
    BlockNode *right;
} BlockNode;

typedef struct {
	void *startAddress;
	uint64_t totalMemory;
	BlockNode *root;            // Root considered at level 0
    uint32_t levels;
    uint32_t minMemory;
} MemoryManagerCDT;


MemoryManagerADT createMemoryManager(void *const restrict managedMemory, uint64_t memAmount) {
    if (managedMemory == NULL || memAmount < MIN_MEM_SIZE)
        return NULL;

    memMan.startAddress = managedMemory;
    memMan.totalMemory = memAmount;
    memMan.root = NULL;
    memMan.levels = 0;
    for (uint32_t i=MIN_MEM_SIZE; i < managedMemory; ) {
        i *= 2;
        memMan.levels++;
    }
    return &memMan;
}

void *allocMemory(const uint64_t size) {

    if (size > memMan.totalMemory)
        return NULL;
    
    // TODO
    return NULL;

}

void free(void *ptrAllocatedMemory) {

    return; // TODO
}

BlockNode *getNextFree() {
	for (uint32_t i = 0; i < LIST_MEM_SIZE; i++) {
		if (list[i].blocks == 0) {
			return &list[i];
		}
	}
	return NULL;
}