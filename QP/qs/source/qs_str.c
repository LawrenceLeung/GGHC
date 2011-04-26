/*****************************************************************************
* Product:  QS/C
* Last Updated for Version: 4.0.01
* Date of the Last Update:  Jun 09, 2008
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
* \brief QS_str() adn QS_str_ROM() implementation
*/

/*..........................................................................*/
void QS_str(char const *s) {
    QS_INSERT_BYTE((uint8_t)QS_STR_T)
    QS_chksum_ = (uint8_t)(QS_chksum_ + (uint8_t)QS_STR_T);
    while ((*s) != (char)'\0') {
                                    /* ASCII characters don't need escaping */
        QS_INSERT_BYTE((uint8_t)(*s))
        QS_chksum_ = (uint8_t)(QS_chksum_ + (uint8_t)(*s));
        ++s;
    }
    QS_INSERT_BYTE((uint8_t)0)
}
/*..........................................................................*/
/*lint -e970 -e971            ignore MISRA rules 13 and 14 in this function */
void QS_str_ROM(char const Q_ROM * Q_ROM_VAR s) {
    uint8_t b;
    QS_INSERT_BYTE((uint8_t)QS_STR_T)
    QS_chksum_ = (uint8_t)(QS_chksum_ + (uint8_t)QS_STR_T);
    while ((b = (uint8_t)Q_ROM_BYTE(*s)) != (uint8_t)0) {
                                    /* ASCII characters don't need escaping */
        QS_INSERT_BYTE(b)
        QS_chksum_ = (uint8_t)(QS_chksum_ + b);
        ++s;
    }
    QS_INSERT_BYTE((uint8_t)0)
}
