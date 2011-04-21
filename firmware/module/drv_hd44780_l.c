/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name   : drv_hd44780_l.c
 *    Description : I/O function
 *
 *    History :
 *    1. Date        : July 19, 2007
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 15135 $
**************************************************************************/
#include "drv_hd44780_l.h"

/*************************************************************************
 * Function Name: HD44780_IO_Init
 * Parameters: none
 * Return: none
 * Description: Init IO ports directions and level
 *
 *************************************************************************/
void HD44780_IO_Init (void)
{

GPIO_InitTypeDef GPIO_InitStructure;

  // GPIO Init
  // Enable GPIO clock and release reset
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC,
                         ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC,
                         DISABLE);

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

  // E line init
  GPIO_InitStructure.GPIO_Pin = LCD_E;
  GPIO_Init(LCD_E_PORT, &GPIO_InitStructure);

  // RS line init
  GPIO_InitStructure.GPIO_Pin = LCD_RS;
  GPIO_Init(LCD_RS_PORT, &GPIO_InitStructure);

  // Light control line init
  GPIO_InitStructure.GPIO_Pin = LCD_LIGHT;
  GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);

  // Data lines init
  GPIO_InitStructure.GPIO_Pin = LCD_DATA << LCD_DATA_SHIFT;
  GPIO_Init(LCD_LIGHT_PORT, &GPIO_InitStructure);

  // RS Output - Low
  HD44780SetRS(0);
  // E  Output - Low
  HD44780SetE(0);

#if HD4780_WR > 0
  // RW line init
  GPIO_InitStructure.GPIO_Pin = LCD_RW;
  GPIO_Init(LCD_RW_PORT, &GPIO_InitStructure);

  // WR Output - Low
  HD44780SetRW(0);
#endif // HD4780_WR > 0


  // Backlight off
  LCD_LIGHT_OFF();
}

/*************************************************************************
 * Function Name: HD44780SetRS
 * Parameters: Boolean Data
 * Return: none
 * Description: Set RS signal
 *
 *************************************************************************/
void HD44780SetRS (Boolean Data)
{
  GPIO_WriteBit(LCD_RS_PORT,LCD_RS,Data?Bit_SET:Bit_RESET);
}

/*************************************************************************
 * Function Name: HD44780SetE
 * Parameters: Boolean Data
 * Return: none
 * Description: Set E signal
 *
 *************************************************************************/
void HD44780SetE (Boolean Data)
{
  GPIO_WriteBit(LCD_E_PORT,LCD_E,Data?Bit_SET:Bit_RESET);
}

/*************************************************************************
 * Function Name: HD44780WrIO
 * Parameters: Int32U Data
 * Return: none
 * Description: Write to HD44780 I/O
 *
 *************************************************************************/
void HD44780WrIO (Int32U Data)
{
  // Write Data
#if HD4780_WR > 0
GPIO_InitTypeDef  GPIO_InitStructure;
  HD44780SetRW(0);

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = LCD_DATA << LCD_DATA_SHIFT;
  GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);
#endif

  HD44780SetE(1);
  LCD_DATA_PORT->BSRR = (  Data  & 0xF) << LCD_DATA_SHIFT;
  LCD_DATA_PORT->BRR  = ((~Data) & 0xF) << LCD_DATA_SHIFT;
  HD44780_BUS_DLY();
  HD44780SetE(0);
}

#if HD4780_WR > 0
/*************************************************************************
 * Function Name: HD44780SetRW
 * Parameters: Boolean Data
 * Return: none
 * Description: Set R/W signal
 *
 *************************************************************************/
void HD44780SetRW (Boolean Data)
{
  GPIO_WriteBit(LCD_RW_PORT,LCD_RW,Data?Bit_SET:Bit_RESET);
}

/*************************************************************************
 * Function Name: HD44780RdIO
 * Parameters: none
 * Return: Int8U
 * Description: Read from HD44780 I/O
 *
 *************************************************************************/
Int8U HD44780RdIO (void)
{
Int8U Data;
GPIO_InitTypeDef  GPIO_InitStructure;
  // Set Direction
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = LCD_DATA << LCD_DATA_SHIFT;
  GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);

  HD44780SetRW(1);
  // Read Data
  HD44780SetE(1);
  HD44780_BUS_DLY();
  Data = (GPIO_ReadInputData(LCD_DATA_PORT)>>LCD_DATA_SHIFT) & 0xF;
  HD44780SetE(0);
  // Clock E
  return Data;
}
#endif
