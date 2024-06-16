// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard.h>

static uint8_t flags = 0; // bit 0 desplazar izquierda, bit 1 desplazar derecha, bit 2 bloquear mayúscula, bit 3 ctrl izquierdo, bit 4 ctrl derecho

static FifoBuffer *keyboardFifo;

void initializeKeyboardDriver() {
	keyboardFifo = createFifo(KEYBOARD_NAME);
	keyboardFifo->writeEnds++;
}

bool isAlpha(uint8_t c) {
	if (c >= 0x41 && c <= 0x5A) {
		return TRUE;
	}
	if (c >= 0x61 && c <= 0x7A) {
		return TRUE;
	}
	return FALSE;
}

void keyboardHandler() {
	uint8_t c = getKey();
	if (!keyboardFifo) {
		return;
	}
	if (c == 0xE0) {  // 0xE0 es el modificador para las teclas y teclado numérico de multimedia
		c = getKey(); // Los siguientes códigos de tecla contienenla tecla presionada
		switch (c) {
			case 0x48:								// Subir
				putFifo(keyboardFifo, 0x1E, FALSE); // Código de flecha de arriba
				break;
			case 0x50:								// Bajar
				putFifo(keyboardFifo, 0x1F, FALSE); // Código de flecha de abajo
				break;
			case 0x4B:								// Izquierda
				putFifo(keyboardFifo, 0x11, FALSE); // Código de flecha izquierda
				break;
			case 0x4D:								// Derecha
				putFifo(keyboardFifo, 0x10, FALSE); // Código de flecha derecha
				break;
			case 0x1D: // ctrl derecho apretado
				flags |= 0x10;
				break;
			case 0x9D: // ctrl derecho soltado
				flags &= 0xFF - 0x10;
				break;
		}
		return;
	}
	switch (c) {
		case 0x2A: // shift izquierdo presionado
			flags |= 0x01;
			break;
		case 0x36: // shift derecho presionado
			flags |= 0x02;
			break;
		case 0xAA: // shift izquierdo soltado
			flags &= 0xFF - 0x01;
			break;
		case 0xB6: // shift derecho soltado
			flags &= 0xFF - 0x02;
			break;
		case 0x3A: // caps lock invertir
			flags ^= 0x04;
			break;
		case 0x1D: // ctrl izquierdo presionado
			flags |= 0x08;
			break;
		case 0x9D: // ctrl izquierdo presionado
			flags &= 0xFF - 0x08;
			break;
	}
	// Señales específicas
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
	// Caracteres regulares
	if (c <= 0x3B) {
		char ascii = toAscii[c];

		if (ascii != 0) {
			if (flags & SHIFT) {
				if (isAlpha(ascii) && ((flags & CAPS) > 0)) { // Minúscula
					putFifo(keyboardFifo, toAscii[c], FALSE);
				}
				else { // shift regular
					putFifo(keyboardFifo, mods[c], FALSE);
				}
			}
			else if (isAlpha(ascii) && ((flags & 0x04) > 0)) { // Mayúscula
				putFifo(keyboardFifo, mods[c], FALSE);
			}
			else { // Modificador w/o
				putFifo(keyboardFifo, toAscii[c], FALSE);
			}
		}
	}
}
