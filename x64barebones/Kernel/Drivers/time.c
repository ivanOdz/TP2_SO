// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard.h>
#include <scheduler.h>
#include <sound.h>
#include <time.h>
#include <videoDriver.h>

static uint64_t ticks = 0;

void timer_handler() {
	ticks++;
	if ((ticks % (HZ / 40) == 0)) {
		soundNext();
	}

	if ((ticks % (HZ / 2)) == 0) {
		printCursor();
	}

	if (stackSwitcharoo)
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
