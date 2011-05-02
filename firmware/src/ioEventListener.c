#include "jigbox.h"

Q_DEFINE_THIS_FILE

typedef struct
{
    QActive super;
    QTimeEvt tickEvt;
    QTimeEvt accelLEDOffEvt;
    uint32_t lastHitTime;
    TransientSource lastTransientSource;
    uint16_t buttonState;
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
    QActive_ctor(&me->super, (QStateHandler)&IOEventListener_initial);
    me->buttonState = 0;
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
            QTimeEvt_postEvery(&me->tickEvt,(QActive*)me, 1000);     // 1 per second
            return Q_HANDLED();

        case Q_EXIT_SIG:
            QTimeEvt_disarm(&me->tickEvt);
            return Q_HANDLED();

        case Q_INIT_SIG:
            return Q_TRAN(&IOEventListener_idle);

        case BUTTON_PRESSED_SIG:
            me->buttonState |= ((ButtonEvent*)e)->buttonMask;
            UART_printf("press 0x%04x now 0x%04x\r\n", (int)((ButtonEvent*)e)->buttonMask, me->buttonState);
            return Q_HANDLED();

        case BUTTON_RELEASED_SIG:
            me->buttonState &= ~((ButtonEvent*)e)->buttonMask;
            UART_printf("release 0x%04x now 0x%04x\r\n", (int)((ButtonEvent*)e)->buttonMask, me->buttonState);
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
                HitEvent *ev = (HitEvent*)e;
                me->lastHitTime            = systemTime;
                me->lastTransientSource    = ev->transient;
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

                RGB_LED_On(RGB_LED_3, ev->xyz.x, ev->xyz.y, ev->xyz.z);
                QTimeEvt_disarm(&me->accelLEDOffEvt);
                QTimeEvt_postIn(&me->accelLEDOffEvt, (QActive*)me, 50);
            }
            return Q_HANDLED();

        case IOE_ACCEL_LED_OFF_SIG:
            RGB_LED_On(RGB_LED_3, 0, 0, 0);
            return Q_HANDLED();
    }
    return Q_SUPER(&QHsm_top);
}

QState IOEventListener_idle(IOEventListener *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
            break;

        case Q_EXIT_SIG:
            break;

        case TIME_TICK_SIG:
            break;
    }
    return Q_SUPER(&IOEventListener_active);
}

QState IOEventListener_recording(IOEventListener *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
            break;

        case Q_EXIT_SIG:
            break;

        case TIME_TICK_SIG:
            break;
    }
    return Q_SUPER(&IOEventListener_active);
}

QState IOEventListener_playing(IOEventListener *me, QEvent const *e)
{
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
            break;

        case Q_EXIT_SIG:
            break;

        case TIME_TICK_SIG:
            break;
    }
    return Q_SUPER(&IOEventListener_active);
}
