/*************************************************************************
 *
 *    Used with ARM IAR C/C++ Compiler.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name      : usb_buffer.h
 *    Description    : usb_buffer manager module include file
 *
 *    History :
 *    1. Date        : June 23, 2007
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: $
 **************************************************************************/
#include "includes.h"

#ifndef __USB_BUFFER_H
#define __USB_BUFFER_H

#ifdef BUFFER_GLOBAL
#define BUFFER_EXTERN
#else
#define BUFFER_EXTERN extern
#endif

/*************************************************************************
 * Function Name: USB_IO_Data
 * Parameters: USB_Endpoint_t EP, pInt8U pBuffer, Int32U Size, void * pFn
 *
 * Return: USB_IO_Status_t
 *
 * Description: Prepare and send
 *
 *************************************************************************/
USB_IO_Status_t USB_IO_Data (USB_Endpoint_t EP, pInt8U pBuffer,
                             Int32U Size, void * pFn);

#endif // __USB_BUFFER_H

