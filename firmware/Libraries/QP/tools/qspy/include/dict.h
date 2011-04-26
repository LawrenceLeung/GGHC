//////////////////////////////////////////////////////////////////////////////
// Product: Quantum Spy -- Dictionaries
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
#ifndef dict_h
#define dict_h

//............................................................................
struct DictEntry {
    uint32_t key_;
    char name_[32];
};

class Dictionary {
public:
    Dictionary(DictEntry *sto, uint32_t capacity)
        : sto_(sto), capacity_(capacity), entries_(0)
    {}
    void put(uint32_t key, char const *name);
    char const *get(uint32_t key);

private:
    int find(uint32_t key);

private:
    DictEntry notFound_;
    DictEntry *sto_;
    int capacity_;
    int entries_;
};

//............................................................................
struct SigDictEntry {
    uint32_t sig_;
    uint32_t obj_;
    char name_[32];
};

class SigDictionary {
public:
    SigDictionary(SigDictEntry *sto, uint32_t capacity)
        : sto_(sto), capacity_(capacity), entries_(0)
    {}
    void put(uint32_t sig, uint32_t obj, char const *name);
    char const *get(uint32_t sig, uint32_t obj);

private:
    int find(uint32_t sig, uint32_t obj);

private:
    SigDictEntry notFound_;
    SigDictEntry *sto_;
    int capacity_;
    int entries_;
};


#endif                                                               // dict_h

