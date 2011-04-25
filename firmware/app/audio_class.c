/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name   : audio_class.c
 *    Description : AUDIO CLASS module
 *
 *    History :
 *    1. Date        : 18, October 2007
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 1.4 $
 **************************************************************************/
#include "audio_class.h"
#include "stm32f10x_tim.h"
#include "includes.h"
#include "jigbox.h"

__IO Boolean SempEna,MicEna;
Int32U  SempPeriod,DeltaPer,MicCurrBuffer;
__IO pInt16S pSpkData,pMicData;
__IO Int32U  SempPerCurrHold,Delta,MicSempCount,SempCount;

__IO Boolean playNextFrame = FALSE;

static union _Val
{
  Int32S Data;
  struct
  {
    Int16U DataLo;
    Int16S DataHi;
  };
} Val = {0x02000000};


const Int32U VolumeMul [] =
{
  // 256 - 0
  //-48dB- 0x0
  261,
  //-47dB- 0x1
  293,
  //-46dB- 0x2
  328,
  //-45dB- 0x3
  369,
  //-44dB- 0x4
  414,
  //-43dB- 0x5
  464,
  //-42dB- 0x6
  521,
  //-41dB- 0x7
  584,
  //-40dB- 0x8
  655,
  //-39dB- 0x9
  735,
  //-38dB- 0xA
  825,
  //-37dB- 0xB
  926,
  //-36dB- 0xC
  1039,
  //-35dB- 0xD
  1165,
  //-34dB- 0xE
  1308,
  //-33dB- 0xF
  1467,
  //-32dB- 0x10
  1646,
  //-31dB- 0x11
  1847,
  //-30dB- 0x12
  2072,
  //-29dB- 0x13
  2325,
  //-28dB- 0x14
  2609,
  //-27dB- 0x15
  2927,
  //-26dB- 0x16
  3285,
  //-25dB- 0x17
  3685,
  //-24dB- 0x18
  4135,
  //-23dB- 0x19
  4640,
  //-22dB- 0x1A
  5206,
  //-21dB- 0x1B
  5841,
  //-20dB- 0x1C
  6554,
  //-19dB- 0x1D
  7353,
  //-18dB- 0x1E
  8250,
  //-17dB- 0x1F
  9257,
  //-16dB- 0x20
  10387,
  //-15dB- 0x21
  11654,
  //-14dB- 0x22
  13076,
  //-13dB- 0x23
  14672,
  //-12dB- 0x24
  16462,
  //-11dB- 0x25
  18471,
  //-10dB- 0x26
  20724,
  //-9dB - 0x27
  23253,
  //-8dB - 0x28
  26090,
  //-7dB - 0x29
  29274,
  //-6dB - 0x2A
  32846,
  //-5dB - 0x2B
  36854,
  //-4dB - 0x2C
  41350,
  //-3dB - 0x2D
  46396,
  //-2dB - 0x2E
  52057,
  //-1dB - 0x2F
  58409,
  // 0dB - 0x30
  65536,
  // 1dB - 0x31
  73533,
  // 2dB - 0x32
  82505,
  // 3dB - 0x33
  92572,
  // 4dB - 0x34
  103868,
  // 5dB - 0x35
  116541,
  // 6dB - 0x36
  130762,
};

#pragma data_alignment=4
__no_init Int16S AudioSpkData[SampPerFrame * 3];

#pragma data_alignment=4
__no_init Int16S AudioMicData1[SampPerFrame * 2];
#pragma data_alignment=4
__no_init Int16S AudioMicData2[SampPerFrame * 2];

#pragma data_alignment=4
Int8U AudioBuf[2];

Int8U   AudioRequest,AudioCS,AudioCN,AudioId;
Int16U  AudioDataSize;

Int16S  AudioFeat1Vol;
Int32U  AudioSpkVolMul = 30; /* TODO, invent a volume control scheme */
Boolean AudioFeat1Mute;

Int16S  AudioFeat2Vol;
Int32U  AudioMicVolMul;
Boolean AudioFeat2Mute;

/*************************************************************************
 * Function Name: UsbAudioClassInit
 * Parameters: none
 *
 * Return: none
 *
 * Description: USB Class Audio Init
 *
 *************************************************************************/
