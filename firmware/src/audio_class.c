#include "jigbox.h"
#include "audio_class.h"

__IO bool SempEna,MicEna;
uint32_t MicCurrBuffer;
__IO int16_t * pSpkData,pMicData;
__IO uint32_t Delta,MicSempCount,SempCount;

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

int16_t AudioSpkData[SampPerFrame * 3];

int16_t AudioMicData1[SampPerFrame * 2];
int16_t AudioMicData2[SampPerFrame * 2];

uint8_t AudioBuf[2];

uint8_t AudioRequest,AudioCS,AudioCN,AudioId;
uint16_t AudioDataSize;

int16_t AudioFeat1Vol;
uint32_t AudioSpkVolMul = 50;  /* TODO, invent a volume control scheme */
bool AudioFeat1Mute;

int16_t AudioFeat2Vol;
uint32_t AudioMicVolMul;
bool AudioFeat2Mute;

void TIM2_IRQHandler(void)
{
    if (SempEna)
    {
        if(Delta > 1)
        {
            // Get next input sample
            Val.Data = AudioSpkData[SempCount++];
            Delta   -= 2;
            if (Delta < SampPerFrame) // There is room for one more frame
            {
                playNextFrame = true;
            }
            // Correct pointer to sample buffer
            if(SempCount >= sizeof(AudioSpkData)/sizeof(uint16_t))
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
            SempEna         = false;
        }
    }

    // reload PWM
    TIM1->CCR1 =  Val.DataHi;

    if(MicEna)
    {
        // TODO
    }

    TIM2->SR &= (uint16_t) ~TIM_FLAG_Update;
}


