#include "jigbox.h"

static void updateLED1FromAccelerometer(void)
{
}

typedef struct
{
    QActive super;
    QTimeEvt tickEvt;                  // SysTick rate ticker
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
    QTimeEvt_ctor(&me->tickEvt, TIME_TICK_SIG);
    QActive_ctor(&me->super, (QStateHandler)&IOEventListener_initial);
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
            QTimeEvt_postEvery(&me->tickEvt,(QActive*)me, 1);
            return Q_HANDLED();

        case Q_EXIT_SIG:
            QTimeEvt_disarm(&me->tickEvt);
            return Q_HANDLED();

        case Q_INIT_SIG:
            return Q_TRAN(&IOEventListener_idle);

        case EV_BUTTON_PRESSED_SIG:
            break;

        case EV_BUTTON_RELEASED_SIG:
            break;

        case TIME_TICK_SIG:
            if (systemTime % 100 == 0)
                updateLED1FromAccelerometer();
            break;
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
