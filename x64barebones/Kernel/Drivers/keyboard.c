// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard.h>

static uint8_t flags = 0;	// bit 0 shift left, bit 1 shift right, bit 2 caps lock, bit 3 left ctrl, bit 4 right ctrl

static FifoBuffer *keyboardFifo;

void initializeKeyboardDriver() {
	keyboardFifo = createFifo(KEYBOARD_NAME);
	keyboardFifo->writeEnds++;
}

uint8_t isAlpha(uint8_t c) {
	if (c >= 0x41 && c <= 0x5A) {
		return TRUE;
	}
	if (c >= 0x61 && c <= 0x7A) {
		return TRUE;
	}
	return FALSE;
}

void keyboard_handler() {
	uint8_t c = getKey();
	if (!keyboardFifo) {
		return;
	}
	if (c == 0xE0) {  // 0xE0 is modifier for multimedia keys & numpad
		c = getKey(); // the following keycode contains the actual key pressed
		switch (c) {
			case 0x48:								// up
				putFifo(keyboardFifo, 0x1E, FALSE); // code for up arrow in font
				break;
			case 0x50:								// down
				putFifo(keyboardFifo, 0x1F, FALSE); // code for down arrow in font
				break;
			case 0x4B:								// left
				putFifo(keyboardFifo, 0x11, FALSE); // code for left arrow in font
				break;
			case 0x4D:								// right
				putFifo(keyboardFifo, 0x10, FALSE); // code for right arrow in font
				break;
			case 0x1D: // right ctrl pressed
				flags |= 0x10;
				break;
			case 0x9D: // right ctrl released
				flags &= 0xFF - 0x10;
				break;
		}
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
		uint8_t retNow = 0;
		switch (c) {
			case 0x2E: // C (Kill)
				killRunningForegroundProcess();
				retNow = 1;
				break;
			case 0x20: // D (EOF)
				putFifo(keyboardFifo, EOF, FALSE);
				retNow = 1;
				break;
		}
		if (retNow) {
			return;
		}
	}
	// regular characters
	if (c <= 0x3B) {
		uint8_t ascii = toAscii[c];

		if (ascii != 0) {
			if (flags & SHIFT) {
				if (isAlpha(ascii) && ((flags & CAPS) > 0)) { // also caps lock and is letter -> lower case
					putFifo(keyboardFifo, toAscii[c], FALSE);
				}
				else { // no caps lock / caps lock but not letter -> regular shift
					putFifo(keyboardFifo, mods[c], FALSE);
				}
			}
			else if (isAlpha(ascii) && ((flags & 0x04) > 0)) { // no shift but caps lock and is letter -> upper case
				putFifo(keyboardFifo, mods[c], FALSE);
			}
			else { // w/o modifier
				putFifo(keyboardFifo, toAscii[c], FALSE);
			}
		}
	}
}
