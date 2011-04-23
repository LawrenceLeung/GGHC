/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name   : usb_desc.c
 *    Description : usb decriptors module
 *
 *    History :
 *    1. Date        : 22, July 2007
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: $
 **************************************************************************/
#include "usb_desc.h"

const UsbStandardDeviceDescriptorStr_t UsbStandardDeviceDescriptorStr =
{
  sizeof(UsbStandardDeviceDescriptorStr_t),   // bLength
  UsbDescriptorDevice,                        // bDescriptorType
  0x110,                                      // bcdUSB
  0,                                          // bDeviceClass
  0,                                          // bDeviceSubClass
  0,                                          // bDeviceProtocol
  Ep0MaxSize,                                 // bMaxPacketSize0
  0xFFFF,                                     // idVendor
  0x0016,                                     // idProduct
  0x0000,                                     // bcdDevice
  iManufacturerStr,                           // iManufacturer
  iProductStr,                                // iProduct
  iSerialNumberStr,                           // iSerialNumber
  1,                                          // bNumConfigurations
};

#pragma data_alignment=4
static const Int8U UsbFS_Cfg0[] =
{
  // Configuration Descriptor
  sizeof(UsbStandardConfigurationDescriptor_t),
  UsbDescriptorConfiguration,
  _2BL(sizeof(UsbStandardConfigurationDescriptor_t) +
       /* Interface 0 */
       sizeof(UsbStandardInterfaceDescriptor_t) +
       sizeof(UsbClassSpecificACInterfaceDescriptor_t) + 1 +
       sizeof(UsbInputTerminalDescriptor_t) +
       sizeof(UsbOutputTerminalDescriptor_t)+
       sizeof(UsbInputTerminalDescriptor_t) +
       sizeof(UsbOutputTerminalDescriptor_t)+
       sizeof(ClassSpecFeatureUnitDescriptor_t) + 3 +
       sizeof(ClassSpecFeatureUnitDescriptor_t) + 3 +
       sizeof(ClassSpecSelectorUnitDescriptor_t)+ 2 +
       /* Interface 1 */
       sizeof(UsbStandardInterfaceDescriptor_t) +
       sizeof(UsbStandardInterfaceDescriptor_t) +
       sizeof(UsbClassSpecificASInterfaceDescriptor_t) +
       sizeof(UsbTypeIFormatTypeDescriptor_t) + 3 +
       sizeof(StandardASEndpointDescriptor_t) +
       sizeof(ClassSpecASEndpointDescriptor_t)+
       /* Interface 2 */
       sizeof(UsbStandardInterfaceDescriptor_t) +
       sizeof(UsbStandardInterfaceDescriptor_t) +
       sizeof(UsbClassSpecificASInterfaceDescriptor_t) +
       sizeof(UsbTypeIFormatTypeDescriptor_t) + 3 +
       sizeof(StandardASEndpointDescriptor_t) +
       sizeof(ClassSpecASEndpointDescriptor_t)
  ),
  3,
  1,
  0,
  UsbConfigurationCommmonAttr,
  UsbConfigPower_mA(100),

  // Interface 0 Standard AC Interface Descriptor
  sizeof(UsbStandardInterfaceDescriptor_t),
  UsbDescriptorInterface,
  USB_CTRL_INTERFACE,
  0,
  0,
  UsbDeviceClassAudio,
  AUDIOCONTROL,
  PR_PROTOCOL_UNDEFINED,
  0,

  // Class-Specific AC Interface Descriptor
  sizeof(UsbClassSpecificACInterfaceDescriptor_t) + 1,
  CS_INTERFACE,
  HEADER,
  _2BL(0x0100),
  _2BL(sizeof(UsbClassSpecificACInterfaceDescriptor_t) + 1 +
       sizeof(UsbInputTerminalDescriptor_t) +
       sizeof(UsbOutputTerminalDescriptor_t)+
       sizeof(UsbInputTerminalDescriptor_t) +
       sizeof(ClassSpecFeatureUnitDescriptor_t) + 3 +
       sizeof(ClassSpecFeatureUnitDescriptor_t) + 3 +
       sizeof(ClassSpecSelectorUnitDescriptor_t)+ 2 +
       sizeof(UsbOutputTerminalDescriptor_t)),
  2,
  1,
  2,

  // Input Terminal Descriptor - SPK
  sizeof(UsbInputTerminalDescriptor_t),
  CS_INTERFACE,
  INPUT_TERMINAL,
  SpkInTermID,
  _2BL(AudioUsbStreaming),
  0,
  1,
  _2BL(AUDIO_CHANNEL_M),
  0,
  0,

  // Output Terminal Descriptor - SPK
  sizeof(UsbOutputTerminalDescriptor_t),
  CS_INTERFACE,
  OUTPUT_TERMINAL,
  SpkOutTermID,
  _2BL(AudioOutputSpk),
  0,
  FeatUnit1Id,
  0,

  // Feature Unit Descriptor - SPK
  sizeof(ClassSpecFeatureUnitDescriptor_t) + 3,
  CS_INTERFACE,
  FEATURE_UNIT,
  FeatUnit1Id,
  SpkInTermID,
  1,
  FeatUnitMute+FeatUnitVolume,
  0,
  0,

  // Input Terminal Descriptor - MIC
  sizeof(UsbInputTerminalDescriptor_t),
  CS_INTERFACE,
  INPUT_TERMINAL,
  MicInTermID,
  _2BL(AudioInputMic),
  0,
  1,
  _2BL(AUDIO_CHANNEL_M),
  0,
  0,

  // Feature Unit Descriptor - MIC
  sizeof(ClassSpecFeatureUnitDescriptor_t) + 3,
  CS_INTERFACE,
  FEATURE_UNIT,
  FeatUnit2Id,
  MicInTermID,
  1,
  FeatUnitMute + FeatUnitVolume,
  0,
  0,

  // USB stream Output Terminal Descriptor - MIC
  sizeof(UsbOutputTerminalDescriptor_t),
  CS_INTERFACE,
  OUTPUT_TERMINAL,
  MicOutTermID,
  _2BL(AudioUsbStreaming),
  0,
  SelUnit1ID,
  0,

  // Selector Unit Descriptor - MIC
  sizeof(ClassSpecSelectorUnitDescriptor_t)+2,
  CS_INTERFACE,
  SELECTOR_UNIT,
  SelUnit1ID,
  1,
  FeatUnit2Id,
  0,

  // Interface 1
  // AudioStreaming Interface Descriptor
  // Zero-bandwidth Alternate Setting 0
  // Standard AS Interface Descriptor
  sizeof(UsbStandardInterfaceDescriptor_t),
  UsbDescriptorInterface,
  USB_SPK_INTERFACE,
  0,
  0,
  UsbDeviceClassAudio,
  AUDIOSTREAMING,
  PR_PROTOCOL_UNDEFINED,
  0,

  // Operational Alternate Setting 1
  // Standard AS Interface Descriptor
  sizeof(UsbStandardInterfaceDescriptor_t),
  UsbDescriptorInterface,
  1,
  1,
  1,
  UsbDeviceClassAudio,
  AUDIOSTREAMING,
  PR_PROTOCOL_UNDEFINED,
  0,

  // Class-Specific AS General Interface Descriptor
  sizeof(UsbClassSpecificASInterfaceDescriptor_t),
  CS_INTERFACE,
  AS_GENERAL,
  SpkInTermID,
  2,
  _2BL(PCM),

  // USB Spk Type I Format Type Descriptor
  sizeof(UsbTypeIFormatTypeDescriptor_t)+ 3,
  CS_INTERFACE,
  FORMAT_TYPE,
  FORMAT_TYPE_I,
  1,
  SubFrameSize,
  8*SubFrameSize,
  1,
  (Int8U)(SampFreq),(Int8U)(SampFreq>>8),(Int8U)(SampFreq>>16),

  // USB SPK Standard Endpoint Descriptor
  sizeof(StandardASEndpointDescriptor_t),
  UsbDescriptorEp,
  UsbEpOut(SpkEp>>1),
  (Int8U)UsbEpTransferIsochronous + (Int8U)UsbEpSynchAdaptive,
  _2BL(SpkEpMaxSize),
  1,
  0,
  0,

  // USB SPK Class-Specific Isoc. Audio Data Endpoint Descriptor
  sizeof(ClassSpecASEndpointDescriptor_t),
  CS_ENDPOINT,
  EP_GENERAL,
  0,
  0,
  _2BL(0),

  // Interface 2
  // AudioStreaming Interface Descriptor
  // Zero-bandwidth Alternate Setting 0
  // Standard AS Interface Descriptor
  sizeof(UsbStandardInterfaceDescriptor_t),
  UsbDescriptorInterface,
  USB_MIC_INTERFACE,
  0,
  0,
  UsbDeviceClassAudio,
  AUDIOSTREAMING,
  PR_PROTOCOL_UNDEFINED,
  0,

  // Operational Alternate Setting 1
  // Standard AS Interface Descriptor
  sizeof(UsbStandardInterfaceDescriptor_t),
  UsbDescriptorInterface,
  2,
  1,
  1,
  UsbDeviceClassAudio,
  AUDIOSTREAMING,
  PR_PROTOCOL_UNDEFINED,
  0,

  // Class-Specific AS General Interface Descriptor
  sizeof(UsbClassSpecificASInterfaceDescriptor_t),
  CS_INTERFACE,
  AS_GENERAL,
  MicOutTermID,
  1,
  _2BL(PCM),

  // USB MIC Type I Format Type Descriptor
  sizeof(UsbTypeIFormatTypeDescriptor_t)+ 3,
  CS_INTERFACE,
  FORMAT_TYPE,
  FORMAT_TYPE_I,
  1,
  SubFrameSize,
  8*SubFrameSize,
  1,
  (Int8U)(SampFreq),(Int8U)(SampFreq>>8),(Int8U)(SampFreq>>16),

  // USB MIC Standard Endpoint Descriptor
  sizeof(StandardASEndpointDescriptor_t),
  UsbDescriptorEp,
  UsbEpIn(MicEp>>1),
  (Int8U)UsbEpTransferIsochronous + (Int8U)UsbEpSynchAsynchronous,
  _2BL(MicEpMaxSize),
  1,
  0,
  0,

  // USB Mic Class-Specific Isoc. Audio Data Endpoint Descriptor
  sizeof(ClassSpecASEndpointDescriptor_t),
  CS_ENDPOINT,
  AS_GENERAL,
  0,
  0,
  _2BL(0),

  0,
};

