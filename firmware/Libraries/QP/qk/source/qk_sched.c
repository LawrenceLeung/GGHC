/*****************************************************************************
* Product: QK/C
* Last Updated for Version: 4.1.00
* Date of the Last Update:  Oct 09, 2009
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#include "qk_pkg.h"
#include "qassert.h"

Q_DEFINE_THIS_MODULE(qk_sched)

/**
* \file
* \ingroup qk
* \brief QK_schedule_() implementation.
*/

/*..........................................................................*/
/* NOTE: the QK scheduler is entered and exited with interrupts LOCKED */
#ifndef QF_INT_KEY_TYPE
void QK_schedule_(void) {
#else
void QK_schedule_(QF_INT_KEY_TYPE intLockKey_) {
#endif
    uint8_t p;
                      /* the QK scheduler must be called at task level only */
    Q_REQUIRE(QK_intNest_ == (uint8_t)0);

#if (QF_MAX_ACTIVE <= 8)
        /* determine the priority of the highest-priority task ready to run */
    QPSet8_findMax(&QK_readySet_, p);
#else
        /* determine the priority of the highest-priority task ready to run */
    QPSet64_findMax(&QK_readySet_, p);
#endif

#ifdef QK_NO_MUTEX
    if (p > QK_currPrio_) {                     /* do we have a preemption? */
#else                                /* QK priority-ceiling mutexes allowed */
    if ((p > QK_currPrio_) && (p > QK_ceilingPrio_)) {
#endif
        uint8_t pin = QK_currPrio_;            /* save the initial priority */
        QActive *a;
#ifdef QK_TLS                                 /* thread-local storage used? */
        uint8_t pprev = pin;
#endif
        do {
            QEvent const *e;
            a = QF_active_[p];              /* obtain the pointer to the AO */

            QK_currPrio_ = p;     /* this becomes the current task priority */

#ifdef QK_TLS                                 /* thread-local storage used? */
            if (p != pprev) {                   /* are we changing threads? */
                QK_TLS(a);               /* switch new thread-local storage */
                pprev = p;
            }
#endif
            QS_BEGIN_NOLOCK_(QS_QK_SCHEDULE, QS_aoObj_, a)
                QS_TIME_();                                    /* timestamp */
                QS_U8_(p);                        /* the priority of the AO */
                QS_U8_(pin);                      /* the preempted priority */
            QS_END_NOLOCK_()

            QK_INT_UNLOCK_();                      /* unlock the interrupts */

            e = QActive_get_(a);          /* get the next event for this AO */
            QF_ACTIVE_DISPATCH_(&a->super, e);        /* dispatch to the AO */
            QF_gc(e);            /* garbage collect the event, if necessary */

            QK_INT_LOCK_();
                          /* determine the highest-priority AO ready to run */
#if (QF_MAX_ACTIVE <= 8)
            QPSet8_findMax(&QK_readySet_, p);
#else
            QPSet64_findMax(&QK_readySet_, p);
#endif

#ifdef QK_NO_MUTEX
        } while (p > pin);      /* is the new priority higher than initial? */
#else                                /* QK priority-ceiling mutexes allowed */
        } while ((p > pin) && (p > QK_ceilingPrio_));
#endif
        QK_currPrio_ = pin;                 /* restore the initial priority */

#ifdef QK_TLS                                 /* thread-local storage used? */
        if (pin != (uint8_t)0) {   /* no extended context for the idle loop */
            a = QF_active_[pin];         /* the pointer to the preempted AO */
            QK_TLS(a);                          /* restore the original TLS */
        }
#endif
    }
}
