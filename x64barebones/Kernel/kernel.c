#include <idtLoader.h>
#include <keyboard.h>
#include <lib.h>
#include <libc.h>
#include <memoryManager.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <sound.h>
#include <stdint.h>
#include <videoDriver.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const shellModuleAddress = (void *) 0x400000;
static void *const heapStartAddress = (void *) 0x500000;

typedef int (*EntryPoint)();

void clearBSS(void *bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void *getStackBase() {
	return (void *) ((uint64_t) &endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					 - sizeof(uint64_t)						// Begin at the top of the stack
	);
}

void *initializeKernelBinary() {
	void *moduleAddresses[] = {
		shellModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

int main() {
	load_idt();
	initializeVideoDriver();
	createMemoryManager(heapStartAddress, 0x10000000);
	void *test[10] = {0};
	for (uint8_t i = 0; i < 10; i++) {
		test[i] = allocMemory(0x10000);
		printf("%d Allocated 0x%x bytes at 0x%x\n", i, 0x10000, test[i]);
	}
	free(test[0]);
	free(test[1]);
	// printNodes();
	// printList();
	test[0] = allocMemory(0x20000);
	printf("Allocated 0x%x bytes at 0x%x\n\n", 0x20000, test[0]);
	printNodes();
	free(test[4]);
	free(test[5]);
	free(test[6]);
	printNodes();
	test[4] = allocMemory(0x20000);
	printf("4 Allocated 0x%x bytes at 0x%x\n", 0x20000, test[4]);
	test[5] = allocMemory(0x5000);
	printf("5 Allocated 0x%x bytes at 0x%x\n", 0x5000, test[5]);
	test[6] = allocMemory(0x5000);
	printf("6 Allocated 0x%x bytes at 0x%x\n\n", 0x5000, test[6]);
	free(test[4]);
	free(test[5]);
	free(test[6]);
	test[4] = allocMemory(0x30000);
	printf("4 Allocated 0x%x bytes at 0x%x\n", 0x30000, test[4]);
	test[5] = allocMemory(0x2000);
	printf("5 Allocated 0x%x bytes at 0x%x\n", 0x20000, test[5]);
	test[6] = allocMemory(0x1000);
	printf("6 Allocated 0x%x bytes at 0x%x\n", 0x10000, test[6]);

	((EntryPoint) shellModuleAddress)();
	syscall_puts(STD_ERR, (uint8_t *) "Shell has quit, kernel halting", 31);

	return 0;
}
