// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <memoryManager.h>
#include <stddef.h>

#ifdef BUDDY
#define BLOCK_SIZE	  8 /// 8 chars = 64 bits
#define LIST_MEM_SIZE 8192
#define MIN_MEM_SIZE  8
#define MAX_LEVELS	  32

//#define BUDDY_ITERATIVE
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
	uint64_t totalBlocks;
	uint64_t largestAvailableMemoryBlock;
	uint32_t levels;
	uint32_t minBlockMemory;
} MemoryManagerCDT;

BlockNode list[LIST_MEM_SIZE] = {0};
BlockNode *route[MAX_LEVELS] = {0};

static MemoryManagerCDT memMan;

void *findMemory(uint64_t size, BlockNode *node, uint8_t virginNode);
BlockNode *newNode(void *address, uint64_t size);
BlockNode *getNextFree();
uint8_t binarySearch(void *addr, BlockNode *node, uint8_t *found);
void deleteNode(BlockNode *node);
void *getMemoryRecursive(MemoryInfo *meminfo, BlockNode *node, void *lastUsedAddress);

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
	while (size_pow2 < memAmount && (size_pow2 * 2) <= memAmount)
		size_pow2 *= 2;
	memMan.totalBlocks = size_pow2 / BLOCK_SIZE;
	for (int i = 0; i < LIST_MEM_SIZE; i++) {
		list[i].addr = NULL;
		list[i].left = NULL;
		list[i].right = NULL;
		list[i].blocks = 0;
	}
	memMan.root = NULL;
	memMan.levels = 0;
	memMan.minBlockMemory = MIN_MEM_SIZE;
	return &memMan;
}

