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
#define BUFFER_DEFAULT_SIZE 255
#define AVAILABLE_COMMANDS	(sizeof(avCommands) / sizeof(availableCommand))

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

// ---------------------------------------------------------------------------------

typedef struct {
	const uint8_t *name;
	uint8_t builtin;
	int (*function)(int, char **); // argc, argv

} availableCommand;

static const availableCommand avCommands[] = {

	{(uint8_t *) "help", TRUE, command_help},
	{(uint8_t *) "clear", TRUE, command_clear},
	{(uint8_t *) "snake", FALSE, snake},
	{(uint8_t *) "ex", FALSE, testExceptions},
	{(uint8_t *) "setFontSize", TRUE, command_setFontSize},
	{(uint8_t *) "getTime", FALSE, time},
	{(uint8_t *) "getRegisters", TRUE, command_getRegisters}, // false
	{(uint8_t *) "argvTest", FALSE, testArgv},
	{(uint8_t *) "tunes", TRUE, command_tunes},
	{(uint8_t *) "mm_test", FALSE, mm_test},
	{(uint8_t *) "test_processes", TRUE, test_processes},
	{(uint8_t *) "ps", FALSE, ps},
	{(uint8_t *) "nice", TRUE, nice},
	{(uint8_t *) "block", TRUE, block}};

int64_t commands(uint8_t *strBuffer);

#endif