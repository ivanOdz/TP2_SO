#ifndef COMMANDS_H
#define COMMANDS_H

#include <libc.h>
#include <snake.h>
#include <stdint.h>
#include <tests.h>
#include <userProcesses.h>

// ------------------------------- TEXTS -------------------------------------------
#define STR_PREAMBLE_FOR_COMANDS "Estos son los comandos disponibles para ejecutar:\n"
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
	{"snake", FALSE, (int (*)(int, char **)) snake},
	{"ex", FALSE, (int (*)(int, char **)) testExceptions},
	{"setFontSize", TRUE, command_setFontSize},
	{"getTime", FALSE, (int (*)(int, char **)) time},
	{"getRegisters", TRUE, command_getRegisters},
	{"argvTest", FALSE, (int (*)(int, char **)) testArgv},
	{"tunes", TRUE, command_tunes},
	{"mm_test", FALSE, (int (*)(int, char **)) mm_test},
	{"test_processes", FALSE, (int (*)(int, char **)) test_processes},
	{"test_priority", FALSE, (int (*)(int, char **)) test_priority},
	{"ps", FALSE, (int (*)(int, char **)) ps},
	{"nice", FALSE, (int (*)(int, char **)) nice},
	{"kill", TRUE, command_kill},
	{"block", FALSE, (int (*)(int, char **)) block},
	{"mem", FALSE, (int (*)(int, char **)) mem},
	{"loop", FALSE, (int (*)(int, char **)) greeting},
	{"cat", FALSE, (int (*)(int, char **)) cat},
	{"filter", FALSE, (int (*)(int, char **)) filter},
	{"wc", FALSE, (int (*)(int, char **)) wc}};
#endif