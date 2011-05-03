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

// TODO make a playback interface
// that would allow the ioEventListener to step through the playback loop
// event by event

// reset the relative time and playback position
void startPlayback(void);

// playback the events in a loop
// each time through the loop
// fills ev with the current RecordedEvent,
// and also says how many ticks in the future the next one should be played
// returns false if no events recorded
bool getNextPlaybackEvent(RecordedEvent *ev, Ticks *delayBeforePlaying);

#endif