void UsbAudioClassInit (void)
{
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
GPIO_InitTypeDef GPIO_InitStructure;
ADC_InitTypeDef   ADC_InitStructure;
RCC_ClocksTypeDef RCC_Clocks;
NVIC_InitTypeDef NVIC_InitStructure;
//#ifdef TODO
//TIM1_TimeBaseInitTypeDef TIM1_TimeBaseInitStruct;
TIM_OCInitTypeDef  TIM1_OCInitStructure;
//#endif

  // Init Audio Class variables
  SempEna         =\
  AudioFeat1Mute  =\
  AudioFeat2Mute  = FALSE;
  SempCount       =\
  AudioFeat1Vol   =\
  AudioFeat2Vol   = 0;
  pSpkData        = AudioSpkData;
  SempPerCurrHold = SempPeriod;
  Delta = 0;

  // Get different on chips' clocks.
  RCC_GetClocksFreq(&RCC_Clocks);

  // PWM DAC (TIM2) 10 bit PA8
  // Enable GPIO clock and release reset
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,
                         ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA,
                         DISABLE);

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Init PWM TIM1
  // Enable Timer1 clock and release reset
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1,DISABLE);

//#ifdef TODO
  // Time base configuration
  TIM_TimeBaseStructure.TIM_Prescaler = 0;  // Max frequency
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 0x3FF; // 10 bit resolution
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  // Channel 1 Configuration in PWM mode
  TIM1_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM1_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM1_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM1_OCInitStructure.TIM_Pulse = 0x200;
  TIM1_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM1_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM1_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM1_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  TIM_OC1Init(TIM1, &TIM1_OCInitStructure);
  // Double buffered
  TIM_ARRPreloadConfig(TIM1, ENABLE);
  // TIM1 counter enable
  TIM_Cmd(TIM1, ENABLE);

  // TIM1 Main Output Enable
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
//#endif
  
  // ADC init PA1
  // Enable ADC clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  // ADC Deinit
  ADC_DeInit(ADC1);
  // Assign PA1 to ADC ch 1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Speed = (GPIOSpeed_TypeDef)0;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // ADC Structure Initialization
  ADC_StructInit(&ADC_InitStructure);

  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  // Enable the ADC
  ADC_Cmd(ADC1, ENABLE);

  // ADC calibration
  // Enable ADC1 reset calibration register
  ADC_ResetCalibration(ADC1);
  // Check the end of ADC1 reset calibration register
  while(ADC_GetResetCalibrationStatus(ADC1) == SET);

  // Start ADC1 calibration
  ADC_StartCalibration(ADC1);
  // Check the end of ADC1 calibration
  while(ADC_GetCalibrationStatus(ADC1) == SET);

  // Configure channel
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_13Cycles5);

  // Init Sample Timer - Timer2
  // TIM2 clock enable
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  // Reset TIM2
  TIM_DeInit(TIM2);
  // Time base configuration
  SempPeriod = RCC_Clocks.PCLK2_Frequency/SampFreq;
  DeltaPer = (SempPeriod/(SampPerFrame*2)) + 1;
  TIM_TimeBaseStructure.TIM_Period = SempPeriod / 100;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;      // max resolution
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  // Disable double buffer of the APR register
  TIM_ARRPreloadConfig(TIM2, ENABLE);
  // Clear update interrupt bit
  TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
  // Enable update interrupt
  TIM_ITConfig(TIM2,TIM_FLAG_Update,ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USB_INTR_AUDIO_SAMP_TIMER_PRIORITY; // max priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  // TIM2 enable counter
  TIM_Cmd(TIM2, ENABLE);

  //UsbClassAudioConfigure(NULL);
  //UsbCoreInit();

  playNextFrame = TRUE; // Signal that it is OK to load audio samples


}

/*************************************************************************
 * Function Name: UsbClassAudioConfigure
 * Parameters:  pUsbDevCtrl_t pDev
 *
 * Return: none
 *
 * Description: USB Class Audio configure
 *
 *************************************************************************/
