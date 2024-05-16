#include <defs.h>
#include <lib.h>
#include <videoDriver.h>

#define ZERO_EXCEPTION_ID			0
#define INVALID_OPCODE_EXCEPTION_ID 1

static void zero_division();
static void invalid_opcode();

void exceptionDispatcher(int exception, stack_registers regs) {
	if (exception == ZERO_EXCEPTION_ID) {
		zero_division(regs);
	}
	if (exception == INVALID_OPCODE_EXCEPTION_ID) {
		invalid_opcode(regs);
	}
}

static void zero_division(stack_registers regs) {
	drawWord(STD_ERR, (uint8_t *) "EXCEPTION 00 - DIVIDE BY ZERO\nRegistry Dump:\n");
	printRegs(regs, STD_ERR);
	drawWord(STD_ERR, (uint8_t *) "\nProgram has aborted. Returning to shell...\n\n\n");
}

static void invalid_opcode(stack_registers regs) {
	drawWord(STD_ERR, (uint8_t *) "EXCEPTION 06 - INVALID OP-CODE\nRegistry Dump:\n");
	printRegs(regs, STD_ERR);
	drawWord(STD_ERR, (uint8_t *) "\nProgram has aborted. Returning to shell...\n\n\n");
}