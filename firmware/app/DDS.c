typedef union {
	uint32_t full;
	struct {
		uint16_t fraction;
		uint16_t integer;
	} fixedPoint;
} DDS_FixedPoint;

typedef struct {
	int16_t *source;		// sampled waveform
	uint16_t sourceSize;	// number of samples in source MUST BE power of 2
	uint16_t sourceIndexMask;	
	DDS_FixedPoint accumulator;	// 16.16 pointer 
	DDS_FixedPoint stepSize;	// inverse of frequency
} DDS_Context;

int16_t nextSample(DDS_Context *context)
{
	// round index to nearest sample index
	DDS_FixedPoint roundedIndex = context->accumulator;
	roundedIndex.full += 0x00008000;
	// return value is indexed sample
	int16_t retval = context->source[ roundedIndex.fixedPoint.integer ];
	// update index
	context->accumulator.full += context->stepSize.full;
	// wrap at end of source buffer
	// in other words:
	// context->accumulator.fixedPoint.integer %= context->sourceSize;
	context->accumulator.fixedPoint.integer &= context->sourceIndexMask;
	return retval;
}
