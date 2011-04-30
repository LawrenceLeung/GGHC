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

bool LedState = 0; // LED is ON when corresponding bit is 1

void LEDsSet (unsigned int);

/*************************************************************************
 * Function Name: LEDsSet
 * Parameters: unsigned int State
 *
 * Return: none
 *
 * Description: Set LEDS State
 *
 *************************************************************************/
void LEDsSet (unsigned int State)
{
    GPIO_WriteBit(GPIOC,GPIO_Pin_12 ,(State)?Bit_RESET:Bit_SET);
}

void Initialize(void)
{
	// Initialize clock system
	InitTimers();

	// GPIO initialize
	// Enable GPIO clock and release reset
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
						   RCC_APB2Periph_GPIOB |
						   RCC_APB2Periph_GPIOC,
						   ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA |
						   RCC_APB2Periph_GPIOB |
						   RCC_APB2Periph_GPIOC,
						   DISABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	// Assign PC12 to LED
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  //GPIO_Pin_12
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	LEDInit();

	LEDsSet(LedState);

	InitAudioDevice();

	ButtonsInit();
}

#define N_ACTIVE_OBJECTS 2
#define MAX_EVENTS_PER_ACTIVE_OBJECT 10

// static QEvent const *l_tableQueueSto[N_PHILO];
static QSubscrList   l_subscrSto[MAX_PUB_SIG];
static QEvent const *l_activeObjQueueSto[N_ACTIVE_OBJECTS][MAX_EVENTS_PER_ACTIVE_OBJECT];

static union SmallEvent {
    void *min_size;
    /* other event types to go into this pool */
} l_smlPoolSto[2*N_ACTIVE_OBJECTS];              /* storage for the small event pool */


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
  Initialize();
  SysTick_Config(SystemCoreClock / SYS_TICK_HZ);  // TODO move this

  AccelerometerReport_t buffer;
  Init_Accelerometer();
  uint8_t brightness = 0;
  while (true)
  {
	  readAccelerometer(&buffer);
      RGB_LED_On(RGB_LED_1, buffer.x, buffer.y, buffer.z);
      RGB_LED_On(RGB_LED_2, buffer.x, buffer.y, buffer.z);

      RGB_LED_On(RGB_LED_3, brightness, brightness, brightness);
      brightness++;
      delay(MSEC(100));
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
                    (void *)0, 0, (QEvent *)0);
  QActive_start(AO2, 1, /*priority*/,
                    l_activeObjQueueSto[1], Q_DIM(l_activeObjQueueSto[1]),
                    (void *)0, 0, (QEvent *)0);
  QF_run();                                     /* run the QF application */

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
