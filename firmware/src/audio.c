/*************************************************************************
 *
 *    History :
 *       @author      : Alan Widmer
 **************************************************************************/
#include "jigbox.h"

#include "voices.h"

#define LOOP_DLY_100US 450

#define SampPerFrame 48 //TODO Obsolete
Note notes[NUMBER_OF_NOTES];

static uint32_t metronomePeriod;

const uint32_t VolumeMul [] =
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
    65536
};

const int16_t volumeMulSize = sizeof(VolumeMul) / sizeof(uint32_t);


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
    DDS_Context contextNote1, contextNote2, contextNote3, contextNote4;
    DDS_Context contextNote5, contextNote6, contextNote7, contextNote8;
    DDS_Context contextNote9, contextNote10;

    const float defaultFrequencies[NUMBER_OF_NOTES] = {
        261.626,
        261.626, 311.127, 349.228, 391.995, 466.164,
        261.626, 311.127, 349.228, 391.995, 466.164
    };

    const DDS_Context * defaultDDS[NUMBER_OF_NOTES] = {
        &contextNote0, &contextNote1, &contextNote2, &contextNote3,
        &contextNote4, &contextNote5, &contextNote6, &contextNote7,
        &contextNote8, &contextNote9, &contextNote10
    };

    const audioBuf_t const * defaultVoice[NUMBER_OF_NOTES] = {
        voice2, voice2, voice2, voice2, voice2, voice2,
        voice1, voice1, voice1, voice1, voice1
    };

    /* Initialize DDS for all the notes and the metronome (note 0) */
    for(int i=0; i < NUMBER_OF_NOTES; i++)
      {
        notes[i].frequency = defaultFrequencies[i];
        notes[i].DDScontext = * defaultDDS[i];
        notes[i].noteVoiceBuffer = defaultVoice[i];
        DDS_initializeContext(&(notes[i].DDScontext), notes[i].noteVoiceBuffer, VOICE_SIZE );
        DDS_setFrequency(&(notes[i].DDScontext), 261.626,
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

    currentSysTime = systemTime;
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

/** startNote
 * @brief Commands a note to start playing immediately
 * @param note The note context previously returned from the defineNote() function
 * @param noteStyle If set to single, play the voice buffer completely one time.
 *  If set to continuous, play the voice buffer repeatedly until the stopNote()
 *  function is called.
 *  @param attenuation A value in db from 0 to 30 that sets the volume for the
 *  note to be played. 0db means play the voice buffer at the volume it was
 *  sampled at.
 */
void startNote(noteContext_t note, noteStyle_t style, uint16_t attenuation)
{

  notes[note].noteOn = true;
  /* VolumeMul array is arranged from quietest to loudest but we are using
   * a parameter to specify attenuation so we need to index the array starting
   * at the end.
   */
  notes[note].volume = VolumeMul[(volumeMulSize - attenuation) - 1];
  switch (style)
  {
  case Continuous:
    notes[note].continuous = true;
    break;
  default:
    notes[note].continuous = false;
  }
}

/** stopNote
 * Immediately stop playing a note and reset to the start of the voice buffer.
 * The works regardless of whether the note style in startNote() function was
 * single or continuous
 */
void stopNote(noteContext_t note)
{
  DDS_restart(&(notes[note].DDScontext));
  notes[note].noteOn = false;
}


noteContext_t defineNote(float frequency, audioBuf_t * voiceBuffer, uint32_t voiceBufferSize)
{
  static noteContext_t nextNote = 1;
  notes[nextNote].frequency = frequency;
  notes[nextNote].noteVoiceBuffer = voiceBuffer;
  notes[nextNote].noteVoiceSize = voiceBufferSize;
  nextNote++;
  assert(nextNote <= NUMBER_OF_NOTES);
  return nextNote;
}

#include "jigbox.h"

static union _Val
{
    int32_t Data;
    struct
    {
        uint16_t DataLo;
        int16_t DataHi;
    };
}
Val = {0x02000000};

bool AudioFeat1Mute = false;

void TIM2_IRQHandler(void)
{
  static bool endOfBuf;
  for (int voice = 0; voice < NUMBER_OF_NOTES; voice++)
    {
      if (notes[voice].noteOn)
        {
          Val.Data += (uint32_t)DDS_nextSample(&(notes[voice].DDScontext),
              &endOfBuf)
              * notes[voice].volume;
          if(endOfBuf && !notes[voice].continuous)
            { /* If the note is to be played only once and the end of buffer
                 is reached then turn it off */
              notes[voice].noteOn = false;
              DDS_restart(&(notes[voice].DDScontext));
            }
        }
    }

  if(!AudioFeat1Mute)
  {
    // Add offset and scale back to 10bit

    Val.DataHi = Val.DataHi / 64;
    Val.DataHi += 0x200;    // Check for overflow and correct value
    if(Val.DataHi < 50) // Near 0
    {
            Val.DataHi = 50;
    }
    else if(Val.DataHi > 975) // Near 10 bit limit
    {
            Val.DataHi = 975;
    }
  }
  else
  {
    // set middle of range
    Val.DataHi = 0x200; // Half way up 10 bit range
  }


  // reload PWM
  TIM1->CCR1 =  Val.DataHi;

// TODO OLD METHOD  TIM2->SR &= (uint16_t) ~TIM_FLAG_Update;
  TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);


}


