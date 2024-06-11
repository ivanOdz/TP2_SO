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

extern uint64_t atomicAdd(uint16_t *a, uint16_t b);
extern uint64_t atomicExchange(void *a, uint16_t b);
extern void *atomicCompareExchange(void *value, uint64_t expected, uint64_t newValue);
extern void atomicHighValueCheck(uint16_t *value, uint16_t top, uint16_t resetValue);
extern void atomicLowValueCheck(uint16_t *value, uint16_t bottom, uint16_t resetValue);

#endif