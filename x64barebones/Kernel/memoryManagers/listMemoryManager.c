// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <memoryManager.h>
#include <stddef.h>

#ifdef LIST

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
static BlockNode staticAllocatedNodes[LIST_MEM_SIZE] = {0};

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
		staticAllocatedNodes[i].blocks = 0;
		staticAllocatedNodes[i].base = NULL;
		staticAllocatedNodes[i].next = NULL;
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
		memMan.first = staticAllocatedNodes;
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

void getMemoryInfo(MemoryInfo *mminfo) {
	uint64_t currentNodeMemorySize;
	BlockNode *current = memMan.first;
	mminfo->mmType = 'L';
	mminfo->startAddress = memMan.startAddress;
	mminfo->totalMemory = memMan.totalMemory;
	mminfo->occupiedMemory = 0;
	mminfo->fragmentedMemory = 0;
	mminfo->maxFragmentedSize = 0;
	mminfo->minFragmentedSize = mminfo->totalMemory;

	/// Mientras recorro la lista, cuento cuanta memoria esta ocupada
	/// freeMemory la calculo con la memoria total - memoria ocuapda
	/// FragmentedMemoryPercentage -> memoria total libre entre nodos/memoria entre primer nodo y donde termina el ultimo nodo

	/// Si el primer nodo es NULL, tengo toda la memoria libre, sin fragmentacion.
	if (current == NULL) {
		mminfo->freeMemory = memMan.totalMemory;
		mminfo->fragmentedMemory = 0;
		mminfo->endAddress = memMan.first;
		return;
	}

	while (current != NULL) {
		currentNodeMemorySize = current->blocks * BLOCK_SIZE;
		mminfo->occupiedMemory += currentNodeMemorySize;
		if (current->next != NULL) { /// Si el siguiente nodo no es null, puedo tener memoria fragmentada
			uint64_t fragmented = current->next->base - (current->base + currentNodeMemorySize);
			mminfo->fragmentedMemory += fragmented;
			if (fragmented > mminfo->maxFragmentedSize)
				mminfo->maxFragmentedSize = fragmented;
			if (fragmented < mminfo->minFragmentedSize)
				mminfo->minFragmentedSize = fragmented;
		}
		else { /// Si el siguiente nodo es null, ya puedo calcular la dir final
			mminfo->endAddress = current->base + currentNodeMemorySize;
		}
		/// Si el siguiente nodo es null, ya estoy en el ultimo nodo. no calculo memoryFreeBetweenNodes.
		// Avanzo
		current = current->next;
	}
	mminfo->freeMemory = memMan.totalMemory - mminfo->occupiedMemory;
}

BlockNode *getNextFree() {
	for (uint32_t i = 0; i < LIST_MEM_SIZE; i++) {
		if (staticAllocatedNodes[i].blocks == 0) {
			return &staticAllocatedNodes[i];
		}
	}
	return NULL;
}

uint8_t freeMemory(void *ptrAllocatedMemory) {
	BlockNode *current = memMan.first;
	if (!current)
		return 0;
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
			return 1;
		}
		current = current->next;
	}
	return 0;
}

void printMem() {
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

#endif