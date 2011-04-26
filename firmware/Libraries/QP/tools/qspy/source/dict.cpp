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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "dict.h"

//............................................................................
static int Dictionary_comp(void const *arg1, void const *arg2) {
    uint32_t key1 = ((DictEntry const *)arg1)->key_;
    uint32_t key2 = ((DictEntry const *)arg2)->key_;
    if (key1 > key2) {
        return 1;
    }
    else if (key1 < key2) {
        return -1;
    }
    else {
        return 0;
    }
}
//............................................................................
void Dictionary::put(uint32_t key, char const *name) {
    if (find(key) < 0) {                                    // is the key new?
        assert(entries_ < capacity_ - 1);
        sto_[entries_].key_ = key;
        strncpy(sto_[entries_].name_, name, sizeof(sto_[entries_].name_));
        ++entries_;
                                         // keep the entries sorted by the key
        qsort(sto_, (size_t)entries_, sizeof(sto_[0]), &Dictionary_comp);
    }
}
//............................................................................
char const *Dictionary::get(uint32_t key) {
    if (key == 0) {
        return "NULL";
    }
    int idx = find(key);
    if (idx >= 0) {
        return sto_[idx].name_;
    }
    else {
        // entry not found
        sprintf(notFound_.name_, "%08X", key);
        return notFound_.name_;
    }
}
//............................................................................
int Dictionary::find(uint32_t key) {
                                                // binary search algorithm ...
    int mid;
    int first = 0;
    int last = entries_;
    while (first <= last) {
        mid = (first + last) / 2;
        if (sto_[mid].key_ == key) {
            return mid;
        }
        if (sto_[mid].key_ > key) {
            last = mid - 1;
        }
        else {
            first = mid + 1;
        }
    }

    // entry not found
    return -1;
}

//----------------------------------------------------------------------------
static int SigDictionary_comp(void const *arg1, void const *arg2) {
    uint32_t sig1 = ((SigDictEntry const *)arg1)->sig_;
    uint32_t sig2 = ((SigDictEntry const *)arg2)->sig_;
    if (sig1 > sig2) {
        return 1;
    }
    else if (sig1 < sig2) {
        return -1;
    }
    else {                                                     // sig1 == sig2
        return 0;
    }
}
//............................................................................
void SigDictionary::put(uint32_t sig, uint32_t obj, char const *name) {
    if (find(sig, obj) < 0) {                        // is the (sig, obj) new?
        assert(entries_ < capacity_ - 1);
        sto_[entries_].sig_ = sig;
        sto_[entries_].obj_ = obj;
        strncpy(sto_[entries_].name_, name, sizeof(sto_[entries_].name_));
        ++entries_;
                                         // keep the entries sorted by the sig
        qsort(sto_, (size_t)entries_, sizeof(sto_[0]), &SigDictionary_comp);
    }
}
//............................................................................
char const *SigDictionary::get(uint32_t sig, uint32_t obj) {
    if (sig == 0) {
        return "NULL";
    }
    int idx = find(sig, obj);
    if (idx >= 0) {
        return sto_[idx].name_;
    }
    else {
        // entry not found
        sprintf(notFound_.name_, "%08X,Obj=%08X", sig, obj);
        return notFound_.name_;
    }
}
//............................................................................
int SigDictionary::find(uint32_t sig, uint32_t obj) {
                                                // binary search algorithm ...
    int mid;
    int first = 0;
    int last = entries_;
    while (first <= last) {
        mid = (first + last) / 2;
        if (sto_[mid].sig_ == sig) {
            if (obj == 0) {                                  // global signal?
                return mid;
            }
            else {                             // signal specific to an object
                first = mid;
                do {
                    if ((sto_[first].obj_ == 0)
                        || (sto_[first].obj_ == obj))
                    {
                        return first;
                    }
                    --first;
                } while ((first >= 0) && (sto_[first].sig_ == sig));
                last = mid + 1;
                while ((last < entries_) && (sto_[last].sig_ == sig)) {
                    if ((sto_[last].obj_ == 0)
                        || (sto_[last].obj_ == obj))
                    {
                        return last;
                    }
                    ++last;
                }
                // entry not found
                return -1;
            }
        }
        if (sto_[mid].sig_ > sig) {
            last = mid - 1;
        }
        else {
            first = mid + 1;
        }
    }

    // entry not found
    return -1;
}
