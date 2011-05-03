/*************************************************************************
 *
 *    History :
 *       @author      : Alan Widmer
 **************************************************************************/
#include "jigbox.h"
#include "voices.h"


Note notes[NUMBER_OF_NOTES];

static uint32_t const VolumeMul[] =
{
    65536, // 0dB
    58409, //-1dB
    52057, //-2dB
    46396, //-3dB
    41350, //-4dB
    36854, //-5dB
    32846, //-6dB
    29274, //-7dB
    26090, //-8dB
    23253, //-9dB
    20724, //-10dB
    18471, //-11dB
    16462, //-12dB
    14672, //-13dB
    13076, //-14dB
    11654, //-15dB
    10387, //-16dB
    9257, //-17dB
    8250, //-18dB
    7353, //-19dB
    6554, //-20dB
    5841, //-21dB
    5206, //-22dB
    4640, //-23dB
    4135, //-24dB
    3685, //-25dB
    3285, //-26dB
    2927, //-27dB
    2609, //-28dB
    2325, //-29dB
    2072, //-30dB
    1847, //-31dB
    1646, //-32dB
    1467, //-33dB
    1308, //-34dB
    1165, //-35dB
    1039, //-36dB
    926, //-37dB
    825, //-38dB
    735, //-39dB
    655, //-40dB
    584, //-41dB
    521, //-42dB
    464, //-43dB
    414, //-44dB
    369, //-45dB
    328, //-46dB
    293, //-47dB
    261, //-48dB
};

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
    /* Initialize DDS for all the notes */
    for (int i=0; i < NUMBER_OF_NOTES; i++)
    {
        notes[i].noteOn = false;
    }
}

/** startNote
 * @brief Commands a note to start playing immediately
 * @param pitch the ratio between the recorded rate and the playback rate
 * @param voice The index into the pre-defined allVoices array
 * @param noteStyle If set to Single, play the voice buffer completely one time.
 *  If set to Continuous, play the voice buffer repeatedly until the stopNote()
 *  function is called.
 *  @param attenuation A value in db from 0 to 30 that sets the attenuation in
 *  dB for the note to be played. 0db means play the voice buffer at the volume
 *  it was sampled at.
 */
noteContext_t startNote(float pitch, uint8_t voice, noteStyle_t style, uint16_t attenuation)
{
    int i = 0;
    /* Find the next free slot in the notes array */
    while ((notes[i].noteOn) && (i < NUMBER_OF_NOTES))
    {
        i++;
    }
    assert(i < NUMBER_OF_NOTES);
    assert(attenuation < sizeof(VolumeMul)/sizeof(VolumeMul[0]));

    notes[i].noteOn          = true;
    notes[i].volume          = VolumeMul[attenuation];
    notes[i].noteVoiceBuffer = allVoices[voice].voice_data;
    DDS_initializeContext(&(notes[i].DDScontext), allVoices[voice].voice_data, allVoices[voice].voice_nsamples);
    DDS_setFrequency(&(notes[i].DDScontext), 1.0f, pitch, SAMPLE_FREQUENCY);
    notes[i].continuous = (style == Continuous);

    return i;
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

static union
{
    int32_t Data;
    struct
    {
        uint16_t DataLo;
        int16_t DataHi;
    };
} Val = {0x02000000};

bool AudioFeat1Mute = false;

void TIM2_IRQHandler(void)
{
    static bool endOfBuf;
    for (int note = 0; note < NUMBER_OF_NOTES; note++)
    {
        if (notes[note].noteOn)
        {
            Val.Data += (uint32_t)DDS_nextSample(&(notes[note].DDScontext),
                                                 &endOfBuf)
                        * notes[note].volume;
            if(endOfBuf && !notes[note].continuous)
            {
                /* If the note is to be played only once and the end of buffer
                   is reached then turn it off */
                notes[note].noteOn = false;
                DDS_restart(&(notes[note].DDScontext));
            }
        }
    }

    if(!AudioFeat1Mute)
    {
        // Add offset and scale back to 10bit
        Val.DataHi  = Val.DataHi / 64;
        Val.DataHi += 0x200;           // Check for overflow and correct value
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


