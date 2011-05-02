#ifndef __AUDIO_H
#define __AUDIO_H
#include "DDS.h"
#include "voices.h"

#define NUMBER_OF_NOTES 11 // Metronome plus 5 notes for record and 5 for playback
#define WAV_SAMPLE_RATE 8000 // Want to change to 16000 but need to resample voices
#define SampFreq 16000

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
    uint32_t noteVoiceSize; // Number of elements in buffer above
    float frequency; //Relative to frequency of voice sample
    uint32_t volume;
    DDS_Context DDScontext;
    bool continuous;
} Note;

extern Note notes[NUMBER_OF_NOTES];
#endif //__AUDIO_H

typedef uint8_t noteContext_t;
typedef enum
{
	Single,
	Continuous
} noteStyle_t;

void startNote(noteContext_t note, noteStyle_t style, uint16_t attenuation);
void stopNote(noteContext_t note);
noteContext_t defineNote(float frequency, audioBuf_t * voiceBuffer, uint32_t voiceBufferSize);
