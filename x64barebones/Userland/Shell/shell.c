// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <commands.h>
#include <libc.h>
#include <stdint.h>

#define COLORS_DEFAULT 0x0020FF00
#define BUFFER_SIZE	   256
#define HISTORY_SIZE   10

typedef struct commandBuffer {
	char buffer[BUFFER_SIZE];
	uint8_t position;
	uint8_t editCursor;
} commandBuffer;

void emptyCommandBuffer(commandBuffer *buffer);
void deleteChars(commandBuffer *command);
int64_t runCommand(char *strBuffer);
void printEdit(commandBuffer *command);
void shell(int argc, char **argv) {
	commandBuffer *command = malloc(sizeof(commandBuffer));
	if (!command) {
		fprintf(STD_ERR, "SHELL ERROR\n");
		exit(1);
	}
	commandBuffer *history[10];
	for (int i = 0; i < HISTORY_SIZE; i++)
		history[i] = NULL;
	int historyIndex = 0;
	emptyCommandBuffer(command);
	runCommand("help");
	char incoming;
	while (TRUE) {
		uint64_t read = SyscallRead(STD_IN, &incoming, 1);
		if (read) {
			switch (incoming) {
				case '\n':
					if (!command->position) {
						printf("%s\n", history[0]->buffer);
						runCommand(history[0]->buffer);
						emptyCommandBuffer(command);
					}
					else {
						if (command->editCursor == command->position) {
							putchar('\n');
							command->buffer[command->position] = 0;
						}
						else {
							deleteChars(command);
							command->editCursor = command->position;
							command->buffer[command->position] = 0;
							printf("%s\n", command->buffer);
						}
						runCommand(command->buffer);
						if (history[HISTORY_SIZE - 1])
							free(history[HISTORY_SIZE - 1]);
						for (int i = HISTORY_SIZE - 1; i; i--) {
							history[i] = history[i - 1];
						}
						history[0] = command;
						historyIndex = 0;
						command = malloc(sizeof(commandBuffer));
						emptyCommandBuffer(command);
					}
					break;
				case '\b':
					if (command->position == command->editCursor) {
						if (command->position) {
							command->buffer[--command->position] = 0;
							command->editCursor--;
							putchar('\b');
						}
					}
					else {
						if (command->editCursor) {
							deleteChars(command);
							--command->position;
							for (int i = --command->editCursor; i < command->position; i++) {
								command->buffer[i] = command->buffer[i + 1];
							}
							printEdit(command);
						}
					}
					break;
				case '\e':
					runCommand("clear");
					emptyCommandBuffer(command);
					break;
				case '\t':
					for (int listItem = 0; listItem < AVAILABLE_COMMANDS; listItem++) {
						if (strincludes(avCommands[listItem].name, command->buffer)) {
							deleteChars(command);
							emptyCommandBuffer(command);
							command->position = strcpy(command->buffer, avCommands[listItem].name);
							command->buffer[command->position++] = ' ';
							command->editCursor = command->position;
							puts(command->buffer);
							shell_fmt.enableCursorBlink = TRUE;
							SyscallSetFormat(&shell_fmt);
						}
					}
					break;
				case 0x1E: // up arrow
					if (historyIndex < HISTORY_SIZE && history[historyIndex]) {
						deleteChars(command);
						command->position = strcpy(command->buffer, history[historyIndex++]->buffer);
						command->editCursor = command->position;
						puts(command->buffer);
					}
					break;
				case 0x1F: // down arrow
					if (historyIndex) {
						deleteChars(command);
						command->position = strcpy(command->buffer, history[--historyIndex]->buffer);
						command->editCursor = command->position;
						puts(command->buffer);
					}
					else {
						deleteChars(command);
						emptyCommandBuffer(command);
					}
					break;
				case 0x11: // left arrow
					deleteChars(command);
					if (shell_fmt.enableCursorBlink) {
						shell_fmt.enableCursorBlink = 0;
						SyscallSetFormat(&shell_fmt);
					}
					if (command->editCursor)
						command->editCursor--;
					printEdit(command);
					break;
				case 0x10: // right arrow
					deleteChars(command);
					if (command->editCursor < command->position)
						command->editCursor++;
					if (command->editCursor == command->position) {
						shell_fmt.enableCursorBlink = 1;
						SyscallSetFormat(&shell_fmt);
					}
					printEdit(command);
					break;
				default:
					if (command->position < BUFFER_SIZE - 2) {
						if (command->editCursor == command->position) {
							command->buffer[command->position++] = incoming;
							command->editCursor = command->position;
							putchar(incoming);
						}
						else {
							deleteChars(command);
							++command->position;
							for (int i = command->position; i > command->editCursor; i--) {
								command->buffer[i] = command->buffer[i - 1];
							}
							command->buffer[command->editCursor++] = incoming;
							printEdit(command);
						}
					}
					else {
						fprintf(STD_ERR, "\nCommand is too long\n>> ");
						emptyCommandBuffer(command);
					}
			}
			yield();
		}
	}
	exit(0);
}

