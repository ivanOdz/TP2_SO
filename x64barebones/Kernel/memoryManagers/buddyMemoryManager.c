#include <libc.h>
#include <memoryManager.h>
#include <stddef.h>

#define BLOCK_SIZE	        8 /// 8 chars = 64 bits
#define LIST_MEM_SIZE       8192
#define MIN_MEM_SIZE        8
#define MAX_LEVELS          32

BlockNode list[LIST_MEM_SIZE] = {0};
BlockNode route[MAX_LEVELS] = {0};

static MemoryManagerCDT memMan;

typedef struct {
    void *addr;
    uint64_t size;
    BlockNode *left;
    BlockNode *right;
} BlockNode;

typedef struct {
	void *startAddress;
    BlockNode *root;            // Root considered at level 0
	uint64_t totalMemory;
//    uint64_t freeMemory;
    uint64_t largestAvailableMemoryBlock;
    uint32_t levels;
    uint32_t minBlockMemory;
} MemoryManagerCDT;


MemoryManagerADT createMemoryManager(void *const restrict managedMemory, uint64_t memAmount) {
    if (managedMemory == NULL || memAmount < MIN_MEM_SIZE)
        return NULL;
    memMan.startAddress = managedMemory;
    memMan.totalMemory = memAmount;
    memMan.freeMemory = memAmount;
    memMan.root = NULL;
    memMan.levels = 0;
    memMan.minBlockMemory = MIN_MEM_SIZE;

    for (uint32_t i = MIN_MEM_SIZE; i < managedMemory; i *= 2) {
        if (memMan.levels < MAX_LEVELS)
            memMan.levels++;
        else
            memMan.minBlockMemory *= 2;
    }

    return &memMan;
}

void *allocMemory(const uint64_t size) {

    if (size > memMan.largestAvailableMemoryBlock)
        return NULL;
    
    void *newAlloc = NULL;
    BlockNode actualNode = memMan.root;
    uint64_t actualAddress = memMan.startAddress;
    uint32_t listIndex = 0;     // Points to the father of the actual node
    uint8_t memFound = 0;

    while (!memFound) {

        if (actualNode == NULL) {
            actualNode = getNextFree();
            actualNode.left = NULL;
            actualNode.right = NULL;
            actualNode.addr = actualAddress;
            actualNode.size = memMan.totalMemory / (listIndex + 1);
            if (listIndex > 0)
                if (route[listIndex].left == NULL)
                    route[listIndex].left = actualNode;
                else
                    route[listIndex].right = actualNode;
        }
        if (size <= (actualNode.size / 2)) {                            // Go deeper
            route[listIndex] = actualNode;
            listIndex++;
            actualNode = actualNode.left;
        }
        else if (actualNode.left != NULL || actualNode.right != NULL) { // Go to the right side or climb up
            
            if (route[listIndex].left == actualNode) {
                actualAddress = actualNode.addr + actualNode.size;           
                actualNode = route[listIndex].right;
            }
            else {
                while (listIndex) {
                    actualNode = route[listIndex--];
                }
            }
        }
        else {
            memFound = 1;
            newAlloc = actualAddress;
        }
    }
    
    return newAlloc;
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