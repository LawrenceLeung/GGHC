/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name   : main.c
 *    Description : Define main module
 *
 *    History :
 *    1. Date        : 19, July 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *  This example project shows how to use the IAR Embedded Workbench for ARM
 * to develop code for the IAR-STM32-SK evaluation board. It shows
 * basic use of I/O, timer and the interrupt controllers.
 *  Displays running lights on the board LED's.
 *
 *  Jumpers:
 *   PWR_SEL - depending of power source
 *
 *    $Revision: 19278 $
 **************************************************************************/
#include "jigbox.h"

void Initialize(void)
{
    // GPIO initialize
    // Enable GPIO clock and release reset
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
                           |RCC_APB2Periph_GPIOB
                           |RCC_APB2Periph_GPIOC
                           |RCC_APB2Periph_GPIOD
                           |RCC_APB2Periph_AFIO,
                           ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA
                           |RCC_APB2Periph_GPIOB
                           |RCC_APB2Periph_GPIOC
                           |RCC_APB2Periph_GPIOD
                           |RCC_APB2Periph_AFIO,
                           DISABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    // Assign PC12 to LED
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12; //GPIO_Pin_12
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Initialize clock system
    InitTimers();

    UART_Init();

    LEDInit();

    InitAudioDevice();

    ButtonsInit();
}

// Quantum Platform

#define N_ACTIVE_OBJECTS 1
#define MAX_EVENTS_PER_ACTIVE_OBJECT 20

// static QSubscrList   l_subscrSto[MAX_PUB_SIG];
static QEvent const *l_IOEQueueSto[MAX_EVENTS_PER_ACTIVE_OBJECT];

typedef union __attribute__((__packed__))
{
    void *min_size;
    HitEvent hit;
    ButtonEvent button;
} SmallEvent;

static SmallEvent l_smlPoolSto[MAX_EVENTS_PER_ACTIVE_OBJECT*N_ACTIVE_OBJECTS];   /* storage for the small event pool */

int main(void)
{
    Initialize();

    UART_printString("Jigbox Lives!\r\n");

    /* instantiate active objects */
    IOListener_ctor();

    /* initialize the framework and the underlying RT kernel */
    QF_init();

    // init publish-subscribe
    // QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));

    // initialize event pools...
    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));


    // start the active objects...
    QActive_start(AO_IOEventListener,
                  1, /*priority*/
                  l_IOEQueueSto, Q_DIM(l_IOEQueueSto),
                  (void*)0, 0, (QEvent*)0);

    if (!Init_Accelerometer())
    {
        UART_printString("Init_Accelerometer failed!\r\n");
    }

    QF_run();                          /* run the QF application */

    return 0;
}
