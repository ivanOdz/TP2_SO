// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <font.h>
#include <lib.h>
#include <pipesManager.h>
#include <videoDriver.h>

struct vbe_mode_info_structure {
	uint16_t attributes;  // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;	  // deprecated
	uint8_t window_b;	  // deprecated
	uint16_t granularity; // deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;		   // number of bytes per horizontal line
	uint16_t width;		   // width in pixels
	uint16_t height;	   // height in pixels
	uint8_t w_char;		   // unused...
	uint8_t y_char;		   // ...
	uint8_t planes;
	uint8_t bpp;		   // bits per pixel in this mode
	uint8_t banks;		   // deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;	   // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint8_t *framebuffer; 		  // physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__((packed));

// Tamaño de la pantalla y variables relacionadas
#define SCREEN_WIDTH		VBE_mode_info->width
#define SCREEN_HEIGHT		VBE_mode_info->height
#define DEFAULT_CHAR_WIDTH	8
#define DEFAULT_CHAR_HEIGHT 16
#define X_OFFSET			10
#define Y_OFFSET			20
#define STDERR_FG			0x00FF0000
#define STDERR_BG			0x00000000

typedef struct vbe_mode_info_structure *VBEInfoPtr;

static VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;
static uint8_t *framebuffer;
static int cursor_x, cursor_y;
static uint32_t fontColor = (uint32_t) 0x0020FF00;		 // Default color verde
static uint32_t backgroundColor = (uint32_t) 0x00000000; // Default color negro

static void putBuffer(uint8_t *buffer, uint32_t color);

static uint8_t charWidth = 8;
static uint8_t charHeight = 16;
static uint8_t charSpacing = 8;
static uint8_t fontSize = 1;
static uint8_t cursorActive = 1;

static FifoBuffer *terminalFifo;
static FifoBuffer *errorFifo;
static FifoBuffer *devnull;

void scrollUp() {
	// Copia todo el framebuffer una línea hacia arriba
	uint64_t lineSize = SCREEN_WIDTH * charHeight * (VBE_mode_info->bpp / 8);
	uint64_t frameSize = SCREEN_WIDTH * (SCREEN_HEIGHT - charHeight) * (VBE_mode_info->bpp / 8);
	memmove(framebuffer, framebuffer + lineSize, frameSize);
	// Borra la última línea
	memset(framebuffer + frameSize, backgroundColor, lineSize);
}

void *memmove(void *dest, const void *src, uint64_t n) {
	uint8_t *pd = dest;
	const uint8_t *ps = src;

	if (pd == ps || n == 0) {
		return dest;
	}

	if (pd < ps || pd >= (ps + n)) {
		while (n--) {
			*pd++ = *ps++;
		}
	}
	else {
		pd += n;
		ps += n;
		while (n--) {
			*--pd = *--ps;
		}
	}

	return dest;
}

void print(char *word) {
	drawWord(STD_OUT, word);
}

void initializeVideoDriver() {
	terminalFifo = createFifo(CONSOLE_NAME);
	terminalFifo->readEnds++;
	errorFifo = createFifo(ERROR_NAME);
	terminalFifo->writeEnds++;
	devnull = createFifo(DEV_NULL);
	cursorActive = 1;
	framebuffer = VBE_mode_info->framebuffer;
	cursor_x = X_OFFSET;
	cursor_y = Y_OFFSET;
	setBackground(backgroundColor);
}

void putPixel(uint64_t x, uint64_t y, uint32_t color) {
	uint64_t offset = (x * ((VBE_mode_info->bpp) / 8)) + (y * VBE_mode_info->pitch);
	putBuffer((uint8_t *) framebuffer + offset, color);
}

void putBuffer(uint8_t *buffer, uint32_t color) {
	if (color & 0xFF000000) {
		return;
	}
	buffer[0] = (color) &0xFF;
	buffer[1] = (color >> 8) & 0xFF;
	buffer[2] = (color >> 16) & 0xFF;
}

void drawchar_transparent(char c, uint64_t x, uint64_t y, uint32_t fgcolor) {
	uint64_t cx, cy;
	static const uint64_t mask[8] = {128, 64, 32, 16, 8, 4, 2, 1};	// Cambiamos el orden de los bits en el array mask
	const uint8_t *glyph = font + (uint8_t) c * 16;
	uint8_t *buffer = framebuffer + y * VBE_mode_info->pitch + x * (VBE_mode_info->bpp >> 3);

	for (cy = 0; cy < charHeight; cy++) {
		for (cx = 0; cx < charWidth; cx++) {
			if (glyph[cy / fontSize] & mask[cx / fontSize]) {		// Verificamos si el bit correspondiente está encendido
				putBuffer(buffer + cx * (VBE_mode_info->bpp >> 3), fgcolor);
			}
		}
		buffer += VBE_mode_info->pitch;
	}
}

void setFontSize(uint8_t size) {
	charWidth = 8 * size;
	charHeight = 16 * size;
	charSpacing = 8 * size;
	fontSize = size;
}

void backSpace() {
	drawchar_transparent(219, cursor_x, cursor_y, backgroundColor);

	if (cursor_x < charSpacing + X_OFFSET) {
		cursor_x = VBE_mode_info->width - ((VBE_mode_info->width - X_OFFSET) % charSpacing) - charSpacing;
		if (cursor_y < charHeight + Y_OFFSET && cursor_x > 0) {
			cursor_y = Y_OFFSET;
			cursor_x = X_OFFSET;
		}
		else {
			cursor_y -= charHeight;
		}
	}
	else {
		cursor_x -= charSpacing;
	}
	drawchar_transparent(219, cursor_x, cursor_y, backgroundColor);
}

void printChar(char c, uint32_t fg) {
	if (c == '\t') {
		setTab();
	}
	else if (c == '\n') {
		drawchar_transparent(219, cursor_x, cursor_y, backgroundColor);
		nextLine();
	}
	else if (c == '\b') {
		backSpace();
	}
	else if (c == '\e') {
		clear();
	}
	else if (c == '\r') {
		drawchar_transparent(219, cursor_x, cursor_y, backgroundColor);
		cursor_x = X_OFFSET;
	}
	else {
		// Dibuja el caracter en la posición actual
		drawchar_transparent(219, cursor_x, cursor_y, backgroundColor);
		drawchar_transparent(c, cursor_x, cursor_y, fg);
		cursor_x += charSpacing;
		if (cursor_x + charWidth >= VBE_mode_info->width) {
			nextLine();
		}
	}
}

void drawWord(uint8_t fd, char *word) {
	uint64_t i = 0;
	uint32_t foreground = (fd == STD_ERR) ? STDERR_FG : fontColor;

	while (word[i] != 0) {
		printChar(word[i++], foreground);
	}
}

// Manejar el carácter de salto de línea (\n)
void nextLine() {
	if (cursor_y + 2 * charHeight - 10 >= VBE_mode_info->height - Y_OFFSET) {
		scrollUp();				// Realiza un desplazamiento hacia arriba
	}
	else {
		cursor_y += charHeight; // Mover a la siguiente línea
	}
	cursor_x = X_OFFSET;
}

// Manejar el carácter de tabulación (\t)
// Asumiendo que un tabulador es equivalente a 4 espacios
void setTab() {
	for (int i = 0; i < 4; i++) {
		printChar(' ', fontColor);
	}
}

void clear() {
	setBackground();
	cursor_x = X_OFFSET;
	cursor_y = Y_OFFSET;
}

// Funcion para colorear el fondo de un color liso a eleccion pasado como Hexadecimal
void setBackground() {
	uint8_t *framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
	uint64_t pitch = VBE_mode_info->pitch;

	for (uint64_t j = 0; j < VBE_mode_info->height; j++) {
		for (uint64_t i = 0; i < VBE_mode_info->width; i++) {
			framebuffer[i * 3 + j * pitch] = (backgroundColor) &0xFF;
			framebuffer[i * 3 + j * pitch + 1] = (backgroundColor >> 8) & 0xFF;
			framebuffer[i * 3 + j * pitch + 2] = (backgroundColor >> 16) & 0xFF;
		}
	}
}

void printCursor() {
	static uint8_t setCursor = 0;

	if (setCursor && cursorActive) {
		for (uint16_t cont = 0; cont < charWidth; cont++) {
			putPixel(cursor_x + cont, cursor_y + 12 * fontSize, fontColor);
			putPixel(cursor_x + cont, cursor_y + 12 * fontSize + 1, fontColor);
		}
		setCursor = 0;
	}
	else {
		for (uint16_t cont = 0; cont < charWidth; cont++) {
			putPixel(cursor_x + cont, cursor_y + 12 * fontSize, backgroundColor);
			putPixel(cursor_x + cont, cursor_y + 12 * fontSize + 1, backgroundColor);
		}
		setCursor = 1;
	}
}

uint64_t syscall_puts(uint8_t fd, char *buf, uint64_t size) {
	uint32_t foreground = (fd == STD_ERR) ? STDERR_FG : fontColor;
	uint64_t i;
	for (i = 0; i < size; i++) {
		printChar(buf[i], foreground);
	}

	return i;
}

void updateScreen() {
	char c;
	while ((c = getFifo(errorFifo, FALSE)) != EOF && c) {
		printChar(c, STDERR_FG);
	}
	while ((c = getFifo(terminalFifo, FALSE)) != EOF && c) {
		printChar(c, fontColor);
	}
	devnull->readCursor = devnull->writeCursor;
}

uint64_t setFormat(const text_format *fmt) {
	backgroundColor = fmt->bg;
	fontColor = fmt->fg;
	setFontSize(fmt->size);
	cursorActive = fmt->enableCursorBlink;

	return sizeof(text_format);
}

uint64_t getFormat(text_format *fmt) {
	fmt->bg = backgroundColor;
	fmt->fg = fontColor;
	fmt->size = fontSize;
	fmt->enableCursorBlink = cursorActive;

	return sizeof(text_format);
}

void putBlock(draw_type *draw) {
	if (draw->border_width) {
		for (int i = 0; i < draw->border_width; i++) {
			for (int j = 0; j < draw->width; j++) {
				putPixel(draw->x + j, draw->y - i, draw->border_color);
				putPixel(draw->x + j, draw->y + draw->height - i, draw->border_color);
			}
			for (int j = 0; j < draw->height; j++) {
				putPixel(draw->x - i, draw->y + j, draw->border_color);
				putPixel(draw->x + draw->width - i, draw->y + j, draw->border_color);
			}
		}
	}

	uint8_t *buffer = framebuffer + draw->y * VBE_mode_info->pitch + draw->x * (VBE_mode_info->bpp >> 3);

	for (uint64_t i = 0; i < draw->width; i++) {
		for (uint64_t j = 0; j < draw->height; j++) {
			if (draw->texture) {
				putBuffer(buffer + j * (VBE_mode_info->bpp >> 3), draw->texture[j + i * draw->width]);
			}
		}
		buffer += VBE_mode_info->pitch;
	}
}