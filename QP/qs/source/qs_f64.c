/*****************************************************************************
* Product:  QS/C
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
#include "qs_pkg.h"

/**
* \file
* \ingroup qs
* \brief QS_f64() implementation
*/

/*..........................................................................*/
void QS_f64(uint8_t format, double d) {
    union F64Rep {
        double d;
        struct UInt2 {
            uint32_t u1, u2;
        } i;
    } fu64;
    fu64.d = d;

    QS_INSERT_ESC_BYTE(format)

    QS_INSERT_ESC_BYTE((uint8_t)fu64.i.u1)
    fu64.i.u1 >>= 8;
    QS_INSERT_ESC_BYTE((uint8_t)fu64.i.u1)
    fu64.i.u1 >>= 8;
    QS_INSERT_ESC_BYTE((uint8_t)fu64.i.u1)
    fu64.i.u1 >>= 8;
    QS_INSERT_ESC_BYTE((uint8_t)fu64.i.u1)

    QS_INSERT_ESC_BYTE((uint8_t)fu64.i.u2)
    fu64.i.u2 >>= 8;
    QS_INSERT_ESC_BYTE((uint8_t)fu64.i.u2)
    fu64.i.u2 >>= 8;
    QS_INSERT_ESC_BYTE((uint8_t)fu64.i.u2)
    fu64.i.u2 >>= 8;
    QS_INSERT_ESC_BYTE((uint8_t)fu64.i.u2)
}
