#ifndef __INCLUDED_RECORDPLAYBACK_H
#define __INCLUDED_RECORDPLAYBACK_H

#include "i2c.h"

// record/playback interface
//
// record events with times
// playback with quantization


typedef struct {
    Ticks performedTimestamp;
    Ticks quantizedTimestamp;
    TransientSource transient;
    AccelerometerReport_t accelValues;
} RecordedEvent;


void startRecording(Ticks metronomePeriod);

void addEvent(RecordedEvent *);

void endRecording(void);

void quantizeRecording(void);


#endif
