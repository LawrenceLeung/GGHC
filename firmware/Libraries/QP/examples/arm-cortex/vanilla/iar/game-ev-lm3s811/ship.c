/*****************************************************************************
* Product: "Fly'n'Shoot" game example
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
#include "bsp.h"
#include "game.h"

/*Q_DEFINE_THIS_FILE */

#define SHIP_WIDTH  5
#define SHIP_HEIGHT 3

/* local objects -----------------------------------------------------------*/
typedef struct ShipTag {
    QActive super;                              /* extend the QActive class */
    uint8_t x;
    uint8_t y;
    uint8_t exp_ctr;
    uint16_t score;
} Ship;                                           /* the Ship active object */

static QState Ship_initial  (Ship *me, QEvent const *e);
static QState Ship_active   (Ship *me, QEvent const *e);
static QState Ship_parked   (Ship *me, QEvent const *e);
static QState Ship_flying   (Ship *me, QEvent const *e);
static QState Ship_exploding(Ship *me, QEvent const *e);

static Ship l_ship;          /* the sole instance of the Ship active object */

/* global objects ----------------------------------------------------------*/
QActive * const AO_Ship = (QActive *)&l_ship;  /* opaque pointer to Ship AO */

/*..........................................................................*/
void Ship_ctor(void) {
    Ship *me = &l_ship;
    QActive_ctor(&me->super, (QStateHandler)&Ship_initial);
    me->x = GAME_SHIP_X;
    me->y = GAME_SHIP_Y;
}

/* HSM definition ----------------------------------------------------------*/
/*..........................................................................*/
QState Ship_initial(Ship *me, QEvent const *e) {
    (void)e;                  /* avoid the "unreferenced parameter" warning */

    QActive_subscribe((QActive *)me, TIME_TICK_SIG);
    QActive_subscribe((QActive *)me, PLAYER_TRIGGER_SIG);


    QS_OBJ_DICTIONARY(&l_ship);        /* object dictionary for Ship object */

    QS_FUN_DICTIONARY(&Ship_initial); /* function dictionaries for Ship HSM */
    QS_FUN_DICTIONARY(&Ship_active);
    QS_FUN_DICTIONARY(&Ship_parked);
    QS_FUN_DICTIONARY(&Ship_flying);
    QS_FUN_DICTIONARY(&Ship_exploding);

    QS_SIG_DICTIONARY(PLAYER_SHIP_MOVE_SIG, &l_ship);      /* local signals */
    QS_SIG_DICTIONARY(TAKE_OFF_SIG,         &l_ship);
    QS_SIG_DICTIONARY(HIT_WALL_SIG,         &l_ship);
    QS_SIG_DICTIONARY(HIT_MINE_SIG,         &l_ship);
    QS_SIG_DICTIONARY(DESTROYED_MINE_SIG,   &l_ship);

    return Q_TRAN(&Ship_active);             /* top-most initial transition */
}
/*..........................................................................*/
QState Ship_active(Ship *me, QEvent const *e) {
    switch (e->sig) {
        case Q_INIT_SIG: {                     /* nested initial transition */
            return Q_TRAN(&Ship_parked);
        }
        case PLAYER_SHIP_MOVE_SIG: {
            me->x = ((ObjectPosEvt const *)e)->x;
            me->y = ((ObjectPosEvt const *)e)->y;
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState Ship_parked(Ship *me, QEvent const *e) {
    switch (e->sig) {
        case TAKE_OFF_SIG: {               /* permition to take off granted */
            return Q_TRAN(&Ship_flying);
        }
    }
    return Q_SUPER(&Ship_active);
}
/*..........................................................................*/
QState Ship_flying(Ship *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            ScoreEvt *sev;

            me->score = 0;                               /* reset the score */
            sev = Q_NEW(ScoreEvt, SCORE_SIG);
            sev->score = me->score;
            QActive_postFIFO(AO_Tunnel, (QEvent *)sev);
            return Q_HANDLED();
        }
        case TIME_TICK_SIG: {
            /* tell the Tunnel to draw the Ship and test for hits */
            ObjectImageEvt *oie = Q_NEW(ObjectImageEvt, SHIP_IMG_SIG);
            oie->x   = me->x;
            oie->y   = me->y;
            oie->bmp = SHIP_BMP;
            QActive_postFIFO(AO_Tunnel, (QEvent *)oie);

            ++me->score;  /* increment the score for surviving another tick */

            if ((me->score % 10) == 0) {           /* is the score "round"? */
                ScoreEvt *sev = Q_NEW(ScoreEvt, SCORE_SIG);
                sev->score = me->score;
                QActive_postFIFO(AO_Tunnel, (QEvent *)sev);
            }

            return Q_HANDLED();
        }
        case PLAYER_TRIGGER_SIG: {                   /* trigger the Missile */
            ObjectPosEvt *ope = Q_NEW(ObjectPosEvt, MISSILE_FIRE_SIG);
            ope->x = me->x;
            ope->y = me->y + SHIP_HEIGHT - 1;
            QActive_postFIFO(AO_Missile, (QEvent *)ope);
            return Q_HANDLED();
        }
        case DESTROYED_MINE_SIG: {
            me->score += ((ScoreEvt const *)e)->score;
            /* the score will be sent to the Tunnel by the next TIME_TICK */
            return Q_HANDLED();
        }
        case HIT_WALL_SIG:
        case HIT_MINE_SIG: {
            return Q_TRAN(&Ship_exploding);
        }
    }
    return Q_SUPER(&Ship_active);
}
/*..........................................................................*/
QState Ship_exploding(Ship *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            me->exp_ctr = 0;
            return Q_HANDLED();
        }
        case TIME_TICK_SIG: {
            if (me->exp_ctr < 15) {
                ObjectImageEvt *oie;

                ++me->exp_ctr;

                  /* tell the Tunnel to draw the current stage of Explosion */
                oie = Q_NEW(ObjectImageEvt, EXPLOSION_SIG);
                oie->bmp = EXPLOSION0_BMP + (me->exp_ctr >> 2);
                oie->x   = me->x;                         /* x of explosion */
                oie->y   = (int8_t)((int)me->y - 4 + SHIP_HEIGHT);
                QActive_postFIFO(AO_Tunnel, (QEvent *)oie);
            }
            else {
                ScoreEvt *gameOver = Q_NEW(ScoreEvt, GAME_OVER_SIG);
                gameOver->score = me->score;
                QActive_postFIFO(AO_Tunnel, (QEvent *)gameOver);
                return Q_TRAN(&Ship_parked);
            }
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&Ship_active);
}
