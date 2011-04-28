#ifndef __INCLUDED_SYSTICK_H
#define __INCLUDED_SYSTICK_H

#include "stm32f10x.h"

// 10 msec ticks
#define SYS_TICK_USEC 10000

extern void Init_SysTick(void);

// function to be called from the main loop to do an inefficient but timed delay
// ONLY CALL THIS FROM MAIN LOOP!
extern void delay(uint32_t ticks);

// returns the SysTick value
uint32_t systemTime(void);

#endif