void UsbClassAudioConfigure (pUsbDevCtrl_t pDev)
{
  if(pDev == NULL)
  {
    if(UsbCoreReq(UsbCoreReqConfiquration) != 0)
    {
      SempEna = MicEna = FALSE;
    }
  }
  else
  {
    if((pDev->Configuration == 1))
    {
      if (pDev->AlternateSetting[USB_SPK_INTERFACE] == 1)
      {
        pSpkData = AudioSpkData;
        Delta = 0;
        USB_IO_Data(SpkEp,(pInt8U)pSpkData,SpkEpMaxSize,(void*)AudioInHandler);
      }
      if (pDev->AlternateSetting[USB_MIC_INTERFACE] == 1)
      {
        MicEna = FALSE;
        pMicData = AudioMicData1;
        MicCurrBuffer = 0;
        // Send microphone output buffer
        USB_IO_Data( MicEp,
                     NULL,
                     0,
                     NULL);
        USB_IO_Data( MicEp,
                     NULL,
                     0,
                     NULL);
        USB_IO_Data( MicEp,
                     NULL,
                     0,
                    (void*)AudioOutHandler);
      }
      else
      {
        MicEna = FALSE;
      }
    }
  }
}

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
void AudioInHandler (USB_Endpoint_t EP)
{
  assert(SpkEp == EP);

  if (EpCnfg[EP].Status == COMPLETE)
  {
    pSpkData += EpCnfg[EP].Size/sizeof(Int16U);
    if(pSpkData >= AudioSpkData + sizeof(AudioSpkData)/sizeof(Int16U))
    {
      pSpkData = AudioSpkData;
    }

    if(SempEna)
    {
      // Adapt sample rate
      if (Delta > (SampPerFrame * SubFrameSize))
      {
        SempPerCurrHold = SempPeriod - DeltaPer;
      }
      else if (Delta < (SampPerFrame * SubFrameSize))
      {
        SempPerCurrHold = SempPeriod + DeltaPer;
      }
      else
      {
        SempPerCurrHold = SempPeriod;
      }
    }
    else
    {
      if(pSpkData >= (AudioSpkData + (2*sizeof(AudioSpkData))/(3*sizeof(Int16U))))
      {
        // Enable output
        SempCount = 0;
        SempEna   = TRUE;
      }
    }
    ENTR_CRT_SECTION();
    Delta  += EpCnfg[EP].Size;
    EXT_CRT_SECTION();
  }
  else
  {
    // reset all
    pSpkData  = AudioSpkData;
    SempCount = 0;
    SempEna   = FALSE;
    SempPerCurrHold = SempPeriod;
    Delta = 0;
  }

  USB_IO_Data(SpkEp,(pInt8U)pSpkData,SpkEpMaxSize,(void*)AudioInHandler);
}

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
void AudioOutHandler (USB_Endpoint_t EP)
{
Int32U PacketLength;

  ENTR_CRT_SECTION();
  MicEna = TRUE;
  // Set pointer to free buffer
  if(++MicCurrBuffer & 1)
  {
    PacketLength = (Int32U)pMicData - (Int32U)AudioMicData1;
    pMicData = AudioMicData2;
  }
  else
  {
    PacketLength = (Int32U)pMicData - (Int32U)AudioMicData2;
    pMicData = AudioMicData1;
  }
  EXT_CRT_SECTION();
  // Send microphone output buffer
  USB_IO_Data( MicEp,
              (pInt8U)((MicCurrBuffer & 1)?AudioMicData1:AudioMicData2),
              PacketLength,
              (void*)AudioOutHandler);
}

/*************************************************************************
 * Function Name: Tim2Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 2 interrupt handler
 *		
 *************************************************************************/

