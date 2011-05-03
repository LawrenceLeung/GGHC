#ifndef __AUDIO_H
#define __AUDIO_H
#include "DDS.h"
#include "voices.h"

#define NUMBER_OF_NOTES 11 // Metronome plus 5 notes for record and 5 for playback

#define SAMPLE_FREQUENCY 16000

#define METRONOME_VOICE 5   // preferred voice index into allVoices[] for metronome

void InitAudioDevice(void);
void metronome(void);
void SetMetronomePeriod(unsigned long newPeriod);
void PlayFrame(void);
void MixFrame(int voice);
void ClearMixBuf(void);

typedef struct
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

typedef uint8_t noteContext_t;

typedef enum
{
	Single,
	Continuous
} noteStyle_t;

noteContext_t startNote(float pitch, uint8_t voice, noteStyle_t style, uint16_t attenuation);
void stopNote(noteContext_t note);

#endif //__AUDIO_H
