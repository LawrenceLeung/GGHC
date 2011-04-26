/*****************************************************************************
* Product:  QS/C
* Last Updated for Version: 4.1.05
* Date of the Last Update:  Oct 26, 2010
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2010 Quantum Leaps, LLC. All rights reserved.
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
#include "qs_pkg.h"

/**
* \file
* \ingroup qs
* \brief QS_getBlock() implementation
*/

/*..........................................................................*/
/* get up to *pn bytes of contiguous memory */
uint8_t const *QS_getBlock(uint16_t *pNbytes) {
    uint8_t *block;
    if (QS_used_ == (QSCtr)0) {
        *pNbytes = (uint16_t)0;
        block = (uint8_t *)0;               /* no bytes to return right now */
    }
    else {
        QSCtr t;
        QSCtr n = (QSCtr)(QS_end_ - QS_tail_);
        if (n > QS_used_) {
            n = QS_used_;
        }
        if (n > (QSCtr)(*pNbytes)) {
            n = (QSCtr)(*pNbytes);
        }
        *pNbytes = (uint16_t)n;
        QS_used_ = (QSCtr)(QS_used_ - n);
        t = QS_tail_;
        QS_tail_ = (QSCtr)(QS_tail_ + n);
        if (QS_tail_ == QS_end_) {
            QS_tail_ = (QSCtr)0;
        }
        block = &QS_ring_[t];
    }
    return block;
}
