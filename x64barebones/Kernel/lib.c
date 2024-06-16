// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <lib.h>
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
	char buffer[32] = {'0'};
	clearBuffer(buffer);
	drawWord(fd, "\nRAX: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.rax, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tRBX: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.rbx, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tRCX: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.rcx, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tRDX: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.rdx, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\nRBP: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.rbp, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tRDI: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.rdi, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tRSI: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.rsi, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\t R8: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.r8, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\n R9: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.r9, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tR10: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.r10, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tR11: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.r11, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tR12: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.r12, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\nR13: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.r13, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tR14: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.r14, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tR15: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.r15, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tRIP: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.rip, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\n CS: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.cs, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\t SS: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.ss, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "\tRSP: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.rsp, (char *) (buffer + 16), 16)), 16);

	clearBuffer(buffer);
	drawWord(fd, "  FLAGS: 0x");
	syscallPuts(fd, (buffer + uintToBase(regs.rflags, (char *) (buffer + 16), 16)), 16);
	drawWord(fd, "\n");
}

void clearBuffer(char *buffer) {
	for (int i = 0; i < 32; i++) {
		buffer[i] = '0';
	}
}