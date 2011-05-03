#include "jigbox.h"
#include "recordPlayback.h"

void startRecording(Ticks metronomePeriod)
{
}

void addEvent(RecordedEvent *ev)
{
}

void endRecording(void)
{
}

void quantizeRecording(void)
{
}

// reset the relative time and playback position
void startPlayback(void)
{
}

// playback the events in a loop
// each time through the loop
// fills ev with the current RecordedEvent,
// and also says how many ticks in the future the next one should be played
// returns false if no events recorded
bool getNextPlaybackEvent(RecordedEvent *ev, Ticks *delayBeforePlaying)
{
	return true;
}
