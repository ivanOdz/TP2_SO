#ifndef LIBC_H
#define LIBC_H

#include <stdint.h>
#include <stdarg.h>

#define NULL    0

#define STD_IN  0
#define STD_OUT 1
#define STD_ERR 2


#define COLORS_BLUE     0x000000FF
#define COLORS_GREEN    0x0000FF00
#define COLORS_RED      0x00FF0000
#define COLORS_BLACK    0x00000000
#define COLORS_WHITE    0x00FFFFFF

#define COLORS_ORANGE   0x00EF3F00
#define COLORS_CIAN     0x0000E0FF

typedef struct {
    uint32_t bg;
    uint32_t fg;
    uint8_t size;
    uint8_t enableCursorBlink;
} text_format;

typedef struct{
	uint8_t hora;
	uint8_t min;
	uint8_t seg;
} time_t;

typedef struct {
    uint16_t x, y;
    uint16_t width, height;
    uint8_t border_width;
    uint32_t border_color;
    uint32_t* texture;
} draw_type;

extern uint64_t SyscallWrite(uint8_t fd, uint8_t * buffer, uint64_t size);
extern uint64_t SyscallRead(uint8_t fd, uint8_t * buffer, uint64_t size);
extern uint64_t SyscallClear();
extern uint64_t SyscallGetRegisters();
extern uint64_t SyscallGetRTC(time_t * buffer);
extern uint64_t SyscallGetFormat(text_format * buffer);
extern uint64_t SyscallSetFormat(text_format * buffer);
extern uint64_t SyscallPutBlock(draw_type* draw);
extern uint64_t SyscallGetTicks();
extern uint64_t SyscallAudio(uint8_t isLoop, const uint8_t * buffer, uint64_t size);
extern uint64_t SyscallSetTimer(uint16_t delay);

int64_t strcmp(const uint8_t* str1, const uint8_t* str2);
uint64_t strlen(const uint8_t* str);
int64_t stringToInt(char* str, uint32_t strlen);
uint32_t rand();
void srand(uint32_t seed);
uint32_t randBetween(uint32_t start, uint32_t end);

void puts(uint8_t* str);
void putchar(uint8_t c);
void fputchar(uint8_t fd, uint8_t c);
void fputs(uint8_t fd, uint8_t* str);
void printf(char* fmt, ...);
void fprintf(uint8_t fd, char* fmt, ...);
void fprintf_args(uint8_t fd, char* fmt, va_list args);
void text_color_set(uint32_t color);
uint32_t text_color_get();
int command_argvTest(int argc, char** argv);
void printPadded(uint8_t fd, uint8_t* buffer, uint8_t pad, uint64_t totalLen);
extern void invalidOpcode();



#endif