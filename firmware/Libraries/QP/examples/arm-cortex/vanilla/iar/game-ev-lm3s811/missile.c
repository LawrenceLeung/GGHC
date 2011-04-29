/*****************************************************************************
* Product: "Fly'n'Shoot" game example
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 06, 2008
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
#include "qp_port.h"
#include "bsp.h"
#include "game.h"

/* Q_DEFINE_THIS_FILE */

/* local objects -----------------------------------------------------------*/
typedef struct MissileTag {
    QActive super;                              /* extend the QActive class */
    uint8_t x;
    uint8_t y;
    uint8_t exp_ctr;
} Missile;                                     /* the Missile active object */

static QState Missile_initial  (Missile *me, QEvent const *e);
static QState Missile_armed    (Missile *me, QEvent const *e);
static QState Missile_flying   (Missile *me, QEvent const *e);
static QState Missile_exploding(Missile *me, QEvent const *e);

static Missile l_missile; /* the sole instance of the Missile active object */

/* Public-scope objects ----------------------------------------------------*/
QActive * const AO_Missile = (QActive *)&l_missile;       /* opaque pointer */

/*..........................................................................*/
void Missile_ctor(void) {
    Missile *me = &l_missile;
    QActive_ctor(&me->super, (QStateHandler)&Missile_initial);
}

/* HSM definition ----------------------------------------------------------*/
/*..........................................................................*/
QState Missile_initial(Missile *me, QEvent const *e) {
    (void)e;                  /* avoid the "unreferenced parameter" warning */

    QActive_subscribe((QActive *)me, TIME_TICK_SIG);

    QS_OBJ_DICTIONARY(&l_missile);  /* object dictionary for Missile object */

    QS_FUN_DICTIONARY(&Missile_initial);    /* dictionaries for Missile HSM */
    QS_FUN_DICTIONARY(&Missile_armed);
    QS_FUN_DICTIONARY(&Missile_flying);
    QS_FUN_DICTIONARY(&Missile_exploding);

    QS_SIG_DICTIONARY(MISSILE_FIRE_SIG,   &l_missile);     /* local signals */
    QS_SIG_DICTIONARY(HIT_WALL_SIG,       &l_missile);
    QS_SIG_DICTIONARY(DESTROYED_MINE_SIG, &l_missile);

    return Q_TRAN(&Missile_armed);
}
/*..........................................................................*/
QState Missile_armed(Missile *me, QEvent const *e) {
    switch (e->sig) {
        case MISSILE_FIRE_SIG: {
                                       /* initialize position from the Ship */
            me->x = ((ObjectPosEvt const *)e)->x;
            me->y = ((ObjectPosEvt const *)e)->y;
            return Q_TRAN(&Missile_flying);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Missile_flying(Missile *me, QEvent const *e) {
    switch (e->sig) {
        case TIME_TICK_SIG: {
            ObjectImageEvt *oie;
            if (me->x + GAME_MISSILE_SPEED_X < GAME_SCREEN_WIDTH) {
                me->x += GAME_MISSILE_SPEED_X;
                /*tell the Tunnel to draw the Missile and test for wall hits*/
                oie = Q_NEW(ObjectImageEvt, MISSILE_IMG_SIG);
                oie->x   = me->x;
                oie->y   = me->y;
                oie->bmp = MISSILE_BMP;
                QActive_postFIFO(AO_Tunnel, (QEvent *)oie);
            }
            else { /* Missile outside the range, make it ready to fire again*/
                return Q_TRAN(&Missile_armed);
            }
            return Q_HANDLED();
        }
        case HIT_WALL_SIG: {
            return Q_TRAN(&Missile_exploding);
        }
        case DESTROYED_MINE_SIG: {
            /* tell the Ship the score for destroing this Mine */
            QActive_postFIFO(AO_Ship, e);

            /* re-arm immediately & let the destroyed Mine do the exploding */
            return Q_TRAN(&Missile_armed);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Missile_exploding(Missile *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            me->exp_ctr = 0;
            return Q_HANDLED();
        }
        case TIME_TICK_SIG: {
            if ((me->x >= GAME_SPEED_X) && (me->exp_ctr < 15)) {
                ObjectImageEvt *oie;

                ++me->exp_ctr;             /* advance the explosion counter */
                me->x -= GAME_SPEED_X;    /* move the explosion by one step */

                /* tell the Tunnel to render the current stage of Explosion */
                oie = Q_NEW(ObjectImageEvt, EXPLOSION_SIG);
                oie->x   = me->x + 3;                 /* x-pos of explosion */
                oie->y   = (int8_t)((int)me->y - 4);               /* y-pos */
                oie->bmp = EXPLOSION0_BMP + (me->exp_ctr >> 2);
                QActive_postFIFO(AO_Tunnel, (QEvent *)oie);
            }
            else {          /* explosion finished or moved outside the game */
                return Q_TRAN(&Missile_armed);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
