#ifndef __LED_H_INCLUDED
#define __LED_H_INCLUDED

#include "jigbox.h"

typedef enum
{
    LED_1_RED,      // Red1 = TIM12_CH1
    LED_1_GREEN,    // Green1 = TIM2_CH4
    LED_1_BLUE,     // Blue1 = TIM1_CH2
    LED_2_RED,      // Red2 = TIM2_CH3
    LED_2_GREEN,    // Green2 = TIM12_CH1
    LED_2_BLUE,     // Blue2 = TIM3_CH4
    LED_3_RED,      // Red3 = TIM4_CH3
    LED_3_GREEN,    // Green3 = TIM4_CH4
    LED_3_BLUE,     // Blue3 = TIM3_CH3

    N_LEDS          // number of off-board LEDs
} Single_LED_t;

typedef enum
{
    RGB_LED_1,
    RGB_LED_2,
    RGB_LED_3,

    N_RGB_LEDS      // number of RGB LEDs
} RGB_LED_t;

typedef uint8_t LED_Brightness;        // 0=OFF, 0xFF=FULL_ON

void LEDInit();

bool Single_LED_On(Single_LED_t led, LED_Brightness howmuch);

bool RGB_LED_On(RGB_LED_t led, LED_Brightness red, LED_Brightness green, LED_Brightness blue);

#endif
