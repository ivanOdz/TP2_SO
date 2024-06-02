#include <libc.h>
#define MAX_BLOCKS_DEFAULT	   4000
#define MAX_BLOCK_SIZE_DEFAULT 30000000
#define MIN_BLOCK_SIZE_DEFAULT 10
#define BURN_IN_DEFAULT		   3
#define NULL_MALLOC_RETRY	   100
#define HELP_STRING			   "\nmm_test is a program designed to stress test the system memory manager \
by reserving a bunch of memory in blocks and ensuring \nthese dont overlap by filling each block with a \
probe value and testing for it. If a block fails, you'll see a red warning, \n\
though the test will proceed.\nArguments:\n\n\
-help\t\t\tdisplays this help message\n\n\
-maxmemsize x\tsets maximum memory usage in MiB (defaults to all available free memory. May use less memory than what's set\n\t\t\t\t here if malloc cant allocate a random size block anymore because of internal fragmentation) (ex. -mem 128)\n\n\
-maxblocksize x  sets maximum memory block size in bytes (defaults to %d) (ex. -maxblocksize 200)\n\n\
-minblocksize x  sets minimum memory block size in bytes (defaults to %d) (ex. -minblocksize 20)\n\n\
-burnin x\t\tsets how many times the test will loop over (defaults to %d) (ex. -burn-in 4)\n\n\
-maxblocks x\t sets maximum amount of blocks to allocate (defaults to %d) (ex. -max-blocks 1000)\n\n\
Usage example:\n\t\t mm_test -maxmemsize 256 -maxblocks 4000 -burnin 5\n\n"

uint64_t argumentParse(int arg, int argc, char **argv) {
	if (arg + 1 >= argc) {
		fprintf(STD_ERR, "Argument error, missing value\n");
		return 0;
	}
	else {
		uint64_t result = stringToInt(argv[arg + 1], strlen((uint8_t *) argv[arg + 1]));
		if (!result)
			fprintf(STD_ERR, "Argument error, expected value for %s, got %s\n", argv[arg], argv[arg + 1]);
		return result;
	}
}

