#include "jigbox.h"
#include "recordPlayback.h"
Q_DEFINE_THIS_FILE

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

// TODO just for testing map pressed/released buttons
static void startOrStopSelectedNotes(uint8_t buttons, bool start)
{
    static noteContext_t startedNotes[8] = { NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE, NO_NOTE };

    for (uint8_t i = 0; buttons; i++, buttons>>=1)
    {
        if (buttons & 1)
        {
            if (start)
            {
                startedNotes[i] = startNote(1.0, i, Single, BUTTON_SOUND_ATTENUATION);
            }
            else
            {
                noteContext_t n = startedNotes[i];
                if (n != NO_NOTE)
                {
                    stopNote(n);
                    startedNotes[i] = NO_NOTE;
                }
            }
        }
    }
}

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
} IOEventListener;

static IOEventListener l_eventListener; // the sole instance of the IOEventListener active Object
QActive * const AO_IOEventListener = (QActive*)&l_eventListener; // opaque pointer to that instance

static QState IOEventListener_initial(IOEventListener *me, QEvent const *e);
static QState IOEventListener_active(IOEventListener *me, QEvent const *e);
static QState IOEventListener_idle(IOEventListener *me, QEvent const *e);
static QState IOEventListener_recording(IOEventListener *me, QEvent const *e);
static QState IOEventListener_playing(IOEventListener *me, QEvent const *e);

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
            me->buttonState |= ((ButtonEvent*)e)->buttonMask;
            UART_printf("press 0x%04x now 0x%04x\r\n", (int)((ButtonEvent*)e)->buttonMask, me->buttonState);
            // switch modes if necessary
            // TODO find a good way to signal going into record mode
            uint8_t nbits = bitsSetIn(me->buttonState);
            if (nbits >= 3)
            {
                QEvent *modeSwitchEvt = Q_NEW(QEvent, IOE_MODE_SWITCH_SIG);
                QActive_postFIFO((QActive*)me, modeSwitchEvt);
            }
            else
            {
                startOrStopSelectedNotes(((ButtonEvent*)e)->buttonMask, true);
            }
            return Q_HANDLED();

        case BUTTON_RELEASED_SIG:
            me->buttonState &= ~((ButtonEvent*)e)->buttonMask;
            UART_printf("release 0x%04x now 0x%04x\r\n", (int)((ButtonEvent*)e)->buttonMask, me->buttonState);
            startOrStopSelectedNotes(((ButtonEvent*)e)->buttonMask, false);
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
        }
            return Q_HANDLED();

        case HIT_SIG:
        {

        	// dedupe subsequent hits shortly after the first
        	if (me->lastHitTime>0 && me->lastHitTime>systemTime-HIT_DEDUPE_MS){
        		return Q_HANDLED();
        	}

            HitEvent *ev = (HitEvent*)e;
            me->lastHitTime         = systemTime;
            me->lastTransientSource = ev->transient;
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

            // flash the HIT_FLASH_LED
            RGB_LED_On(HIT_FLASH_LED, ev->xyz.x, ev->xyz.y, ev->xyz.z);
            QTimeEvt_disarm(&me->accelLEDOffEvt);
            QTimeEvt_postIn(&me->accelLEDOffEvt, (QActive*)me, HIT_FLASH_TIME);

            // play selected notes
            UART_printString("Play note\r\n");
        }
            return Q_HANDLED();

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

        case TIME_TICK_SIG:
            break;

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

        case IOE_METRONOME_SIG:
            startNote(1.0, METRONOME_VOICE, Single, 12);
            return Q_HANDLED();

        case IOE_MODE_SWITCH_SIG:
            return Q_TRAN(&IOEventListener_playing);

        case TIME_TICK_SIG:
            return Q_HANDLED();

        case HIT_SIG:
            {
                HitEvent *hit = (HitEvent*)e;
                RecordedEvent rec = {
                    .performedTimestamp = systemTime,
                    .quantizedTimestamp = systemTime,
                    .transient = hit->transient,
                    .accelValues = hit->xyz,
                	.buttonState = me->buttonState
                	};
                // record the event
                addEvent(&rec);
            }
            // now let the parent state flash the LED and play the sound
            return Q_SUPER(&IOEventListener_active);

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

                HitEvent *hitEvt = Q_NEW(HitEvent, HIT_SIG);
                hitEvt->transient = revt.transient;
                hitEvt->xyz = revt.accelValues;
                QActive_postFIFO((QActive*)me, (QEvent*)hitEvt);		// let parent play the hit
            }
            return Q_HANDLED();

        case TIME_TICK_SIG:
            return Q_HANDLED();
    }
    return Q_SUPER(&IOEventListener_active);
}
