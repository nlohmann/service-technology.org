/*****************************************************************************\
 Cosme -- Checking Simulation, Matching, and Equivalence

 Copyright (c) 2010 Andreas Lehmann

 Cosme is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Cosme is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Cosme.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef __BITSET_H
#define __BITSET_H

#include "BitSetC.h"

BitSetC::BitSetC(size_t bits, bool makeEmpty) {
    this->mBits = new bitset_base_t[BITNSLOTS(bits)];
    this->mBitCount = bits;

    for (size_t i = 0; i < BITNSLOTS(this->mBitCount); i++) {
        this->mBits[i] = (makeEmpty) ? BITSET_BASE_T_MIN : BITSET_BASE_T_MAX;
    }
}

BitSetC::BitSetC(const BitSetC& other) {
    this->mBits = new bitset_base_t[BITNSLOTS(other.bitCount())];
    this->mBitCount = other.bitCount();

    for (size_t i = 0; i < BITNSLOTS(this->mBitCount); i++) {
        this->mBits[i] = other.getSlot(i);
    }
}

BitSetC::~BitSetC() {
    delete[] this->mBits;
}

void BitSetC::makeIntersection(BitSetC* a) {
    assert(a != NULL);
    assert(this->mBitCount == a->bitCount());

    for (size_t i = 0; i < BITNSLOTS(this->mBitCount); i++) {
        this->mBits[i] &= a->getSlot(i);
    }

}

void BitSetC::makeUnion(BitSetC* a) {
    assert(a != NULL);
    assert(this->mBitCount == a->bitCount());

    for (size_t i = 0; i < BITNSLOTS(this->mBitCount); i++) {
        this->mBits[i] |= a->getSlot(i);
    }
}

void BitSetC::makeDifference(BitSetC* a) {
    assert(a != NULL);
    assert(this->mBitCount == a->bitCount());

    for (size_t i = 0; i < BITNSLOTS(this->mBitCount); i++) {
        this->mBits[i] &= (~a->getSlot(i));
    }
}

bool BitSetC::isIntersectionEmpty(BitSetC* a) const {
    assert(a != NULL);
    assert(this->mBitCount == a->bitCount());

    for (size_t i = 0; i < BITNSLOTS(this->mBitCount); i++) {
        if ((this->mBits[i] & a->getSlot(i)) != 0) {
            return false;
        }
    }
    return true;
}

bool BitSetC::isSubset(BitSetC* a) const {
    assert(a != NULL);
    assert(this->mBitCount == a->bitCount());

    for (size_t i = 0; i < BITNSLOTS(this->mBitCount); i++) {
        if ((this->mBits[i] & (~a->getSlot(i))) != 0) {
            return false;
        }
    }
    return true;
}

bool BitSetC::isEqual(BitSetC* a) const {
    assert(a != NULL);
    assert(this->mBitCount == a->bitCount());

    for (size_t i = 0; i < BITNSLOTS(this->mBitCount); i++) {
        if ((this->mBits[i] ^ a->getSlot(i)) != 0) {
            return false;
        }
    }
    return true;
}

std::string BitSetC::toString() const {
    std::stringstream tmpStr;

    tmpStr << "(" << this->mBitCount << ") ";
    for (size_t i = 0; i < this->mBitCount; i++) {
        if (BITTEST(this->mBits, i)) {
            tmpStr << '1';
        } else {
            tmpStr << '0';
        }
    }
    return tmpStr.str();
}

#endif //__BITSET_H
