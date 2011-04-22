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
#include <intrinsics.h>
#include "includes.h"
/* #include "stm32f10x_lib.h" */

Int32U CriticalSecCntr;

volatile unsigned int LedState = 0; // LED is ON when corresponding bit is 1

void LEDsSet (unsigned int);

/*************************************************************************
 * Function Name: Timer1IntrHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 1 interrupt handler
 *
 *************************************************************************/
void Timer1IntrHandler (void)
{
  // Clear update interrupt bit
  TIM1_ClearITPendingBit(TIM1_FLAG_Update);
  if(((LedState <<= 1) > 0x8000) || (LedState == 0))
  {
    LedState = 1;
  }
  LEDsSet(LedState);
  ReadButtons();
}

/*************************************************************************
 * Function Name: Clk_Init
 * Parameters: Int32U Frequency
 * Return: Int32U
 *
 * Description: Init clock system
 *
 *************************************************************************/
void Clk_Init (void)
{
  // 1. Clocking the controller from internal HSI RC (8 MHz)
  RCC_HSICmd(ENABLE);
  // wait until the HSI is ready
  while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  // 2. Enable ext. high frequency OSC
  RCC_HSEConfig(RCC_HSE_ON);
  // wait until the HSE is ready
  while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
  // 3. Init PLL
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9); // 72MHz
  RCC_PLLCmd(ENABLE);
  // wait until the PLL is ready
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  // 4. Set system clock dividers
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  RCC_ADCCLKConfig(RCC_PCLK2_Div8);
  RCC_PCLK2Config(RCC_HCLK_Div1);
  RCC_PCLK1Config(RCC_HCLK_Div2);
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
#ifdef EMB_FLASH
  // 5. Init Embedded Flash
  // Zero wait state, if 0 < HCLK 24 MHz
  // One wait state, if 24 MHz < HCLK 56 MHz
  // Two wait states, if 56 MHz < HCLK 72 MHz
  // Flash wait state
  FLASH_SetLatency(FLASH_Latency_2);
  // Half cycle access
  FLASH_HalfCycleAccessCmd(FLASH_HalfCycleAccess_Disable);
  // Prefetch buffer
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
#endif // EMB_FLASH
  // 5. Clock system from PLL
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
}

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
    GPIO_WriteBit(GPIOC,GPIO_Pin_12 ,(State & (1<<1))?Bit_RESET:Bit_SET);
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
void main(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM1_TimeBaseInitTypeDef TIM1_TimeBaseInitStruct;

#ifdef DEBUG
   debug();
#endif

  ENTR_CRT_SECTION();

  // Init clock system
  Clk_Init();

  // NVIC init
#ifndef  EMB_FLASH
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  // GPIO Init
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

  // Timer1 Init
  // Enable Timer1 clock and release reset
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1,DISABLE);

  // Set timer period 0.1 sec
  TIM1_TimeBaseInitStruct.TIM1_Prescaler = 720;  // 10us resolution
  TIM1_TimeBaseInitStruct.TIM1_CounterMode = TIM1_CounterMode_Up;
  TIM1_TimeBaseInitStruct.TIM1_Period = 10000;  // 100 ms
  TIM1_TimeBaseInitStruct.TIM1_ClockDivision = TIM1_CKD_DIV1;
  TIM1_TimeBaseInitStruct.TIM1_RepetitionCounter = 0;
  TIM1_TimeBaseInit(&TIM1_TimeBaseInitStruct);

  // Clear update interrupt bit
  TIM1_ClearITPendingBit(TIM1_FLAG_Update);
  // Enable update interrupt
  TIM1_ITConfig(TIM1_FLAG_Update,ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // Enable timer counting
  TIM1_Cmd(ENABLE);

  EXT_CRT_SECTION();

  while(1)
  {
  }
}
#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
