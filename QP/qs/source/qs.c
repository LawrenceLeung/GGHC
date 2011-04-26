/*****************************************************************************
* Product: QS/C
* Last Updated for Version: 4.1.03
* Date of the Last Update:  Feb 15, 2010
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
* \brief QS internal variables definitions and
* core QS functions implementations.
*/

/*..........................................................................*/
uint8_t QS_glbFilter_[32];                              /* global QS filter */

/*..........................................................................*/
uint8_t *QS_ring_;               /* pointer to the start of the ring buffer */
QSCtr QS_end_;                      /* offset of the end of the ring buffer */
QSCtr QS_head_;               /* offset to where next byte will be inserted */
QSCtr QS_tail_;              /* offset of where next byte will be extracted */
QSCtr QS_used_;             /* number of bytes currently in the ring buffer */
uint8_t QS_seq_;                              /* the record sequence number */
uint8_t QS_chksum_;                   /* the checksum of the current record */
uint8_t QS_full_;                    /* the ring buffer is temporarily full */

/*..........................................................................*/
/*lint -e970 -e971            ignore MISRA rules 13 and 14 in this function */
char const Q_ROM * Q_ROM_VAR QS_getVersion(void) {
    static char const Q_ROM Q_ROM_VAR version[] = {
        ((QP_VERSION >> 12) & 0xF) + '0',
        '.',
        ((QP_VERSION >>  8) & 0xF) + '0',
        '.',
        ((QP_VERSION >>  4) & 0xF) + '0',
        (QP_VERSION         & 0xF) + '0',
        '\0'
    };
    return version;
}
/*..........................................................................*/
void QS_initBuf(uint8_t sto[], uint32_t stoSize) {
    QS_ring_ = &sto[0];
    QS_end_  = (QSCtr)stoSize;
}
/*..........................................................................*/
void QS_filterOn(uint8_t rec) {
    uint8_t i;
    if (rec == QS_ALL_RECORDS) {
        for (i = (uint8_t)0; i < (uint8_t)sizeof(QS_glbFilter_); ++i) {
            QS_glbFilter_[i] = (uint8_t)0xFFU;
        }
    }
    else {
        QS_glbFilter_[rec >> 3] |= (uint8_t)(1U << (rec & 0x07));
    }
}
/*..........................................................................*/
void QS_filterOff(uint8_t rec) {
    uint8_t i;
    if (rec == QS_ALL_RECORDS) {
        for (i = (uint8_t)0; i < (uint8_t)sizeof(QS_glbFilter_); ++i) {
            QS_glbFilter_[i] = (uint8_t)0;
        }
    }
    else {
        QS_glbFilter_[rec >> 3] &= (uint8_t)(~(1U << (rec & 0x07)));
    }
}
/*..........................................................................*/
void QS_begin(uint8_t rec) {
    QS_chksum_ = (uint8_t)0;                          /* clear the checksum */
    ++QS_seq_;                      /* always increment the sequence number */
    QS_INSERT_ESC_BYTE(QS_seq_)                /* store the sequence number */
    QS_INSERT_ESC_BYTE(rec)                          /* store the record ID */
}
/*..........................................................................*/
void QS_end(void) {
    QS_INSERT_CHKSUM_BYTE()
    QS_INSERT_BYTE(QS_FRAME)
    if (QS_used_ > QS_end_) {                 /* overrun over the old data? */
        QS_tail_ = QS_head_;              /* shift the tail to the old data */
        QS_used_ = QS_end_;                     /* the whole buffer is used */
    }
}
/*..........................................................................*/
void QS_u8(uint8_t format, uint8_t d) {
    QS_INSERT_ESC_BYTE(format)
    QS_INSERT_ESC_BYTE(d)
}
/*..........................................................................*/
void QS_u16(uint8_t format, uint16_t d) {
    QS_INSERT_ESC_BYTE(format)
    QS_INSERT_ESC_BYTE((uint8_t)d)
    d >>= 8;
    QS_INSERT_ESC_BYTE((uint8_t)d)
}
/*..........................................................................*/
void QS_u32(uint8_t format, uint32_t d) {
    QS_INSERT_ESC_BYTE(format)
    QS_INSERT_ESC_BYTE((uint8_t)d)
    d >>= 8;
    QS_INSERT_ESC_BYTE((uint8_t)d)
    d >>= 8;
    QS_INSERT_ESC_BYTE((uint8_t)d)
    d >>= 8;
    QS_INSERT_ESC_BYTE((uint8_t)d)
}