static const Int8U * const UsbFS_CfgArray[] =
{
  UsbFS_Cfg0,
  NULL,
};

static const UsbEP_ExtDesc_t UsbEPExt_Cfg0EP1 =
{
  1,                  // Configuration
  UsbEpOut(SpkEp>>1),  // EP address
  {
    EP_SLOT1, TRUE
  }
};

static const UsbEP_ExtDesc_t UsbEPExt_Cfg0EP2 =
{
  1,                  // Configuration
  UsbEpIn(MicEp>>1), // EP address
  {
    EP_SLOT2, TRUE
  }
};

static const UsbEP_ExtDesc_t * const UsbFS_EPExtArray[] =
{
  &UsbEPExt_Cfg0EP1,
  &UsbEPExt_Cfg0EP2,
  NULL,
};

#pragma data_alignment=4
const Int8U UsbLanguagesStr [] =
{
  // Length of LanguagesStr + 2
  4,
  // Desciptor Type
  UsbDescriptorString,
  // Languages ID
  // Languages1 ID English
  0x09,0x04,
};

#pragma data_alignment=4
const Int8U ManufacturerStrLan1 [] =
{
  24, // length
  UsbDescriptorString,  // Descriptor
  'I',0,'A',0,'R',0,' ',0,'S',0,'y',0,'s',0,'t',0,'e',0,'m',0,'s',0
};

#pragma data_alignment=4
const Int8U ProductStrLan1 [] =
{
  38, //length
  UsbDescriptorString, // Descriptor
  'A',0,'u',0,'d',0,'i',0,'o',0,' ',0,'d',0,'e',0,'v',0,'i',0,'c',0,'e',0,' ',0,'c',0,'l',0,'a',0,'s',0,'s',0,
};


#pragma data_alignment=4
const Int8U SerialNumberStrLan1 [] =
{
  18, //length
  UsbDescriptorString, // Descriptor
  '0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'0',0,'1',0, // Str
};

const Int8U * const UsbLanguages1Strings[] =
{
  // iManufacturerStr
  ManufacturerStrLan1,
  // iProductStr
  ProductStrLan1,
  // iSerialNumberStr
  SerialNumberStrLan1,
  // Terminator
  NULL
};

static const Int8U * const * const UsbFS_StringArray[] =
{
  UsbLanguages1Strings,
  NULL,
};

const void * const UsbDescArray[] =
{
  UsbFS_CfgArray,
  UsbFS_EPExtArray,
  UsbLanguagesStr,
  UsbFS_StringArray,
  NULL,
};
