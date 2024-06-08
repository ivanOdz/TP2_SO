// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "./../Include/naiveConsole.h"
#include <sound.h>
#include <stdint.h>
#include <videoDriver.h>

#define FREQDELAY 1193180 / 8
#define IS_LOOP	  0b000000001
#define IS_SFX	  0b000000010

typedef struct {
	const uint8_t *buffer;
	uint64_t length;
	uint64_t current;
	uint64_t current_delay_left;
	uint8_t isLoop;
} song_type;

static song_type bg, sfx;

// esta pensado para hacer sonar un stream tipo midi pero muy simplificado donde cada par de uint8s indica freq del parlante y tiempo hasta la proxima nota respectivamente
void playSound(uint8_t flags, const uint8_t *snd, uint64_t length) {
	if (flags & IS_SFX) {
		sfx.length = length;
		sfx.isLoop = 0;
		sfx.buffer = snd;
		sfx.current = 0;
		sfx.current_delay_left = 0;
	}
	else {
		bg.length = length;
		bg.isLoop = flags & IS_LOOP;
		bg.buffer = snd;
		bg.current = 0;
		bg.current_delay_left = 0;
	}
	spkMov();
}

void soundNext() {
	song_type *playing;

	if (sfx.length) {
		playing = &sfx;
	}
	else {
		playing = &bg;
	}

	if (playing->current < playing->length || (playing->current == playing->length && playing->current_delay_left)) {
		if (playing->current_delay_left == 0) {
			if (playing->buffer[playing->current] != 0) {
				spkMov();
				setPIT2Freq((uint16_t) (FREQDELAY / playing->buffer[playing->current]));
			}
			else {
				spkStop();
				setPIT2Freq((uint16_t) (FREQDELAY * 8));
			}
			playing->current++;
			playing->current_delay_left = playing->buffer[playing->current++];
		}
		else {
			playing->current_delay_left--;

			if (getPIT2Freq() != playing->buffer[playing->current - 2]) {
				if (playing->buffer[playing->current - 2] != 0) {
					setPIT2Freq((uint16_t) (FREQDELAY / playing->buffer[playing->current - 2]));
				}
				else {
					setPIT2Freq((uint16_t) (FREQDELAY * 8));
				}
			}
		}
	}
	else {
		if (playing->isLoop == 1) {
			playing->current = 0;
		}
		else {
			if (playing->buffer == bg.buffer)
				spkStop();
			playing->length = 0;
		}
	}
}

void beep() {
	return;
}