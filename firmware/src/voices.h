#ifndef __VOICES_H
#define __VOICES_H


#define VOICE_SIZE 16384
#define NUMBER_OF_VOICES 2

typedef int16_t audioBuf_t;

/* If adding voices here, change the NUMBER_OF_VOICES above and the
 * allVoices array.
 */
extern const audioBuf_t voice1[VOICE_SIZE];
extern const audioBuf_t voice2[VOICE_SIZE];

#endif //__AUDIO_H
