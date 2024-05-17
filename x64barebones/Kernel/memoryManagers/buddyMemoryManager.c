#include <libc.h>
#include <memoryManager.h>
#include <stddef.h>

#ifdef BUDDY
#define BLOCK_SIZE	  8 /// 8 chars = 64 bits
#define LIST_MEM_SIZE 8192
#define MIN_MEM_SIZE  8
#define MAX_LEVELS	  32

#define BUDDY_RECURSIVE

typedef struct BlockNode {
	void *addr;
	uint64_t blocks;
	struct BlockNode *left;
	struct BlockNode *right;
} BlockNode;

typedef struct MemoryManagerCDT {
	void *startAddress;
	BlockNode *root; // Root considered at level 0
	uint64_t totalMemory;
} MemoryManagerCDT;

BlockNode list[LIST_MEM_SIZE] = {0};
BlockNode route[MAX_LEVELS] = {0};

static MemoryManagerCDT memMan;

void *findMemory(uint64_t size, BlockNode *node, uint8_t virginNode);
BlockNode *newNode(void *address, uint64_t size);
BlockNode *getNextFree();

MemoryManagerADT createMemoryManager(void *const restrict managedMemory, uint64_t memAmount) {
	if (managedMemory == NULL || memAmount < MIN_MEM_SIZE)
		return NULL;
	memMan.startAddress = managedMemory;
	if ((uint64_t) memMan.startAddress % BLOCK_SIZE) {
		memMan.startAddress = (void *) ((uint64_t) memMan.startAddress / BLOCK_SIZE);
		memMan.startAddress++;
		memMan.startAddress = (void *) ((uint64_t) memMan.startAddress * BLOCK_SIZE);
	}
	/// Acotar memoria a potencia de 2 (REVISAR QUE FUNCIONE)
	uint64_t size_pow2 = 1;
	while (size_pow2 <= memAmount && (size_pow2 << 1) > memAmount)
		size_pow2 = size_pow2 << 1;
	memMan.totalMemory = size_pow2;

	memMan.root = NULL;
	return &memMan;
}

#ifdef BUDDY_ITERATIVE
void *allocMemory(const uint64_t size) {
	if (size > memMan.largestAvailableMemoryBlock)
		return NULL;

	void *newAlloc = NULL;
	BlockNode actualNode = memMan.root;
	uint64_t actualAddress = memMan.startAddress;
	uint32_t listIndex = 0; // Points to the father of the actual node
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
		if (size <= (actualNode.size / 2)) { // Go deeper
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

#endif

#ifdef BUDDY_RECURSIVE

void *allocMemory(uint64_t size) {
	if (!size)
		return NULL;
	if (!memMan.root) {
		memMan.root = newNode(memMan.startAddress, memMan.totalMemory);
		if (!memMan.root)
			return NULL;
		return findMemory(size / BLOCK_SIZE, memMan.root, 1);
	}
	return findMemory(size / BLOCK_SIZE, memMan.root, 0);
}

void *findMemory(uint64_t size, BlockNode *node, uint8_t virginNode) {
	if (node->blocks < size)
		return NULL;
	/// yo soy candidato ideal
	if (node->blocks >= size && (node->blocks >> 1) < size)
		return node->addr;
	// quiero DFS o como se llame, asi que priorizo ir por ramas ya existentes
	if (!node->left) {
		if (!node->right)
			/// no hay ningun hijo, asi que quiero ver si recien me crearon para seguir avanzando o si soy memoria alocada 😛
			if (virginNode) {
				node->left = newNode(node->addr, node->blocks * BLOCK_SIZE / 2);
				return findMemory(size, node->left, virginNode);
			}
		return NULL;
		void *result = findMemory(size, node->right, virginNode);
		if (result) /// encontro memoria en right
			return result;
		/// nos vamos a left
		node->left = newNode(node->addr, node->blocks * BLOCK_SIZE / 2);
		return findMemory(size, node->left, 1);
	}
	if (!node->right) {
		void *result = findMemory(size, node->left, virginNode);
		if (result) /// encontro memoria en left
			return result;
		/// nos vamos a right
		node->right = newNode(node->addr + node->blocks * BLOCK_SIZE / 2, node->blocks * BLOCK_SIZE / 2);
		return findMemory(size, node->left, 1);
	}
	return NULL;
}

#endif

BlockNode *newNode(void *address, uint64_t size) {
	BlockNode *myNode = getNextFree();
	if (!myNode)
		return NULL;
	uint64_t size_pow2 = 1;
	size = size / BLOCK_SIZE;
	while (size_pow2 < size && (size_pow2 << 1) >= size)
		size_pow2 = size_pow2 << 1;
	myNode->blocks = size_pow2;
	myNode->addr = address;
	myNode->left = NULL;
	myNode->right = NULL;
	return myNode;
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

void getMemoryInfo(MemoryInfo *meminfo) {
	return;
}

#endif