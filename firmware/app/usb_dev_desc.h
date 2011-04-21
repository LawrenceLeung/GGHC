/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : usb_dev_desc.h
 *    Description : Descriptor definitions
 *
 *    History :
 *    1. Date        : September 22, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: $
 **************************************************************************/
#include "includes.h"

#ifndef __USB_DEV_DESC_H
#define __USB_DEV_DESC_H

typedef enum _UsbDescOffset_t
{
  USB_FS_CFG_ARRAY, USB_FS_EP_EXT_ARRAY, USB_FS_LANG_STR, USB_FS_STR_ARRAY, USB_FS_OTHER_CFG_ARRAY,
  USB_HS_CFG_ARRAY, USB_HS_EP_EXT_ARRAY, USB_HS_LANG_STR, USB_HS_STR_ARRAY, USB_HS_OTHER_CFG_ARRAY,
} UsbDescOffset_t;

typedef struct _UsbEP_ExtDesc_t
{
  Int8U           Configuration;
  Int8U           EndpointAddress;
  UsbEP_ExtData_t EP_ExtData;
} UsbEP_ExtDesc_t, *pUsbEP_ExtDesc_t;

extern const UsbStandardDeviceDescriptorStr_t UsbStandardDeviceDescriptorStr;

#if USB_HIGH_SPEED > 0
extern const UsbStandardDeviceQualifierDescriptor_t UsbQualifierDescriptor;
#endif // USB_HIGH_SPEED > 0

extern const void * const UsbDescArray[];

#endif //__USB_DEV_DESC_H
