#include "videoDriver.h"
#include <keyboard.h>
#include <scheduler.h>
#include <sound.h>
#include <time.h>

static unsigned long ticks = 0;

void timer_handler() {
	if ((ticks % (HZ / 35) == 0)) {
		soundNext();
	}

	ticks++;

	if ((ticks % (HZ / 2)) == 0) {
		printCursor();
	}
	schedyield();
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / HZ;
}

uint64_t get_ticks() {
	return ticks;
}
