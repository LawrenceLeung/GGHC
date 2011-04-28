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

uint32_t CriticalSecCntr;

// TODO check if this even needs to be global.
bool LedState = 0; // LED is ON when corresponding bit is 1

// TODO Del __IO bool LedUpdate = false; // LED state should be updated if true
// TODO Del __IO bool ButtonUpdate = false;  // Buttons should be read and variables updated if true

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
{ /* TODO I modified this so the input is a bool instead of a bit map */
  /* This is incompatible with the example code! */
  /* TODO Replace this code with the jigbox driver when it is done */
    GPIO_WriteBit(GPIOC,GPIO_Pin_12 ,(State)?Bit_RESET:Bit_SET);
}

// TODO move and / or eliminate this
int16_t silentSound[SampPerFrame]=
{
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

void Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* TODO refactor to move all initialization to a separate function to
	 * cleanup main()
	 */
	// Initialize clock system
	Clk_Init();
	InitTimers();
	Init_SysTick();

	// NVIC initialize
	#ifndef  EMB_FLASH
	/* Set the Vector Table base location at 0x20000000 */
	//  TODO del NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
	// TODO del  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	#endif
	//  TODO del NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

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

	// Assign PC12 to LED
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  //GPIO_Pin_12
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	LEDsSet(LedState);

	InitAudioDevice();

	LEDInit();
	ButtonsInit();

}

void PlayFrame(int voice)
{
    // TODO Add a for loop to get all notes from all buttons.
    if(notes[voice].noteOn){
        AudioMemoryBufPlay(notes[voice].noteVoiceBuffer);
    }else{
        AudioMemoryBufPlay(silentSound);
    }
}

/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description: main
 *
 *************************************************************************/
int main(void)
{

static int LedCount = 0;

  Initialize();

  while(1)
  {
    metronome();

    int i;

    for (i=0; i < nBUTTONS; i++)
    {
      if (buttons[i].pressed)
      {
        notes[i+1].noteOn = true;
      }
      else
      {
  	    notes[i+1].noteOn = false;
      }
    }

    i = 0; // Metronome voice
    // TODO refactor this into a function in audio.c
    if (playNextFrame)
    {
      PlayFrame(i);
      playNextFrame = false;
    }
    // end TODO

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
  return 1;
}
#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
