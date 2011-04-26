/*****************************************************************************
* Product: "Fly'n'Shoot" game example
* Last Updated for Version: 4.0.04
* Date of the Last Update:  Aug 01, 2009
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
#include "qp_port.h"
#include "bsp.h"
#include "game.h"

/* Local-scope objects -----------------------------------------------------*/
static QEvent const * l_missileQueueSto[2];
static QEvent const * l_shipQueueSto[3];
static QEvent const * l_tunnelQueueSto[GAME_MINES_MAX + 5];
static ObjectPosEvt   l_smlPoolSto[GAME_MINES_MAX + 8]; /* small event pool */
static ObjectImageEvt l_medPoolSto[GAME_MINES_MAX + 8];/* medium event pool */
static QSubscrList    l_subscrSto[MAX_PUB_SIG];

/*..........................................................................*/
void main(int argc, char *argv[]) {
                          /* explicitly invoke the active objects' ctors... */
    Missile_ctor();
    Ship_ctor();
    Tunnel_ctor();

    BSP_init(argc, argv);           /* initialize the Board Support Package */

    QF_init();     /* initialize the framework and the underlying RT kernel */

                                           /* initialize the event pools... */
    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));

    QF_psInit(l_subscrSto, Q_DIM(l_subscrSto));   /* init publish-subscribe */

                            /* send object dictionaries for event queues... */
    QS_OBJ_DICTIONARY(l_missileQueueSto);
    QS_OBJ_DICTIONARY(l_shipQueueSto);
    QS_OBJ_DICTIONARY(l_tunnelQueueSto);

                             /* send object dictionaries for event pools... */
    QS_OBJ_DICTIONARY(l_smlPoolSto);
    QS_OBJ_DICTIONARY(l_medPoolSto);

               /* send signal dictionaries for globally published events... */
    QS_SIG_DICTIONARY(TIME_TICK_SIG,      0);
    QS_SIG_DICTIONARY(PLAYER_TRIGGER_SIG, 0);
    QS_SIG_DICTIONARY(PLAYER_QUIT_SIG,    0);
    QS_SIG_DICTIONARY(GAME_OVER_SIG,      0);

                                             /* start the active objects... */
    QActive_start(AO_Missile,
                  1,                                            /* priority */
                  l_missileQueueSto, Q_DIM(l_missileQueueSto), /* evt queue */
                  (void *)0, 0,                      /* no per-thread stack */
                  (QEvent *)0);                  /* no initialization event */
    QActive_start(AO_Ship,
                  2,                                            /* priority */
                  l_shipQueueSto,    Q_DIM(l_shipQueueSto),    /* evt queue */
                  (void *)0, 0,                      /* no per-thread stack */
                  (QEvent *)0);                  /* no initialization event */
    QActive_start(AO_Tunnel,
                  3,                                            /* priority */
                  l_tunnelQueueSto,  Q_DIM(l_tunnelQueueSto),  /* evt queue */
                  (void *)0, 0,                      /* no per-thread stack */
                  (QEvent *)0);                  /* no initialization event */

    QF_run();                                     /* run the QF application */
}
