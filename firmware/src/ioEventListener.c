#include "jigbox.h"
#include "recordPlayback.h"
Q_DEFINE_THIS_FILE

typedef struct
{
    QActive super;
    QTimeEvt tickEvt;
    QTimeEvt accelLEDOffEvt;
    QTimeEvt metronomeEvt;
    QTimeEvt playbackEvt;
    Ticks lastHitTime;
    TransientSource lastTransientSource;
    uint8_t buttonState;
	uint8_t currentVoice;
} IOEventListener;

static IOEventListener l_eventListener; // the sole instance of the IOEventListener active Object
QActive * const AO_IOEventListener = (QActive*)&l_eventListener; // opaque pointer to that instance

static QState IOEventListener_initial(IOEventListener *me, QEvent const *e);
static QState IOEventListener_active(IOEventListener *me, QEvent const *e);
static QState IOEventListener_idle(IOEventListener *me, QEvent const *e);
static QState IOEventListener_recording(IOEventListener *me, QEvent const *e);
static QState IOEventListener_playing(IOEventListener *me, QEvent const *e);

// UTILITY FUNCTIONS
//
// count the bits set in a word
static uint8_t bitsSetIn(uint16_t w)
{
    uint8_t nbits = 0;
    while (w)
    {
        if (w & 1)
            nbits++;
        w >>= 1;
    }
    return nbits;
}

// change currentVoice
static void handleButtonPressed(IOEventListener *me, uint8_t buttonMask)
{
	me->buttonState |= buttonMask;

	UART_printf("press 0x%04x now 0x%04x\r\n", buttonMask, me->buttonState);
	for (int i = 0; buttonMask; i++, buttonMask >>= 1)
	{
		if (buttonMask & 1)
		{
			me->currentVoice = i;
			return;
		}
	}
}

// log a message, play a sound, and light the LED.
static void handleHitEvent(IOEventListener *me, HitEvent *ev, uint8_t voice)
{
    UART_printf("%08d hit tr=0x%02x",
                me->lastHitTime,
                (int)*(uint8_t*)&(ev->transient));

    if (ev->transient.X_Trans_Evt)
        UART_printf(" X = %d", ev->xyz.x);
    else
        ev->xyz.x = 0;

    if (ev->transient.Y_Trans_Evt)
        UART_printf(" Y = %d", ev->xyz.y);
    else
        ev->xyz.y = 0;

    if (ev->transient.Z_Trans_Evt)
        UART_printf(" Z = %d", ev->xyz.z);
    else
        ev->xyz.z = 0;

    UART_printString("\r\n");

    /* flash the HIT_FLASH_LED */
    RGB_LED_On(HIT_FLASH_LED, ev->xyz.x, ev->xyz.y, ev->xyz.z);

    QTimeEvt_disarm(&me->accelLEDOffEvt);
    QTimeEvt_postIn(&me->accelLEDOffEvt, (QActive*)me, HIT_FLASH_TIME);

	startNote(1.0, voice, Single, HIT_SOUND_ATTENUATION);
}

void IOListener_ctor(void)
{
    IOEventListener *me = &l_eventListener;
    QTimeEvt_ctor(&me->tickEvt, IOE_TICK_SIG);
    QTimeEvt_ctor(&me->accelLEDOffEvt, IOE_ACCEL_LED_OFF_SIG);
    QTimeEvt_ctor(&me->metronomeEvt, IOE_METRONOME_SIG);
    QTimeEvt_ctor(&me->playbackEvt, IOE_PLAYBACK_SIG);
    QActive_ctor(&me->super, (QStateHandler)&IOEventListener_initial);
    me->buttonState = 0;
    me->lastHitTime=0;
	me->currentVoice = DEFAULT_VOICE;
}

QState IOEventListener_initial(IOEventListener *me, QEvent const *e)
{
    return Q_TRAN(&IOEventListener_active);
}

