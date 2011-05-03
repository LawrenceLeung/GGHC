#ifndef __INCLUDED_DDS_H
#define __INCLUDED_DDS_H

#include <stdint.h>
#include <stdbool.h>
#include "voices.h"

// define this as non-zero if your buffers will all be a power of 2 samples long.
#ifndef DDS_BUFFERS_POWER_OF_TWO
#   define DDS_BUFFERS_POWER_OF_TWO 0
#endif

// Representation of our accumulator and step values
typedef union
{
    uint32_t full;
    struct
    {
        uint16_t fraction;
        uint16_t integer;
    } fixedPoint;
} DDS_FixedPoint;

// Context for a single DDS channel
typedef struct
{
	audioBuf_t const * source;                   // sampled waveform
    uint16_t sourceSize;               // number of samples in source MUST BE power of 2
#if DDS_BUFFERS_POWER_OF_TWO
    uint16_t sourceIndexMask;          // word with low N bits set
#endif
    DDS_FixedPoint accumulator;        // 16.16 pointer
    DDS_FixedPoint stepSize;           // inverse of frequency
} DDS_Context;

// Set up a DDS_Context for the given source.
void DDS_initializeContext(DDS_Context *context, audioBuf_t const *_src, int16_t _srcSize);

// Given a desired frequency and a sample rate, set the stepSize to the proper value.
void DDS_setFrequency(DDS_Context *context, float originalFrequency, float desiredFrequency, float sampleRate);

// Return the next sample for the given context.
audioBuf_t DDS_nextSample(DDS_Context *context, bool * isEndOfBuf);

// Reset the context back to the start of the source buffer
void DDS_restart(DDS_Context *context);

#endif