void Tim2Handler (void)
{
union _Val MicTemp;
  if (SempEna)
  {
    if(Delta > 1)
    {
      // Get next input sample
      Val.Data = AudioSpkData[SempCount++];
      Delta   -= 2;
      if (Delta < SampPerFrame) // There is room for one more frame
      {
        playNextFrame = TRUE;
      }
      // Correct pointer to sample buffer
      if(SempCount >= sizeof(AudioSpkData)/sizeof(Int16U))
      {
        SempCount = 0;
      }
      if(!AudioFeat1Mute && AudioSpkVolMul)
      {
        // Apply volume
        Val.Data   *= AudioSpkVolMul;
        // Add offset
        Val.DataHi += 0x200;
        // Check for overflow and correct value
        if(Val.DataHi < 50)
        {
          Val.DataHi = 50;
        }
        else if(Val.DataHi > 975)
        {
          Val.DataHi = 975;
        }
      }
      else
      {
        // set middle of range
        Val.DataHi = 0x200;
      }
    }
    else
    {
      // Disable output stream after emptying the buffer
      SempEna = FALSE;
      SempPerCurrHold = SempPeriod;
    }
  }
  else
  {
    SempPerCurrHold = SempPeriod;
  }
  // reload PWM
  TIM1->CCR1 =  Val.DataHi;
  if(MicEna)
  {
    if(!AudioFeat2Mute && AudioMicVolMul)
    {

      // Get ADC sample and remove offset
      MicTemp.Data  = (ADC1->DR>>2) - 0x200;
      // Start the conversion
      ADC1->CR2 |= ((Int32U)0x00500000);

      // Add volume
      MicTemp.Data *= AudioMicVolMul;
      // check for overflow and correct the result
      if(MicTemp.DataHi < -511)
      {
        MicTemp.Data = -511 * 65536;
      }
      else if(MicTemp.DataHi > 511)
      {
        MicTemp.Data = 511 * 65536;
      }
      // Scale to 16bit
      MicTemp.Data <<= 6;
    }
    else
    {
      // set middle of range
      MicTemp.DataHi = 0;
    }
    // write to output buffer
    *pMicData++ = MicTemp.DataHi;
    if(MicCurrBuffer & 1)
    {
      if (pMicData >= (AudioMicData2 + (SampPerFrame * 2)))
      {
        MicEna = FALSE;
      }
    }
    else
    {
      if (pMicData >= (AudioMicData1+(SampPerFrame * 2)))
      {
        MicEna = FALSE;
      }
    }
  }

  TIM2->ARR = SempPerCurrHold; // reload output compare
  TIM2->SR &= (uint16_t)~TIM_FLAG_Update;
}

/*************************************************************************
 * Function Name: AudioFeatureGetReg
 * Parameters:  Int32U CS, Int32U Id
 *
 * Return: Boolean
 *
 * Description:
 *
 *************************************************************************/
static
Boolean AudioFeatureGetReg (Int32U CS, Int32U Id)
{
  switch (CS)
  {
  case REQUEST_GET_CUR:
    // Load current value of Volume in the transmit buffer
    if (Id == FeatUnit1Id)
    {
      AudioBuf[1] = (AudioFeat1Vol>>8)& 0xFF;
      AudioBuf[0] =  AudioFeat1Vol    & 0xFF;
    }
    else if (Id == FeatUnit2Id)
    {
      AudioBuf[1] = (AudioFeat2Vol>>8)& 0xFF;
      AudioBuf[0] =  AudioFeat2Vol    & 0xFF;
    }
    else
    {
      return(FALSE);
    }
    break;
  case REQUEST_GET_MIN:
    // Load minimum value of Volume in the transmit buffer
    if (Id == FeatUnit1Id)
    {
      AudioBuf[1] = (Feat1MinVol>>8)  & 0xFF;
      AudioBuf[0] =  Feat1MinVol      & 0xFF;
    }
    else if (Id == FeatUnit2Id)
    {
      AudioBuf[1] = (Feat2MinVol>>8)  & 0xFF;
      AudioBuf[0] =  Feat2MinVol      & 0xFF;
    }
    else
    {
      return(FALSE);
    }
    break;
  case REQUEST_GET_MAX:
    // Load maximum value of Volume in the transmit buffer
    if (Id == FeatUnit1Id)
    {
      AudioBuf[1] = (Feat1MaxVol>>8)  & 0xFF;
      AudioBuf[0] =  Feat1MaxVol      & 0xFF;
    }
    else if (Id == FeatUnit2Id)
    {
      AudioBuf[1] = (Feat2MaxVol>>8)  & 0xFF;
      AudioBuf[0] =  Feat2MaxVol      & 0xFF;
    }
    else
    {
      return(FALSE);
    }
    break;
  case REQUEST_GET_RES:
    // Load resolution value of Volume in the transmit buffer
    if (Id == FeatUnit1Id)
    {
      AudioBuf[1] = (Feat1ResVol>>8)  & 0xFF;
      AudioBuf[0] =  Feat1ResVol      & 0xFF;
    }
    else if (Id == FeatUnit2Id)
    {
      AudioBuf[1] = (Feat2ResVol>>8)  & 0xFF;
      AudioBuf[0] =  Feat2ResVol      & 0xFF;
    }
    else
    {
      return(FALSE);
    }
    break;
  default:
    return(FALSE);
  }
  return(TRUE);
}

/*************************************************************************
 * Function Name: UsbClassAudioRequest
 * Parameters:  pUsbSetupPacket_t pSetup
 *
 * Return: UsbCommStatus_t
 *
 * Description: USB Class Audio Requests
 *
 *************************************************************************/
