/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : audio_desc.h
 *    Description : AUDIO descriptor definitions
 *
 *    History :
 *    1. Date        : November 29, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 1.4 $
 **************************************************************************/
#ifndef __AUDIO_DESC_H
#define __AUDIO_DESC_H

#include "arm_comm.h"
#include "usb_desc.h"

typedef enum
{
  SUBCLASS_UNDEFINED = 0, AUDIOCONTROL, AUDIOSTREAMING,
  MIDISTREAMING,
} AudioInterfaceSubclass_t;

typedef enum
{
  PR_PROTOCOL_UNDEFINED = 0,
} AudioInterfaceProtocol_t;

typedef enum
{
  CS_UNDEFINED = 0x20,CS_DEVICE,CS_CONFIGURATION,CS_STRING,
  CS_INTERFACE,CS_ENDPOINT,
} AudioClassSpecificDescriptorTypes_t;

typedef enum
{
  AC_DESCRIPTOR_UNDEFINED = 0,HEADER,INPUT_TERMINAL,OUTPUT_TERMINAL,
  MIXER_UNIT,SELECTOR_UNIT,FEATURE_UNIT,PROCESSING_UNIT,EXTENSION_UNIT,
} AudioACInterfaceDescriptorSubtypes_t;

typedef enum
{
  AS_DESCRIPTOR_UNDEFINED = 0,AS_GENERAL,FORMAT_TYPE,FORMAT_SPECIFIC,
} AudioASInterfaceDescriptorSubtypes_t;

typedef enum
{
  DESCRIPTOR_UNDEFINED = 0,EP_GENERAL,
} AudioASEndpoinDescriptorSubtypes_t;

typedef enum
{
  FORMAT_TYPE_UNDEFINED = 0,FORMAT_TYPE_I,FORMAT_TYPE_II,FORMAT_TYPE_III,
} AudioFormatTypeCodes_t;

typedef enum
{
  TYPE_I_UNDEFINED = 0,PCM,PCM8,IEEE_FLOAT,ALAW,MULAW,
} AudioDataFormatTypeI_t;

typedef enum
{
  TYPE_II_UNDEFINED = 0x1000,MPEG,AC_3,
} AudioDataFormatTypeII_t;

typedef enum
{
  TYPE_III_UNDEFINED = 0x2000,IEC1937_AC_3,IEC1937_MPEG_1,
  IEC1937_MPEG_2_NOEXT, IEC1937_MPEG_2_EXT, IEC1937_MPEG_2_Layer1_LS,
  IEC1937_MPEG_2_Layer2_3_LS,
} AudioDataFormatTypeIII_t;

typedef enum
{
  Undefined = 0, Milliseconds, DecodedPCMsamples,
} ClassEpLockDelay_t;

typedef union
{
  Int16U wChannelConfig;
  struct
  {
     Int16U LF : 1;   // Left Front
     Int16U RF : 1;   // Right Front
     Int16U CF : 1;   // Center Front
     Int16U LFE: 1;   // Low Frequency Enhancement
     Int16U Ls : 1;   // Left Surround
     Int16U Rs : 1;   // Right Surround
     Int16U Lc : 1;   // Left of Center
     Int16U Rc : 1;   // Right of Center

     Int16U S  : 1;   // Surround
     Int16U Sl : 1;   // Side Left
     Int16U Sr : 1;   // Side Right
     Int16U T  : 1;   // Top
     Int16U    : 4;   // Reserved
  };
} ChannelConfig_t;

#define FeatUnitMute                        (1UL<<0)
#define FeatUnitVolume                      (1UL<<1)
#define FeatUnitBass                        (1UL<<2)
#define FeatUnitMid                         (1UL<<3)
#define FeatUnitTreble                      (1UL<<4)
#define FeatUnitGraphicEqualizer            (1UL<<5)
#define FeatUnitAutomaticGain               (1UL<<6)
#define FeatUnitDelay                       (1UL<<7)
#define FeatUnitBassBoost                   (1UL<<8)
#define FeatUnitLoudness                    (1UL<<9)

// Predefined Audio Channel Configuration Bits
#define AUDIO_CHANNEL_M                     0x0000  // Mono
#define AUDIO_CHANNEL_L                     0x0001  // Left Front
#define AUDIO_CHANNEL_R                     0x0002  // Right Front
#define AUDIO_CHANNEL_C                     0x0004  // Center Front
#define AUDIO_CHANNEL_LFE                   0x0008  // Low Frequency Effects
#define AUDIO_CHANNEL_LS                    0x0010  // Left Surround
#define AUDIO_CHANNEL_RS                    0x0020  // Right Surround
#define AUDIO_CHANNEL_LC                    0x0040  // Left of Center
#define AUDIO_CHANNEL_RC                    0x0080  // Right of Center
#define AUDIO_CHANNEL_S                     0x0100  // Surround
#define AUDIO_CHANNEL_SL                    0x0200  // Side Left
#define AUDIO_CHANNEL_SR                    0x0400  // Side Right
#define AUDIO_CHANNEL_T                     0x0800  // Top

