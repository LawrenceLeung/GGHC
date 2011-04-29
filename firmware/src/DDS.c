#include <DDS.h>
#include <math.h>

void DDS_initializeContext(DDS_Context *context, audioBuf_t const * _src, int16_t _srcSize)
{
    context->source = _src;
    context->sourceSize = _srcSize;
#if DDS_BUFFERS_POWER_OF_TWO
    context->sourceIndexMask = _srcSize - 1;
#endif
    context->accumulator.full = 0;
    context->stepSize.full = 1 << 16;
}

// Given a desired frequency and a sample rate, set the stepSize to the proper value.
void DDS_setFrequency(DDS_Context *context, float originalFrequency, float desiredFrequency, float sampleRate)
{
    // step size = Fdesired/Foriginal
    float fstep = (desiredFrequency * 65536.0) / originalFrequency;
    context->stepSize.full = (uint32_t)fstep;
}

// Return the next sample for the given context.
audioBuf_t DDS_nextSample(DDS_Context *context)
{
    // round index to nearest sample index
    DDS_FixedPoint roundedIndex = context->accumulator;
    roundedIndex.full += 0x00008000;

    // return value is indexed sample
    int16_t retval = context->source[ roundedIndex.fixedPoint.integer ];
    // update index
    context->accumulator.full += context->stepSize.full;
    // wrap at end of source buffer
#if DDS_BUFFERS_POWER_OF_TWO
    context->accumulator.fixedPoint.integer &= context->sourceIndexMask;
#else
    context->accumulator.fixedPoint.integer %= context->sourceSize;
#endif
    return retval;
}

void DDS_restart(DDS_Context *context)
{
  context->accumulator.full = 0;
}
