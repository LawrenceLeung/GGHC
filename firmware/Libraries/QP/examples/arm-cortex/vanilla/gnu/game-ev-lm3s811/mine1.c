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

Q_DEFINE_THIS_FILE

/* local objects -----------------------------------------------------------*/
typedef struct Mine1Tag {
    QHsm super;                                    /* extend the QHsm class */
    uint8_t x;
    uint8_t y;
    uint8_t exp_ctr;
} Mine1;

static QState Mine1_initial  (Mine1 *me, QEvent const *e);
static QState Mine1_unused   (Mine1 *me, QEvent const *e);
static QState Mine1_used     (Mine1 *me, QEvent const *e);
static QState Mine1_planted  (Mine1 *me, QEvent const *e);
static QState Mine1_exploding(Mine1 *me, QEvent const *e);

static Mine1 l_mine1[GAME_MINES_MAX];             /* a pool of type-1 mines */

                             /* helper macro to provide the ID of this mine */
#define MINE_ID(me_)    ((me_) - l_mine1)

/*..........................................................................*/
QHsm *Mine1_ctor(uint8_t id) {
    Mine1 *me;
    Q_REQUIRE(id < GAME_MINES_MAX);

    me = &l_mine1[id];
    QHsm_ctor(&me->super, (QStateHandler)&Mine1_initial);/*superclass' ctor */
    return (QHsm *)me;
}
/*..........................................................................*/
QState Mine1_initial(Mine1 *me, QEvent const *e) {
    static  uint8_t dict_sent;
    (void)e;                  /* avoid the "unreferenced parameter" warning */

    if (!dict_sent) {
        QS_OBJ_DICTIONARY(&l_mine1[0]); /* obj. dictionaries for Mine1 pool */
        QS_OBJ_DICTIONARY(&l_mine1[1]);
        QS_OBJ_DICTIONARY(&l_mine1[2]);
        QS_OBJ_DICTIONARY(&l_mine1[3]);
        QS_OBJ_DICTIONARY(&l_mine1[4]);

        QS_FUN_DICTIONARY(&Mine1_initial);/*fun. dictionaries for Mine1 HSM */
        QS_FUN_DICTIONARY(&Mine1_unused);
        QS_FUN_DICTIONARY(&Mine1_used);
        QS_FUN_DICTIONARY(&Mine1_planted);
        QS_FUN_DICTIONARY(&Mine1_exploding);

        dict_sent = 1;
    }

    QS_SIG_DICTIONARY(MINE_PLANT_SIG,    me);              /* local signals */
    QS_SIG_DICTIONARY(MINE_DISABLED_SIG, me);
    QS_SIG_DICTIONARY(MINE_RECYCLE_SIG,  me);
    QS_SIG_DICTIONARY(SHIP_IMG_SIG,      me);
    QS_SIG_DICTIONARY(MISSILE_IMG_SIG,   me);

    return Q_TRAN(&Mine1_unused);
}
/*..........................................................................*/
QState Mine1_unused(Mine1 *me, QEvent const *e) {
    switch (e->sig) {
        case MINE_PLANT_SIG: {
            me->x = ((ObjectPosEvt const *)e)->x;
            me->y = ((ObjectPosEvt const *)e)->y;
            return Q_TRAN(&Mine1_planted);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Mine1_used(Mine1 *me, QEvent const *e) {
    switch (e->sig) {
        case Q_EXIT_SIG: {
            /* tell the Tunnel that this mine is becoming disabled */
            MineEvt *mev = Q_NEW(MineEvt, MINE_DISABLED_SIG);
            mev->id = MINE_ID(me);
            QActive_postFIFO(AO_Tunnel, (QEvent *)mev);
            return Q_HANDLED();
        }
        case MINE_RECYCLE_SIG: {
            return Q_TRAN(&Mine1_unused);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Mine1_planted(Mine1 *me, QEvent const *e) {
    uint8_t x;
    uint8_t y;
    uint8_t bmp;

    switch (e->sig) {
        case TIME_TICK_SIG: {
            if (me->x >= GAME_SPEED_X) {
                ObjectImageEvt *oie;

                me->x -= GAME_SPEED_X;              /* move the mine 1 step */

                /* tell the Tunnel to draw the Mine */
                oie = Q_NEW(ObjectImageEvt, MINE_IMG_SIG);
                oie->x   = me->x;
                oie->y   = me->y;
                oie->bmp = MINE1_BMP;
                QActive_postFIFO(AO_Tunnel, (QEvent *)oie);
            }
            else {
                return Q_TRAN(&Mine1_unused);
            }
            return Q_HANDLED();
        }
        case SHIP_IMG_SIG: {
            x   = (uint8_t)((ObjectImageEvt const *)e)->x;
            y   = (uint8_t)((ObjectImageEvt const *)e)->y;
            bmp = (uint8_t)((ObjectImageEvt const *)e)->bmp;

            /* test for incoming Ship hitting this mine */
            if (do_bitmaps_overlap(MINE1_BMP, me->x, me->y, bmp, x, y)) {
                static MineEvt const mine1_hit = {
                    { HIT_MINE_SIG, 0 },        /* the QEvent base instance */
                    1               /* type of the mine (1 for Mine type-1) */
                };
                QActive_postFIFO(AO_Ship, (QEvent *)&mine1_hit);

                /* go straight to 'disabled' and let the Ship do
                * the exploding
                */
                return Q_TRAN(&Mine1_unused);
            }
            return Q_HANDLED();
        }
        case MISSILE_IMG_SIG: {
            x   = (uint8_t)((ObjectImageEvt const *)e)->x;
            y   = (uint8_t)((ObjectImageEvt const *)e)->y;
            bmp = (uint8_t)((ObjectImageEvt const *)e)->bmp;

            /* test for incoming Missile hitting this mine */
            if (do_bitmaps_overlap(MINE1_BMP, me->x, me->y, bmp, x, y)) {
               static ScoreEvt const mine1_destroyed = {
                    { DESTROYED_MINE_SIG, 0 },  /* the QEvent base instance */
                    25                  /* score for destroying Mine type-1 */
                };
                QActive_postFIFO(AO_Missile, (QEvent *)&mine1_destroyed);
                return Q_TRAN(&Mine1_exploding);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Mine1_used);
}
/*..........................................................................*/
QState Mine1_exploding(Mine1 *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            me->exp_ctr = 0;
            return Q_HANDLED();
        }
        case TIME_TICK_SIG: {
            if ((me->x >= GAME_SPEED_X) && (me->exp_ctr < 15)) {
                ObjectImageEvt *oie;

                ++me->exp_ctr;             /* advance the explosion counter */
                me->x -= GAME_SPEED_X;          /* move explosion by 1 step */

                /* tell the Game to render the current stage of Explosion */
                oie = Q_NEW(ObjectImageEvt, EXPLOSION_SIG);
                oie->x   = me->x + 1;                     /* x of explosion */
                oie->y   = (int8_t)((int)me->y - 4 + 2);  /* y of explosion */
                oie->bmp = EXPLOSION0_BMP + (me->exp_ctr >> 2);
                QActive_postFIFO(AO_Tunnel, (QEvent *)oie);
            }
            else {
                return Q_TRAN(&Mine1_unused);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Mine1_used);
}
