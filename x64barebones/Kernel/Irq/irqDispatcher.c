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
#include <semaphores.h>

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
	return timer_handler();
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
	if (!process->fileDescriptors[fd].pipe) {
		return -1;
	}
	uint64_t written = writeFifo(process->fileDescriptors[fd].pipe, buf, size, TRUE);
	if (strcmp(process->fileDescriptors[fd].pipe->name, CONSOLE_NAME) == 0 || strcmp(process->fileDescriptors[fd].pipe->name, ERROR_NAME) == 0)
		updateScreen();
	return written;
}

int64_t syscall_read(uint8_t fd, char *buf, uint64_t size) {
	PCB *process = getCurrentProcess();
	if (!process->fileDescriptors[fd].pipe) {
		return -1;
	}
	return readFifo(process->fileDescriptors[fd].pipe, buf, size, TRUE);
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

void syscall_putBlock(draw_type *draw) {
	putBlock(draw);
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

int64_t syscall_semaphore_create(uint32_t initialValue) {
	return (int64_t)semaphoreCreate(initialValue);
}

int64_t syscall_semaphore_open(uint16_t id) {
	return (int64_t)semaphoreOpen(id);
}

int64_t syscall_semaphore_close(uint16_t id) {
	return (int64_t)semaphoreClose(id);
}

void syscall_semaphore_binary_post(uint16_t id) {
	return semaphoreBinaryPost(id);
}

void syscall_semaphore_binary_wait(uint16_t id) {
	semaphoreBinaryWait(id);
}

void syscall_semaphore_post(uint16_t id) {
	semaphorePost(id);
}

void syscall_semaphore_wait(uint16_t id) {
	semaphoreWait(id);
}

typedef int (*EntryPoint)();
