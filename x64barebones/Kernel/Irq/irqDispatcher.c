#include <defs.h>
#include <keyboard.h>
#include <lib.h>
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

uint64_t syscall_write(uint8_t fd, uint8_t *buf, uint64_t size) {
	switch (fd) {
		case STD_OUT:
		case STD_ERR:
			return syscall_puts(fd, (uint8_t *) buf, size);
		default:
			return 0;
	}
}

uint64_t syscall_read(uint8_t fd, uint8_t *buf, uint64_t size) {
	switch (fd) {
		case STD_IN:
			return consume_keys(buf, size);
		default:
			return 0;
	}
}

uint64_t syscall_clear() {
	clear();
	return 0;
}

uint64_t syscall_getRTC(time_t *buf) {
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

typedef int (*EntryPoint)();
