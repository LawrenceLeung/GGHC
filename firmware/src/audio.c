/*************************************************************************
 *
 *    History :
 *       @author      : Alan Widmer
 **************************************************************************/
#include "jigbox.h"
#include "audio_class.h"
#include "voices.h"

#define LOOP_DLY_100US 450

Note notes[NUMBER_OF_NOTES];

static uint32_t metronomePeriod;

audioBuf_t metronomeVoice[SampPerFrame] = {
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
    DDS_Context contextNote0;
    DDS_Context contextNote1;
    DDS_Context contextNote2;
    DDS_Context contextNote3;
    DDS_Context contextNote4;
    DDS_Context contextNote5;

    // Reset STNBY pin: done by unsoldering HP_SB_E jumper

    // Note 0 is the metronome note
    notes[0].frequency       = 261.626; // Middle C
    notes[0].noteVoiceBuffer = metronomeVoice; // Sinewave buffer
    notes[0].context = contextNote0; // TODO not yet used
    notes[0].noteOn = false;

    // Note 1 maps to the first button.
    notes[1].frequency       = 261.626; // Middle C
    notes[1].noteVoiceBuffer = voice1;
    notes[1].context = contextNote1;

    // Note 2 maps to the first button.
    notes[2].frequency       = 311.127; // Middle E-flat
    notes[2].noteVoiceBuffer = voice1;
    notes[2].context = contextNote2;

    // Note 3 maps to the first button.
    notes[3].frequency       = 349.228; // Middle F
    notes[3].noteVoiceBuffer = voice1;
    notes[3].context = contextNote3;

    // Note 4 maps to the first button.
    notes[4].frequency       = 391.995; // Middle G
    notes[4].noteVoiceBuffer = voice1; // Sinewave buffer
    notes[4].context = contextNote4;

    // Note 4 maps to the first button.
    notes[5].frequency       = 466.164; // Middle G
    notes[5].noteVoiceBuffer = voice1; // Sinewave buffer
    notes[5].context = contextNote5;

    /* Initialize DDS for the notes but not the metronome (note 0) */
    for(int i=1; i < NUMBER_OF_NOTES; i++)
      {
        DDS_initializeContext(&(notes[i].context), notes[i].noteVoiceBuffer, VOICE_SIZE );
        DDS_setFrequency(&(notes[i].context), 261.626,
            notes[i].frequency, WAV_SAMPLE_RATE);
        notes[i].noteOn = false;
      }


    metronomePeriod = 100;
  //  playNextFrame   = true; // Kick off the audio playing
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