#ifdef BUDDY_ITERATIVE
void *allocMemory(const uint64_t size) {
	if (size > memMan.largestAvailableMemoryBlock)
		return NULL;

	void *newAlloc = NULL;
	BlockNode *actualNode = memMan.root;
	void *actualAddress = memMan.startAddress;
	uint32_t listIndex = 0; // Points to the father of the actual node
	uint8_t memFound = 0;

	while (!memFound) {
		if (actualNode == NULL) {
			actualNode = getNextFree();
			actualNode->left = NULL;
			actualNode->right = NULL;
			actualNode->addr = actualAddress;
			actualNode->blocks = memMan.totalBlocks / (listIndex + 1);
			if (listIndex > 0) {
				if (route[listIndex]->left == NULL)
					route[listIndex]->left = actualNode;
				else
					route[listIndex]->right = actualNode;
			}
			else {
				route[listIndex] = actualNode;
			}
		}
		if (size <= (actualNode->blocks / 2)) { // Go deeper
			listIndex++;
			route[listIndex] = actualNode;
			actualNode = actualNode->left;
		}
		else if (actualNode->left != NULL && actualNode->right != NULL) { // Go to the right side or climb up
			if (route[listIndex]->left == actualNode) {
				actualAddress = actualNode->addr + actualNode->blocks;
				actualNode = route[listIndex]->right;
			}
			else {
				while (listIndex && route[listIndex]->right != NULL) {
					listIndex--;
				}
				if (listIndex == 0)
					return NULL;
				else
					actualNode = route[listIndex]->right;
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

	uint64_t size_pow2 = 1;
	while (((size - 1) / BLOCK_SIZE) >= size_pow2)
		size_pow2 <<= 1;

	if (!memMan.root) {
		memMan.root = newNode(memMan.startAddress, memMan.totalBlocks);
		if (!memMan.root)
			return NULL;
		return findMemory(size_pow2, memMan.root, 1);
	}
	return findMemory(size_pow2, memMan.root, 0);
}

void *findMemory(uint64_t blocks, BlockNode *node, uint8_t virginNode) {
	if (node->blocks < blocks) {
		return NULL;
	}
	/// yo soy candidato ideal
	uint8_t minimal = (node->blocks >> 1) < blocks;
	if (minimal && virginNode) {
		return node->addr;
	}

	// quiero DFS o como se llame, asi priorizo ir por ramas ya existentes
	if (!node->left) {
		if (!node->right) {
			/// no hay ningun hijo, asi que quiero ver si recien me crearon para seguir avanzando o si soy memoria alocada 😛
			if (virginNode) {
				node->left = newNode(node->addr, node->blocks >> 1);
				void *found = findMemory(blocks, node->left, virginNode);
				if (found == NULL) {
					// no iba por aca
					deleteNode(node->left);
					node->left = NULL;
				}
				return found;
			}
			return NULL;
		}
		void *result = findMemory(blocks, node->right, virginNode);
		if (result) /// encontro memoria en right
			return result;
		if (!minimal) {
			/// nos vamos a left
			node->left = newNode(node->addr, node->blocks / 2);
			result = findMemory(blocks, node->left, 1);
			if (result == NULL) {
				// no iba por aca
				deleteNode(node->left);
				node->left = NULL;
			}
			return result;
		}
		return NULL;
	}
	if (!node->right) {
		void *result = findMemory(blocks, node->left, virginNode);
		if (result) /// encontro memoria en left
			return result;
		/// nos vamos a right
		node->right = newNode(node->addr + (node->blocks * BLOCK_SIZE / 2), node->blocks / 2);
		result = findMemory(blocks, node->right, 1);
		if (result == NULL) {
			// no iba por aca
			deleteNode(node->right);
			node->right = NULL;
		}
		return result;
	}
	void *result = findMemory(blocks, node->left, 0);
	if (result) {
		return result;
	}
	result = findMemory(blocks, node->right, 0);
	return result;
}

#endif

BlockNode *newNode(void *address, uint64_t blocks) {
	BlockNode *myNode = getNextFree();
	if (!myNode)
		return NULL;
	myNode->blocks = blocks;
	myNode->addr = address;
	myNode->left = NULL;
	myNode->right = NULL;
	return myNode;
}

uint8_t freeMemory(void *ptrAllocatedMemory) {
	uint8_t found = 0;
	if (memMan.root)
		binarySearch(ptrAllocatedMemory, memMan.root, &found);
	return found;
}

uint8_t binarySearch(void *addr, BlockNode *node, uint8_t *found) {
	if (node->addr == addr && !node->left && !node->right) {
		deleteNode(node);
		*found = 1;
		return 1;
	}
	void *breakAddr = node->addr + (node->blocks * BLOCK_SIZE) / 2;
	if (addr >= breakAddr) {
		// if its here, it must be in right
		if (!node->right)
			return 0;
		if (binarySearch(addr, node->right, found)) {
			node->right = NULL;
			// found & deleted. should I delete myself as well?
			if (node->left == NULL) {
				deleteNode(node);
				return 1;
			}
			return 0;
		}
	}
	else {
		// if its here it must be in left
		if (!node->left)
			return 0;
		if (binarySearch(addr, node->left, found)) {
			node->left = NULL;
			// found & deleted. should I delete myself as well?
			if (node->right == NULL) {
				deleteNode(node);
				return 1;
			}
			return 0;
		}
	}
	return 0;
}

void deleteNode(BlockNode *node) {
	node->addr = NULL;
	node->blocks = 0;
	node->left = NULL;
	node->right = NULL;
	if (node == memMan.root)
		memMan.root = NULL;
	return;
}

BlockNode *getNextFree() {
	for (uint32_t i = 0; i < LIST_MEM_SIZE; i++) {
		if (list[i].blocks == 0) {
			return &list[i];
		}
	}
	return NULL;
}
/*
typedef struct MemoryInfo {
*	void *startAddress;
*	uint64_t totalMemory;
	uint64_t freeMemory;
	uint64_t occupiedMemory;
	uint64_t fragmentedMemory;
	uint64_t minFragmentedSize;
	uint64_t maxFragmentedSize;
	uint64_t assignedNodes;
*	void *endAddress;
} MemoryInfo;*/
void getMemoryInfo(MemoryInfo *meminfo) {
	meminfo->mmType = 'B';
	meminfo->startAddress = memMan.startAddress;
	meminfo->totalMemory = memMan.totalBlocks * BLOCK_SIZE;
	meminfo->occupiedMemory = 0;
	meminfo->fragmentedMemory = 0;
	meminfo->minFragmentedSize = memMan.totalBlocks * BLOCK_SIZE;
	meminfo->maxFragmentedSize = 0;
	meminfo->assignedNodes = 0;
	if (memMan.root)
		meminfo->endAddress = getMemoryRecursive(meminfo, memMan.root, memMan.startAddress);
	else
		meminfo->endAddress = memMan.startAddress;
	meminfo->freeMemory = meminfo->totalMemory - meminfo->occupiedMemory;
	return;
}
// casos  -> 2 ramas -> me adentro
//			1 rama  -> me adentro a una y retorno la otra
//			0 ramas -> sumo memoria, comparo addr ultima y sumo frag y free, retorno
void *getMemoryRecursive(MemoryInfo *meminfo, BlockNode *node, void *lastUsedAddress) {
	if (!node->left && !node->right) {
		// leaf
		meminfo->occupiedMemory += node->blocks * BLOCK_SIZE;
		meminfo->assignedNodes++;
		if (node->addr != lastUsedAddress) {
			uint64_t fragmented = node->addr - lastUsedAddress;
			meminfo->fragmentedMemory += fragmented;
			if (fragmented > meminfo->maxFragmentedSize)
				meminfo->maxFragmentedSize = fragmented;
			if (fragmented < meminfo->minFragmentedSize)
				meminfo->minFragmentedSize = fragmented;
		}
		return node->addr + node->blocks * BLOCK_SIZE;
	}
	// branch
	if (node->left)
		lastUsedAddress = getMemoryRecursive(meminfo, node->left, lastUsedAddress);
	if (node->right)
		lastUsedAddress = getMemoryRecursive(meminfo, node->right, lastUsedAddress);
	return lastUsedAddress;
}

#endif
