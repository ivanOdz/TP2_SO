#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void *memset(void *destination, int32_t character, uint64_t length);
void *memcpy(void *destination, const void *source, uint64_t length);

char *cpuVendor(char *result);

char getChar();

typedef struct {
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rsi, rdi, rbp, rdx, rcx, rbx, rax, rip, cs, rflags, rsp, ss;
} stack_registers;

void printRegs(stack_registers regs, uint64_t fd);
void clearBuffer(uint8_t *buffer);

extern int haltProcess(int argc, char **argv);

extern void atomicExchange(void *a, void *b);
extern void *atomicCompareExchange(void **value, void *expected, void *newValue);
extern void atomicHighValueCheck(void **value, void *top, void *resetValue);
extern void atomicLowValueCheck(void **value, void *bottom, void *resetValue);

#endif