/* shellCodeModule.c */
#include <commands.h>
#include <libc.h>
#include <stdint.h>

#define COLORS_DEFAULT	0x0020FF00
#define BUFFER_MAX_SIZE 256

static uint8_t shellBuffer[BUFFER_MAX_SIZE] = {0};
static uint8_t lastBuffer[BUFFER_MAX_SIZE] = {0};

static uint8_t index = 0;

static void empty_command_buffer(uint8_t deleteChars);

int shell() {
	uint8_t buf[BUFFER_MAX_SIZE] = {0};
	commands((uint8_t *) "help");
	empty_command_buffer(FALSE);
	SyscallWrite(STD_OUT, (uint8_t *) ">> ", 3);

	while (1) {
		uint64_t read = SyscallRead(STD_IN, buf, 254);

		if (buf[0] != 0) {
			for (int position = 0; position < read - 1; position++) {
				if (buf[position] == '\n') {
					if (!index) {
						printf("%s\n", lastBuffer);
						commands(lastBuffer);
						SyscallWrite(STD_OUT, (uint8_t *) ">> ", 3);
					}
					else {
						putchar(buf[position]);
						commands((uint8_t *) shellBuffer);
						empty_command_buffer(FALSE);
						SyscallWrite(STD_OUT, (uint8_t *) ">> ", 3);
					}
				}
				else if (buf[position] == '\b') {
					if (index != 0) {
						shellBuffer[--index] = 0;
						putchar(buf[position]);
					}
				}
				else if (buf[position] == '\e') {
					putchar(buf[position]);
					empty_command_buffer(FALSE);
				}
				else if (buf[position] == '\t') {
					for (int command = 0; command < AVAILABLE_COMMANDS; command++) {
						if (strincludes(avCommands[command].name, shellBuffer)) {
							empty_command_buffer(TRUE);

							strcpy(shellBuffer, avCommands[command].name);
							index = strlen(shellBuffer);
							SyscallWrite(STD_OUT, shellBuffer, index);
						}
					}
				}
				else if (index < BUFFER_MAX_SIZE - 2) {
					shellBuffer[index++] = buf[position];
					putchar(buf[position]);
				}
				else {
					fprintf(STD_ERR, "\nCommand is too long\n");
					empty_command_buffer(FALSE);
					SyscallWrite(STD_OUT, (uint8_t *) ">> ", 3);
				}
			}
		}
	}

	return 0;
}

void empty_command_buffer(uint8_t deleteChars) {
	for (int i = 0; i < BUFFER_MAX_SIZE; i++) {
		lastBuffer[i] = shellBuffer[i];
		shellBuffer[i] = '\0';
	}
	while (index--) {
		if (deleteChars)
			putchar('\b');
	}
	index = 0;
}
