//////////////////////////////////////////////////////////////////////////////
// Product: Quantum Spy -- TCP/IP communication HAL
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
#include "hal.h"

#include <winsock2.h>                       // for WindSock network facilities
#include <stdio.h>
#include <conio.h>

//............................................................................
SOCKET l_serverSock = INVALID_SOCKET;
SOCKET l_clentSock  = INVALID_SOCKET;

//............................................................................
bool tcpOpen(int portNum) {

    // initialize Windows sockets
    static WSADATA wsaData;
    int wsaErr = WSAStartup(MAKEWORD(2,0), &wsaData);
    if (wsaErr == SOCKET_ERROR) {
        printf("Windows Sockets cannot be initialized.\n"
               "The library reported error 0x%08X.", wsaErr);
        return false;
    }

    l_serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    // TCP socket
    if (l_serverSock == INVALID_SOCKET){
        printf("Server socket cannot be created.\n"
               "Windows socket error 0x%08X.",
               WSAGetLastError());
        return false;
    }

    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons((unsigned short)portNum);

    // bind() associates a local address and port combination with the
    // socket just created. This is most useful when the application is a
    // server that has a well-known port that clients know about in advance.
    if (bind(l_serverSock, (struct sockaddr *)&local, sizeof(local))
        == SOCKET_ERROR)
    {
        printf("Error by binding server socket.\n");
        return false;
    }

    if (listen(l_serverSock, 1) == SOCKET_ERROR) {
        printf("Server socket listen failed.\n"
               "Windows socket error 0x%08X.",
               WSAGetLastError());
        return false;
    }
    /*
    // Set the socket to non-blocking mode.
    ULONG ioctl_opt = 1;
    if (ioctlsocket(l_serverSock, FIONBIO, &ioctl_opt) == SOCKET_ERROR) {
        printf("Socket configuration failed.\n"
               "Windows socket error 0x%08X.",
               WSAGetLastError());
        return false;
    }
    */
    return true;                                                    // success
}
//............................................................................
void tcpClose(void) {
    if (l_serverSock != INVALID_SOCKET) {
        closesocket(l_serverSock);
    }
    WSACleanup();
}
//............................................................................
int tcpRead(unsigned char *buf, size_t size) {
    int n;

    if (l_clentSock == INVALID_SOCKET) {      // still waiting for the client?
        n = 0;                                      // no data from the client
        if (kbhit()) {
            return -1;                                            // terminate
        }

        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(l_serverSock, &readSet);

        struct timeval delay;
        delay.tv_sec = 0;
        delay.tv_usec = 200000;

        int nfound = ::select(0, &readSet, 0, 0, &delay);
        if (nfound == SOCKET_ERROR) {
            printf("Server socket select failed.\n"
                   "Windows socket error 0x%08X.",
                   WSAGetLastError());
            return -1;                                            // terminate
        }

        if (FD_ISSET(l_serverSock, &readSet)) {
            struct sockaddr_in fromAddr;
            int fromLen = (int)sizeof(fromAddr);
            l_clentSock = accept(l_serverSock,
                                 (struct sockaddr *)&fromAddr, &fromLen);
            if (l_clentSock == INVALID_SOCKET) {
                printf("Server socket accept failed.\n"
                       "Windows socket error 0x%08X.",
                       WSAGetLastError());
                return -1;                                        // terminate
            }
            printf("Accepted connection from %s, port %d\n",
                   inet_ntoa(fromAddr.sin_addr),
                   htons(fromAddr.sin_port));
        }
    }
    else {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(l_clentSock, &readSet);

        int nfound = ::select(0, &readSet, 0, 0, 0);     // selective blocking
        if (nfound == SOCKET_ERROR) {
            printf("Client socket select failed.\n"
                   "Windows socket error 0x%08X.",
                   WSAGetLastError());
            return -1;                                            // terminate
        }
        if (FD_ISSET(l_clentSock, &readSet)) {
            n = recv(l_clentSock, (char *)buf, size, 0);
            if (n == SOCKET_ERROR) {
                printf("Client socket error.\n"
                       "Windows socket error 0x%08X.",
                       WSAGetLastError());
            }
            if (n <= 0) {                                // the client hang up
                closesocket(l_clentSock);

                // go back to waiting for a client, or a keypress to terminate
                l_clentSock = INVALID_SOCKET;
                return 0;                               // no data from client
            }
        }
        else {
            n = 0;                                  // no data from the client
        }
    }

    return (int)n;
}
