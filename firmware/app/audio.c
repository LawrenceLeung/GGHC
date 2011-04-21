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
 *    1. Date        : 18, October 2007
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *  This example project shows how to use the IAR Embedded Workbench
 * for ARM to develop code for the IAR STM32-SK board.
 *  It implements usb audio device with one output and one input.
 *
 *  Jumpers:
 *   PWR_SEL - depending of power source
 *
 *    $Revision: 1.1.2.1 $
 **************************************************************************/
#include "includes.h"

#define LOOP_DLY_100US  450

/*************************************************************************
 * Function Name: Dly100us
 * Parameters: Int32U Dly
 *
 * Return: none
 *
 * Description: Delay Dly * 100us
 *
 *************************************************************************/
void Dly100us(void *arg)
{
Int32U Dly = (Int32U)arg;
  while(Dly--)
  {
    for(volatile int i = LOOP_DLY_100US; i; i--);
  }
}

/*************************************************************************
 * Function Name: initAudioDevice
 * Parameters: none
 *
 * Return: none
 *
 * Description: main
 *
 *************************************************************************/
void initAudioDevice(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
#ifdef DEBUG
  debug();
#endif
    
  ENTR_CRT_SECTION();

  // Audio Device Class
  UsbAudioClassInit();
// Set STNBY pin
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Chip on
    GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_RESET);
    
  // Soft connection enable
  USB_ConnectRes(TRUE);

  EXT_CRT_SECTION();
}
