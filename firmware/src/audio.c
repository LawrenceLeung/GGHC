/*************************************************************************
 *
 *    History :
 *       @author      : Alan Widmer
 *
 * @file
 * This example project shows how to use the IAR Embedded Workbench
 * for ARM to develop code for the IAR STM32-SK board.
 * It implements usb audio device with one output and one input.
 *
 *    $Revision: 1.1.2.1 $
 **************************************************************************/
#include "jigbox.h"
#include "audio_class.h"
#include "voices.h"

#define LOOP_DLY_100US 450

Note notes[NUMBER_OF_NOTES];

static uint32_t metronomePeriod;

audioBuf_t testSound[SampPerFrame] = {
    0, 267, 529, 783, 1023, 1246, 1447, 1623, 1772, 1891, 1977, 2029, 2047,
    2029, 1977, 1891, 1772, 1623, 1447, 1246, 1023, 783, 529, 267, 0, -268,
    -530, -784, -1024, -1247, -1448, -1624, -1773, -1892, -1978, -2030, -2047,
    -2030, -1978, -1892, -1773, -1624, -1448, -1247, -1024, -784, -530, -268
};

audioBuf_t silentSound[SampPerFrame]=
{
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

audioBuf_t mixBuffer[SampPerFrame]=
{
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

/*************************************************************************
 * Function Name: Dly100us
 * Parameters: uint32_t Dly
 *
 * Return: none
 *
 * Description: Delay Dly * 100us
 *
 *************************************************************************/
void Dly100us(void *arg)
{
    uint32_t Dly = (uint32_t)arg;
    __IO int i;
    while(Dly--)
    {
        for(i = LOOP_DLY_100US; i; i--)
        {
        }
    }
}

void SetMetronomePeriod(uint32_t newPeriod)
{
    metronomePeriod = newPeriod;
}

/*************************************************************************
 * Function Name: InitAudioDevice
 * Parameters: none
 *
 * Return: none
 *
 * Description: Initializes the usb audio device. This turns on the USB
 * interface and waits for a connection from the USB bus master.
 *************************************************************************/
void InitAudioDevice(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    DDS_Context contextNote1;
    DDS_Context contextNote2;
    DDS_Context contextNote3;
    DDS_Context contextNote4;
    DDS_Context contextNote5;

    DDS_initializeContext(&contextNote1, voice1, 16384 );


    // Audio Device Class
    UsbAudioClassInit();
    // Set STNBY pin
    GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Chip on
    GPIO_WriteBit(GPIOC, GPIO_Pin_3, Bit_RESET);

    // TODO Maybe the noteVoiceBuffer is redundant as it is replaced by the context

    // Note 0 is the metronome note
    notes[0].frequency       = 216; // Middle C
    notes[0].period          = 100; // TODO
    notes[0].noteVoiceBuffer = testSound; // Sinewave buffer
    notes[0].noteOn          = true; // TODO start with it on just for test

    // Note 1 maps to the first button.
    notes[1].frequency       = 216; // Middle C
    notes[1].period          = 100; // TODO
    notes[1].noteVoiceBuffer = voice1; // Sinewave buffer
    notes[1].noteOn          = true; // TODO start with it on just for test
    notes[1].context = contextNote1;

    DDS_setFrequency(&(notes[1].context), 216, 216, 8000);

    metronomePeriod          = 100;
    playNextFrame = true; // Kick off the audio playing
}

void metronome(void)
{
    static uint32_t previousSysTime;
    static bool metronomeUpBeat      = false;
    uint32_t deltaSysTime, currentSysTime;
    const uint32_t metronomeUpPeriod = 10;

    currentSysTime = systemTime();
    deltaSysTime = currentSysTime - previousSysTime;

    if (deltaSysTime > metronomePeriod)
    {
        notes[0].noteOn = true;
        metronomeUpBeat = true;
        previousSysTime = currentSysTime;
    }
    else
    {
        if (metronomeUpBeat && (deltaSysTime > metronomeUpPeriod))
        {
            notes[0].noteOn = false;
            metronomeUpBeat = false;
        }
    }
}

void ClearMixBuf(void)
{
  int i;
  for (i=0; i < SampPerFrame; i++)
  {
    mixBuffer[i] = 0;
  }
}

void MixFrame(int voice)
{
  int i;

    if (voice != 0)
    {
      if(notes[voice].noteOn)
      {
        for (i=0; i < SampPerFrame; i++)
        {
          mixBuffer[i] += DDS_nextSample(&(notes[voice].context));
        }
      }
      else
        {
          DDS_restart(&(notes[voice].context));
        }
    }
    else
    { /* Voice 0 is always the metronome */
      if(notes[voice].noteOn)
      {
        for (i=0; i < SampPerFrame; i++)
        {
          mixBuffer[i] += notes[voice].noteVoiceBuffer[i];
        }
      }
  }
}

void PlayFrame(void)
{
    AudioMemoryBufPlay(mixBuffer);
}
