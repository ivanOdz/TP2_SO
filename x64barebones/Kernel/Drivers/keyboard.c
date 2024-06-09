// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard.h>
#include <memoryManager.h>
#include <pipesManager.h>
#include <processes.h>
#include <scheduler.h>
#include <stdint.h>
#include <videoDriver.h>

static uint8_t flags = 0; // bit 0 shift left, bit 1 shift right, bit 2 caps lock, bit 3 left ctrl, bit 4 right ctrl
static FifoBuffer fifo = {0};

void initializeKeyboardDriver() {
	strcpy(fifo.name, "stdin");
	fifo.readCursor = fifo.buffer;
	fifo.writeCursor = fifo.buffer;
	fifo.readEnds = 0;
	fifo.writeEnds = 0;
}

uint8_t isAlpha(uint8_t c) {
	if (c >= 0x41 && c <= 0x5A)
		return TRUE;
	if (c >= 0x61 && c <= 0x7A)
		return TRUE;

	return FALSE;
}

uint64_t consume_keys(char *buf, uint64_t size) {
	uint64_t i = 0;
	while ((i < size) && (fifo.readCursor != fifo.writeCursor)) {
		buf[i++] = *(fifo.readCursor++);
		if (fifo.readCursor >= fifo.buffer + PIPES_BUFFER_SIZE) {
			fifo.readCursor = fifo.buffer;
		}
	}
	return i;
}

/*
typedef struct BlockedProcessesNode{
	PID_t blockedPid;
	struct BlockedProcessesNode * next;
} BlockedProcessesNode;*/

int64_t consume_keys2(char *dest, FifoBuffer *src, uint64_t size) {
	uint64_t i = 0;
	while (i < size && *(src->readCursor) != EOF) {
		if (((src->writeCursor - src->buffer + 1) % PIPES_BUFFER_SIZE) == (src->readCursor - src->buffer)) {
			BlockedProcessesNode *aux;
			PCB *process;
			while (src->blockedProcessesOnWrite) {
				aux = src->blockedProcessesOnWrite->next;
				if ((process = getProcess(src->blockedProcessesOnWrite->blockedPid))) {
					process->blockedOn.fd = FALSE;
				}
				freeMemory(src->blockedProcessesOnWrite);
				src->blockedProcessesOnWrite = aux;
			}
		}
		if (src->readCursor == src->writeCursor) {
			// CHANGE STATE A BLOCKED
			PCB *process = getCurrentProcess();
			process->blockedOn.fd = TRUE;
			// AÑADIR EL PROCESO BLOCKEADO AL PIPE CON ALLOC MEMORY
			BlockedProcessesNode *blockProcess = allocMemory(sizeof(BlockedProcessesNode));
			if (blockProcess == NULL) {
				return -1;
			}
			BlockedProcessesNode *current = src->blockedProcessesOnRead;
			if (current == NULL) {
				current = blockProcess;
			}
			while (current->next != NULL) {
				current = current->next;
			}
			current->next = blockProcess;

			// YIELD
			process->stackPointer = forceyield();
		}
		dest[i++] = *(src->readCursor++);
		if (src->readCursor >= src->buffer + PIPES_BUFFER_SIZE) { // BUFFER CIRCULAR
			src->readCursor = src->buffer;
		}
	}
	return i;
}

void keyboard_handler() {
	uint8_t c = getKey();

	if (c == 0xE0) {  // 0xE0 is modifier for multimedia keys & numpad
		c = getKey(); // the following keycode contains the actual key pressed
		switch (c) {
			case 0x48:						  // up
				*(fifo.writeCursor++) = 0x1E; // code for up arrow in font
				break;
			case 0x50:						  // down
				*(fifo.writeCursor++) = 0x1F; // code for down arrow in font
				break;
			case 0x4B:						  // left
				*(fifo.writeCursor++) = 0x11; // code for left arrow in font
				break;
			case 0x4D:						  // right
				*(fifo.writeCursor++) = 0x10; // code for right arrow in font
				break;
			case 0x1D: // right ctrl pressed
				flags |= 0x10;
				break;
			case 0x9D: // right ctrl released
				flags &= 0xFF - 0x10;
				break;
		}
		if (fifo.writeCursor >= fifo.buffer + PIPES_BUFFER_SIZE) {
			fifo.writeCursor = fifo.buffer;
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
		switch (c) {
		}
		if (c == 0x2E) { // C (Kill)
			killRunningForegroundProcess();
			return;
		}
		if (c == 0x20) // D (EOF)
			*(fifo.writeCursor++) = EOF;
		if (c == 0x13) { // R (Panic! at the Kernel)
			setFontSize(4);
			drawWord(STD_ERR, "\ePanic! at the Kernel");
			setFontSize(1);
			for (int i = 0; i < 100000000; i++)
				;
			main();
		}
	}

	// regular characters
	if (c <= 0x3B) {
		uint8_t ascii = toAscii[c];

		if (ascii != 0) {
			if (flags & SHIFT) {
				if (isAlpha(ascii) && ((flags & CAPS) > 0)) { // also caps lock and is letter -> lower case
					*(fifo.writeCursor++) = toAscii[c];
				}
				else { // no caps lock / caps lock but not letter -> regular shift
					*(fifo.writeCursor++) = mods[c];
				}
			}
			else if (isAlpha(ascii) && ((flags & 0x04) > 0)) { // no shift but caps lock and is letter -> upper case
				*(fifo.writeCursor++) = mods[c];
			}
			else { // w/o modifier
				*(fifo.writeCursor++) = toAscii[c];
			}
		}
		if (fifo.writeCursor >= fifo.buffer + PIPES_BUFFER_SIZE) {
			fifo.writeCursor = fifo.buffer;
		}
	}
}
