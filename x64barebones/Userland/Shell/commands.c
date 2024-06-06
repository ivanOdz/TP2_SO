#include <commands.h>
#include <libc.h>
#include <rick.h>
#include <roll.h>
#include <sc.h>
#include <snake.h>

static text_format shell_fmt = {0x00000000, 0x0000FF00, 1, 1};

int64_t commands(uint8_t *strBuffer) {
	uint8_t command[BUFFER_DEFAULT_SIZE] = {0};
	int i, argc;
	uint8_t *argv[BUFFER_DEFAULT_SIZE];

	// trim whitespace
	i = 0;
	while (strBuffer[i] == ' ') {
		i++;
	}

	// Get command
	for (int pos = 0; i < BUFFER_DEFAULT_SIZE && strBuffer[i] != ' ' && strBuffer[i] != 0; i++, pos++) {
		command[pos] = strBuffer[i];
	}
	// Get arguments
	for (argc = 1; i < BUFFER_DEFAULT_SIZE && strBuffer[i] != 0; i++) {
		if (strBuffer[i] == ' ') {
			strBuffer[i] = 0;
			i++;
			while (strBuffer[i] == ' ' && i < BUFFER_DEFAULT_SIZE) {
				i++;
			}
			argv[argc] = strBuffer + i;
			argc++;
		}
	}
	argv[0] = command;
	argv[argc] = NULL;

	for (uint64_t cont = 0; cont < AVAILABLE_COMMANDS; cont++) {
		// Search for command and execute
		if (strcmp(command, (avCommands[cont]).name) == 0) {
			if (avCommands[cont].function != NULL) {
				int retValue;
				if (avCommands[cont].builtin) {
					retValue = avCommands[cont].function(argc, (char **) argv);
					SyscallSetFormat(&shell_fmt);
					SyscallSetTimer(SHELL_TIMER);
				}
				else {
					PID_t childPID = execv(avCommands[cont].function, argv, FOREGROUND);
					ReturnStatus *wstatus = malloc(sizeof(ReturnStatus));
					if (childPID) {
						PID_t exited = waitpid(childPID, wstatus);
						SyscallSetFormat(&shell_fmt);
						SyscallSetTimer(SHELL_TIMER);
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
				return 0;
			}
			else {
				uint32_t lastColor = text_color_get();
				text_color_set(COLORS_ORANGE);
				fprintf(STD_OUT, "Cannot execute \'%s\', not implemented or invalid pointer\n", command);
				text_color_set(lastColor);
				return -1;
			}
		}
	}
	fprintf(STD_ERR, "Command \'%s\' not found\n", command);
	return -1;
}

int command_help(int argc, char **argv) {
	uint32_t lastColor = text_color_get();
	// Print text
	puts((uint8_t *) STR_PREAMBLE_FOR_COMANDS);
	// Print available commands in BLUE
	text_color_set(COLORS_CIAN);

	for (uint64_t cont = 0; cont < AVAILABLE_COMMANDS; cont++) {
		printf("%s\t", avCommands[cont].name);
	}
	text_color_set(lastColor);

	putchar('\n');

	return COMMAND_EXCECUTED;
}

int command_setFontSize(int argc, char **argv) {
	int8_t newSize = stringToInt(argv[1], strlen((uint8_t *) argv[1]));
	if (newSize <= 0 || newSize > 10) {
		fprintf(STD_ERR, "Invalid size...\n");
		return 1;
	}
	shell_fmt.size = newSize;
	SyscallSetFormat(&shell_fmt);
	SyscallClear();
	return 0;
}

int command_clear(int argc, char **argv) {
	SyscallClear();
	return 0;
}

int command_tunes(int argc, char **argv) {
	draw_type ricky = {0, 0, RICKWIDTH, RICKHEIGHT, 0, 0, (uint32_t *) rick};
	text_format fmt = {0x00000000, 0x00C0C0C0, 3, 0};
	SyscallSetTimer(25000);
	SyscallSetFormat(&fmt);
	SyscallClear();
	SyscallAudio(1, roll, rolllength);
	for (int i = 0; i + RICKWIDTH < 1024; i += RICKWIDTH) {
		for (int j = 0; j + RICKHEIGHT < 768; j += RICKHEIGHT) {
			ricky.x = i;
			ricky.y = j;
			SyscallPutBlock(&ricky);
		}
	}
	while (1) {
	}
	return 0;
}

int command_getRegisters(int argc, char **argv) {
	SyscallGetRegisters();
	return 0;
}