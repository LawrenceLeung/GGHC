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

typedef enum JigboxSignals
{
    // published signals
    TIME_TICK_SIG = Q_USER_SIG, // at SysTick rate
    EV_BUTTON_PRESSED_SIG,             /* published by button debouncer to signal the press of a button */
    EV_BUTTON_RELEASED_SIG,
    EV_HIT_SIG,                        /* accelerometer listener detected a hit event */
    TERMINATE_SIG,                     /* published by BSP to terminate the application */

    MAX_PUB_SIG,                       /* the last published signal */

    // non-published signals
    EV_START_NOTE_SIG,
    EV_STOP_NOTE_SIG,
    EV_NOTE_DONE_SIG,

    MAX_SIG                            /* the last signal */
} JigboxSignal;

#endif                                 /* __JIGBOX_H */
