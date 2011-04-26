/*****************************************************************************
* Product: QF/C
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 07, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
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
#include "qf_pkg.h"
#include "qassert.h"

Q_DEFINE_THIS_MODULE(qeq_lifo)

/**
* \file
* \ingroup qf
* \brief QEQueue_postLIFO() definition. NOTE: this function is used for
* the "raw" thread-safe queues and NOT for the queues of active objects.
*/

/*..........................................................................*/
void QEQueue_postLIFO(QEQueue *me, QEvent const *e) {
    QF_INT_LOCK_KEY_
    QF_INT_LOCK_();

    QS_BEGIN_NOLOCK_(QS_QF_EQUEUE_POST_LIFO, QS_eqObj_, me)
        QS_TIME_();                                            /* timestamp */
        QS_SIG_(e->sig);                        /* the signal of this event */
        QS_OBJ_(me);                                   /* this queue object */
        QS_U8_(e->dynamic_);          /* the dynamic attribute of the event */
        QS_EQC_(me->nFree);                       /* number of free entries */
        QS_EQC_(me->nMin);                    /* min number of free entries */
    QS_END_NOLOCK_()

    if (e->dynamic_ != (uint8_t)0) {                 /* is it a pool event? */
        ++((QEvent *)e)->dynamic_;       /* increment the reference counter */
                   /* NOTE: cast the 'const' away, which is legitimate because
                    * it's a dynamic event
                    */
    }

    if (me->frontEvt != (QEvent *)0) {           /* is the queue not empty? */
            /* the queue must be able to accept the event (cannot overflow) */
        Q_ASSERT(me->nFree != (QEQueueCtr)0);

        ++me->tail;
        if (me->tail == me->end) {                /* need to wrap the tail? */
            me->tail = (QEQueueCtr)0;                        /* wrap around */
        }

        me->ring[me->tail] = me->frontEvt;      /* buffer the old front evt */

        --me->nFree;                        /* update number of free events */
        if (me->nMin > me->nFree) {
            me->nMin = me->nFree;                  /* update minimum so far */
        }
    }

    me->frontEvt = e;                   /* stick the new event to the front */

    QF_INT_UNLOCK_();
}
