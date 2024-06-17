#ifndef COMMANDS_H
#define COMMANDS_H

#include <libc.h>
#include <snake.h>
#include <stdint.h>
#include <tests.h>
#include <userProcesses.h>

// ------------------------------- TEXTS -------------------------------------------
#define STR_PREAMBLE_FOR_COMANDS "These are the avaliable commands:\n"
// ---------------------------------------------------------------------------------
#define AVAILABLE_COMMANDS (sizeof(avCommands) / sizeof(availableCommand))

#define COMMAND_EXCECUTED			0
#define COMMAND_NOT_FOUND			1
#define COMMAND_NOT_IMPLEMENTED		2
#define COMMAND_INSUFICIENT_ARGS	3
#define COMMAND_ARGS_NOT_RECOGNIZED 4
#define SHELL_TIMER					65000
// ------------------------------- Built in commands -------------------------------
int command_help(int argc, char **argv);
int command_clear(int argc, char **argv);
int command_setFontSize(int argc, char **argv);
int command_getRegisters();
int command_tunes(int argc, char **argv);
int command_argvTest(int argc, char **argv);
int command_kill(int argc, char **argv);

// ---------------------------------------------------------------------------------

typedef struct {
	const char *name;
	uint8_t builtin;
	int (*function)(int, char **); // argc, argv
} availableCommand;

static text_format shell_fmt = {0x00000000, 0x0000FF00, 1, 1};

static const availableCommand avCommands[] = {
	{"help", TRUE, command_help},
	{"clear", TRUE, command_clear},
	{"setFontSize", TRUE, command_setFontSize},
	{"getRegisters", TRUE, command_getRegisters},
	{"tunes", TRUE, command_tunes},

	{"argvTest", FALSE, (int (*)(int, char **)) testArgv},
	{"mm_test", FALSE, (int (*)(int, char **)) mm_test},
	{"processes_test", FALSE, (int (*)(int, char **)) test_processes},
	{"priority_test", FALSE, (int (*)(int, char **)) test_priority},
	{"sync_test", FALSE, (int (*)(int, char **)) testSemaphores},

	{"snake", FALSE, (int (*)(int, char **)) snake},
	{"ex", FALSE, (int (*)(int, char **)) testExceptions},
	{"mem", FALSE, (int (*)(int, char **)) mem},
	{"ps", FALSE, (int (*)(int, char **)) ps},
	{"lsof", FALSE, (int (*)(int, char **)) lsof},
	{"loop", FALSE, (int (*)(int, char **)) greeting},
	{"kill", TRUE, command_kill},
	{"nice", FALSE, (int (*)(int, char **)) nice},
	{"block", FALSE, (int (*)(int, char **)) block},
	{"cat", FALSE, (int (*)(int, char **)) cat},
	{"filter", FALSE, (int (*)(int, char **)) filter},
	{"wc", FALSE, (int (*)(int, char **)) wc},
	{"phylo", FALSE, (int (*)(int, char **)) phylo},
};
#endif