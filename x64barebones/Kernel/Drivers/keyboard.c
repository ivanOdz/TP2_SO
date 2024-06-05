#include <keyboard.h>
#include <stdint.h>
#include <videoDriver.h>

#define EOF			0xFF
#define BUFFER_SIZE 0x100

static uint8_t buffer[BUFFER_SIZE];
static uint8_t readCursor = 0;
static uint8_t writeCursor = 0;
static uint8_t flags = 0; // bit 0 shift left, bit 1 shift right, bit 2 caps lock, bit 3 left ctrl, bit 4 right ctrl

uint8_t isAlpha(uint8_t c) {
	if (c >= 0x41 && c <= 0x5A)
		return 1;
	if (c >= 0x61 && c <= 0x7A)
		return 1;

	return 0;
}

uint64_t consume_keys(uint8_t *buf, uint64_t size) {
	uint64_t i = 0;

	while ((i < size - 1) && (readCursor != writeCursor)) {
		buf[i++] = buffer[readCursor++];
		readCursor &= 0xFF;
	}
	buf[i++] = 0;

	return i;
}

void keyboard_handler() {
	uint8_t c = getKey();

	if (c == 0xE0) {  // 0xE0 is modifier for multimedia keys & numpad
		c = getKey(); // the following keycode contains the actual key pressed
		switch (c) {
			case 0x48:						  // up
				buffer[writeCursor++] = 0x1E; // code for up arrow in font
				break;
			case 0x50:						  // down
				buffer[writeCursor++] = 0x1F; // code for down arrow in font
				break;
			case 0x4B:						  // left
				buffer[writeCursor++] = 0x11; // code for left arrow in font
				break;
			case 0x4D:						  // right
				buffer[writeCursor++] = 0x10; // code for right arrow in font
				break;
			case 0x1D: // right ctrl pressed
				flags |= 0x10;
				break;
			case 0x9D: // right ctrl released
				flags &= 0xFF - 0x10;
				break;
		}
		writeCursor &= 0xFF;
		return;
	}
	switch (c) {
		case 0x2A: // shift izq pressed
			flags |= 0x01;
			break;
		case 0x36: // shift der pressed
			flags |= 0x02;
			break;
		case 0xAA: // shift izq released
			flags &= 0xFF - 0x01;
			break;
		case 0xB6: // shift der released
			flags &= 0xFF - 0x02;
			break;
		case 0x3A: // caps lock toggled
			flags ^= 0x04;
			break;
		case 0x1D: // left ctrl pressed
			flags |= 0x08;
			break;
		case 0x9D: // left ctrl pressed
			flags &= 0xFF - 0x08;
			break;
	}

	// specific signals
	if (flags & CONTROL) {
		if (c == 0x2E) // C (Kill)
			killRunningForegroundProcess();
		if (c == 0x20) // D (EOF)
			buffer[writeCursor++] = 0;
	}

	// regular characters
	if (c <= 0x3B) {
		uint8_t ascii = toAscii[c];

		if (ascii != 0) {
			if (flags & SHIFT) {
				if (isAlpha(ascii) && ((flags & CAPS) > 0)) { // also caps lock and is letter -> lower case
					buffer[writeCursor++] = toAscii[c];
				}
				else { // no caps lock / caps lock but not letter -> regular shift
					buffer[writeCursor++] = mods[c];
				}
			}
			else if (isAlpha(ascii) && ((flags & 0x04) > 0)) { // no shift but caps lock and is letter -> upper case
				buffer[writeCursor++] = mods[c];
			}
			else { // w/o modifier
				buffer[writeCursor++] = toAscii[c];
			}
		}
		writeCursor &= 0xFF;
	}
}
