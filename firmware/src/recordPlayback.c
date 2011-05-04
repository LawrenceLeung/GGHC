#include "jigbox.h"
#include "recordPlayback.h"


// hacky recorded event buffer
static const uint16_t maxRecordedEvents=100;
uint16_t lastRecordedEventIdx;
RecordedEvent recordedEvents[100];
Ticks recordStartTime;


void startRecording(Ticks metronomePeriod)
{
	lastRecordedEventIdx=-1;
	recordStartTime=systemTime;
	UART_printf("Start recording\r\n");
}

void addRecordedEvent(RecordedEvent *ev)
{
	lastRecordedEventIdx++;

	// todo: better failure
	if (lastRecordedEventIdx>=maxRecordedEvents){
		lastRecordedEventIdx=0;
	}
	memcpy(&recordedEvents[lastRecordedEventIdx],ev,sizeof(RecordedEvent));
}

void endRecording(void)
{

	UART_printf("End recording\r\n");
}

void quantizeRecording(void)
{
}


uint16_t playbackRecordedEventIdx;

// reset the relative time and playback position
void startPlayback(void)
{

	UART_printf("Start playback. Should have %d events\r\n",lastRecordedEventIdx);
	playbackRecordedEventIdx=0;
}

// playback the events in a loop
// each time through the loop
// fills ev with the current RecordedEvent,
// and also says how many ticks in the future the next one should be played
// returns false if no events recorded
bool getNextPlaybackEvent(RecordedEvent *ev, Ticks *delayBeforePlaying)
{

	if (lastRecordedEventIdx==0) {return false;}
	if (playbackRecordedEventIdx>lastRecordedEventIdx) {
		playbackRecordedEventIdx=0;
	}
	memcpy(ev,&recordedEvents[playbackRecordedEventIdx],sizeof(RecordedEvent));

	if (playbackRecordedEventIdx==lastRecordedEventIdx){
		*delayBeforePlaying=RECORDING_METRONOME_PERIOD;
	} else {
		*delayBeforePlaying=(recordedEvents[playbackRecordedEventIdx+1].quantizedTimestamp- ev->quantizedTimestamp);
	}


    playbackRecordedEventIdx++;
	return true;
}
