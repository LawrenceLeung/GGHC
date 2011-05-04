/*************************************************************************
 * Jigsaw GGHC project defines.
 * This file should contain all the definitions that are specific to the
 * project including the hardware connection definitions.
 *
 **************************************************************************/

#ifndef  __JIGBOX_H
#define  __JIGBOX_H

#ifndef USE_STDPERIPH_DRIVER
#  define USE_STDPERIPH_DRIVER
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <stdbool.h>

#include "stm32f10x.h"
#include "qp_port.h"
#include "bsp.h"

#include "audio.h"
#include "LED.h"
#include "DDS.h"
#include "timer.h"
#include "buttons.h"
#include "i2c.h"
#include "UART.h"
#include "ioEventListener.h"

/* PUSH BUTTONS
 * There are 5 buttons for the pentatonic scale notes. As the mapping of
 * buttons to notes maybe dynamic, the buttons are named 1-5 for the hardware
 * access firmware.
 * Buttons are connected as follows:
 * BUTTON_1 PC2/ADC12/WP
 * BUTTON_2 PC1/ADC11/CP
 * BUTTON_3 PC0/ADC10
 * BUTTON_4 PB13/SPI2_SCK
 * BUTTON_5 PB12/SPI2_NSS
 */

#define BUTTON_1_PORT GPIOC
#define BUTTON_1_BIT  GPIO_Pin_2
#define BUTTON_2_PORT GPIOC
#define BUTTON_2_BIT  GPIO_Pin_1
#define BUTTON_3_PORT GPIOC
#define BUTTON_3_BIT  GPIO_Pin_0
#define BUTTON_4_PORT GPIOB
#define BUTTON_4_BIT  GPIO_Pin_13
#define BUTTON_5_PORT GPIOB
#define BUTTON_5_BIT  GPIO_Pin_12

/* ACCELEROMETER
 * The accelerometer is connected to the second I2C port
 * Two interrupts can be generated from the accelerometer
 * one on port PC3 and the other on PC4
 */
#define ACCEL_INT1_PORT GPIOC
#define ACCEL_INT1_PIN  GPIO_Pin_3
#define ACCEL_INT1_PORT GPIOC
#define ACCEL_INT1_PIN  GPIO_Pin_3

// Audio/play preferences
// preferred voice index into allVoices[] for metronome
#define METRONOME_VOICE 5 
// TODO just for testing
// how loud to play the button sounds
#define BUTTON_SOUND_ATTENUATION 1
// how long to flash the LED on a hit event
#define HIT_FLASH_TIME MSEC(50)
// which LED to flash
#define HIT_FLASH_LED RGB_LED_3

// TODO make adaptive
// 120 BPM fixed for now
#define RECORDING_METRONOME_PERIOD MSEC(500)

typedef enum JigboxSignals
{
    // published signals
    TIME_TICK_SIG = Q_USER_SIG, // at SysTick rate
    BUTTON_PRESSED_SIG,             /* published by button debouncer to signal the press of a button */
    BUTTON_RELEASED_SIG,
    HIT_SIG,                        /* accelerometer listener detected a hit event */

    MAX_PUB_SIG,                       /* the last published signal */

    // non-published
    IOE_TICK_SIG,               // internal IOE ticker
    IOE_PLAYBACK_SIG,           // timer ev posted to self for next playback event
    IOE_ACCEL_LED_OFF_SIG,      // timer ev posted to self for turning off LED
    IOE_METRONOME_SIG,          // repeating timer ev posted to self for metronome ticking
    IOE_MODE_SWITCH_SIG,        // change idle/record/playback modes (from 3-button push, for instance)

    MAX_SIG                            /* the last signal */
} JigboxSignal;

#endif                                 /* __JIGBOX_H */
