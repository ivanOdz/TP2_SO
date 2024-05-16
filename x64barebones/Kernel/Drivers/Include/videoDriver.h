#ifndef _VIDEODRIVER_H_
#define _VIDEODRIVER_H_

#include <stdint.h>

typedef struct {

    uint32_t bg;
    uint32_t fg;
    uint8_t size;
	uint8_t enableCursorBlink;

} text_format;

typedef struct {
    uint16_t x, y;
    uint16_t width, height;
    uint8_t border_width;
    uint32_t border_color;
    uint32_t* texture;
} draw_type;


void initializeVideoDriver();
void putPixel(uint64_t x, uint64_t y, uint32_t color);
void setBackground();
void drawWord(uint8_t fd, uint8_t * word);
void nextLine();
void setTab();
void drawchar_transparent(uint8_t c, uint64_t x, uint64_t y, uint32_t fgcolor);
void print(uint8_t * word);             // imprime un string en formato C.
void printChar(uint8_t c, uint32_t fg);
void setFontSize(uint8_t fontSize);
void scrollUp();
void printCursor();
void clear();
void putchar(uint8_t c);
void initializeSnakeGame(uint16_t pixelsX, uint16_t pixelsY);
void putBlock(draw_type* draw);

uint64_t syscall_puts(uint8_t fd, uint8_t * buf, uint64_t size);
uint64_t setFormat(const text_format * fmt);
uint64_t getFormat(text_format * fmt);
void *memmove(void *dest, const void *src, uint64_t n);
void *memset(void *s, int c, uint64_t n);

#endif