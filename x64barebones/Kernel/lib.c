// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <lib.h>
#include <naiveConsole.h>
#include <stdint.h>
#include <videoDriver.h>

void *memset(void *destination, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t) c;
	char *dst = (char *) destination;

	while (length--) {
		dst[length] = chr;
	}

	return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length) {
	/*
	 * memcpy does not support overlapping buffers, so always do it
	 * forwards. (Don't change this without adjusting memmove.)
	 *
	 * For speedy copying, optimize the common case where both pointers
	 * and the length are word-aligned, and copy word-at-a-time instead
	 * of byte-at-a-time. Otherwise, copy by bytes.
	 *
	 * The alignment logic below should be portable. We rely on
	 * the compiler to be reasonably intelligent about optimizing
	 * the divides and modulos out. Fortunately, it is.
	 */
	uint64_t i;

	if ((uint64_t) destination % sizeof(uint32_t) == 0 &&
		(uint64_t) source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0) {
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *) source;

		for (i = 0; i < length / sizeof(uint32_t); i++) {
			d[i] = s[i];
		}
	}
	else {
		uint8_t *d = (uint8_t *) destination;
		const uint8_t *s = (const uint8_t *) source;

		for (i = 0; i < length; i++) {
			d[i] = s[i];
		}
	}

	return destination;
}

// recibe un struct de registros por stack (basado en el pushState/popState macro) usa uintToBase para convertir a hexa
void printRegs(stack_registers regs, uint64_t fd) {
	setFontSize(1);
	uint8_t buffer[32] = {'0'};
	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\nRAX: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.rax, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tRBX: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.rbx, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tRCX: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.rcx, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tRDX: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.rdx, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\nRBP: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.rbp, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tRDI: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.rdi, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tRSI: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.rsi, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\t R8: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.r8, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\n R9: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.r9, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tR10: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.r10, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tR11: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.r11, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tR12: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.r12, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\nR13: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.r13, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tR14: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.r14, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tR15: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.r15, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tRIP: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.rip, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\n CS: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.cs, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\t SS: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.ss, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "\tRSP: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.rsp, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, (uint8_t *) "  FLAGS: 0x");
	syscall_puts(fd, (uint8_t *) (buffer + uintToBase(regs.rflags, (char *) (buffer + 16), 16)), 16);
	drawWord(fd, "\n");
}

void clearBuffer(uint8_t *buffer) {
	for (int i = 0; i < 32; i++) {
		buffer[i] = '0';
	}
}