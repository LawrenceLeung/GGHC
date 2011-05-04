#ifndef __included_voices_h
#define __included_voices_h

#include <stdint.h>

typedef int16_t audioBuf_t;

typedef struct {
    audioBuf_t const *voice_data;
    uint16_t voice_nsamples;
    const char *voice_name;
} voice_t;

extern voice_t allVoices[8];
#define N_VOICES 8

#define METRONOME_VOICE 5	// snare
#define DEFAULT_VOICE 0

// File:            bigmaraca.wav
// Channels:        1
// Sample rate:     16000
// Bits per sample: 16
// Block align:     2
// Byte rate:       32000
// Sample count:    2875
// Duration:        0h:0m:0s:179ms
extern audioBuf_t const voice_bigmaraca[2875];

// File:            ./bark.wav
// Channels:        1
// Sample rate:     16000
// Bits per sample: 16
// Block align:     2
// Byte rate:       32000
// Sample count:    3509
// Duration:        0h:0m:0s:219ms
extern audioBuf_t const voice_bark[3509];

// File:            ./berimbau.wav
// Channels:        1
// Sample rate:     16000
// Bits per sample: 16
// Block align:     2
// Byte rate:       32000
// Sample count:    4274
// Duration:        0h:0m:0s:267ms
extern audioBuf_t const voice_berimbau[4274];

// File:            ./clave.wav
// Channels:        1
// Sample rate:     16000
// Bits per sample: 16
// Block align:     2
// Byte rate:       32000
// Sample count:    261
// Duration:        0h:0m:0s:16ms
extern audioBuf_t const voice_clave[261];

// File:            ./cowbell.wav
// Channels:        1
// Sample rate:     16000
// Bits per sample: 16
// Block align:     2
// Byte rate:       32000
// Sample count:    2805
// Duration:        0h:0m:0s:175ms
extern audioBuf_t const voice_cowbell[2805];

// File:            ./rissetDrum.wav
// Channels:        1
// Sample rate:     16000
// Bits per sample: 16
// Block align:     2
// Byte rate:       32000
// Sample count:    8001
// Duration:        0h:0m:0s:500ms
extern audioBuf_t const voice_rissetDrum[8001];

// File:            ./snare.wav
// Channels:        1
// Sample rate:     16000
// Bits per sample: 16
// Block align:     2
// Byte rate:       32000
// Sample count:    2239
// Duration:        0h:0m:0s:139ms
extern audioBuf_t const voice_snare[2239];

// File:            ./zithrhit.wav
// Channels:        1
// Sample rate:     16000
// Bits per sample: 16
// Block align:     2
// Byte rate:       32000
// Sample count:    7264
// Duration:        0h:0m:0s:454ms
extern audioBuf_t const voice_zithrhit[7264];

// Total bytes: 56706
#endif
