//////////////////////////////////////////////////////////////////////////////
// Product: Quantum Spy -- Host resident component, main entry point
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
#include <stdint.h>
#include <stddef.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "qspy.h"
#include "hal.h"
#include "getopt.h"

//............................................................................
#define FNAME_SIZE 256
#define BUF_SIZE   1024

enum TargetLink {
    NO_LINK,
    FILE_LINK,
    SERIAL_LINK,
    TCP_LINK
};

//............................................................................
int main(int argc, char *argv[]) {
    int optChar;
    char comPort[FNAME_SIZE];
    char inpFileName[FNAME_SIZE];
    char outFileName[FNAME_SIZE];
    char savFileName[FNAME_SIZE];
    char matFileName[FNAME_SIZE];
    TargetLink link = NO_LINK;
                                              // default configuration options
    strcpy(inpFileName, "qs.bin");
    strcpy(comPort, "COM1");
    int tcpPort          = 6601;
    int baudRate         = 38400;
    uint8_t quiet        = 0;
    uint8_t tstampSize   = 4;
    uint8_t objPtrSize   = 4;
    uint8_t funPtrSize   = 4;
    uint8_t sigSize      = 1;
    uint8_t evtSize      = 2;
    uint8_t queueCtrSize = 1;
    uint8_t poolCtrSize  = 2;
    uint8_t poolBlkSize  = 2;
    uint8_t tevtCtrSize  = 2;
    outFileName[0]       = '\0';                  // Output file not specified
    savFileName[0]       = '\0';                    // Save file not specified
    matFileName[0]       = '\0';                  // Matlab file not specified

    time_t now = time(NULL);
    printf("QSPY host application %s\n"
           "Copyright (c) Quantum Leaps, LLC.\n"
           "%s\n", QSPY_VER, ctime(&now));

    static char const help[] =
        "Syntax is: qspy [options]          * = default\n\n"
        "OPTION                    DEFAULT  COMMENT\n"
        "----------------------------------------------------------------\n"
        "-h                                 help (this message)\n"
        "-q                                 quiet mode (no stdout output)\n"
        "-o<File_name>                      produce output to a file\n"
        "-s<File_name>                      save the binary data to a file\n"
        "-m<File_name>                      produce a Matlab file\n"
        "-c<COM_port>  *           COM1     com port input\n"
        "-b<Baud_rate>             38400    baud rate selection\n"
        "-f<File_name>             qs.bin   file input\n"
        "-t                                 TCP/IP input\n"
        "-p<TCP_Port_number>       6601     TCP/IP server port\n"
        "-T<tstamp_size>           4        QS timestamp size (bytes)\n"
        "-O<pointer_size>          4        object pointer size (bytes)\n"
        "-F<pointer_size>          4        function pointer size (bytes)\n"
        "-S<signal_size>           1        signal size (bytes)\n"
        "-E<event_size>            2        event size size (bytes)\n"
        "-Q<queue_counter_size>    1        queue counter size (bytes)\n"
        "-P<pool_counter_size>     2        pool counter size (bytes)\n"
        "-B<pool_blocksize_size>   2        pool blocksize size (bytes)\n"
        "-C<QTimeEvt_counter_size> 2        QTimeEvt counter size\n";

    while ((optChar = getopt(argc, argv,
                             "hqo:s:m:c:b:tp:f:T:O:F:S:E:Q:P:B:C:")) != -1)
    {
        switch (optChar) {
            case 'q': {                                          // quiet mode
                quiet = 1;
                break;
            }
            case 'o': {                                         // file output
                strncpy(outFileName, optarg, sizeof(outFileName));
                printf("-o %s\n", outFileName);
                break;
            }
            case 's': {                          // save binary data to a file
                strncpy(savFileName, optarg, sizeof(savFileName));
                printf("-s %s\n", savFileName);
                break;
            }
            case 'm': {                                  // Matlab file output
                strncpy(matFileName, optarg, sizeof(matFileName));
                printf("-m %s\n", matFileName);
                break;
            }
            case 'c': {                                            // COM port
                if ((link != NO_LINK) && (link != SERIAL_LINK)) {
                    printf("The -c option is incompatible with -p/-f\n");
                    return -1;                                      // failure
                }
                strncpy(comPort, optarg, sizeof(comPort));
                printf("-c %s\n", comPort);
                link = SERIAL_LINK;
                break;
            }
            case 'b': {                                           // baud rate
                if ((link != NO_LINK) && (link != SERIAL_LINK)) {
                    printf("The -b option is incompatible with -p/-f\n");
                    return -1;                                      // failure
                }
                if (sscanf(optarg, "%d", &baudRate) != 1) {
                    printf("incorrect baud rate: %s\n", optarg);
                    return -1;                                      // failure
                }
                printf("-b %d\n", baudRate);
                link = SERIAL_LINK;
                break;
            }
            case 'f': {                                          // File input
                if (link != NO_LINK) {
                    printf("The -f option is incompatible with -c/-b/-p\n");
                    return -1;                                      // failure
                }
                strncpy(inpFileName, optarg, sizeof(inpFileName));
                printf("-f %s\n", inpFileName);
                link = FILE_LINK;
                break;
            }
            case 't': {                                        // TCP/IP input
                if ((link != NO_LINK) && (link != TCP_LINK)) {
                    printf("The -t option is incompatible with -c/-b/-f\n");
                    return -1;
                }
                printf("-t\n");
                link = TCP_LINK;
                break;
            }
            case 'p': {                                         // TCP/IP port
                if ((link != NO_LINK) && (link != TCP_LINK)) {
                    printf("The -p option is incompatible with -c/-b/-f\n");
                    return -1;
                }
                tcpPort = (int)strtoul(optarg, NULL, 10);
                printf("-p %d\n", tcpPort);
                link = TCP_LINK;
                break;
            }
            case 'T': {                                      // timestamp size
                tstampSize = (uint8_t)strtoul(optarg, 0, 10);
                break;
            }
            case 'F': {                               // function pointer size
                funPtrSize = (uint8_t)strtoul(optarg, 0, 10);
                break;
            }
            case 'O': {                                 // object pointer size
                objPtrSize = (uint8_t)strtoul(optarg, 0, 10);
                break;
            }
            case 'S': {                                         // signal size
                sigSize = (uint8_t)strtoul(optarg, 0, 10);
                break;
            }
            case 'E': {                                         // event size
                evtSize = (uint8_t)strtoul(optarg, 0, 10);
                break;
            }
            case 'Q': {                                  // Queue counter size
                queueCtrSize = (uint8_t)strtoul(optarg, 0, 10);
                break;
            }
            case 'P': {                            // Memory-pool counter size
                poolCtrSize = (uint8_t)strtoul(optarg, 0, 10);
                break;
            }
            case 'B': {                          // Memory-pool blocksize size
                poolBlkSize = (uint8_t)strtoul(optarg, 0, 10);
                break;
            }
            case 'C': {                             // Time event counter size
                tevtCtrSize = (uint8_t)strtoul(optarg, 0, 10);
                break;
            }
            case 'h':                                                  // help
            default: {                                       // unknown option
                printf(help);
                return -1;                                     // error return
            }
        }
    }
    if (argc != optind) {
        printf(help);
        return -1;
    }

    // configure the Quantum Spy
    FILE *outFile = (outFileName[0] != '\0'
                     ? fopen(outFileName, "w")
                     : (FILE *)0);
    FILE *matFile = (matFileName[0] != '\0'
                     ? fopen(matFileName, "w")
                     : (FILE*)0);
    FILE *savFile = (savFileName[0] != '\0'
                     ? fopen(savFileName, "wb")     // open for writing binary
                     : (FILE*)0);
    qsConfig(quiet,
             objPtrSize,
             funPtrSize,
             tstampSize,
             sigSize,
             evtSize,
             queueCtrSize,
             poolCtrSize,
             poolBlkSize,
             tevtCtrSize,
             outFile,
             matFile);

    static unsigned char buf[BUF_SIZE];
    int n;

    switch (link) {
        case NO_LINK:                            // intentionally fall through
        case SERIAL_LINK: {           // input trace data from the serial port
            if (!comOpen(comPort, baudRate)) {
                return -1;
            }
            else {
                printf("\nSerial port %s opened, hit any key to quit...\n\n",
                       comPort);
            }
            while ((n = comRead(buf, sizeof(buf))) != -1) {
                if (n > 0) {
                    qsParse(buf, n);
                    if (savFile != (FILE *)0) {
                        fwrite(buf, 1, n, savFile);
                    }
                }
            }
            comClose();
            break;
        }
        case FILE_LINK: {                      // input trace data from a file
            FILE *f = fopen(inpFileName, "rb");     // open for reading binary
            if (f == (FILE *)0) {
                printf("file %s not found\n", inpFileName);
                return -1;
            }
            do {
                n = fread(buf, 1, sizeof(buf), f);
                qsParse(buf, n);
            } while (n == sizeof(buf));

            fclose(f);
            break;
        }
        case TCP_LINK: {                 // input trace data from the TCP port
            if (!tcpOpen(tcpPort)) {
                return -1;
            }
            else {
                printf("\nTCP/IP port %d opened, "
                       "hit any key to quit...\n"
                       "(the target must be stopped)\n",
                       tcpPort);
            }
            while ((n = tcpRead(buf, sizeof(buf))) != -1) {
                if (n > 0) {
                    qsParse(buf, n);
                    if (savFile != (FILE *)0) {
                        fwrite(buf, 1, n, savFile);
                    }
                }
            }
            tcpClose();
            break;
        }
    }
    if (savFile != (FILE *)0) {
        fclose(savFile);
    }
    if (matFile != (FILE *)0) {
        fclose(matFile);
    }

    printf("\nDone.\n");
    return 0;                                                       // success
}
