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
	uint64_t totalMemory;
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
uint8_t binarySearch(void *addr, BlockNode *node, uint8_t level);
void deleteNode(BlockNode *node);

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
	while (size_pow2 <= memAmount && (size_pow2 << 1) < memAmount)
		size_pow2 <<= 1;
	memMan.totalMemory = size_pow2;

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
			actualNode->blocks = memMan.totalMemory / (listIndex + 1);
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
	while (size / BLOCK_SIZE > size_pow2)
		size_pow2 <<= 1;

	if (!memMan.root) {
		memMan.root = newNode(memMan.startAddress, memMan.totalMemory);
		if (!memMan.root)
			return NULL;
		return findMemory(size_pow2, memMan.root, 1);
	}
	return findMemory(size / BLOCK_SIZE, memMan.root, 0);
}

void *findMemory(uint64_t blocks, BlockNode *node, uint8_t virginNode) {
	if (node->blocks < blocks) {
		return NULL;
	}
	/// yo soy candidato ideal
	uint8_t minimal = node->blocks >= blocks && (node->blocks >> 1) < blocks;
	if (minimal && virginNode) {
		return node->addr;
	}

	// quiero DFS o como se llame, asi que priorizo ir por ramas ya existentes
	if (!node->left) {
		if (!node->right) {
			/// no hay ningun hijo, asi que quiero ver si recien me crearon para seguir avanzando o si soy memoria alocada 😛
			if (virginNode) {
				node->left = newNode(node->addr, node->blocks >> 1);
				return findMemory(blocks, node->left, virginNode);
			}
			return NULL;
		}
		void *result = findMemory(blocks, node->right, virginNode);
		if (result) /// encontro memoria en right
			return result;
		if (!minimal) {
			/// nos vamos a left
			node->left = newNode(node->addr, node->blocks / 2);
			return findMemory(blocks, node->left, 1);
		}
		return NULL;
	}
	if (!node->right) {
		void *result = findMemory(blocks, node->left, virginNode);
		if (result) /// encontro memoria en left
			return result;
		/// nos vamos a right
		node->right = newNode(node->addr + (node->blocks * BLOCK_SIZE / 2), node->blocks / 2);
		return findMemory(blocks, node->right, 1);
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

void free(void *ptrAllocatedMemory) {
	if (memMan.root)
		binarySearch(ptrAllocatedMemory, memMan.root, 0);
	return;
}

uint8_t binarySearch(void *addr, BlockNode *node, uint8_t level) {
	if (node->addr == addr && !node->left && !node->right) {
		deleteNode(node);
		return 1;
	}
	void *breakAddr = node->addr + node->blocks * BLOCK_SIZE / 2;
	if (addr >= breakAddr) {
		// if its here, it must be in right
		if (!node->right)
			return 0;
		if (binarySearch(addr, node->right, level + 1)) {
			node->right = NULL;
			// found & deleted. should I delete myself as well?
			if (node->left == NULL && node->right == NULL) {
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
		if (binarySearch(addr, node->left, level + 1)) {
			node->left = NULL;
			// found & deleted. should I delete myself as well?
			if (node->left == NULL && node->right == NULL) {
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

void getMemoryInfo(MemoryInfo *meminfo) {
	return;
}

void printPad(void *nodeAddr, void *memAddr, uint64_t blocks, BlockNode *left, BlockNode *right, int tree, int nodenum) {
	while (tree > 0) {
		if (tree == 1) {
			printf("|__");
		}
		else if (tree & 1) {
			printf("|  ");
		}
		else {
			printf("   ");
		}
		tree = tree >> 1;
	}
	printf("(%d) %x (%x %x %x %x)\n", nodenum, nodeAddr, memAddr, blocks, left, right);
	for (uint32_t i = 0; i < 10000000; i++) {
	}
}

void printGraphRec(BlockNode *Node, int depth, int tree, int nodenum) {
	printPad(Node, Node->addr, Node->blocks, Node->left, Node->right, tree, nodenum);
	int newTree = tree + (1 << (depth));
	if (Node->right == NULL && depth >= 1) {
		newTree = newTree - (1 << (depth - 1));
	}
	if (Node->left)
		printGraphRec(Node->left, depth + 1, newTree, 1);
	if (Node->right)
		printGraphRec(Node->right, depth + 1, newTree, 2);
}

void printNodes() {
	printGraphRec(memMan.root, 0, 0, 0);
}

#endif
