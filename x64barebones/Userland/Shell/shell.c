/* shellCodeModule.c */
#include <commands.h>
#include <libc.h>
#include <stdint.h>

#define COLORS_DEFAULT 0x0020FF00
#define BUFFER_SIZE	   256
#define HISTORY_SIZE   10

typedef struct commandBuffer {
	char buffer[BUFFER_SIZE];
	uint8_t position;
} commandBuffer;

void emptyCommandBuffer(commandBuffer *buffer);
void deleteChars(commandBuffer *command);
int64_t runCommand(uint8_t *strBuffer);
commandBuffer *initBuffers();

int shell() {
	commandBuffer *command = malloc(sizeof(commandBuffer));
	commandBuffer **history = malloc(HISTORY_SIZE * sizeof(commandBuffer *));
	for (int i = 0; i < HISTORY_SIZE; i++)
		history[i] = NULL;
	int historyIndex = 0;
	emptyCommandBuffer(command);
	runCommand((uint8_t *) "help");
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
						putchar('\n');
						command->buffer[command->position] = 0;
						runCommand((uint8_t *) command->buffer);
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
					if (command->position) {
						command->buffer[--(command->position)] = 0;
						putchar('\b');
					}
					break;
				case '\e':
					runCommand("Clear");
					emptyCommandBuffer(command);
					break;
				case '\t':
					for (int listItem = 0; listItem < AVAILABLE_COMMANDS; listItem++) {
						if (strincludes(avCommands[listItem].name, command->buffer)) {
							deleteChars(command);
							emptyCommandBuffer(command);
							strcpy(command->buffer, avCommands[listItem].name);
							command->position = strlen(command->buffer);
							puts(command->buffer);
						}
					}
					break;
				case 0x1E: // up arrow
					if (historyIndex + 1 < HISTORY_SIZE && history[historyIndex + 1]) {
						deleteChars(command);
						command->position = strcpy(command->buffer, history[++historyIndex - 1]->buffer);
						puts(command->buffer);
					}
					break;
				case 0x1F: // down arrow
					if (historyIndex) {
						deleteChars(command);
						command->position = strcpy(command->buffer, history[historyIndex-- - 1]->buffer);
						puts(command->buffer);
					}
					else {
						deleteChars(command);
						emptyCommandBuffer(command);
					}
					break;
				default:
					if (command->position < BUFFER_SIZE - 2) {
						command->buffer[command->position++] = incoming;
						putchar(incoming);
					}
					else {
						fprintf(STD_ERR, "\nCommand is too long\n>> ");
						emptyCommandBuffer(command);
					}
			}
		}
		yield();
	}
	return 0;
}

void emptyCommandBuffer(commandBuffer *command) {
	for (int i = 0; i < BUFFER_SIZE; i++) {
		command->buffer[i] = '\0';
	}
	command->position = 0;
}
void deleteChars(commandBuffer *command) {
	if (command->position)
		for (int i = command->position; i; i--) {
			putchar('\b');
		}
}

int64_t runCommand(uint8_t *run) {
	uint8_t strBuffer[BUFFER_SIZE];
	strcpy(strBuffer, run);
	int i, argc;
	uint8_t *argv[BUFFER_SIZE];

	// trim whitespace & get command
	i = 0;
	while (strBuffer[i] == ' ') {
		i++;
	}
	argv[0] = strBuffer + i;

	// Get arguments
	for (argc = 1; i < BUFFER_SIZE && strBuffer[i] != 0; i++) {
		if (strBuffer[i] == ' ') {
			strBuffer[i] = 0;
			i++;
			while (strBuffer[i] == ' ' && i < BUFFER_SIZE) {
				i++;
			}
			argv[argc] = strBuffer + i;
			argc++;
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
					PID_t childPID = execv(avCommands[cont].function, argv, FOREGROUND);
					ReturnStatus *wstatus = malloc(sizeof(ReturnStatus));
					if (childPID) {
						PID_t exited = waitpid(childPID, wstatus);
						SyscallSetFormat(&shell_fmt);
						printf("\nI'm back, PID %d has taken an L and returned %d\n", exited, wstatus->returnValue);
						if (wstatus->aborted)
							fprintf(STD_ERR, "%s was killed\n", strBuffer);
						retValue = wstatus->returnValue;
					}
					else {
						fprintf(STD_ERR, "Couldn't execute %s\n", strBuffer);
					}
					retValue = 0;
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