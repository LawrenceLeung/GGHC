/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2005
 *
 *    File name   : audio_class.h
 *    Description : AUDIO CLASS definitions
 *
 *    History :
 *    1. Date        : November 29, 2005
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 1.4 $
 **************************************************************************/
#include "includes.h"

#ifndef __AUDIO_CLASS_H
#define __AUDIO_CLASS_H

#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "misc.h"

extern __IO Boolean playNextFrame;

#define MinVol        0x8000

#define Feat1MinVol   0xD000UL
#define Feat1MaxVol   0x0600UL
#define Feat1ResVol   0x0001UL

#define Feat2MinVol   0xD000UL
#define Feat2MaxVol   0x0600UL
#define Feat2ResVol   0x0001UL

typedef enum
{
  FeatUnit1Id = 1, FeatUnit2Id,
  SpkInTermID, SpkOutTermID,
  MicInTermID, MicOutTermID,
  SelUnit1ID,
} AudioID_t;

// Audio Class-Specific Request Codes
#define REQUEST_CODE_UNDEFINED              0x00
#define REQUEST_SET_CUR                     0x01
#define REQUEST_GET_CUR                     0x81
#define REQUEST_SET_MIN                     0x02
#define REQUEST_GET_MIN                     0x82
#define REQUEST_SET_MAX                     0x03
#define REQUEST_GET_MAX                     0x83
#define REQUEST_SET_RES                     0x04
#define REQUEST_GET_RES                     0x84
#define REQUEST_SET_MEM                     0x05
#define REQUEST_GET_MEM                     0x85
#define REQUEST_GET_STAT                    0xFF

// Terminal Control Selectors
#define TE_CONTROL_UNDEFINED                0x00
#define TE_COPY_PROTECT_CONTROL             0x01

// Feature Unit Control Selectors
#define FU_CONTROL_UNDEFINED                0x00
#define FU_MUTE_CONTROL                     0x01
#define FU_VOLUME_CONTROL                   0x02
#define FU_BASS_CONTROL                     0x03
#define FU_MID_CONTROL                      0x04
#define FU_TREBLE_CONTROL                   0x05
#define FU_GRAPHIC_EQUALIZER_CONTROL        0x06
#define FU_AUTOMATIC_GAIN_CONTROL           0x07
#define FU_DELAY_CONTROL                    0x08
#define FU_BASS_BOOST_CONTROL               0x09
#define FU_LOUDNESS_CONTROL                 0x0A

// Endpoint Control Selectors
#define EP_CONTROL_UNDEFINED                0x00
#define SAMPLING_FREQ_CONTROL               0x01
#define PITCH_CONTROL                       0x02

/*************************************************************************
 * Function Name: UsbAudioClassInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB Class Audio Init
 *
 *************************************************************************/
void UsbAudioClassInit (void);

/*************************************************************************
 * Function Name: UsbClassAudioConfigure
 * Parameters:  pUsbDevCtrl_t pDev
 *
 * Return: none
 *
 * Description: USB Class Audio configure
 *
 *************************************************************************/
void UsbClassAudioConfigure (pUsbDevCtrl_t pDev);

/*************************************************************************
 * Function Name: AudioInHandler
 * Parameters:  USB_Endpoint_t EP
 *
 * Return: none
 *
 * Description: USB Class Audio Out EP handler
 *
 *************************************************************************/
static
void AudioInHandler (USB_Endpoint_t EP);

/*************************************************************************
 * Function Name: AudioOutHandler
 * Parameters:  USB_Endpoint_t EP
 *
 * Return: none
 *
 * Description: USB Class Audio In EP handler
 *
 *************************************************************************/
static
void AudioOutHandler (USB_Endpoint_t EP);

/*************************************************************************
 * Function Name: Tim2Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 2 interrupt handler
 *		
 *************************************************************************/
__ramfunc
void Tim2Handler (void);

/*************************************************************************
 * Function Name: AudioFeatureGetReg
 * Parameters:  Int32U CS, Int32U Id
 *
 * Return: Boolean
 *
 * Description:
 *
 *************************************************************************/
static Boolean AudioFeatureGetReg (Int32U CS, Int32U Id);

/*************************************************************************
 * Function Name: UsbClassAudioRequest
 * Parameters:  pUsbSetupPacket_t pSetup
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB Class Audio Requests
 *
 *************************************************************************/
UsbCommStatus_t UsbClassAudioRequest (pUsbSetupPacket_t pSetup);

/*************************************************************************
 * Function Name: UsbClassAudioData
 * Parameters:  USB_Endpoint_t EP
 *
 * Return: none
 *
 * Description: USB Class Audio Data receive
 *
 *************************************************************************/
static
void UsbClassAudioData (USB_Endpoint_t EP);

void AudioMemoryBufPlay (Int16S pSpkBuf[SampPerFrame]);


#endif //__AUDIO_CLASS_H
