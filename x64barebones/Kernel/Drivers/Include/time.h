#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

void timer_handler();
int ticks_elapsed();
int seconds_elapsed();
uint64_t get_ticks();
extern void setPIT0Freq(uint16_t delay);

typedef struct {
	uint8_t hora;
	uint8_t min;
	uint8_t seg;
} time_t;

extern uint8_t getRTC(time_t * timestruct);

#endif