UsbCommStatus_t UsbClassAudioRequest (pUsbSetupPacket_t pSetup)
{
  // Validate Request
  switch (pSetup->mRequestType.Recipient)
  {
  case UsbRecipientInterface:
    // Feature Unit requests only Implement for interface 0
    if ((pSetup->wIndex.Word == (FeatUnit1Id << 8)) ||
        (pSetup->wIndex.Word == (FeatUnit2Id << 8)))
    {
      // Request type
      switch (pSetup->wValue.Hi)
      {
      case FU_MUTE_CONTROL:
        if ((pSetup->bRequest == REQUEST_SET_CUR) &&
            (pSetup->wLength.Word == 1))
        {
          // Set mute flag
          AudioRequest  = pSetup->bRequest;
          AudioId       = pSetup->wIndex.Hi;
          AudioCS       = pSetup->wValue.Hi;
          AudioCN       = pSetup->wValue.Lo;
          AudioDataSize = pSetup->wLength.Word;
          USB_IO_Data(CTRL_ENP_OUT,
                      AudioBuf,
                      USB_T9_Size(1,pSetup->wLength.Word),
                     (void *)UsbClassAudioData);
          return(UsbPass);
        }
        else if ((pSetup->bRequest == REQUEST_GET_CUR) &&
                 (pSetup->wLength.Word == 1))
        {
          // Read mute flag
          if(pSetup->wIndex.Hi == FeatUnit1Id)
          {
            AudioBuf[0]  = AudioFeat1Mute;
          }
          else
          {
            AudioBuf[0]  = AudioFeat2Mute;
          }
          USB_IO_Data(CTRL_ENP_IN,
                      AudioBuf,
                      USB_T9_Size(1,pSetup->wLength.Word),
                     (void *)USB_StatusHandler);
          return(UsbPass);
        }
        break;
      case FU_VOLUME_CONTROL:
        if(pSetup->bRequest & 0x80)
        {
          // Read different volume values
          if((pSetup->wLength.Word == 2) &&
              AudioFeatureGetReg(pSetup->bRequest,pSetup->wIndex.Hi))
          {
          USB_IO_Data(CTRL_ENP_IN,
                      AudioBuf,
                      USB_T9_Size(2,pSetup->wLength.Word),
                     (void *)USB_StatusHandler);
            return(UsbPass);
          }
        }
        else if((pSetup->bRequest == REQUEST_SET_CUR) &&
                (pSetup->wLength.Word  == 2))
        {
          // Set volume value
          AudioRequest  = pSetup->bRequest;
          AudioId       = pSetup->wIndex.Hi;
          AudioCS       = pSetup->wValue.Hi;
          AudioCN       = pSetup->wValue.Lo;
          AudioDataSize = pSetup->wLength.Word;
          USB_IO_Data(CTRL_ENP_OUT,
                      AudioBuf,
                      USB_T9_Size(2,pSetup->wLength.Word),
                     (void *)UsbClassAudioData);
          return(UsbPass);
        }
        break;
      }
    }
    // Selector Unit requests only Implement for interface 0
    else if (pSetup->wIndex.Word == (SelUnit1ID << 8))
    {
      if(pSetup->wValue.Word == 0)
      {
        // Read different selector unit values
        if(pSetup->bRequest & 0x80)
        {
          if(pSetup->wLength.Word == 1)
          {
            AudioBuf[0] = 1;
            USB_IO_Data(CTRL_ENP_IN,
                        AudioBuf,
                        USB_T9_Size(1,pSetup->wLength.Word),
                       (void *)USB_StatusHandler);
            return(UsbPass);
          }
        }
        else
        {
          // Set channel
          AudioRequest  = pSetup->bRequest;
          AudioId       = pSetup->wIndex.Hi;
          AudioDataSize = pSetup->wLength.Word;
          USB_IO_Data(CTRL_ENP_OUT,
                      AudioBuf,
                      USB_T9_Size(1,pSetup->wLength.Word),
                     (void *)UsbClassAudioData);
          return(UsbPass);
        }
      }
    }
    return(UsbFault);
  case UsbRecipientEndpoint:
    return(UsbFault);
  }
  return(UsbFault);
}

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
void UsbClassAudioData (USB_Endpoint_t EP)
{
  if (EpCnfg[EP].Status != COMPLETE)
  {
    USB_StallCtrlEP();
    return;
  }
  if((AudioId == FeatUnit1Id) ||
     (AudioId == FeatUnit2Id))
  {
    switch (AudioCS)
    {
    case FU_MUTE_CONTROL:
      // Set mute flag
      if (AudioId == FeatUnit1Id)
      {
        AudioFeat1Mute = AudioBuf[0];
      }
      else if (AudioId == FeatUnit2Id)
      {
        AudioFeat2Mute = AudioBuf[0];
      }
      else
      {
        USB_StallCtrlEP();
        return;
      }
      break;
    case FU_VOLUME_CONTROL:
      // Set volume value
      if (AudioId == FeatUnit1Id)
      {
        AudioFeat1Vol = AudioBuf[0] + ((Int16U)AudioBuf[1]<<8);
        if((Int16U)AudioFeat1Vol == MinVol)
        {
          AudioSpkVolMul = 0;
        }
        else
        {
          if (AudioFeat1Vol < (Int16S)Feat1MinVol)
          {
            AudioFeat1Vol = Feat1MinVol;
          }
          else if(AudioFeat1Vol > (Int16S)Feat1MaxVol)
          {
            AudioFeat1Vol = Feat1MaxVol;
          }
          // -48dB - +6dB multiply
          AudioSpkVolMul = (VolumeMul[((AudioFeat1Vol>>8)+0x30) & 0x3F]) >> 6;
        }
      }
      else if (AudioId == FeatUnit2Id)
      {
        AudioFeat2Vol = AudioBuf[0] + ((Int16U)AudioBuf[1]<<8);
        if((Int16U)AudioFeat2Vol == MinVol)
        {
          AudioMicVolMul = 0;
        }
        else
        {
          if (AudioFeat2Vol < (Int16S)Feat2MinVol)
          {
            AudioFeat2Vol = Feat2MinVol;
          }
          else if(AudioFeat2Vol > (Int16S)Feat2MaxVol)
          {
            AudioFeat2Vol = Feat2MaxVol;
          }
          // -48dB - +6dB multiply
          AudioMicVolMul = VolumeMul[((AudioFeat2Vol>>8)+0x30) & 0x3F];
        }
      }
      else
      {
        USB_StallCtrlEP();
        return;
      }
      break;
    default:
      USB_StallCtrlEP();
      return;
    }
  }
  else if (AudioId == SelUnit1ID)
  {
    // empty
  }
  else
  {
    USB_StallCtrlEP();
    return;
  }
  USB_StatusHandler(CTRL_ENP_IN);
}

