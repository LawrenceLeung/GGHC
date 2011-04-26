//////////////////////////////////////////////////////////////////////////////
// Product: Quantum Spy -- Host resident component
// Last Updated for Version: 4.1.00
// Date of the Last Update:  Oct 09, 2009
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
//
// This software may be distributed and modified under the terms of the GNU
// General Public License version 2 (GPL) as published by the Free Software
// Foundation and appearing in the file GPL.TXT included in the packaging of
// this file. Please note that GPL Section 2[b] requires that all works based
// on this software must also be made publicly available under the terms of
// the GPL ("Copyleft").
//
// Alternatively, this software may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GPL and are specifically designed for licensees interested in
// retaining the proprietary status of their code.
//
// Contact information:
// Quantum Leaps Web site:  http://www.quantum-leaps.com
// e-mail:                  info@quantum-leaps.com
//////////////////////////////////////////////////////////////////////////////
#ifndef qspy_h
#define qspy_h

#define QSPY_VER "4.1.00"

void qsConfig(uint8_t quiet,
              uint8_t objPtrSize,
              uint8_t funPtrSize,
              uint8_t tstampSize,
              uint8_t sigSize,
              uint8_t evtSize,
              uint8_t queueCtrSize,
              uint8_t poolCtrSize,
              uint8_t poolBlkSize,
              uint8_t tevtCtrSize,
              FILE   *outFile,
              FILE   *matFile);
void qsParse(uint8_t const *buf, size_t nBytes);

#endif                                                               // qspy_h

