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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "qspy.h"
#include "dict.h"
#define Q_SPY
#include "qs.h"                                      // the target-resident QS

//............................................................................
class QSpyRecord {
    uint8_t m_rec;                            // enumerated type of the record
    uint8_t const *m_pos;                    // current position in the stream
    int32_t m_len;        // current length of the stream (till the last byte)

public:
    QSpyRecord(uint8_t rec, uint8_t const *pos, int32_t len)
        : m_rec(rec), m_pos(pos), m_len(len) {
    }
    void parse(void);
    void parseUser(void);
    void error(void);
    bool OK(void);
    int32_t getInt(uint8_t size);
    uint32_t getUint(uint8_t size);
    char const *getStr(void);
    uint8_t const *getMem(uint8_t *pl);
    char const *getMatDict(char const *s);            // get MATLAB dictionary

};

//............................................................................
static DictEntry     l_funSto[512];
static DictEntry     l_objSto[256];
static SigDictEntry  l_sigSto[256];
static Dictionary    l_funDict(l_funSto,
                               sizeof(l_funSto)/sizeof(l_funSto[0]));
static Dictionary    l_objDict(l_objSto,
                               sizeof(l_objSto)/sizeof(l_objSto[0]));
static SigDictionary l_sigDict(l_sigSto,
                               sizeof(l_sigSto)/sizeof(l_sigSto[0]));

//............................................................................
static uint8_t l_objPtrSize;
static uint8_t l_funPtrSize;
static uint8_t l_tstampSize;
static uint8_t l_sigSize;
static uint8_t l_evtSize;
static uint8_t l_queueCtrSize;
static uint8_t l_poolCtrSize;
static uint8_t l_poolBlkSize;
static uint8_t l_tevtCtrSize;
static uint8_t l_quiet;
static FILE *  l_outFile;
static FILE *  l_matFile;
static char    l_line[512];            // last formatted line ready for output

