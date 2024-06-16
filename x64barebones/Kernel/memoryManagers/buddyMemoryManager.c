// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <libc.h>
#include <memoryManager.h>
#include <stddef.h>

#ifdef BUDDY
#define BLOCK_SIZE	  8 // 8 chars = 64 bits
#define LIST_MEM_SIZE 8192

typedef struct BlockNode {
	void *addr;
	uint64_t blocks;
	struct BlockNode *left;
	struct BlockNode *right;
} BlockNode;

typedef struct MemoryManagerCDT {
	void *startAddress;
	BlockNode *root;
	uint64_t totalBlocks;
} MemoryManagerCDT;

BlockNode list[LIST_MEM_SIZE] = {0};

static MemoryManagerCDT memMan;

void *findMemory(uint64_t size, BlockNode *node, bool virginNode);
BlockNode *newNode(void *address, uint64_t size);
BlockNode *getNextFree();
bool binaryDelete(void *addr, BlockNode *node, bool *found);
void deleteNode(BlockNode *node);
void *getMemoryRecursive(MemoryInfo *meminfo, BlockNode *node, void *lastUsedAddress);

MemoryManagerADT createMemoryManager(void *const restrict managedMemory, uint64_t memAmount) {
	if (managedMemory == NULL || memAmount < BLOCK_SIZE) {
		return NULL;
	}
	memMan.startAddress = managedMemory;
	if ((uint64_t) memMan.startAddress % BLOCK_SIZE) {
		memMan.startAddress = (void *) ((uint64_t) memMan.startAddress / BLOCK_SIZE);
		memMan.startAddress++;
		memMan.startAddress = (void *) ((uint64_t) memMan.startAddress * BLOCK_SIZE);
	}
	uint64_t size_pow2 = 1;
	while (size_pow2 < memAmount && (size_pow2 * 2) <= memAmount) {
		size_pow2 *= 2;
	}
	memMan.totalBlocks = size_pow2 / BLOCK_SIZE;
	for (int i = 0; i < LIST_MEM_SIZE; i++) {
		list[i].addr = NULL;
		list[i].left = NULL;
		list[i].right = NULL;
		list[i].blocks = 0;
	}
	memMan.root = NULL;

	return &memMan;
}

void *allocMemory(uint64_t size) {
	if (!size) {
		return NULL;
	}
	uint64_t size_pow2 = 1;
	while (((size - 1) / BLOCK_SIZE) >= size_pow2) {
		size_pow2 <<= 1;
	}
	if (!memMan.root) {
		memMan.root = newNode(memMan.startAddress, memMan.totalBlocks);
		if (!memMan.root) {
			return NULL;
		}
		return findMemory(size_pow2, memMan.root, TRUE);
	}
	return findMemory(size_pow2, memMan.root, FALSE);
}

void *findMemory(uint64_t blocks, BlockNode *node, bool virginNode) {
	if (node->blocks < blocks) {
		return NULL;
	}
	uint8_t minimal = (node->blocks >> 1) < blocks;
	if (minimal && virginNode) {
		return node->addr;
	}
	// Quiero DFS o como se llame, asi priorizo ir por ramas ya existentes (best-fit)
	if (!node->left) {
		if (!node->right) {
			if (virginNode) {
				node->left = newNode(node->addr, node->blocks >> 1);
				void *found = findMemory(blocks, node->left, virginNode);
				if (found == NULL) {
					deleteNode(node->left);
					node->left = NULL;
				}
				return found;
			}
			return NULL;
		}
		void *result = findMemory(blocks, node->right, virginNode);
		if (result) {
			return result;
		}
		if (!minimal) {
			node->left = newNode(node->addr, node->blocks / 2);
			result = findMemory(blocks, node->left, TRUE);
			if (result == NULL) {
				deleteNode(node->left);
				node->left = NULL;
			}
			return result;
		}
		return NULL;
	}
	if (!node->right) {
		void *result = findMemory(blocks, node->left, virginNode);
		if (result) {
			return result;
		}
		node->right = newNode(node->addr + (node->blocks * BLOCK_SIZE / 2), node->blocks / 2);
		result = findMemory(blocks, node->right, TRUE);
		if (result == NULL) {
			deleteNode(node->right);
			node->right = NULL;
		}
		return result;
	}
	void *result = findMemory(blocks, node->left, FALSE);
	if (result) {
		return result;
	}
	result = findMemory(blocks, node->right, FALSE);

	return result;
}

