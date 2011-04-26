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
* \brief QS_mem() implementation
*/

/*..........................................................................*/
void QS_mem(uint8_t const *blk, uint8_t size) {
    QS_INSERT_BYTE((uint8_t)QS_MEM_T)
    QS_chksum_ = (uint8_t)(QS_chksum_ + (uint8_t)QS_MEM_T);
    QS_INSERT_ESC_BYTE(size)
    while (size != (uint8_t)0) {
        QS_INSERT_ESC_BYTE(*blk)
        ++blk;
        --size;
    }
}
