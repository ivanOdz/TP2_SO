#include "videoDriver.h"
#include <keyboard.h>
#include <scheduler.h>
#include <sound.h>
#include <time.h>

static unsigned long ticks = 0;

void timer_handler() {
	soundNext();
	ticks++;

	if (ticks % 9 == 0) {
		printCursor();
	}
	schedyield();
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

uint64_t get_ticks() {
	return ticks;
}
