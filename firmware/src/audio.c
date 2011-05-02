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

#ifdef TwelveBit
audioBuf_t metronomeVoice[SampPerFrame] = {
    0, 267, 529, 783, 1023, 1246, 1447, 1623, 1772, 1891, 1977, 2029, 2047,
    2029, 1977, 1891, 1772, 1623, 1447, 1246, 1023, 783, 529, 267, 0, -268,
    -530, -784, -1024, -1247, -1448, -1624, -1773, -1892, -1978, -2030, -2047,
    -2030, -1978, -1892, -1773, -1624, -1448, -1247, -1024, -784, -530, -268
};
#endif

#ifdef TODO
audioBuf_t metronomeVoice[SampPerFrame] =
  { 0, 4276, 8480, 12539, 16383, 19947, 23169, 25995, 28377, 30272, 31650,
      32486, 32767, 32486, 31650, 30272, 28377, 25995, 23169, 19947, 16383,
      12539, 8480, 4276, 0, -4277, -8481, -12540, -16384, -19948, -23170,
      -25996, -28378, -30273, -31651, -32487, -32767, -32487, -31651, -30273,
      -28378, -25996, -23170, -19948, -16384, -12540, -8481, -4277 };
#endif
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
    DDS_Context contextNote1;
    DDS_Context contextNote2;
    DDS_Context contextNote3;
    DDS_Context contextNote4;
    DDS_Context contextNote5;

    // Reset STNBY pin: done by unsoldering HP_SB_E jumper

    // Note 0 is the metronome note
    notes[0].frequency       = 261.626; // Middle C
    notes[0].noteVoiceBuffer = voice2; // Sinewave buffer
    notes[0].DDScontext = contextNote0;
    notes[0].noteOn = false;

    // Note 1 maps to the first button.
    notes[1].frequency       = 261.626; // Middle C
    notes[1].noteVoiceBuffer = voice1;
    notes[1].DDScontext = contextNote1;

    // Note 2 maps to the first button.
    notes[2].frequency       = 311.127; // Middle E-flat
    notes[2].noteVoiceBuffer = voice1;
    notes[2].DDScontext = contextNote2;

    // Note 3 maps to the first button.
    notes[3].frequency       = 349.228; // Middle F
    notes[3].noteVoiceBuffer = voice1;
    notes[3].DDScontext = contextNote3;

    // Note 4 maps to the first button.
    notes[4].frequency       = 391.995; // Middle G
    notes[4].noteVoiceBuffer = voice1; // Sinewave buffer
    notes[4].DDScontext = contextNote4;

    // Note 4 maps to the first button.
    notes[5].frequency       = 466.164; // Middle G
    notes[5].noteVoiceBuffer = voice1; // Sinewave buffer
    notes[5].DDScontext = contextNote5;

    /* Initialize DDS for the notes but not the metronome (note 0) */
    for(int i=0; i < NUMBER_OF_NOTES; i++)
      {
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

void ClearMixBuf(void)
{
  int i;
  for (i=0; i < SampPerFrame; i++)
  {
    mixBuffer[i] = 0;
  }
}

#ifdef TODO /* obsolete */
void MixFrame(int voice)
{
  int i;

    if (voice != 0)
    {
      if(notes[voice].noteOn)
      {
        for (i=0; i < SampPerFrame; i++)
        {
          mixBuffer[i] += DDS_nextSample(&(notes[voice].DDScontext));
        }
      }
      else
        {
          DDS_restart(&(notes[voice].DDScontext));
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
#endif

#ifdef TODO
void PlayFrame(void)
{
    AudioMemoryBufPlay(mixBuffer);
}
#endif


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


//__IO bool SempEna,MicEna;

__IO int16_t * pSpkData;
__IO uint32_t SampCount;

__IO bool playNextFrame = false;

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
int16_t AudioSpkData[SampPerFrame * 3];

uint32_t AudioSpkVolMul = 50;  /* TODO, invent a volume control scheme */
bool AudioFeat1Mute = false;

void TIM2_IRQHandler(void)
{

  for (int voice = 0; voice < NUMBER_OF_NOTES; voice++)
    {
      if (notes[voice].noteOn)
        {
          Val.Data += (uint32_t)DDS_nextSample(&(notes[voice].DDScontext))
              * notes[voice].volume;
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


