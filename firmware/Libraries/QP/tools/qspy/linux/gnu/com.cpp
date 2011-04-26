//////////////////////////////////////////////////////////////////////////////
// Product: Quantum Spy -- Serial Com HAL for Linux/g++
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
#include <stddef.h>                                              // for size_t
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <assert.h>

#include "hal.h"

//............................................................................
static int l_com = 0;                              // COM port file descriptor
static termios l_saved;            // structure with saved terminal attributes
static fd_set l_readSet;  // file-descriptor set for reading COM port and term
static int l_maxFd;                    // maximum file descriptor for select()

//............................................................................
bool comOpen(char const *comName, int baudRate) {

    l_com = open(comName, O_RDWR | O_NOCTTY | O_NONBLOCK);// R/W, non-blocking
    if (l_com == -1) {
        return false;                                           // open failed
    }

    struct termios t;
    if (tcgetattr(l_com, &t) == -1) {
        return false;                             // getting attributes failed
    }
    t.c_cc[VMIN]  = 0;
    t.c_cc[VTIME] = 1;

    t.c_iflag = 0;
    t.c_iflag &= ~(BRKINT | IGNPAR | PARMRK | INPCK |
                   ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF);
    t.c_iflag |= IGNBRK;

    t.c_oflag = 0;
    t.c_oflag &= ~OPOST;

    t.c_lflag = 0;
    t.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL |
                   ICANON | ISIG | NOFLSH | TOSTOP);

    t.c_cflag = 0;
    t.c_cflag &= ~(CSIZE | HUPCL);
    t.c_cflag |= (CLOCAL | CREAD);

    speed_t spd = B115200;                        // the speed of the COM port
    switch (baudRate) {
        case    200: spd =    B200; break;
        case    300: spd =    B300; break;
        case    600: spd =    B600; break;
        case   1200: spd =   B1200; break;
        case   1800: spd =   B1800; break;
        case   2400: spd =   B2400; break;
        case   4800: spd =   B4800; break;
        case   9600: spd =   B9600; break;
        case  19200: spd =  B19200; break;
        case  38400: spd =  B38400; break;
        case  57600: spd =  B57600; break;
        case 115200: spd = B115200; break;
        default: assert(0); break;                    // unsupported baud rate
    }

    // parity
    t.c_cflag &= ~(PARENB | PARODD);                              // no parity
    //t.c_cflag |= (PARENB | PARODD);                            // odd parity
    //t.c_cflag &= ~(PARODD);                                   // even parity
    //t.c_cflag |= (PARENB);                                    // even parity

    // bits in a byte
    //t.c_cflag |= (CS7);                                  // 7 bits in a byte
    t.c_cflag |= (CS8);                                    // 8 bits in a byte

    // stop bits
    t.c_cflag &= ~(CSTOPB);                                      // 1 stop bit
    //t.c_cflag |= (CSTOPB);                                  // 1.5 stop bits

    if (cfsetispeed(&t, spd) == -1) {
        return false;                            // setting input speed failed
    }
    if (cfsetospeed(&t, spd) == -1) {
        return false;                           // setting output speed failed
    }
    if (tcflush(l_com, TCIFLUSH) == -1) {
        return false;                              // flushing the port failed
    }
    if (tcsetattr(l_com, TCSANOW, &t) == -1) {
        return false;                              // seting attributes failed
    }

    // modify the terminal attributes
    if (tcgetattr(0, &l_saved) == -1) {  // get the original terminal settings
        return false;                             // getting attributes failed
    }
    if (tcgetattr(0, &t) == -1) {        // get the modified terminal settings
        return false;                             // getting attributes failed
    }
    t.c_lflag &= ~(ICANON | ECHO);          // disable canonical mode and echo
    if (tcsetattr(0, TCSANOW, &t) == -1) {
        return false;                              // seting attributes failed
    }

    FD_ZERO(&l_readSet);
    FD_SET(l_com, &l_readSet);   // set the COM port to be checked in select()
    FD_SET(0, &l_readSet);       // set the terminal to be checked in select()
    l_maxFd = l_com + 1;
    return true;
}
//............................................................................
void comClose(void) {
    tcsetattr(0, TCSANOW, &l_saved);    // restore the saved terminal settings

    if (l_com != 0) {
        close(l_com);                                    // close the COM port
        l_com = 0;
    }
}
//............................................................................
int comRead(unsigned char *buf, size_t size) {
    fd_set readSet = l_readSet;

     // block indefinitely until the COM port or the Terimal have intput ready
    int n = select(l_maxFd, &readSet, 0, 0, NULL);

    if (FD_ISSET(0, &readSet)) {     // any intput available from the Termial?
        return -1;                                                // terminate
    }
    if (FD_ISSET(l_com, &readSet)) { // any input available from the COM port?
        n = read(l_com, buf, size);
    }
    return n;
}