QState IOEventListener_active(IOEventListener *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
            QTimeEvt_postEvery(&me->tickEvt,(QActive*)me, 1000); // 1 per second
            return Q_HANDLED();

        case Q_EXIT_SIG:
            QTimeEvt_disarm(&me->tickEvt);
            return Q_HANDLED();

        case Q_INIT_SIG:
            return Q_TRAN(&IOEventListener_idle);

        case BUTTON_PRESSED_SIG:
        {
		// must call handleButtonPressed() earlier
            // switch modes if necessary
            // TODO find a good way to signal going into record mode
            uint8_t nbits = bitsSetIn(me->buttonState);
            if (nbits >= 3)
            {
                QEvent *modeSwitchEvt = Q_NEW(QEvent, IOE_MODE_SWITCH_SIG);
                QActive_postFIFO((QActive*)me, modeSwitchEvt);
            }
            return Q_HANDLED();
        }

        case BUTTON_RELEASED_SIG:
            me->buttonState &= ~((ButtonEvent*)e)->buttonMask;
            return Q_HANDLED();

        case IOE_TICK_SIG:
        {
            AccelerometerReport_t report;
            if (readAccelerometer(&report))
            {
                UART_printf("accel x=%d y=%d z=%d\r\n", report.x, report.y, report.z);
            }
            else
            {
                UART_printf("can't read accel!\r\n");
            }
            return Q_HANDLED();
        }

        case HIT_SIG:
        {
        	// dedupe subsequent hits shortly after the first
			if (me->lastHitTime > 0 && (systemTime - me->lastHitTime) < HIT_DEDUPE_MS)
				return Q_IGNORED();

            HitEvent *ev = (HitEvent*)e;
    		me->lastHitTime         = systemTime;
    		me->lastTransientSource = ev->transient;
			handleHitEvent(me, ev, me->currentVoice);
            return Q_HANDLED();
        }

        case IOE_ACCEL_LED_OFF_SIG:
            RGB_LED_On(HIT_FLASH_LED, 0, 0, 0);
            return Q_HANDLED();
    }
    return Q_SUPER(&QHsm_top);
}

QState IOEventListener_idle(IOEventListener *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
            return Q_HANDLED();

        case Q_EXIT_SIG:
            return Q_HANDLED();

        case BUTTON_PRESSED_SIG:
			handleButtonPressed(me, ((ButtonEvent*)e)->buttonMask);
			startNote(1.0, me->currentVoice, Single, BUTTON_SOUND_ATTENUATION);
    		return Q_SUPER(&IOEventListener_active);

        case IOE_MODE_SWITCH_SIG:
            return Q_TRAN(&IOEventListener_recording);
    }
    return Q_SUPER(&IOEventListener_active);
}

QState IOEventListener_recording(IOEventListener *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
            QTimeEvt_postEvery(&me->metronomeEvt,(QActive*)me, RECORDING_METRONOME_PERIOD);
            startRecording(RECORDING_METRONOME_PERIOD);
            return Q_HANDLED();

        case Q_EXIT_SIG:
            endRecording();
            QTimeEvt_disarm(&me->metronomeEvt);
            return Q_HANDLED();

        case BUTTON_PRESSED_SIG:
			handleButtonPressed(me, ((ButtonEvent*)e)->buttonMask);
    		return Q_SUPER(&IOEventListener_active);

        case IOE_METRONOME_SIG:
            startNote(1.0, METRONOME_VOICE, Single, 12);
            return Q_HANDLED();

        case IOE_MODE_SWITCH_SIG:
            return Q_TRAN(&IOEventListener_playing);

        case HIT_SIG:
            {
                HitEvent *hit = (HitEvent*)e;
                RecordedEvent rec = {
                    .performedTimestamp = systemTime,
                    .quantizedTimestamp = systemTime,
                    .transient = hit->transient,
                    .accelValues = hit->xyz,
                	.currentVoice = me->currentVoice
                	};
                // record the event
                addRecordedEvent(&rec);
				// now let the parent state flash the LED and play the sound
				return Q_SUPER(&IOEventListener_active);
            }

    }
    return Q_SUPER(&IOEventListener_active);
}

QState IOEventListener_playing(IOEventListener *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
            {
                startPlayback();
                QTimeEvt_disarm(&me->playbackEvt);
                QTimeEvt_postIn(&me->playbackEvt, (QActive*)me, 1);
            }
            return Q_HANDLED();

        case Q_EXIT_SIG:
            QTimeEvt_disarm(&me->playbackEvt);
            return Q_HANDLED();

        case BUTTON_PRESSED_SIG:
			handleButtonPressed(me, ((ButtonEvent*)e)->buttonMask);
    		return Q_SUPER(&IOEventListener_active);

        case IOE_MODE_SWITCH_SIG:
            return Q_TRAN(&IOEventListener_idle);

        case IOE_PLAYBACK_SIG:
            {
                RecordedEvent revt;	// current event
                Ticks when;			// delay till next one
                if (getNextPlaybackEvent(&revt, &when))
                {
                    QTimeEvt_disarm(&me->playbackEvt);
                    QTimeEvt_postIn(&me->playbackEvt, (QActive*)me, when);
                }

				HitEvent hitEvt = {
                	.transient = revt.transient,
                	.xyz = revt.accelValues };
				handleHitEvent(me, &hitEvt, revt.currentVoice);
            	return Q_HANDLED();
            }

        case TIME_TICK_SIG:
            return Q_HANDLED();
    }
    return Q_SUPER(&IOEventListener_active);
}
