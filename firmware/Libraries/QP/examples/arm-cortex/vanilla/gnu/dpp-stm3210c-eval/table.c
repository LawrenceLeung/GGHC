/*****************************************************************************
* Product: DPP example
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 06, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2007 Quantum Leaps, LLC. All rights reserved.
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
#include "qp_port.h"
#include "dpp.h"
#include "bsp.h"

Q_DEFINE_THIS_FILE

/* Active object class -----------------------------------------------------*/
typedef struct TableTag {
    QActive super;
    uint8_t fork[N_PHILO];
    uint8_t isHungry[N_PHILO];
} Table;

static QState Table_initial(Table *me, QEvent const *e);
static QState Table_serving(Table *me, QEvent const *e);

#define RIGHT(n_) ((uint8_t)(((n_) + (N_PHILO - 1)) % N_PHILO))
#define LEFT(n_)  ((uint8_t)(((n_) + 1) % N_PHILO))
enum ForkState { FREE, USED };

/* Local objects -----------------------------------------------------------*/
static Table l_table;     /* the single instance of the Table active object */

/* Global-scope objects ----------------------------------------------------*/
QActive * const AO_Table = (QActive *)&l_table;      /* "opaque" AO pointer */

/*..........................................................................*/
void Table_ctor(void) {
    uint8_t n;
    Table *me = &l_table;

    QActive_ctor(&me->super, (QStateHandler)&Table_initial);

    for (n = 0; n < N_PHILO; ++n) {
        me->fork[n] = FREE;
        me->isHungry[n] = 0;
    }
}
/*..........................................................................*/
QState Table_initial(Table *me, QEvent const *e) {
    (void)e;        /* suppress the compiler warning about unused parameter */

    QS_OBJ_DICTIONARY(&l_table);
    QS_FUN_DICTIONARY(&QHsm_top);
    QS_FUN_DICTIONARY(&Table_initial);
    QS_FUN_DICTIONARY(&Table_serving);

    QS_SIG_DICTIONARY(DONE_SIG,      0);                  /* global signals */
    QS_SIG_DICTIONARY(EAT_SIG,       0);
    QS_SIG_DICTIONARY(TERMINATE_SIG, 0);

    QS_SIG_DICTIONARY(HUNGRY_SIG,    me);          /* signal just for Table */

    QActive_subscribe((QActive *)me, DONE_SIG);
    QActive_subscribe((QActive *)me, TERMINATE_SIG);

    return Q_TRAN(&Table_serving);
}
/*..........................................................................*/
QState Table_serving(Table *me, QEvent const *e) {
    uint8_t n, m;
    TableEvt *pe;

    switch (e->sig) {
        case HUNGRY_SIG: {
            BSP_busyDelay();
            n = ((TableEvt const *)e)->philoNum;
                      /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (!me->isHungry[n]));

            BSP_displyPhilStat(n, "hungry  ");
            m = LEFT(n);
            if ((me->fork[m] == FREE) && (me->fork[n] == FREE)) {
                me->fork[m] = me->fork[n] = USED;
                pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = n;
                QF_publish((QEvent *)pe);
                BSP_displyPhilStat(n, "eating  ");
            }
            else {
                me->isHungry[n] = 1;
            }
            return Q_HANDLED();
        }
        case DONE_SIG: {
            BSP_busyDelay();
            n = ((TableEvt const *)e)->philoNum;
                      /* phil ID must be in range and he must be not hungry */
            Q_ASSERT((n < N_PHILO) && (!me->isHungry[n]));

            BSP_displyPhilStat(n, "thinking");
            m = LEFT(n);
                                      /* both forks of Phil[n] must be used */
            Q_ASSERT((me->fork[n] == USED) && (me->fork[m] == USED));

            me->fork[m] = me->fork[n] = FREE;
            m = RIGHT(n);                       /* check the right neighbor */
            if (me->isHungry[m] && (me->fork[m] == FREE)) {
                me->fork[n] = me->fork[m] = USED;
                me->isHungry[m] = 0;
                pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = m;
                QF_publish((QEvent *)pe);
                BSP_displyPhilStat(m, "eating  ");
            }
            m = LEFT(n);                         /* check the left neighbor */
            n = LEFT(m);                  /* left fork of the left neighbor */
            if (me->isHungry[m] && (me->fork[n] == FREE)) {
                me->fork[m] = me->fork[n] = USED;
                me->isHungry[m] = 0;
                pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoNum = m;
                QF_publish((QEvent *)pe);
                BSP_displyPhilStat(m, "eating  ");
            }
            return Q_HANDLED();
        }
        case TERMINATE_SIG: {
            QF_stop();
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
