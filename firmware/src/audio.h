#ifndef __AUDIO_H
#define __AUDIO_H

#define NUMBER_OF_NOTES 6 // Metronome plus 5 notes

void InitAudioDevice(void);
void metronome(void);
void SetMetronomePeriod(unsigned long newPeriod);

typedef struct oneNote
{
    bool noteOn; // True if note is currently being played
    int16_t * noteVoiceBuffer; // Points to the wave file
    uint16_t frequency; // May not need this as period is 1/frequency
    uint16_t period; // In sample ticks
} Note;

extern Note notes[NUMBER_OF_NOTES];
#endif //__AUDIO_H
