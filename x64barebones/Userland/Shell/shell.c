/* shellCodeModule.c */
#include <stdint.h>
#include <libc.h>
#include <commands.h>

#define COLORS_DEFAULT  0x0020FF00
#define BUFFER_MAX_SIZE 256

static uint8_t shellBuffer[BUFFER_MAX_SIZE];
static uint8_t index = 0;

static void empty_command_buffer();

int shell() {

    uint8_t buf[BUFFER_MAX_SIZE] = {0};    
    commands((uint8_t *)"help");
    empty_command_buffer();

    while(1) {

		uint64_t read = SyscallRead(STD_IN, buf, 254);
        
		if (buf[0] != 0) {

            for (int i = 0; i < read - 1; i++) {

                if (buf[i] == '\n'){
                    putchar(buf[i]);
                    commands((uint8_t *)shellBuffer);
                    empty_command_buffer();
                } else if (buf[i] == '\b') {
                    if (index != 0){
                        shellBuffer[--index] = 0;
                        putchar(buf[i]);
                    }
                } else if (buf[i] == '\e') {

                    putchar(buf[i]);
                    empty_command_buffer();
                } else if(index < BUFFER_MAX_SIZE-1) {

                    shellBuffer[index++] = buf[i];
                    putchar(buf[i]);
                } else {
                    
                    fprintf(STD_ERR, "\nCommand is too long\n");
                    empty_command_buffer();
                }
            }
		}
    }

    return 0;
}

void empty_command_buffer() {

    while (index) {

        index--;
        shellBuffer[index] = '\0';
    }

    SyscallWrite(STD_OUT, (uint8_t *)">> ", 3);
}
