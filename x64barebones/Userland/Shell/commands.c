// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <commands.h>
#include <libc.h>
#include <rick.h>
#include <roll.h>
#include <sc.h>
#include <snake.h>

int command_help(int argc, char **argv) {
	uint32_t lastColor = text_color_get();
	// Print text
	puts(STR_PREAMBLE_FOR_COMANDS);
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
	int8_t newSize = stringToInt(argv[1], strlen(argv[1]));
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
		yield();
	}
	return 0;
}

int command_getRegisters(int argc, char **argv) {
	SyscallGetRegisters();
	return 0;
}

int command_kill(int argc, char **argv) {
	if (argc != 2 || !argv[1]) {
		fprintf(STD_ERR, "Please provide a PID to obliterate.\n");
		return 1;
	}
	PID_t PID = stringToInt(argv[1], strlen(argv[1]));
	if (PID && kill(PID)) {
		return 0;
	}
	fprintf(STD_ERR, "Couldn't kill process %d\n", PID);
	return 1;
}