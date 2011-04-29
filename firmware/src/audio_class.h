#ifndef __AUDIO_CLASS_H
#define __AUDIO_CLASS_H

// extern __IO bool playNextFrame;

#define MinVol        0x8000

#define Feat1MinVol   0xD000UL
#define Feat1MaxVol   0x0600UL
#define Feat1ResVol   0x0001UL

#define Feat2MinVol   0xD000UL
#define Feat2MaxVol   0x0600UL
#define Feat2ResVol   0x0001UL

#define SubFrameSize                    2
#define SampPerFrame                    48
#define SampFreq                        48 // 8 kHz
#define SpkEp         	                ENP1_OUT
#define SpkEpMaxSize   	                (SampPerFrame * SubFrameSize)
#define SpkDDInd                        0

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

void UsbAudioClassInit(void);

/*************************************************************************
 * Function Name: Tim2Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 2 interrupt handler
 *		
 *************************************************************************/
void Tim2Handler (void);

/*************************************************************************
 * Function Name: UsbClassAudioData
 * Parameters:  USB_Endpoint_t EP
 *
 * Return: none
 *
 * Description: USB Class Audio Data receive
 *
 *************************************************************************/
void AudioMemoryBufPlay (int16_t pSpkBuf[SampPerFrame]);


#endif //__AUDIO_CLASS_H
