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
	if (irq > 6) {
		return 0;
	}
	updateCurrentStack();
	return (*irqList[irq])();
}

uint64_t nullHandler() {
	return 0;
}

uint64_t int_00() {
	return timerHandler();
}

uint64_t int_01() {
	keyboardHandler();
	return 0;
}

uint64_t int_80() {
	return int80_handler();
}

int64_t syscall_write(uint8_t fd, char *buf, uint64_t size) {
	// Tengo que obtener el proceso actual, entrar al fd que me pasan, y escribir en el buffer, si es que tengo lugar, lo que me estan pasando.
	PCB *process = getCurrentProcess();
	if (fd >= MAX_FILE_DESCRIPTORS || !process->fileDescriptors[fd].pipe) {
		return -1;
	}
	uint64_t written = writeFifo(process->fileDescriptors[fd].pipe, buf, size, TRUE);
	if (process->fileDescriptors[fd].pipe->name && (strcmp(process->fileDescriptors[fd].pipe->name, CONSOLE_NAME) == 0 || strcmp(process->fileDescriptors[fd].pipe->name, ERROR_NAME) == 0)) {
		updateScreen();
	}
	return written;
}

int64_t syscall_read(uint8_t fd, char *buf, uint64_t size) {
	PCB *process = getCurrentProcess();
	if (!process->fileDescriptors[fd].pipe) {
		return -1;
	}
	return readFifo(process->fileDescriptors[fd].pipe, buf, size, TRUE);
}
// returns char if readable, 0 if there's nothing to be read, EOF if broken meant for processes that may receive input but can't afford to block forever if they don't
char syscall_tryGetChar(uint8_t fd) {
	PCB *process = getCurrentProcess();
	char c;
	if (!process->fileDescriptors[fd].pipe) {
		return -1;
	}
	readFifo(process->fileDescriptors[fd].pipe, &c, 1, FALSE);
	return c;
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

void syscall_putBlock(draw_type *draw) {
	putBlock(draw);
}

uint64_t syscall_getTicks() {
	return getTicks();
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

uint64_t syscall_execv(int (*processMain)(int argc, char **argv), char **argv, ProcessRunMode runMode) {
	return execute(processMain, argv, runMode);
}

uint64_t syscall_exit(int returnValue) {
	return exitProcess(returnValue);
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

bool syscall_nice(uint16_t pid, uint8_t newPriority) {
	return setProcessPriority(pid, newPriority);
}

uint64_t syscall_block(uint16_t pid) {
	return blockProcess(pid);
}

uint64_t syscall_yield(uint16_t pid) {
	return schedyield();
}

void syscall_sleep(uint64_t ms) {
	processSleep(ms);
}

int64_t syscall_pipe(char *name, int pipefd[2]) {
	return createPipe(name, pipefd);
}

FdInfo *syscall_fdinfo(PID_t pid) {
	return fdInfo(pid);
}

int64_t syscall_open(char *name, FifoMode mode) {
	return openFD(name, mode);
}

int64_t syscall_close(uint64_t fd) {
	return closeFD(fd);
}

int64_t syscall_dup(uint64_t fd) {
	return duplicateFD(fd);
}

int16_t syscall_seminit(int initialValue) {
	return 0; // return ksem_init(initialValue);
}

int16_t syscall_semdestroy(uint16_t id) {
	return 0; // return ksem_init(id);
}

void syscall_sempost(int16_t id) {
	return; // ksem_post(id);
}

void syscall_semwait(int16_t id) {
	return; // ksem_wait(id);
}

typedef int (*EntryPoint)();
