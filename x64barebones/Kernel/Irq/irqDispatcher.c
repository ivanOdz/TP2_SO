// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <keyboard.h>
#include <lib.h>
#include <memoryManager.h>
#include <processes.h>
#include <scheduler.h>
#include <sound.h>
#include <stdint.h>
#include <time.h>
#include <videoDriver.h>

#define MAX_INTS	 256
#define MAX_SYSCALLS 338

extern int int80_handler();

static uint64_t nullHandler();
static uint64_t int_00();
static uint64_t int_01();
static uint64_t int_80();

extern uint64_t syscall_getRegisters(uint8_t fd, uint8_t *buf, uint64_t size);

static uint64_t (*irqList[])(void) = {&int_00, &int_01, &nullHandler, &nullHandler, &nullHandler, &nullHandler, &int_80};

uint64_t irqDispatcher(uint64_t irq) {
	updateCurrentStack();
	return (*irqList[irq])();
}

uint64_t nullHandler() {
	return 0;
}

uint64_t int_00() {
	timer_handler();
	return 0;
}

uint64_t int_01() {
	keyboard_handler();
	return 0;
}

uint64_t int_80() {
	return int80_handler();
}

/*uint64_t syscall_write(uint8_t fd, char *buf, uint64_t size) {
	switch (fd) {
		case STD_OUT:
		case STD_ERR:
			return syscall_puts(fd, buf, size);
		default:
			return 0;
	}
}*/

int64_t syscall_write(uint8_t fd, char *buf, uint64_t size) {
	// Tengo que obtener el proceso actual, entrar al fd que me pasan,
	// y escribir en el buffer, si es que tengo lugar, lo que me estan pasando.
	PCB *process = getCurrentProcess();
	//if (process->fileDescriptors[fd].isBeingUsed == 0) {
	//	return -1;
	//}
	return syscall_puts(fd, buf, size);
}

/*uint64_t syscall_read(uint8_t fd, char *buf, uint64_t size) {
	switch (fd) {
		case STD_IN:
			return consume_keys(buf, size);
		default:
			return 0;
	}
}*/

int64_t syscall_read(uint8_t fd, char *buf, uint64_t size) {
	PCB *process = getCurrentProcess();
	if (process->fileDescriptors[fd].isBeingUsed == 0) {
		return -1;
	}
	return consume_keys2(buf, process->fileDescriptors[fd].pipe, size);
}

uint64_t syscall_clear() {
	clear();
	return 0;
}

uint64_t syscall_getRTC(time_type *buf) {
	return (getRTC(buf));
}

uint64_t syscall_getFormat(text_format *buf) {
	return getFormat(buf);
}
uint64_t syscall_setFormat(text_format *buf) {
	return setFormat(buf);
}

uint64_t syscall_putBlock(draw_type *draw) {
	putBlock(draw);
	return 0;
}

uint64_t syscall_getTicks() {
	return get_ticks();
}
uint64_t syscall_playSound(uint8_t flags, const uint8_t *buffer, uint64_t length) {
	playSound(flags, buffer, length);
	return 0;
}
uint64_t syscall_setTimer(uint16_t delay) {
	setPIT0Freq(delay);
	return 0;
}

void *syscall_malloc(uint64_t size) {
	return allocMemory(size);
}

uint64_t syscall_free(void *memory) {
	freeMemory(memory);
	return 0;
}
uint64_t syscall_meminfo(MemoryInfo *meminfo) {
	getMemoryInfo(meminfo);
	return 0;
}

uint64_t syscall_printMemory() {
	return 0;
}

uint64_t syscall_execv(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode) {
	return execute(processMain, argv, runMode);
}

uint64_t syscall_exit(int returnValue) {
	exitProcess(returnValue);
	return 0;
}
PID_t syscall_kill(PID_t PID) {
	return killProcess(PID);
}

PID_t syscall_getPID() {
	return getCurrentPID();
}

uint64_t syscall_waitpid(uint64_t PID, ReturnStatus *wstatus) {
	return waitPID(PID, wstatus);
}

ProcessInfo *syscall_processInfo() {
	return processInfo();
}

uint64_t syscall_nice(uint16_t pid, uint8_t newPriority) {
	setProcessPriority(pid, newPriority);
	return 0;
}

uint64_t syscall_block(uint16_t pid) {
	return blockProcess(pid);
}

void syscall_yield(uint16_t pid) {
	schedyield();
}

void syscall_sleep(uint64_t ms) {
	processSleep(ms);
}

typedef int (*EntryPoint)();
