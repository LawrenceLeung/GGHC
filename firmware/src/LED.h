#ifndef __LED_H_INCLUDED
#define __LED_H_INCLUDED

#include "jigbox.h"

#define nLEDs 9

void LEDInit();
void LEDOn(int led);
void LEDOff(int led);

#endif
