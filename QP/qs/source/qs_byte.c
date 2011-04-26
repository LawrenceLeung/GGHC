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
* \brief QS_getByte() implementation
*/

/*..........................................................................*/
uint16_t QS_getByte(void) {
    uint16_t ret;
    if (QS_used_ == (QSCtr)0) {
        ret = QS_EOD;                                    /* set End-Of-Data */
    }
    else {
        ret = QS_ring_[QS_tail_];                 /* set the byte to return */
        ++QS_tail_;                                     /* advance the tail */
        if (QS_tail_ == QS_end_) {                     /* tail wrap around? */
            QS_tail_ = (QSCtr)0;
        }
        --QS_used_;                                   /* one less byte used */
    }
    return ret;                                   /* return the byte or EOD */
}
