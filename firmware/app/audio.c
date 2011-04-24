/*************************************************************************
 *
 *    History :
 *       @author      : Alan Widmer
 *
 * @file
 * This example project shows how to use the IAR Embedded Workbench
 * for ARM to develop code for the IAR STM32-SK board.
 * It implements usb audio device with one output and one input.
 *
 *    $Revision: 1.1.2.1 $
 **************************************************************************/
#include "includes.h"

#define LOOP_DLY_100US  450

Note notes[NUMBER_OF_NOTES];

Int16S testSound[SampPerFrame]=
  {
0,
267,
529,
783,
1023,
1246,
1447,
1623,
1772,
1891,
1977,
2029,
2047,
2029,
1977,
1891,
1772,
1623,
1447,
1246,
1023,
783,
529,
267,
0,
-268,
-530,
-784,
-1024,
-1247,
-1448,
-1624,
-1773,
-1892,
-1978,
-2030,
-2047,
-2030,
-1978,
-1892,
-1773,
-1624,
-1448,
-1247,
-1024,
-784,
-530,
-268
};
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
 * Function Name: InitAudioDevice
 * Parameters: none
 *
 * Return: none
 *
 * Description: Initializes the usb audio device. This turns on the USB
 * interface and waits for a connection from the USB bus master.
 *************************************************************************/
void InitAudioDevice(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
#ifdef DEBUG
  debug();
#endif
    
  ENTR_CRT_SECTION();

  // Audio Device Class
 // UsbAudioClassInit();
// Set STNBY pin
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Chip on
    GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_RESET);
    
  // Soft connection enable
  //USB_ConnectRes(TRUE);

  EXT_CRT_SECTION();
  notes[0].frequency = 216; // Middle C
  notes[0].period = 100; // TODO
  notes[0].noteVoiceBuffer = testSound; // Sinewave buffer
  notes[0].noteOn = TRUE; // TODO start with it on just for test

}