BlockNode *newNode(void *address, uint64_t blocks) {
	BlockNode *myNode = getNextFree();
	if (!myNode) {
		return NULL;
	}
	myNode->blocks = blocks;
	myNode->addr = address;
	myNode->left = NULL;
	myNode->right = NULL;

	return myNode;
}

bool freeMemory(void *ptrAllocatedMemory) {
	bool found = FALSE;
	if (memMan.root) {
		binaryDelete(ptrAllocatedMemory, memMan.root, &found);
	}
	return found;
}

// recibe direccion a encontrar y borrar y el nodo a analizar.
// *found retorna si lo encontro o no
// retorna si borro el nodo en *node o no.
bool binaryDelete(void *addr, BlockNode *node, bool *found) {
	if (node->addr == addr && !node->left && !node->right) {
		deleteNode(node);
		*found = TRUE;
		return TRUE;
	}
	void *breakAddr = node->addr + (node->blocks * BLOCK_SIZE) / 2;
	if (addr >= breakAddr) {
		if (!node->right) { // if its here, it must be in right
			return FALSE;
		}
		if (binaryDelete(addr, node->right, found)) {
			node->right = NULL; // found & deleted. should I delete myself as well?
			if (node->left == NULL) {
				deleteNode(node);
				return TRUE;
			}
			return FALSE;
		}
	}
	else {
		if (!node->left) { // if its here it must be in left
			return FALSE;
		}
		if (binaryDelete(addr, node->left, found)) {
			node->left = NULL; // found & deleted. should I delete myself as well?
			if (node->right == NULL) {
				deleteNode(node);
				return TRUE;
			}
			return FALSE;
		}
	}
	return FALSE;
}

void deleteNode(BlockNode *node) {
	node->addr = NULL;
	node->blocks = 0;
	node->left = NULL;
	node->right = NULL;
	if (node == memMan.root) {
		memMan.root = NULL;
	}
	return;
}

BlockNode *getNextFree() {
	for (uint32_t i = 0; i < LIST_MEM_SIZE; i++) {
		if (!list[i].blocks) {
			return &list[i];
		}
	}
	return NULL;
}

void getMemoryInfo(MemoryInfo *meminfo) {
	meminfo->mmType = 'B';
	meminfo->startAddress = memMan.startAddress;
	meminfo->totalMemory = memMan.totalBlocks * BLOCK_SIZE;
	meminfo->occupiedMemory = 0;
	meminfo->fragmentedMemory = 0;
	meminfo->minFragmentedSize = memMan.totalBlocks * BLOCK_SIZE;
	meminfo->maxFragmentedSize = 0;
	meminfo->assignedNodes = 0;
	if (memMan.root) {
		meminfo->endAddress = getMemoryRecursive(meminfo, memMan.root, memMan.startAddress);
	}
	else {
		meminfo->endAddress = memMan.startAddress;
	}
	meminfo->freeMemory = meminfo->totalMemory - meminfo->occupiedMemory;

	return;
}
// casos  -> 2 ramas -> me adentro
//			 1 rama  -> me adentro a una y retorno la otra
//			 0 ramas -> sumo memoria, comparo addr ultima y sumo frag y free, retorno
void *getMemoryRecursive(MemoryInfo *meminfo, BlockNode *node, void *lastUsedAddress) {
	if (!node->left && !node->right) { // hoja
		meminfo->occupiedMemory += node->blocks * BLOCK_SIZE;
		meminfo->assignedNodes++;
		if (node->addr != lastUsedAddress) {
			uint64_t fragmented = node->addr - lastUsedAddress;
			meminfo->fragmentedMemory += fragmented;
			if (fragmented > meminfo->maxFragmentedSize) {
				meminfo->maxFragmentedSize = fragmented;
			}
			if (fragmented < meminfo->minFragmentedSize) {
				meminfo->minFragmentedSize = fragmented;
			}
		}
		return node->addr + node->blocks * BLOCK_SIZE;
	}

	if (node->left) { // rama
		lastUsedAddress = getMemoryRecursive(meminfo, node->left, lastUsedAddress);
	}
	if (node->right) {
		lastUsedAddress = getMemoryRecursive(meminfo, node->right, lastUsedAddress);
	}

	return lastUsedAddress;
}

#endif
