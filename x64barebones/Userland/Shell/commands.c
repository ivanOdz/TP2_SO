#include <commands.h>
#include <libc.h>
#include <rick.h>
#include <roll.h>
#include <sc.h>
#include <snake.h>

static text_format shell_fmt = {0x00000000, 0x0000FF00, 1, 1};

int64_t commands(uint8_t *strBuffer) {
	// Chequear el comando hasta el primer espacio, y ver si es un comando valido, mediante una Syscall al Kernel
	uint8_t command[BUFFER_DEFAULT_SIZE] = {0};
	int i, argc;
	uint8_t *argv[BUFFER_DEFAULT_SIZE];

	// trim espacios
	i = 0;
	while (strBuffer[i] == ' ') {
		i++;
	}

	// Obtengo el comando
	for (int pos = 0; i < BUFFER_DEFAULT_SIZE && strBuffer[i] != ' ' && strBuffer[i] != 0; i++, pos++) {
		command[pos] = strBuffer[i];
	}
	// Obtengo los argumentos
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
				}
				else {
					PID_t childPID = execv(avCommands[cont].function, argv, FOREGROUND);
					ReturnStatus *wstatus = malloc(sizeof(ReturnStatus));
					if (childPID) {
						PID_t exited = waitpid(childPID, wstatus);
						printf("I'm back, PID %d has taken an L\n", exited);
						if (wstatus->aborted)
							fprintf(STD_ERR, "%s was killed\n", strBuffer);
						retValue = wstatus->returnValue;
					}
					else {
						fprintf(STD_ERR, "Couldn't execute %s\n", strBuffer);
					}
					retValue = 0;
				}
				SyscallSetFormat(&shell_fmt);
				SyscallSetTimer(SHELL_TIMER);
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

int command_get_rtc(int argc, char **argv) {
	time_t time;
	int8_t offset = 0;
	if (argc > 1)
		offset = stringToInt(argv[1], strlen((uint8_t *) argv[1]));
	if (offset > 12 || offset < -12) {
		fprintf(STD_ERR, "invalid offset\n");
		offset = 0;
	}
	if (SyscallGetRTC(&time)) {
		time.hora -= 6 * time.hora >> 4;
		if (offset != 0) {
			time.hora += 24;
			time.hora += offset;
			time.hora %= 24;
		}
		printf("Son %s %2d:%2x:%2x (offset%s \b%d)\n", (time.hora != 1) ? "las" : "la", time.hora, time.min, time.seg,
			   (offset >= 0) ? "+" : "-", (uint8_t) ((offset >= 0) ? offset : -offset));
	}
	return 0;
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

int command_testExceptions(int argc, char **argv) {
	int8_t ex = stringToInt(argv[1], strlen((uint8_t *) argv[1]));
	switch (ex) {
		case 1:
			return argc / argv[1][strlen((uint8_t *) argv[1])];
			break;
		case 2:
			invalidOpcode();
			break;
		default:
			fprintf(STD_ERR, "Invalid error code\n");
	}
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

int command_snakeGame(int argc, char **argv) {
	snake(argc, argv);
	return 0;
}

int ps(int argc, char **argv) {
	ProcessInfo *info = SyscallProcessInfo();
	printf("NAME\t\t\t\t PID   PARENT  MODE\t\tSTACK BASE\tSTACK POINTER\t STATUS\tPRIORITY\n");
	printf("==================================================================================================\n");
	while (info != NULL) {
		printf("%-20s %5d %5d   %10s  0x%8x\t0x%8x\t\t%-9s %d\n", info->name, info->pid, info->parent_PID, (info->runMode == 'F') ? "Foreground" : "Background", info->stackBasePointer, info->stackPointer, info->processStatus, info->priority);
		ProcessInfo *temp = info;
		info = info->nextProcessInfo;
		free(temp);
	}
	exit(0);
}

int command_nice(int argc, char **argv) {
	if (argc != 3) {
		fprintf(STD_ERR, "Wrong amount of arguments...\n");
		return 1;
	}
	int16_t pid = stringToInt(argv[1], strlen((uint8_t *) argv[1]));
	int8_t newPriority = stringToInt(argv[2], strlen((uint8_t *) argv[2]));
	if (newPriority < 0 || newPriority > 31) {
		fprintf(STD_ERR, "Priority must be between 0 and 31...\n");
		return 1;
	}
	SyscallNice(pid, newPriority);
	return 0;
}

int command_block(int argc, char **argv) {
	if (argc != 2) {
		fprintf(STD_ERR, "Wrong amount of arguments...\n");
		return 1;
	}
	int16_t pid = stringToInt(argv[1], strlen((uint8_t *) argv[1]));
	SyscallToggleBlockProcess(pid);
	return 0;
}