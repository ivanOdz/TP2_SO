#include <idtLoader.h>
#include <interrupts.h>
#include <keyboard.h>
#include <lib.h>
#include <libc.h>
#include <memoryManager.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <processes.h>
#include <scheduler.h>
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

typedef int (*EntryPoint)(int argc, char **argv);

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
	_cli();
	load_idt();
	initializeVideoDriver();
	createMemoryManager(heapStartAddress, 0x10000000);
	char *argv[2] = {"Shell", NULL};
	initScheduler();
	execute((EntryPoint) shellModuleAddress, argv, FOREGROUND);
	_sti();

	// syscall_puts(STD_ERR, (uint8_t *) "Shell has quit, kernel halting", 31);
	while (TRUE) {
		haltProcess(0, NULL);
	}
	return 0;
}
