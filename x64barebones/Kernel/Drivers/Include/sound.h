#ifndef SOUNDS_H_
#define SOUNDS_H_

#include <stdint.h>

void soundNext();
void beep();
void playSound(uint8_t isLoop, const uint8_t *audio, uint64_t length);

extern void spkMov();
extern void spkStop();
extern void setPIT2Freq(uint16_t delay);
extern uint16_t getPIT2Freq();

#endif
