#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>
#include <libc.h>

uint64_t timerHandler();
uint64_t getTicks();
extern void setPIT0Freq(uint16_t delay);
extern uint8_t getRTC(time_type *timestruct);

#endif
