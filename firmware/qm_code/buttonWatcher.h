/*****************************************************************************
* Model: Jigbox.qm
* File:  ../../firmware/qm_code/buttonWatcher.h
*
* This file has been generated automatically by QP Modeler (QM).
* DO NOT EDIT THIS FILE MANUALLY.
*
* Please visit www.state-machine.com/qm for more information.
*****************************************************************************/
#include "qp_port.h"
#include "jigbox.h"

/* $(AOs::ButtonWatcher) ...................................................*/
typedef struct ButtonWatcherTag {
/* protected: */
    QActive super;
} ButtonWatcher;

/* public: */
void ButtonWatcher_lightLED(ButtonWatcher *me);
void ButtonWatcher_ledsOff(ButtonWatcher *me);

/* protected: */
QState ButtonWatcher_initial(ButtonWatcher *me, QEvent const *e);
QState ButtonWatcher_idle(ButtonWatcher *me, QEvent const *e);
QState ButtonWatcher_pressed(ButtonWatcher *me, QEvent const *e);



