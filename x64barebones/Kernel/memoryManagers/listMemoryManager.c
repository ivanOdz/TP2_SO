#include <libc.h>
#include <memoryManager.h>
#include <stddef.h>

#define BLOCK_SIZE	  8 /// 8 chars = 64 bits
#define LIST_MEM_SIZE 8192

typedef struct BlockNode {
	void *base;
	uint64_t blocks;
	struct BlockNode *next;
} BlockNode;

typedef struct MemoryManagerCDT {
	void *startAddress;
	uint64_t totalMemory;
	BlockNode *first;
} MemoryManagerCDT;

BlockNode *getNextFree();
static MemoryManagerCDT memMan;
static BlockNode list[LIST_MEM_SIZE] = {0};

MemoryManagerADT createMemoryManager(void *const restrict managedMemory, uint64_t memAmount) {
	memMan.startAddress = managedMemory;
	if ((uint64_t) memMan.startAddress % BLOCK_SIZE) {
		memMan.startAddress = (void *) ((uint64_t) memMan.startAddress / BLOCK_SIZE);
		memMan.startAddress++;
		memMan.startAddress = (void *) ((uint64_t) memMan.startAddress * BLOCK_SIZE);
	}
	memMan.totalMemory = memAmount;
	memMan.first = NULL;
	for (uint32_t i = 0; i < LIST_MEM_SIZE; i++) {
		list[i].blocks = 0;
		list[i].base = NULL;
		list[i].next = NULL;
	}
	return &memMan;
}

void *allocMemory(const uint64_t size) {
	if (!size)
		return NULL;
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
	if ((memMan.startAddress != memMan.first->base) && (memMan.first->base >= (memMan.startAddress + blocksToBeAssigned * BLOCK_SIZE))) {
		BlockNode *newNode = getNextFree();
		if (!newNode)
			return NULL;
		newNode->base = memMan.startAddress;
		newNode->blocks = blocksToBeAssigned;
		newNode->next = memMan.first;
		memMan.first = newNode;
		return memMan.startAddress;
	}
	BlockNode *currentNode = memMan.first;
	while (currentNode->next && currentNode->base + (currentNode->blocks + blocksToBeAssigned) * BLOCK_SIZE > currentNode->next->base)
		currentNode = currentNode->next;
	if (!currentNode->next && currentNode->base + (currentNode->blocks + blocksToBeAssigned) * BLOCK_SIZE >= memMan.startAddress + memMan.totalMemory)
		return NULL;
	BlockNode *newNode = getNextFree();
	if (!newNode)
		return NULL;
	newNode->next = currentNode->next;
	newNode->base = currentNode->base + currentNode->blocks * BLOCK_SIZE;
	newNode->blocks = blocksToBeAssigned;
	currentNode->next = newNode;
	return newNode->base;
}

BlockNode *getNextFree() {
	for (uint32_t i = 0; i < LIST_MEM_SIZE; i++) {
		if (list[i].blocks == 0) {
			return &list[i];
		}
	}
	return NULL;
}

void free(void *ptrAllocatedMemory) {
	BlockNode *current = memMan.first;
	if (!current)
		return;
	if (current->base == ptrAllocatedMemory) {
		current->blocks = 0;
		current->base = NULL;
		memMan.first = current->next;
	}
	while (current->next) {
		if (current->next->base == ptrAllocatedMemory) {
			BlockNode *temp = current->next;
			temp->blocks = 0;
			temp->base = NULL;
			current->next = temp->next;
			temp->next = NULL;
			return;
		}
		current = current->next;
	}
	return;
}

void printNodes() {
	if (!memMan.first) {
		printf("No nodes\n");
		return;
	}
	printf("MemMan start %x size %x next %x\n", memMan.startAddress, memMan.totalMemory, memMan.first);
	BlockNode *current = memMan.first;
	while (current) {
		printf("Node %x start %x size %x next %x\n", current, current->base, current->blocks * BLOCK_SIZE, current->next);
		if (current == current->next) {
			printf("ERROR\n");
			return;
		}
		current = current->next;
	}
}

void printList() {
	for (int i = 0; i < 12; i++) {
		printf("Node %d %x %x %x\n", i, list[i].base, list[i].blocks * BLOCK_SIZE, list[i].next);
	}
	printf("\n");
}