/*************************************************************************
 *
 *    Used with ARM IAR C/C++ Compiler.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name      : usb_buffer.c
 *    Description    : USB buffer manager module
 *
 *    History :
 *    1. Date        : June 23, 2007
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: $
 **************************************************************************/
#define USB_BUFFER_GLOBAL
#include "usb_buffer.h"

/*************************************************************************
 * Function Name: USB_IO_Data
 * Parameters: USB_Endpoint_t EP, pInt8U pBuffer, Int32U Size, void * pFn
 *
 * Return: USB_IO_Status_t
 *
 * Description: Prepare and send
 *
 *************************************************************************/
USB_IO_Status_t USB_IO_Data (USB_Endpoint_t EP, pInt8U pBuffer, Int32U Size, void * pFn)
{
  if (Size == (Int32U)-1)
  {
    EpCnfg[EP].Status  = NOT_READY;
    EpCnfg[EP].pFn     = NULL;
  }
  else
  {
    // lock buffer
    if(EpCnfg[EP].Status == BEGIN_SERVICED)
    {
      return(NOT_READY);
    }
    EpCnfg[EP].Offset  = 0;
    EpCnfg[EP].pBuffer = pBuffer;
    EpCnfg[EP].pFn     = pFn;
    if(!(EpCnfg[EP].Size = Size))
    {
      EpCnfg[EP].bZeroPacket = 1;
    }
    EpCnfg[EP].Status  = NO_SERVICED;
    USB_EP_IO(EP);
  }
  return(EpCnfg[EP].Status);
}
