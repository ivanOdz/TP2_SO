#include <MemoryManager.h>

#define BLOCK_SIZE	  8 /// 8 chars = 64 bits
#define LIST_MEM_SIZE 8192

typedef struct MemoryManagerCDT {
	void *startAddress;
	uint64_t totalMemory;
	BlockNode *first;
} MemoryManagerCDT;

typedef struct BlockNode {
	void *base;
	uint64_t blocks;
	BlockNode *next;
} BlockNode;

static MemoryManagerCDT memMan;
static BlockNode list[LIST_MEM_SIZE] = {0};

MemoryManagerADT createMemoryManager(void *const restrict managedMemory, uint64_t memAmount) {
	memMan.startAddress = managedMemory;
	memMan.totalMemory = memAmount;
	memMan.first = NULL;
	return &memMan;
}

void *allocMemory(const uint64_t size) {
	uint64_t blocksToBeAssigned = size / BLOCK_SIZE;
	if (size % BLOCK_SIZE)
		blocksToBeAssigned++;

	/// No first node
	if (!memMan.first) {
		memMan.first = list;
		memMan.first->base = memMan.startAddress;
		memMan.first->blocks = blocksToBeAssigned;
		memMan.first->next = NULL;
		return memMan.startAddress;
	}
	/// big enough gap between first node and mem start
	if (memMan.startAddress != memMan.first->base && memMan.first->base <= memMan.startAddress + blocksToBeAssigned) {
		BlockNode *temp = getNextFree();
		if (temp == NULL)
			return NULL;
		temp->base = memMan.startAddress;
		temp->blocks = blocksToBeAssigned;
		temp->next = memMan.first->next;
		memMan.first = temp;
		return memMan.startAddress;
	}
	BlockNode *currentNode = memMan.first;
	while (currentNode->next && currentNode->base + size * BLOCK_SIZE > currentNode->next->base)
		currentNode = currentNode->next;
	/// malloc te la mete
	if (!currentNode->next && currentNode->base + (currentNode->blocks + blocksToBeAssigned) * BLOCK_SIZE >= memMan.startAddress + memMan.totalMemory)
		return NULL;
	BlockNode *newNode = getNextFree();
	if (!newNode)
		return NULL;
	newNode->next = currentNode->next;
	newNode->base = currentNode->base + currentNode->blocks * BLOCK_SIZE;
	newNode->blocks = blocksToBeAssigned;
	currentNode->next = newNode->next;
	return newNode->base;
}

BlockNode *getNextFree() {
	for (uint32_t i = 0; i < LIST_MEM_SIZE; i++) {
		if (!list[i].blocks)
			return list + i * sizeof(BlockNode);
	}
	return NULL;
}

void free(void *ptrAllocatedMemory) {
	BlockNode *current = memMan.first;
	if (!current)
		return;
	if (current->base == ptrAllocatedMemory) {
		current->blocks = 0;
		memMan.first = current->next;
	}
	while (current->next) {
		if (current->next->base == ptrAllocatedMemory) {
			current->next->blocks = 0;
			current->next = current->next->next;
		}
	}
	return;
}