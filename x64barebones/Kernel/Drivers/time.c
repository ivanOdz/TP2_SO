// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard.h>
#include <scheduler.h>
#include <sound.h>
#include <time.h>
#include <videoDriver.h>

static uint64_t ticks = 0;

uint64_t timerHandler() {
	ticks++;
	soundNext();

	if ((ticks % (HZ / 2)) == 0) {
		printCursor();
	}

	if (stackSwitcharoo) {
		return schedyield();
	}
	return 0;
}

uint64_t getTicks() {
	return ticks;
}
