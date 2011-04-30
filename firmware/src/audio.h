#ifndef __AUDIO_H
#define __AUDIO_H
#include "DDS.h"

#define NUMBER_OF_NOTES 6 // Metronome plus 5 notes
#define WAV_SAMPLE_RATE 8000 // Want to change to 16000 but need to resample voices

void InitAudioDevice(void);
void metronome(void);
void SetMetronomePeriod(unsigned long newPeriod);
void PlayFrame(void);
void MixFrame(int voice);
void ClearMixBuf(void);

typedef struct oneNote
{
    bool noteOn; // True if note is currently being played
    audioBuf_t const * noteVoiceBuffer; // Points to the wave file
    float frequency; // May not need this as period is 1/frequency
    DDS_Context context;
} Note;

extern Note notes[NUMBER_OF_NOTES];
#endif //__AUDIO_H