//............................................................................
static void print(void) {
    if (!l_quiet) {
        fputs(l_line, stdout);
    }
    if (l_outFile != (FILE *)0) {
        fputs(l_line, l_outFile);
    }
}
//............................................................................
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
              FILE   *matFile)
{
    l_quiet        = quiet;
    l_objPtrSize   = objPtrSize;
    l_funPtrSize   = funPtrSize;
    l_tstampSize   = tstampSize;
    l_sigSize      = sigSize;
    l_evtSize      = evtSize;
    l_queueCtrSize = queueCtrSize;
    l_poolCtrSize  = poolCtrSize;
    l_poolBlkSize  = poolBlkSize;
    l_tevtCtrSize  = tevtCtrSize;
    l_outFile      = outFile;
    l_matFile      = matFile;

    if (l_outFile != (FILE *)0) {
        time_t now = time(NULL);
        fprintf(l_outFile, "QSPY host application %s\n"
                           "Copyright (c) Quantum Leaps, LLC.\n"
                           "%s\n", QSPY_VER, ctime(&now));
    }

    sprintf(l_line, "-T %d\n", (unsigned)tstampSize);   print();
    sprintf(l_line, "-O %d\n", (unsigned)objPtrSize);   print();
    sprintf(l_line, "-F %d\n", (unsigned)funPtrSize);   print();
    sprintf(l_line, "-S %d\n", (unsigned)sigSize);      print();
    sprintf(l_line, "-E %d\n", (unsigned)evtSize);      print();
    sprintf(l_line, "-Q %d\n", (unsigned)queueCtrSize); print();
    sprintf(l_line, "-P %d\n", (unsigned)poolCtrSize);  print();
    sprintf(l_line, "-B %d\n", (unsigned)poolBlkSize);  print();
    sprintf(l_line, "-C %d\n", (unsigned)tevtCtrSize);  print();
    sprintf(l_line, "\n");                              print();
}
//............................................................................
bool QSpyRecord::OK(void) {
    if (m_len != (uint8_t)0) {
        sprintf(l_line, "********** %03u: Error %d bytes unparsed\n",
               (unsigned)m_rec, (int)m_len);
        print();
        return false;
    }
    return true;
}
//............................................................................
int32_t QSpyRecord::getInt(uint8_t size) {
    int32_t ret = (int32_t)0;

    if (m_len >= size) {
        if (size == (uint8_t)1) {
            ret = (int32_t)((uint32_t)m_pos[0] << 24);
            ret >>= 24;                                         // sign-extend
        }
        else if (size == (uint8_t)2) {
            ret = (int32_t)((uint32_t)m_pos[1] << 24)
                            | ((uint32_t)m_pos[0] << 16);
            ret >>= 16;                                         // sign-extend
        }
        else if (size == (uint8_t)4) {
            ret = (int32_t)((((((uint8_t)m_pos[3] << 8)
                            | (uint8_t)m_pos[2]) << 8)
                              | (uint8_t)m_pos[1]) << 8)
                                | (uint8_t)m_pos[0];
        }
        else {
            assert(0);
        }
        m_pos += size;
        m_len -= size;
    }
    else {
        m_len = -1;
        sprintf(l_line, "Uint overrun\n");
        print();
    }
    return ret;
}
//............................................................................
uint32_t QSpyRecord::getUint(uint8_t size) {
    uint32_t ret = (uint32_t)0;

    if (m_len >= size) {
        if (size == (uint8_t)1) {
            ret = (uint32_t)m_pos[0];
        }
        else if (size == (uint8_t)2) {
            ret = (((uint32_t)m_pos[1] << 8) | (uint32_t)m_pos[0]);
        }
        else if (size == (uint8_t)4) {
            ret = ((((((uint32_t)m_pos[3] << 8)
                        | (uint32_t)m_pos[2]) << 8)
                          | (uint32_t)m_pos[1]) << 8)
                            | (uint32_t)m_pos[0];
        }
        else {
            assert(0);
        }
        m_pos += size;
        m_len -= size;
    }
    else {
        m_len = -1;
        sprintf(l_line, "Uint overrun\n");
        print();
    }
    return ret;
}
//............................................................................
char const *QSpyRecord::getStr(void) {
    uint8_t const *p;
    int32_t l;

    for (l = m_len, p = m_pos; l > 0; --l, ++p) {
        if (*p == (uint8_t)0) {
            char const *s = (char const *)m_pos;
            m_len = l - 1;
            m_pos = p + 1;
            return s;
        }
    }
    m_len = -1;
    sprintf(l_line, "String overrun\n");
    print();
    return "";
}
//............................................................................
uint8_t const *QSpyRecord::getMem(uint8_t *pl) {

    if ((m_len >= 1) && ((*m_pos) <= m_len - 1)) {
        uint8_t const *mem = m_pos + 1;
        *pl = *m_pos;
        m_len -= 1 + *m_pos;
        m_pos += 1 + *m_pos;
        return mem;
    }

    m_len = -1;
    *pl = (uint8_t)0;
    sprintf(l_line, "Mem overrun\n");
    print();

    return (uint8_t *)0;
}
//............................................................................
char const *QSpyRecord::getMatDict(char const *s) {
    static char dict[65];
    char *pc = dict;
    while ((*s != '\0') && (pc < &dict[sizeof(dict)] - 1)) {
        if ((*s == '[') || (*s == ']')
            || (*s == '.') || (*s == ':'))
        {
            *pc++ = '_';
            ++s;
        }
        else {
            *pc++ = *s++;
        }
    }
    *pc = '\0';
    return (dict[0] == '&' ? dict+1 : dict);
}
//............................................................................
void QSpyRecord::parseUser(void) {
    uint32_t f;
    int32_t i;
    uint32_t u;
    static char const *ifmt[] = {
        "%li ",  "%1i ",  "%2i ",  "%3i ",
        "%4i ",  "%5i ",  "%6i ",  "%7i ",
        "%8li ", "%9i ",  "%10i ", "%11i ",
        "%12i ", "%13i ", "%14i ", "%15i "
    };
    static char const *ufmt[] = {
        "%lu ",  "%1u ",  "%2u ",  "%3u ",
        "%4u ",  "%5u ",  "%6u ",  "%7u ",
        "%8lu ", "%9u ",  "%10u ", "%11u ",
        "%12u ", "%13u ", "%14u ", "%15u "
    };
    static char const *efmt[] = {
        "%7.0Le ",   "%9.1Le ",   "%10.2Le ",  "%11.3Le ",
        "%12.4Le ",  "%13.5Le ",  "%14.6Le ",  "%15.7Le ",
        "%16.8Le ",  "%17.9Le ",  "%18.10Le ", "%19.11Le ",
        "%20.12Le ", "%21.13Le ", "%22.14Le ", "%23.15Le ",
    };

    u = getUint(l_tstampSize);
    sprintf(l_line, "%010u User%03u: ", u, (unsigned)(m_rec - QS_USER));
    print();

    if (l_matFile != (FILE *)0) {
        fprintf(l_matFile, "%3d %10u ", m_rec, u);
    }

    while (m_len > 0) {
        char const *s;
        f = getUint(1);                                 // get the format byte

        switch (f & 0x0F) {
            case QS_I8_T: {
                i = getInt(1);
                sprintf(l_line, ifmt[f >> 4], i);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, ifmt[f >> 4], i);
                }
                break;
            }
            case QS_U8_T: {
                u = getUint(1);
                sprintf(l_line, ufmt[f >> 4], u);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, ufmt[f >> 4], u);
                }
                break;
            }
            case QS_I16_T: {
                i = getInt(2);
                sprintf(l_line, ifmt[f >> 4], i);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, ifmt[f >> 4], i);
                }
                break;
            }
            case QS_U16_T: {
                u = getUint(2);
                sprintf(l_line, ufmt[f >> 4], u);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, ufmt[f >> 4], u);
                }
                break;
            }
            case QS_I32_T: {
                i = getInt(4);
                sprintf(l_line, ifmt[f >> 4], i);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, ifmt[f >> 4], i);
                }
                break;
            }
            case QS_U32_T: {
                u = getUint(4);
                sprintf(l_line, ufmt[f >> 4], u);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, ufmt[f >> 4], u);
                }
                break;
            }
            case QS_F32_T: {
                u = (uint32_t)getUint(4);
                sprintf(l_line, efmt[f >> 4], (double)(*((float *)&u)));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, efmt[f >> 4],
                            (double)(*((float *)&u)));
                }
                break;
            }
            case QS_F64_T: {
                union F64Rep {
                    double d;
                    struct UInt2 {
                        uint32_t u1, u2;
                    } i;
                } data;
                data.i.u1 = (uint32_t)getUint(4);
                data.i.u2 = (uint32_t)getUint(4);
                sprintf(l_line, efmt[f >> 4], data.d);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, efmt[f >> 4], data.d);
                }
                break;
            }
            case QS_STR_T: {
                s = getStr();
                sprintf(l_line, "%s ", s);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%s ", s);
                }
                break;
            }
            case QS_MEM_T: {
                uint8_t l;
                uint8_t const *mem = getMem(&l);
                while (l-- != (uint8_t)0) {
                    sprintf(l_line, "%02X ", (int)*mem);
                    print();
                    if (l_matFile != (FILE *)0) {
                        fprintf(l_matFile, "%03d ", (int)*mem);
                    }
                    ++mem;
                }
                break;
            }
            case QS_SIG_T: {
                u = getUint(l_sigSize);
                uint32_t o = getUint(l_objPtrSize);
                sprintf(l_line, "%s,Obj=%s ",
                       l_sigDict.get(u, o),
                       l_objDict.get(o));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%04u %10u ", u, o);
                }
                break;
            }
            case QS_OBJ_T: {
                u = getUint(l_objPtrSize);
                sprintf(l_line, "%s ", l_objDict.get(u));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%10u ", u);
                }
                break;
            }
            case QS_FUN_T: {
                u = getUint(l_funPtrSize);
                sprintf(l_line, "%s ", l_funDict.get(u));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%10u ", u);
                }
                break;
            }
            default: {
                sprintf(l_line, "Unknown format\n");
                print();
                m_len = -1;
                break;
            }
        }
    }
    sprintf(l_line, "\n");
    print();
    if (l_matFile != (FILE *)0) {
        fprintf(l_matFile, "\n");
    }
}
//............................................................................
void QSpyRecord::parse(void) {
    uint32_t t, a, b, c, d, e;
    char const *s;

    switch (m_rec) {

        // QEP records .......................................................
        case QS_QEP_STATE_ENTRY: {
            a = getUint(l_objPtrSize);
            b = getUint(l_funPtrSize);
            if (OK()) {
                sprintf(l_line, "           Q_ENTRY: "
                       "Obj=%s State=%s\n",
                       l_objDict.get(a),
                       l_funDict.get(b));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u\n",
                            m_rec, a, b);
                }
            }
            break;
        }
        case QS_QEP_STATE_EXIT: {
            a = getUint(l_objPtrSize);
            b = getUint(l_funPtrSize);
            if (OK()) {
                sprintf(l_line, "           Q_EXIT : "
                       "Obj=%s State=%s\n",
                       l_objDict.get(a),
                       l_funDict.get(b));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u\n",
                            m_rec, a, b);
                }
            }
            break;
        }
        case QS_QEP_STATE_INIT: {
            a = getUint(l_objPtrSize);
            b = getUint(l_funPtrSize);
            c = getUint(l_funPtrSize);
            if (OK()) {
                sprintf(l_line, "           Q_INIT : "
                       "Obj=%s Source=%s Target=%s\n",
                       l_objDict.get(a),
                       l_funDict.get(b),
                       l_funDict.get(c));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u %10u\n",
                            m_rec, a, b, c);
                }
            }
            break;
        }
        case QS_QEP_INIT_TRAN: {
            t = getUint(l_tstampSize);
            a = getUint(l_objPtrSize);
            b = getUint(l_funPtrSize);
            if (OK()) {
                sprintf(l_line, "%010u ==>Init: "
                       "Obj=%s New=%s\n",
                       t,
                       l_objDict.get(a),
                       l_funDict.get(b));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u %10u\n",
                            m_rec, t, a, b);
                }
            }
            break;
        }
        case QS_QEP_INTERN_TRAN: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(l_objPtrSize);
            c = getUint(l_funPtrSize);
            if (OK()) {
                sprintf(l_line, "%010u Intern : "
                       "Obj=%s Sig=%s Source=%s\n",
                       t,
                       l_objDict.get(b),
                       l_sigDict.get(a, b),
                       l_funDict.get(c));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %10u %10u\n",
                            m_rec, t, a, b, c);
                }
            }
            break;
        }
        case QS_QEP_TRAN: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(l_objPtrSize);
            c = getUint(l_funPtrSize);
            d = getUint(l_funPtrSize);
            if (OK()) {
                sprintf(l_line, "%010u ==>Tran: "
                       "Obj=%s Sig=%s Source=%s New=%s\n",
                       t,
                       l_objDict.get(b),
                       l_sigDict.get(a, b),
                       l_funDict.get(c),
                       l_funDict.get(d));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %10u %10u %10u\n",
                            m_rec, t, a, b, c, d);
                }
            }
            break;
        }
        case QS_QEP_IGNORED: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(l_objPtrSize);
            c = getUint(l_funPtrSize);
            if (OK()) {
                sprintf(l_line, "%010u Ignored: "
                       "Obj=%s Sig=%s Active=%s\n",
                       t,
                       l_objDict.get(b),
                       l_sigDict.get(a, b),
                       l_funDict.get(c));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %10u %10u\n",
                            m_rec, t, a, b, c);
                }
            }
            break;
        }
        case QS_QEP_DISPATCH: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(l_objPtrSize);
            c = getUint(l_funPtrSize);
            if (OK()) {
                sprintf(l_line, "%010u Disp==>: "
                       "Obj=%s Sig=%s Active=%s\n",
                       t,
                       l_objDict.get(b),
                       l_sigDict.get(a, b),
                       l_funDict.get(c));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %10u %10u\n",
                            m_rec, t, a, b, c);
                }
            }
            break;
        }

        // QF records ........................................................
        case QS_QF_ACTIVE_ADD:
        case QS_QF_ACTIVE_REMOVE: {
            t = getUint(l_tstampSize);
            a = getUint(l_objPtrSize);
            b = getUint(1);
            if (OK()) {
                sprintf(l_line, "%010u AO.%s : "
                       "Active=%s Prio=%2u\n",
                       t,
                       (m_rec == QS_QF_ACTIVE_ADD ? "ADD" : "REM"),
                       l_objDict.get(a),
                       b);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u %2u\n",
                            m_rec, t, a, b);
                }
            }
            break;
        }
        case QS_QF_ACTIVE_SUBSCRIBE:
        case QS_QF_ACTIVE_UNSUBSCRIBE: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(l_objPtrSize);
            if (OK()) {
                sprintf(l_line, "%010u AO.%s: "
                       "Active=%s Sig=%s\n",
                       t,
                       (m_rec == QS_QF_ACTIVE_SUBSCRIBE ? "SUB " : "USUB"),
                       l_objDict.get(b),
                       l_sigDict.get(a, b));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %10u\n",
                            m_rec, t, a, b);
                }
            }
            break;
        }
        case QS_QF_ACTIVE_POST_FIFO:
        case QS_QF_ACTIVE_POST_LIFO: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(l_objPtrSize);
            c = getUint(1);
            d = getUint(l_queueCtrSize);
            e = getUint(l_queueCtrSize);
            if (OK()) {
                sprintf(l_line, "%010u AO.%cIFO: Obj=%s "
                       "Evt(Sig=%s, Pool=%1u, Ref=%2u) "
                       "Queue(nFree=%3u, nMin=%3u)\n",
                       t,
                       (m_rec == QS_QF_ACTIVE_POST_FIFO ? 'F' : 'L'),
                       l_objDict.get(b),
                       l_sigDict.get(a, b),
                       (uint32_t)(c >> 6), (uint32_t)(c & 0x3F),
                       d, e);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %10u %3u %3u %4u %4u\n",
                            m_rec, t, a, b,
                            (uint32_t)(c >> 6), (uint32_t)(c & 0x3F),
                            d, e);
                }
            }
            break;
        }
        case QS_QF_ACTIVE_GET:
        case QS_QF_EQUEUE_GET: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(l_objPtrSize);
            c = getUint(1);
            d = getUint(l_queueCtrSize);
            if (OK()) {
                sprintf(l_line, "%010u %s "
                       "%s Evt(Sig=%s, Pool=%1u, Ref=%2u) "
                       "Queue(nUsed=%3u)\n",
                       t,
                       (m_rec == QS_QF_ACTIVE_GET ? "AO.GET : Active="
                                                  : "EQ.GET : EQueue="),
                       l_objDict.get(b),
                       l_sigDict.get(a, b),
                       (uint32_t)(c >> 6), (uint32_t)(c & 0x3F),
                       d);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %10u %3u %3u %4u\n",
                            m_rec, t, a, b,
                            (uint32_t)(c >> 6), (uint32_t)(c & 0x3F),
                            d);
                }
            }
            break;
        }
        case QS_QF_ACTIVE_GET_LAST:
        case QS_QF_EQUEUE_GET_LAST: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(l_objPtrSize);
            c = getUint(1);
            if (OK()) {
                sprintf(l_line, "%010u %s "
                       "%s Evt(Sig=%s, Pool=%1u, Ref=%2u)\n",
                       t,
                       (m_rec == QS_QF_ACTIVE_GET_LAST ? "AO.GETL: Active="
                                                       : "EQ.GETL: EQueue="),
                       l_objDict.get(b),
                       l_sigDict.get(a, b),
                       (uint32_t)(c >> 6), (uint32_t)(c & 0x3F));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %10u %3u %3u\n",
                            m_rec, t, a, b,
                            (uint32_t)(c >> 6), (uint32_t)(c & 0x3F));
                }
            }
            break;
        }
        case QS_QF_EQUEUE_INIT: {
            a = getUint(l_objPtrSize);
            b = getUint(l_queueCtrSize);
            if (OK()) {
                sprintf(l_line, "           EQ.INIT: Obj=%s Len=%2u\n",
                       l_objDict.get(a),
                       b);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u\n",
                            m_rec, a, b);
                }
            }
            break;
        }
        case QS_QF_EQUEUE_POST_FIFO:
        case QS_QF_EQUEUE_POST_LIFO: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(l_objPtrSize);
            c = getUint(1);
            d = getUint(l_queueCtrSize);
            e = getUint(l_queueCtrSize);
            if (OK()) {
                sprintf(l_line, "%010u EQ.%cIFO: Obj=%s "
                       "Evt(Sig=%s, Pool=%1u, Ref=%2u) "
                       "Queue(nFree=%3u, nMin=%3u)\n",
                       t,
                       (m_rec == QS_QF_EQUEUE_POST_FIFO ? 'F' : 'L'),
                       l_objDict.get(b),
                       l_sigDict.get(a, b),
                       (uint32_t)(c >> 6), (uint32_t)(c & 0x3F),
                       d, e);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %10u %3u %3u %4u %4u\n",
                            m_rec, t, a, b,
                            (uint32_t)(c >> 6), (uint32_t)(c & 0x3F),
                            d, e);
                }
            }
            break;
        }
        case QS_QF_MPOOL_INIT: {
            a = getUint(l_objPtrSize);
            b = getUint(l_poolCtrSize);
            if (OK()) {
                sprintf(l_line, "           MP.INIT: Obj=%s nBlocks=%4u\n",
                       l_objDict.get(a),
                       b);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u\n",
                            m_rec, a, b);
                }
            }
            break;
        }
        case QS_QF_MPOOL_GET: {
            t = getUint(l_tstampSize);
            a = getUint(l_objPtrSize);
            b = getUint(l_poolCtrSize);
            c = getUint(l_poolCtrSize);
            if (OK()) {
                sprintf(l_line, "%010u MP.GET : Obj=%s "
                       "nFree=%4u nMin=%4u\n",
                       t,
                       l_objDict.get(a),
                       b, c);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u %4u %4u\n",
                            m_rec, t, a, b, c);
                }
            }
            break;
        }
        case QS_QF_MPOOL_PUT: {
            t = getUint(l_tstampSize);
            a = getUint(l_objPtrSize);
            b = getUint(l_poolCtrSize);
            if (OK()) {
                sprintf(l_line, "%010u MP.PUT : Obj=%s "
                       "nFree=%4u\n",
                       t,
                       l_objDict.get(a),
                       b);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u %4u\n",
                            m_rec, t, a, b);
                }
            }
            break;
        }
        case QS_QF_PUBLISH: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(1);
            if (OK()) {
                sprintf(l_line, "%010u PUBLISH: "
                       "Evt(Sig=%s, Pool=%1u, Ref=%2u)\n",
                       t,
                       l_sigDict.get(a, 0),
                       (uint32_t)(b >> 6), (uint32_t)(b & 0x3F));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %10u\n",
                            m_rec, t, a,
                            (uint32_t)(b >> 6), (uint32_t)(b & 0x3F));
                }
            }
            break;
        }
        case QS_QF_NEW: {
            t = getUint(l_tstampSize);
            a = getUint(l_evtSize);
            c = getUint(l_sigSize);
            if (OK()) {
                sprintf(l_line, "%010u NEW    : Evt(Sig=%s, size=%5u)\n",
                       t,
                       l_sigDict.get(c, 0),
                       a);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %4u\n",
                            m_rec, t, a, c);
                }
            }
            break;
        }
        case QS_QF_GC_ATTEMPT:
        case QS_QF_GC: {
            t = getUint(l_tstampSize);
            a = getUint(l_sigSize);
            b = getUint(1);
            if (OK()) {
                sprintf(l_line,
                       "%010u %s : Evt(Sig=%s, Pool=%1d, Ref=%2d)\n",
                       t,
                       (m_rec == QS_QF_GC_ATTEMPT ? "GC-ATT" : "GC    "),
                       l_sigDict.get(a, 0),
                       (uint32_t)(b >> 6), (uint32_t)(b & 0x3F));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %3u %3u\n",
                            m_rec, t, a,
                            (uint32_t)(b >> 6), (uint32_t)(b & 0x3F));
                }
            }
            break;
        }
        case QS_QF_TICK: {
            a = getUint(l_tevtCtrSize);
            if (OK()) {
                sprintf(l_line, "           TICK   : Ctr=%10u\n", a);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u\n", m_rec, a);
                }
            }
            break;
        }
        case QS_QF_TIMEEVT_ARM:
        case QS_QF_TIMEEVT_DISARM: {
            t = getUint(l_tstampSize);
            a = getUint(l_objPtrSize);
            b = getUint(l_objPtrSize);
            c = getUint(l_tevtCtrSize);
            d = getUint(l_tevtCtrSize);
            if (OK()) {
                sprintf(l_line, "%010u TE.%s: Obj=%s Act=%s "
                       "nTicks=%4u Interval=%4u\n",
                       t,
                       (m_rec == QS_QF_TIMEEVT_ARM ? "ARM " : "DARM"),
                       l_objDict.get(a),
                       l_objDict.get(b),
                       c, d);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u %10u %10u %10u\n",
                            m_rec, t, a, b, c, d);
                }
            }
            break;
        }
        case QS_QF_TIMEEVT_AUTO_DISARM: {
            a = getUint(l_objPtrSize);
            b = getUint(l_objPtrSize);
            if (OK()) {
                sprintf(l_line, "           TE.ADRM: Obj=%s Act=%s\n",
                       l_objDict.get(a),
                       l_objDict.get(b));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u\n",
                            m_rec, a, b);
                }
           }
            break;
        }
        case QS_QF_TIMEEVT_DISARM_ATTEMPT: {
            t = getUint(l_tstampSize);
            a = getUint(l_objPtrSize);
            b = getUint(l_objPtrSize);
            if (OK()) {
                sprintf(l_line, "%010u TE.DATT: Obj=%s Act=%s\n",
                       t,
                       l_objDict.get(a),
                       l_objDict.get(b));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u %10u\n",
                            m_rec, t, a, b);
                }
            }
            break;
        }
        case QS_QF_TIMEEVT_REARM: {
            t = getUint(l_tstampSize);
            a = getUint(l_objPtrSize);
            b = getUint(l_objPtrSize);
            c = getUint(l_tevtCtrSize);
            d = getUint(l_tevtCtrSize);
            e = getUint(1);
            if (OK()) {
                sprintf(l_line, "%010u TE.RARM: Obj=%s Act=%s "
                       "nTicks=%4u Interval=%4u WasArmed=%1u\n",
                       t,
                       l_objDict.get(a),
                       l_objDict.get(b),
                       c, d, e);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u %10u %10u %10u %1u\n",
                            m_rec, t, a, b, c, d, e);
                }
            }
            break;
        }
        case QS_QF_TIMEEVT_POST: {
            t = getUint(l_tstampSize);
            a = getUint(l_objPtrSize);
            b = getUint(l_sigSize);
            c = getUint(l_objPtrSize);
            if (OK()) {
                sprintf(l_line, "%010u TE.POST: Obj=%s Sig=%s Act=%s\n",
                       t,
                       l_objDict.get(a),
                       l_sigDict.get(b, c),
                       l_objDict.get(c));
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %10u %4u %10u\n",
                            m_rec, t, a, b, c);
                }
            }
            break;
        }
        case QS_QF_INT_LOCK:
        case QS_QF_INT_UNLOCK: {
            t = getUint(l_tstampSize);
            a = getUint(1);
            if (OK()) {
                sprintf(l_line, "%010u %s: "
                       "IntLockNest=%2d\n",
                       t,
                       (m_rec == QS_QF_INT_LOCK ? "QF_intL" : "QF_intU"),
                       a);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %3u\n",
                            m_rec, t, a);
                }
           }
            break;
        }
        case QS_QF_ISR_ENTRY:
        case QS_QF_ISR_EXIT: {
            t = getUint(l_tstampSize);
            a = getUint(1);
            b = getUint(1);
            if (OK()) {
                sprintf(l_line, "%010u %s: "
                       "IsrNest=%2u, CurrPrio=%3u\n",
                       t,
                       (m_rec == QS_QF_ISR_ENTRY ? "QF_isrE" : "QF_isrX"),
                       a, b);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %3u %3u\n",
                            m_rec, t, a, b);
                }
            }
            break;
        }

        // QK records ........................................................
        case QS_QK_MUTEX_LOCK:
        case QS_QK_MUTEX_UNLOCK: {
            t = getUint(l_tstampSize);
            a = getUint(1);
            b = getUint(1);
            if (OK()) {
                sprintf(l_line, "%010u %s: "
                       "OrgPrio=%2u, CurrPrio=%3u\n",
                       t,
                       (m_rec == QS_QK_MUTEX_LOCK ? "QK_muxL" : "QK_muxU"),
                       a, b);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %3u %3u\n",
                            m_rec, t, a, b);
                }
            }
            break;
        }
        case QS_QK_SCHEDULE: {
            t = getUint(l_tstampSize);
            a = getUint(1);
            b = getUint(1);
            if (OK()) {
                sprintf(l_line, "%010u QK_sche: "
                       "prio=%2u, pin=%3u\n",
                       t, a, b);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %3u %3u\n",
                            m_rec, t, a, b);
                }
            }
            break;
        }

        // Miscallaneous QS records ..........................................
        case QS_SIG_DICTIONARY: {
            a = getUint(l_sigSize);
            b = getUint(l_objPtrSize);
            s = getStr();
            if (OK()) {
                l_sigDict.put(a, b, s);
                sprintf(l_line, "           Sig Dic: %08X,Obj=%08X ->%s\n",
                       a, b, s);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %s=[%4u %10u];\n",
                        m_rec, getMatDict(s), a, b);
                }
            }
            break;
        }
        case QS_OBJ_DICTIONARY: {
            a = getUint(l_objPtrSize);
            s = getStr();
            if (OK()) {
                l_objDict.put(a, ((s[0] == '&') ? s + 1 : s));
                sprintf(l_line, "           Obj Dic: %08X->%s\n",
                       a, s);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %s=%10u;\n",
                            m_rec, getMatDict(s), a);
                }
            }
            break;
        }
        case QS_FUN_DICTIONARY: {
            a = getUint(l_funPtrSize);
            s = getStr();
            if (OK()) {
                l_funDict.put(a, ((s[0] == '&') ? s + 1 : s));
                sprintf(l_line, "           Fun Dic: %08X->%s\n",
                       a, s);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %s=%10u;\n",
                            m_rec, getMatDict(s), a);
                }
            }
            break;
        }
        case QS_ASSERT: {
            t = getUint(l_tstampSize);
            a = getUint(2);
            s = getStr();
            if (OK()) {
                sprintf(l_line, "%010u !ASSERT: File=%s Line=%4hu\n",
                       t, s, a);
                print();

                if (l_matFile != (FILE *)0) {
                    fprintf(l_matFile, "%3d %10u %4u %s\n",
                            m_rec, t, a, s);
                }
            }
            break;
        }

        // User records ......................................................
        default: {
            if (m_rec >= QS_USER) {
                parseUser();
            }
            else {
                sprintf(l_line, "           ???    : Rec=%3u, Len=%3d\n",
                       (unsigned)m_rec, (int)m_len);
                print();
            }
            break;
        }
    }
}

