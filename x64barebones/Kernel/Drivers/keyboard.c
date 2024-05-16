#include <keyboard.h>
#include <stdint.h>
#include <videoDriver.h>

#define EOF			0xFF
#define BUFFER_SIZE 0x100

static uint8_t buffer[BUFFER_SIZE];
static uint8_t readCursor = 0;
static uint8_t writeCursor = 0;
static uint8_t flags = 0; // bit 0 shift left, bit 1 shift right, bit 2 caps lock

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
	// Accedo al buffer mediante esto los cursores son bytes asi que el overflow lo circulariza solo
	uint8_t c = getKey();

	if (c == 0xE0) {  // 0xE0 es modifier para keys multimedia (no existe) y numpad (nadie tiene, lo cual es muy triste pero cierto)
		c = getKey(); // entonces levantamos la segunda parte del keyCode y salimos como si nada hubiera pasado

		return;
	}
	switch (c) {
		case 0x2A: // shift izq presionado
			flags |= 0x01;
			break;
		case 0x36: // shift der presionado
			flags |= 0x02;
			break;
		case 0xAA: // shift izq soltado
			flags &= 0xFE;
			break;
		case 0xB6: // shift der soltado
			flags &= 0xFD;
			break;
		case 0x3A: // caps lock presionado
			flags ^= 0x04;
			break;
	}
	if (c <= 0x3B) {
		uint8_t ascii = toAscii[c];

		if (ascii != 0) { // es imprimible?

			if (flags & 0x03) { // shift on

				if (isAlpha(ascii) && ((flags & 0x04) > 0)) { // caps lock tambien on y es letra -> en realidad quiero minuscula
					buffer[writeCursor++] = toAscii[c];
				}
				else { // sin caps lock / con caps lock pero no es letra -> quiero shift normal
					buffer[writeCursor++] = mods[c];
				}
			}
			else if (isAlpha(ascii) && ((flags & 0x04) > 0)) { // sin shift pero caps lock y es letra -> quiero mayus
				buffer[writeCursor++] = mods[c];
			}
			else { // sin ningun modificador
				buffer[writeCursor++] = toAscii[c];
			}
		}
		writeCursor &= 0xFF;
	}
}