int mm_test(int argc, char **argv) {
	uint64_t maxBlocks = MAX_BLOCKS_DEFAULT;
	uint64_t maxMemSize = 0;
	uint64_t maxBlockSize = MAX_BLOCK_SIZE_DEFAULT;
	uint64_t minBlockSize = MIN_BLOCK_SIZE_DEFAULT;
	uint64_t burnin = BURN_IN_DEFAULT;
	for (int arg = 1; arg < argc; arg++) {
		printf("Parsing %s\n", argv[arg]);
		if (strcmp((uint8_t *) argv[arg], (uint8_t *) "-help") == 0) {
			printf(HELP_STRING, MAX_BLOCK_SIZE_DEFAULT, MIN_BLOCK_SIZE_DEFAULT, BURN_IN_DEFAULT, MAX_BLOCKS_DEFAULT);
			return 0;
		}
		else if (strcmp((uint8_t *) argv[arg], (uint8_t *) "-maxmemsize") == 0) {
			maxMemSize = argumentParse(arg++, argc, argv);
			if (!maxMemSize)
				return -1;
		}
		else if (strcmp((uint8_t *) argv[arg], (uint8_t *) "-maxblocksize") == 0) {
			maxBlockSize = argumentParse(arg++, argc, argv);
			if (!maxBlockSize)
				return -1;
		}
		else if (strcmp((uint8_t *) argv[arg], (uint8_t *) "-minblocksize") == 0) {
			minBlockSize = argumentParse(arg++, argc, argv);
			if (!minBlockSize)
				return -1;
		}
		else if (strcmp((uint8_t *) argv[arg], (uint8_t *) "-burnin") == 0) {
			burnin = argumentParse(arg++, argc, argv);
			if (!burnin)
				return -1;
		}
		else if (strcmp((uint8_t *) argv[arg], (uint8_t *) "-maxblocks") == 0) {
			maxBlocks = argumentParse(arg++, argc, argv);
			if (!maxBlocks)
				return -1;
		}
		else {
			fprintf(STD_ERR, "Invalid argument provided (got %s)\n", argv[arg]);
			return -1;
		}
	}
	SyscallClear();

	time_t time;
	SyscallGetRTC(&time);
	srand(time.hora << 16 | time.min << 8 | time.seg);

	uint8_t **test = (uint8_t **) malloc(maxBlocks * sizeof(uint8_t *));
	uint64_t *testsize = (uint64_t *) malloc(maxBlocks * sizeof(uint64_t));

	MemoryInfo mminfoo;
	memoryManagerStats(&mminfoo);

	if (!maxMemSize)
		maxMemSize = mminfoo.freeMemory >> 20;
	printf("Starting memtest - abuse...\n");
	printf("MM Type: %c, Max memory: %d MiB, Max Blocks: %d, Max Block Size: %d bytes, Min Block Size: %d bytes, Burn-in: %d\n", mminfoo.mmType, maxMemSize, maxBlocks, maxBlockSize, minBlockSize, burnin);
	memoryManagerStats(&mminfoo);
	while (burnin--) {
		printf("\nNew iteration (%d left)\n", burnin);
		memoryManagerStats(&mminfoo);
		printf("Free Memory: 0x%x\tOcupied Memory: 0x%x\tAssigned Nodes:%d\n", mminfoo.freeMemory, mminfoo.occupiedMemory, mminfoo.assignedNodes);
		for (int i = 0; i < maxBlocks; i++) {
			test[i] = NULL;
			testsize[i] = 0;
		}
		printf("Allocating memory slots and writing probe data (0000 of %4d blocks) (000MiB of %3dMiB)", maxBlocks, maxMemSize);
		int assigned = 0;
		uint64_t assignedBytes = 0;
		uint8_t tries = 0;
		do {
			testsize[assigned] = randBetween(minBlockSize, maxBlockSize);
			uint8_t *malloctest = (uint8_t *) malloc(testsize[assigned] * sizeof(uint8_t));
			test[assigned] = malloctest;
			uint8_t probe = randBetween(0, 255);
			if (test[assigned] == NULL) {
				tries++;
			}
			else {
				tries = 0;
				assignedBytes += testsize[assigned];
				for (uint64_t j = 0; j < testsize[assigned]; j++) {
					test[assigned][j] = probe;
				}
				assigned++;
			}
			printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b%4d of %4d blocks) (%3dMiB of %3dMiB)", assigned, maxBlocks, assignedBytes >> 20, maxMemSize);
		} while (assigned < maxBlocks && assignedBytes <= maxMemSize << 20 && tries < NULL_MALLOC_RETRY);

		printf("\nAssigned %d slots out of %d\n", assigned, maxBlocks);
		memoryManagerStats(&mminfoo);
		printf("Free Memory: 0x%x\tOcupied Memory: 0x%x\tFragmented Memory: 0x%x\tMax Frag Block Size:%x\nMin Frag Block Size:%x\tAssigned Nodes:%d\n", mminfoo.freeMemory, mminfoo.occupiedMemory, mminfoo.fragmentedMemory, mminfoo.maxFragmentedSize, mminfoo.minFragmentedSize, mminfoo.assignedNodes);
		printf("Testing allocated memory for overlaps and corruption (0000)");
		for (int testNum = 0; testNum < assigned; testNum++) {
			uint8_t *memSlot = test[testNum];
			uint8_t probe = memSlot[0];
			for (uint64_t position = 1; position < testsize[testNum]; position++) {
				if (memSlot[position] != probe) {
					fprintf(STD_ERR, "\nTest %d Error at 0x%x (pos %d of %d) (read %d, expected %d, previous %d)\n", testNum, memSlot + position, position, testsize[testNum], memSlot[position], memSlot[0], memSlot[position - 1]);
					printf("Testing allocated memory for overlaps and corruption (0000)");
					break;
				}
			}
			printf("\b\b\b\b\b%4d)", testNum + 1);
		}
		// SyscallPrintMem();
		printf("\nFreeing allocated memory...\n");
		for (int i = 0; i < assigned; i++) {
			free(test[i]);
		}
		memoryManagerStats(&mminfoo);
		printf("Free Memory: 0x%x\tOcupied Memory: 0x%x\tAssigned Nodes:%d\n", mminfoo.freeMemory, mminfoo.occupiedMemory, mminfoo.assignedNodes);
	}
	free(test);
	free(testsize);
	return 0;
}

int test_processes(int argc, char **argv) {
	return 0;
}
