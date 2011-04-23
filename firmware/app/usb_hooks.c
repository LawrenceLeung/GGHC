/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    File name   : usb_hooks.c
 *    Description : USB hook functions
 *
 *    History :
 *    1. Date        : June 1, 2007
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: $
 **************************************************************************/

#define USB_HOOKS_GLOBAL
#include "usb_hooks.h"

/*************************************************************************
 * Function Name: USB_INTR_ENTRY_HOOK
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB interrupt entry hook function
 *
 *************************************************************************/
void USB_INTR_ENTRY_HOOK (void)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_INTR_EXIT_HOOK
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB interrupt exit hook function
 *
 *************************************************************************/
void USB_INTR_EXIT_HOOK (void)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_HIGH_INTR_ENTRY_HOOK
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB high priority interrupt entry hook function
 *
 *************************************************************************/
void USB_HIGH_INTR_ENTRY_HOOK (void)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_INTR_EXIT_HOOK
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB high priority interrupt exit hook function
 *
 *************************************************************************/
void USB_HIGH_INTR_EXIT_HOOK (void)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_ERR_HOOK
 * Parameters: none
 *
 * Return: none
 *
 * Description: Called at USB error
 *
 *************************************************************************/
void USB_ERR_HOOK (Int32U Error)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_FRAME_HOOK
 * Parameters: Int32U Number
 *
 * Return: none
 *
 * Description: Called at USB SOF
 *
 *************************************************************************/
void USB_FRAME_HOOK (Int32U Number)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_CONNECT_HOOK
 * Parameters: Boolean Connect
 *
 * Return: none
 *
 * Description: Called at USB connect status change
 *
 *************************************************************************/
void USB_CONNECT_HOOK (Boolean Connect)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_SUSPEND_HOOK
 * Parameters: Boolean Suspend
 *
 * Return: none
 *
 * Description: Called at USB suspend status change
 *
 *************************************************************************/
void USB_SUSPEND_HOOK (Boolean Suspend)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_RESET_HOOK
 * Parameters: none
 *
 * Return: none
 *
 * Description: Called at USB reset
 *
 *************************************************************************/
void USB_RESET_HOOK (void)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_CONFIGURE_HOOK
 * Parameters: pUsbDevCtrl_t pDev
 *
 * Return: none
 *
 * Description: Called when configuration change
 *
 *************************************************************************/
void USB_CONFIGURE_HOOK (pUsbDevCtrl_t pDev)
{
  // Add code here ...
  UsbClassAudioConfigure(pDev);
}

/*************************************************************************
 * Function Name: USB_EP_STALL_HOOK
 * Parameters: USB_Endpoint_t EP
 *
 * Return: none
 *
 * Description: Called when EP stalled
 *
 *************************************************************************/
void USB_EP_STALL_HOOK (USB_Endpoint_t EP)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_EP_UNSTALL_HOOK
 * Parameters: USB_Endpoint_t EP
 *
 * Return: none
 *
 * Description: Called when EP unstalled
 *
 *************************************************************************/
void USB_EP_UNSTALL_HOOK (USB_Endpoint_t EP)
{
  // Add code here ...
}

/*************************************************************************
 * Function Name: USB_GET_DESCRIPTOR_HOOK
 * Parameters: pUsbSetupPacket_t pSetup
 *
 * Return: UsbCommStatus_t
 *
 * Description: Called when get descriptor receiving
 *
 *************************************************************************/
UsbCommStatus_t USB_GET_DESCRIPTOR_HOOK (pUsbSetupPacket_t pSetup)
{
  // Add code here ...
  return(UsbFault);
}

/*************************************************************************
 * Function Name: USB_CLASS_REQUEST_HOOK
 * Parameters: USB_Endpoint_t EP
 *
 * Return: UsbCommStatus_t
 *
 * Description: Called when class request receiving
 *
 *************************************************************************/
UsbCommStatus_t USB_CLASS_REQUEST_HOOK (pUsbSetupPacket_t pSetup)
{
  return(UsbClassAudioRequest(pSetup));
  // Add code here ...
  //return(UsbFault);
}

/*************************************************************************
 * Function Name: USB_VENDOR_REQUEST_HOOK
 * Parameters: USB_Endpoint_t EP
 *
 * Return: UsbCommStatus_t
 *
 * Description: Called when vendor request receiving
 *
 *************************************************************************/
UsbCommStatus_t USB_VENDOR_REQUEST_HOOK (pUsbSetupPacket_t pSetup)
{
  // Add code here ...
  return(UsbFault);
}
