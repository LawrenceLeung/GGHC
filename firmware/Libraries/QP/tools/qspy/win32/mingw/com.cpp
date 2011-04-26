//////////////////////////////////////////////////////////////////////////////
// Product: Quantum Spy -- Serial communication HAL
// Last Updated for Version: 4.0.00
// Date of the Last Update:  Apr 07, 2008
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
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
#include <string.h>                                              // for size_t
#include "hal.h"

#include <windows.h>            // for Windows serial communication facilities
#include <stdio.h>
#include <conio.h>

//............................................................................
static HANDLE l_com;
static COMMTIMEOUTS l_timeouts;

#define BUF_SIZE 1024
#define COM_SETTINGS \
    "baud=%d parity=N data=8 stop=1 odsr=off dtr=on octs=off rts=on"

//............................................................................
bool comOpen(char const *comName, int baudRate) {
    DCB dcb = {0};
    char comSettings[120];

    l_com = 0;
    sprintf(comSettings,
            COM_SETTINGS,
            baudRate);

    l_com = CreateFile(comName,
                         GENERIC_READ | GENERIC_WRITE,
                         0,                                // exclusive access
                         NULL,                            // no security attrs
                         OPEN_EXISTING,
                         0,                   // standard (not-overlapped) I/O
                         NULL);

    if (l_com == INVALID_HANDLE_VALUE) {
        printf("Error by opening the COM port: %s at the baud rate %d\n",
               comName, baudRate);
        return false;
    }

    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(l_com, &dcb)) {
        printf("Error by retreiving port settings\n");
        return (HANDLE)0;
    }

    dcb.fAbortOnError = 0;                             // don't abort on error
    // Fill in the DCB
    if (!BuildCommDCB(comSettings, &dcb)) {
        printf("Error by parsing command line parameters\n");
        return false;
    }

    if (!SetCommState(l_com, &dcb)) {
        printf("Error setting up the port\n");
        return false;
    }

    SetupComm(l_com, BUF_SIZE, BUF_SIZE);        // setup the device buffers

    // purge any information in the buffers
    PurgeComm(l_com, PURGE_TXABORT | PURGE_RXABORT |
                     PURGE_TXCLEAR | PURGE_RXCLEAR);

    // the timeouts for the serial communication are set accorging to the
    // following remark from the Win32 help documentation:
    //
    // If an application sets ReadIntervalTimeout and
    // ReadTotalTimeoutMultiplier to MAXDWORD and sets
    // ReadTotalTimeoutConstant to a value greater than zero and less than
    // MAXDWORD, one of the following occurs when the ReadFile function
    // is called:
    // 1. If there are any characters in the input buffer, ReadFile
    // returns immediately with the characters in the buffer.
    // 2. If there are no characters in the input buffer, ReadFile waits
    // until a character arrives and then returns immediately.
    // 3. If no character arrives within the time specified by
    // ReadTotalTimeoutConstant, ReadFile times out.
    //
    l_timeouts.ReadIntervalTimeout = MAXDWORD;
    l_timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
    l_timeouts.ReadTotalTimeoutConstant = 50;
    l_timeouts.WriteTotalTimeoutMultiplier = 0;
    l_timeouts.WriteTotalTimeoutConstant = 50;
    SetCommTimeouts(l_com, &l_timeouts);

    return true;                                                    // success
}
//............................................................................
void comClose(void) {
    CloseHandle(l_com);
}
//............................................................................
int comRead(unsigned char *buf, size_t size) {
    DWORD n;
    if (!ReadFile(l_com, buf, (DWORD)size, &n, NULL)) {
        printf("*** Reading serial port failed; GetLastError=%ld\n",
               GetLastError());
        DWORD errors;
        COMSTAT comstat;
        ClearCommError(l_com, &errors, &comstat);
    }
    if (_kbhit()) {
        return -1;                                                // terminate
    }
    return (int)n;
}