void printEdit(commandBuffer *command) {
	if (command->editCursor)
		SyscallWrite(STD_OUT, command->buffer, command->editCursor);
	if (command->editCursor == command->position) {
		shell_fmt.enableCursorBlink = 1;
		SyscallSetFormat(&shell_fmt);
		return;
	}
	uint32_t colorSwap = shell_fmt.bg;
	shell_fmt.bg = shell_fmt.fg;
	shell_fmt.fg = colorSwap;
	SyscallSetFormat(&shell_fmt);
	SyscallWrite(STD_OUT, command->buffer + command->editCursor, 1);
	colorSwap = shell_fmt.bg;
	shell_fmt.bg = shell_fmt.fg;
	shell_fmt.fg = colorSwap;
	SyscallSetFormat(&shell_fmt);
	int remainingChars = command->position - (command->editCursor + 1);
	if (remainingChars)
		SyscallWrite(STD_OUT, command->buffer + command->editCursor + 1, remainingChars);
}

void emptyCommandBuffer(commandBuffer *command) {
	for (int i = 0; i < BUFFER_SIZE; i++) {
		command->buffer[i] = '\0';
	}
	command->position = 0;
	command->editCursor = 0;
}
void deleteChars(commandBuffer *command) {
	if (command->position)
		for (int i = command->position; i; i--) {
			putchar('\b');
		}
}

int64_t runCommand(char *run) {
	char strBuffer[BUFFER_SIZE];
	strcpy(strBuffer, run);
	int argc;
	char *argv[BUFFER_SIZE];

	// trim whitespace & get command
	int position = 0;
	while (position < BUFFER_SIZE && strBuffer[position] == ' ') {
		position++;
	}
	argv[0] = strBuffer + position;
	// Get arguments
	for (argc = 1; position < BUFFER_SIZE && strBuffer[position] != 0; position++) {
		if (strBuffer[position] == ' ') {
			strBuffer[position++] = 0;
			while (position < BUFFER_SIZE && strBuffer[position] == ' ') {
				position++;
			}
			if (position < BUFFER_SIZE && strBuffer[position] != 0)
				argv[argc++] = strBuffer + position;
		}
	}
	argv[argc] = NULL;

	for (uint64_t cont = 0; cont < AVAILABLE_COMMANDS; cont++) {
		// Search for command and execute
		if (strcmp(argv[0], (avCommands[cont]).name) == 0) {
			if (avCommands[cont].function != NULL) {
				int retValue;
				if (avCommands[cont].builtin) {
					retValue = avCommands[cont].function(argc, (char **) argv);
				}
				else {
					PID_t childPID = execv((void (*)(int, char **)) avCommands[cont].function, argv, FOREGROUND);
					ReturnStatus wstatus;
					if (childPID) {
						PID_t exited = waitpid(childPID, &wstatus);
						SyscallSetFormat(&shell_fmt);
						printf("\nI'm back, PID %d has taken an L and returned %d\n", exited, wstatus.returnValue);
						if (wstatus.aborted)
							fprintf(STD_ERR, "%s was killed\n", strBuffer);
						retValue = wstatus.returnValue;
					}
					else {
						fprintf(STD_ERR, "Couldn't execute %s\n", strBuffer);
						retValue = 0;
					}
				}
				if (retValue)
					printf("Command %s exited with code %d\n", avCommands[cont].name, retValue);
				puts(">> ");
				return 0;
			}
			else {
				uint32_t lastColor = text_color_get();
				text_color_set(COLORS_ORANGE);
				fprintf(STD_OUT, "Cannot execute \'%s\', not implemented or invalid pointer\n", run);
				text_color_set(lastColor);
				puts(">> ");
				return -1;
			}
		}
	}
	fprintf(STD_ERR, "Command \'%s\' not found\n", run);
	puts(">> ");
	return -1;
}