//............................................................................
#define QS_FRAME            ((uint8_t)0x7E)
#define QS_ESC              ((uint8_t)0x7D)
#define QS_GOOD_CHKSUM      ((uint8_t)0xFF)
#define QS_ESC_XOR          0x20

#define QS_MAX_RECORD_SIZE  256

void qsParse(uint8_t const *buf, size_t nBytes) {
    static uint8_t esc;
    static uint8_t seq;
    static uint8_t chksum;
    static uint8_t record[QS_MAX_RECORD_SIZE];
    static uint8_t *pos = record;                // position within the record

    while (nBytes-- != 0) {
        uint8_t b = *buf++;

        if (esc) {
            esc = (uint8_t)0;
            b ^= QS_ESC_XOR;
        }
        else {
            if (b == QS_FRAME) {
                if ((chksum == QS_GOOD_CHKSUM) && (pos >= &record[3])) {
                    if ((uint8_t)(seq + 1) != record[0]) {
                        sprintf(l_line, "*** Dropped %3u records\n",
                               (unsigned)((record[0] - seq - 1) & 0xFF));
                        print();
                    }
                    seq = record[0];

                    QSpyRecord qrec(record[1],
                                    &record[2],
                                    (uint32_t)(pos - record - 3));
                    qrec.parse();
                }
                else {
                    sprintf(l_line, "*** Incorrect record past seq=%3u\n",
                           (unsigned)seq);
                    print();
                }
                                          // get ready for the next record ...
                chksum = (uint8_t)0;
                pos = record;
                continue;
            }
            else if (b == QS_ESC) {
                esc = (uint8_t)1;
                continue;
            }
        }

        chksum = (uint8_t)(chksum + b);
        if (pos < &record[sizeof(record)]) {
            *pos++ = b;
        }
        else {
            sprintf(l_line, "Error, record too long\n");
            print();
            chksum = (uint8_t)0;
            pos = record;
            esc = (uint8_t)0;
        }
    }
}