//  USB Terminal Types
#define AudioUsbUndefined                   0x0100
#define AudioUsbStreaming                   0x0101
#define AudioUsbVendorSpecific              0x01FF

// Input Terminal Types
#define AudioInputUndefined                 0x0200
#define AudioInputMic                       0x0201
#define AudioInputDesktopMic                0x0202
#define AudioInputPersonalMic               0x0203
#define AudioInputOmniDirectionalMic        0x0204
#define AudioInputMicArray                  0x0205
#define AudioInputProcessingMicArray        0x0206

//  Output Terminal Types
#define AudioOutputUndefined                0x0300
#define AudioOutputSpk                      0x0301
#define AudioOutputHeadphones               0x0302
#define AudioOutputHeadMountedDisplayAudio  0x0303
#define AudioOutputDesktopSpk               0x0304
#define AudioOutputRoomSpk                  0x0305
#define AudioOutputCommunicationSpk         0x0306
#define AudioOutputLowFrequencyEffectsSpk   0x0307


#pragma pack(1)
typedef struct
{
  Int8U             bLength;
  Int8U             bDescriptorType;
  Int8U             bDescriptorSubtype;
  Int16U            bcdADC;
  Int16U            wTotalLength;
  Int8U             bInCollection;
  Int8U             baInterfaceNr1;
} UsbClassSpecificACInterfaceDescriptor_t, *pUsbClassSpecificACInterfaceDescriptor_t;

typedef struct
{
  Int8U             bLength;
  Int8U             bDescriptorType;
  Int8U             bDescriptorSubtype;
  Int8U             bTerminalID;
  Int16U            wTerminalType;
  Int8U             bAssocTerminal;
  Int8U             bNrChannels;
  ChannelConfig_t   wChannelConfig;
  Int8U             iChannelNames;
  Int8U             iTerminal;
} UsbInputTerminalDescriptor_t, *pUsbInputTerminalDescriptor_t;

typedef struct
{
  Int8U             bLength;
  Int8U             bDescriptorType;
  Int8U             bDescriptorSubtype;
  Int8U             bTerminalLink;
  Int8U             bDelay;
  Int16U            wFormatTag;
} UsbClassSpecificASInterfaceDescriptor_t, *pUsbClassSpecificASInterfaceDescriptor_t;

typedef struct
{
  Int8U             bLength;
  Int8U             bDescriptorType;
  Int8U             bDescriptorSubtype;
  Int8U             bTerminalID;
  Int16U            wTerminalType;
  Int8U             bAssocTerminal;
  Int8U             bSourceID;
  Int8U             iTerminal;
} UsbOutputTerminalDescriptor_t, *pUsbOutputTerminalDescriptor_t;

typedef struct
{
  Int8U             bLength;
  Int8U             bDescriptorType;
  Int8U             bDescriptorSubtype;
  Int8U             bFormatType;
  Int8U             bNrChannels;
  Int8U             bSubframeSize;
  Int8U             bBitResolution;
  Int8U             bSamFreqType;
} UsbTypeIFormatTypeDescriptor_t, *pUsbTypeIFormatTypeDescriptor_t;

typedef struct
{
  Int8U             bLength;
  Int8U             bDescriptorType;
  Int8U             bEndpointAddress;
  EpAttr_t          bmAttributes;
  Int16U            wMaxPacketSize;
  Int8U             bInterval;
  Int8U             bRefresh;
  Int8U             bSynchAddress;
} StandardASEndpointDescriptor_t, *pStandardASEndpointDescriptor_t;

typedef union
{
  Int8U Attribute;
  struct
  {
    Int8U SamplFreq     : 1;
    Int8U Pitch         : 1;
    Int8U               : 5;
    Int8U MaxPacketsOnly: 1;
  };
} ClassEpAttr_t, *pClassEpAttr_t;

typedef struct
{
  Int8U             bLength;
  Int8U             bDescriptorType;
  Int8U             bDescriptorSubtype;
  ClassEpAttr_t     bmAttributes;
  Int8U             bLockDelayUnits;
  Int16U            wLockDelay;
} ClassSpecASEndpointDescriptor_t, *pClassSpecASEndpointDescriptor_t;

typedef struct
{
  Int8U             bLength;
  Int8U             bDescriptorType;
  Int8U             bDescriptorSubtype;
  Int8U             bUnitID;
  Int8U             bSourceID;
  Int8U             bControlSize;
} ClassSpecFeatureUnitDescriptor_t, *pClassSpecFeatureUnitDescriptor_t;

typedef struct
{
  Int8U             bLength;
  Int8U             bDescriptorType;
  Int8U             bDescriptorSubtype;
  Int8U             bUnitID;
  Int8U             bNrInPins;
} ClassSpecSelectorUnitDescriptor_t, *pClassSpecSelectorUnitDescriptor_t;

typedef struct
{
  Int8U             bLength;
  Int8U             bDescriptorType;
  Int8U             bDescriptorSubtype;
  Int8U             bUnitID;
  Int8U             bNrInPins;
} ClassSpecMixUnitDescriptor_t, *pClassSpecMixrUnitDescriptor_t;

#pragma pack()

#endif //__AUDIO_DESC_H
