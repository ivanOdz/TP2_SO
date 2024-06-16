// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <lib.h>
#include <processes.h>
#include <scheduler.h>
#include <videoDriver.h>

#define ZERO_EXCEPTION_ID			0
#define INVALID_OPCODE_EXCEPTION_ID 1

static void zeroDivisionException();
static void invalidOpcodeException();

void exceptionDispatcher(int exception, stack_registers regs) {
	if (exception == ZERO_EXCEPTION_ID) {
		zeroDivisionException(regs);
	}
	if (exception == INVALID_OPCODE_EXCEPTION_ID) {
		invalidOpcodeException(regs);
	}
	killProcess(getCurrentPID());
}

static void zeroDivisionException(stack_registers regs) {
	drawWord(STD_ERR, "EXCEPTION 00 - DIVIDE BY ZERO\nRegistry Dump:\n");
	printRegs(regs, STD_ERR);
	drawWord(STD_ERR, "\nProgram has aborted. Returning to shell...\n");
}

static void invalidOpcodeException(stack_registers regs) {
	drawWord(STD_ERR, "EXCEPTION 06 - INVALID OP-CODE\nRegistry Dump:\n");
	printRegs(regs, STD_ERR);
	drawWord(STD_ERR, "\nProgram has aborted. Returning to shell...\n");
}