/** AudioMemoryBufPlay
 * Takes a buffer with audio wave data and starts to play it
 * through the headphone output. Once the buffer has started playing,
 * timer2 will keep loading new data to the audio output until the entire
 * buffer has been played. Pass a NULL pointer to stop the currently playing
 * buffer.
 *
 * @param pSpkBuf
 */

void AudioMemoryBufPlay (Int16S pSpkBuf[SampPerFrame])
{
  int i;

  if (pSpkBuf != NULL)
  {
    // AudioSpkData is a circular buffer
    // Copy new buffer to AudioSpkData
    for (i = 0; i < SampPerFrame; i++)
    {
      * pSpkData = pSpkBuf[i];
      pSpkData++;
      if(pSpkData >= AudioSpkData + sizeof(AudioSpkData)/sizeof(Int16U))
      {
        pSpkData = AudioSpkData;
      }
    }

    if(SempEna)
    {
      // Adapt sample rate
      if (Delta > (SampPerFrame * SubFrameSize))
      {
        SempPerCurrHold = SempPeriod - DeltaPer;
      }
      else if (Delta < (SampPerFrame * SubFrameSize))
      {
        SempPerCurrHold = SempPeriod + DeltaPer;
      }
      else
      {
        SempPerCurrHold = SempPeriod;
      }
    }
    else
    {
      // Enable output
      SempCount = 0;
      SempEna   = TRUE;
    }
    ENTR_CRT_SECTION();
    Delta  += SampPerFrame;
    EXT_CRT_SECTION();
  }
  else
  {
    // reset all
    pSpkData  = AudioSpkData;
    SempCount = 0;
    SempEna   = FALSE;
    SempPerCurrHold = SempPeriod;
    Delta = 0;
  }

}
