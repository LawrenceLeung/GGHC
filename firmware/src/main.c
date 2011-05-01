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
#include "audio_class.h"

void Initialize(void)
{
	// GPIO initialize
	// Enable GPIO clock and release reset
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
						   RCC_APB2Periph_GPIOB |
						   RCC_APB2Periph_GPIOC |
						   RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_AFIO,
						   ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA |
						   RCC_APB2Periph_GPIOB |
						   RCC_APB2Periph_GPIOC |
						   RCC_APB2Periph_GPIOD |
                           RCC_APB2Periph_AFIO,
						   DISABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	// Assign PC12 to LED
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  //GPIO_Pin_12
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// Initialize clock system
	InitTimers();

    UART_Init();

	LEDInit();

	InitAudioDevice();

	ButtonsInit();
}

#define N_ACTIVE_OBJECTS 2
#define MAX_EVENTS_PER_ACTIVE_OBJECT 5

static QSubscrList   l_subscrSto[MAX_PUB_SIG];
static QEvent const *l_activeObjQueueSto[N_ACTIVE_OBJECTS][MAX_EVENTS_PER_ACTIVE_OBJECT];

static union SmallEvent {
    void *min_size;
    /* other event types to go into this pool */
} l_smlPoolSto[2*N_ACTIVE_OBJECTS];              /* storage for the small event pool */


// only call from Main thread
// TODO remove!
void delay(int ticks)
{
    uint32_t endTime = systemTime + ticks;
    while (systemTime < endTime)
        /* spin */;
}

#define SYS_TICK_HZ 1000
#define SYS_TICK_USEC (1000000/SYS_TICK_HZ)
#define MSEC(msec) ((msec)*1000/SYS_TICK_USEC)

int main(void)
{
    /* init hardware */
    SysTick_Config(SystemCoreClock / SYS_TICK_HZ); // TODO move this

    Initialize();
    UART_printString("Jigbox Lives!\r\n");

    AccelerometerReport_t buffer;
    Init_Accelerometer();
    uint8_t brightness = 0;
    while (true)
    {
        bool ok = readAccelerometer(&buffer);

        // UART_printf("ACCEL ST=0x%x x=%d y=%d z=%d\r\n", buffer.status, buffer.x, buffer.y, buffer.z);

        RGB_LED_On(RGB_LED_2, brightness, brightness, brightness);
        RGB_LED_On(RGB_LED_1, brightness, brightness, brightness);
        RGB_LED_On(RGB_LED_3, brightness, brightness, brightness);

        brightness++;
        delay(MSEC(20));
    }

#if 0
    /* instantiate active objects */
    AO1_ctor();
    AO2_ctor();
    /* initialize the framework and the underlying RT kernel */
    QF_init();
    // init publish-subscribe
    QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));
    // initialize event pools...
    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));

    // start the active objects...
    QActive_start(AO1, 0, /*priority*/,
                  l_activeObjQueueSto[0], Q_DIM(l_activeObjQueueSto[0]),
                  (void*)0, 0, (QEvent*)0);
    QActive_start(AO2, 1, /*priority*/,
                  l_activeObjQueueSto[1], Q_DIM(l_activeObjQueueSto[1]),
                  (void*)0, 0, (QEvent*)0);
    QF_run();                          /* run the QF application */

#endif

    return 0;
} /* main */


#if 0
  // pressing any button turns on a corresponding tone
  // for (i=0; i < nBUTTONS; i++)
  // {
  //     notes[i+1].noteOn = buttons[i].pressed;
  // }

  while(1)
  {
    metronome();

    int i;


    i = 0; // Metronome voice

    if (playNextFrame)
    {
      ClearMixBuf();
      for(i = 0; i < NUMBER_OF_NOTES; i++)
      {
        MixFrame(i);
      }
      PlayFrame();
      playNextFrame = false;
    }


    if (LedUpdate)
    {
      LEDsSet(LedState);
      LedState = !LedState; // Toggle the Olimex onboard LED
      LedUpdate = false;
      LEDOff(LedCount);
      LedCount++;
      if (LedCount == nLEDs)
      {
        LedCount = 0;
      }
      LEDOn(LedCount);
    }

    if (ButtonUpdate)
    {
      ButtonsRead();
      ButtonUpdate = false;
    }

  }
#endif
