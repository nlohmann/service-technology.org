/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.

 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    binDecision.cc
 *
 * \brief   functions for representing a Petri net's state in a tree
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#include "owfn.h"
#include "debug.h"
#include "state.h"
#include "binDecision.h"


unsigned int bin_p;     ///< (=place); index in CurrentMarking
int bin_pb;             ///< next bit of place to be processed;
unsigned char bin_byte; ///< byte to be matched against tree vector; constructed from CurrentMarking
int bin_t;              ///< index in tree vector
unsigned char * bin_v;  ///< current tree vector
int bin_s;              ///< nr of bits pending in byte from previous iteration
int bin_d;              ///< difference position (set in binSearch for later binInsert)
int bin_dir;            ///< did we go "old" (value:0) or "new" (value:1) in last decision?
int bin_b;              ///< bit nr at start of byte
binDecision* fromdec = NULL;
binDecision* todec = NULL;     ///< next decision in bintree (binSearch)
binDecision* vectordec = NULL; ///< bindecision of bin_v (binSearch)

// to create a new set of markings, create a new binDecision* = null
// add new marking to the set with binSearch(); binInsert();
// never insert without prior search!

//! \brief constructor (2 parameters)
//! \param b bitnumber
//! \param BitVectorSize size of the bit vector
binDecision::binDecision(int b, long int BitVectorSize) :
    bitnr(b), nextold(NULL), nextnew(NULL) {

    vector = new unsigned char [(BitVectorSize - (bitnr + 2)) / 8 + 2];
    for (int i = 0; i < (BitVectorSize - (bitnr + 2))/8 + 2; ++i) {
        vector[i] = 0;
    }
}


//! \brief destructor
binDecision::~binDecision() {
	delete[] vector;

    delete state;
    delete nextold;
    delete nextnew;
}


//! \brief turns an integer into a binary number
//! \param DESCRIPTION
//! \param DESCRIPTION
//! \param DESCRIPTION
//! \param DESCRIPTION
void inttobits(unsigned char* bytepos,
               int bitpos,
               int nrbits,
               unsigned int value) {
    // store value as a bit sequence of a char array starting at byte bytepos,
    // bit bitpos and consisting of nrbits bits
    if (nrbits <= 8 - bitpos) {
        // value fits completely into first byte
        *bytepos |= value << (8 - (bitpos + nrbits));
        return;
    }

    *bytepos |= value >> (nrbits + bitpos - 8);
    nrbits -= 8 - bitpos;
    value = value % (1 << nrbits);
    bytepos++;
    while (nrbits > 8) {
        nrbits -= 8;
        *bytepos = value >> nrbits;
        bytepos++;
        value = value % (1 << nrbits);
    }

    if (nrbits) {
        *bytepos = value << 8 - nrbits;
    }

    return;
}


//! \brief computes the logarithm to base 2 of m
//! \param m integer for the computation
//! \return returns log2 of m
int logzwo(int m) {
    int k = 0;
    while (m) {
        k++;
        m = m >> 1;
    }

    return k;
}


//! \brief DESCRIPTION
//! \param PN DESCRIPTION
//! \return DESCRIPTION
State* binInsert(oWFN* PN) {
    TRACE(TRACE_5, "binInsert(oWFN* PN)\n");

    return binInsert(PN->binHashTable + (PN->getPlaceHashValue()), PN);
}


//! \brief DESCRIPTION
//! \param Bucket DESCRIPTION
//! \param PN DESCRIPTION
//! \return DESCRIPTION
State* binInsert(binDecision** Bucket, oWFN* PN) {
    // relies on a previous bin_search with all values bin_* set properly!
    TRACE(TRACE_5, "binInsert(binDecision** Bucket, oWFN* PN): start\n");

  //   cout << "binInsert: inserting "
  //        << PN->getMarkingAsString(PN->CurrentMarking) << endl;

    binDecision* newd;
    int vby, vbi;

    if (!*Bucket) {
        // empty hash bucket --> create first entry
        NonEmptyHash++;
        *Bucket = new binDecision(-1, PN->BitVectorSize);

        // 3. remaining places
        vby = vbi = 0;
        for (bin_p = 0; bin_p < PN->getPlaceCount(); bin_p++) {
            inttobits((*Bucket)->vector + vby, vbi,
                      PN->getPlace(bin_p)->nrbits, PN->CurrentMarking[bin_p]);
            vby += (vbi + PN->getPlace(bin_p) -> nrbits) / 8;
            vbi = (vbi + PN->getPlace(bin_p) -> nrbits) % 8;
        }

        (*Bucket)-> prev = NULL;
        (*Bucket)->state = new State;
        (*Bucket)->state->my_entry = *Bucket;

        // cout << "\t at address: " << (*Bucket)->state << endl;
        TRACE(TRACE_5, "binInsert(binDecision** Bucket, oWFN* PN): end\n");
        return (*Bucket)->state;
    }

    newd = new binDecision(bin_b + bin_d, PN->BitVectorSize);

    if (bin_dir) {
        newd->nextold = fromdec->nextnew;
        fromdec->nextnew = newd;
    } else {
        newd->nextold = fromdec -> nextold;
        fromdec->nextold = newd;
    }

    newd->prev = vectordec;

    // fill vector
    // 1. remaining   bin_byte
    if (bin_d < 7) {
        newd->vector[0] = bin_byte << (bin_d + 1);
        vby = 0;
        vbi = 7 - bin_d;
    } else {
        vby = 0;
        vbi = 0;
    }

    // 2. remaining bit of current place
    if (bin_pb) {
        inttobits(newd->vector, vbi, PN->getPlace(bin_p)->nrbits - bin_pb,
                  PN->CurrentMarking[bin_p] % (1 << (PN->getPlace(bin_p)->nrbits - bin_pb)));
        vby += (vbi + PN->getPlace(bin_p)->nrbits- bin_pb) / 8;
        vbi = (vbi + PN->getPlace(bin_p)->nrbits- bin_pb) % 8;
        bin_p++;
    }

    // 3. remaining places
    for (; bin_p < PN->getPlaceCount(); bin_p++) {
        inttobits(newd->vector + vby, vbi, PN->getPlace(bin_p)->nrbits,
                  PN->CurrentMarking[bin_p]);
        vby += (vbi + PN->getPlace(bin_p) -> nrbits) / 8;
        vbi = (vbi + PN->getPlace(bin_p) -> nrbits) % 8;
    }

    newd->state = new State;
    newd->state->my_entry = newd;

    // cout << "\t at address: " << newd->state << endl;

    TRACE(TRACE_5, "binInsert(binDecision** Bucket, oWFN* PN): end\n");
    return newd->state;
}


//! \brief deletes the top level state in HashTable[h]
//! \param Bucket DESCRIPTION
//! \param BitVectorSize DESCRIPTION
void binDelete(binDecision** Bucket, long int BitVectorSize) {
    // procedure:
    // - top level state has nextold, but no nextnew (no branch at -1)
    // - descend to end of nextold list (that state has largest common prefix with top level state)
    // - copy remainder of that state into top level state vector
    // - append nextnew of that state to end of nextold list (its nextold is 0)
    // - free that state's vector and decision record

    binDecision** oldlist;

    if ((*Bucket)->nextold) {
        int vbyte, obyte;
        int upper, lower;
        unsigned char *v, *o;
        v = (*Bucket)->vector;

        for (oldlist = &((*Bucket)->nextold);
             (*oldlist)->nextold; oldlist = &((*oldlist)->nextold)) {
            ;
        }

        vbyte = (*oldlist)->bitnr / 8;
        upper = (*oldlist)->bitnr % 8;
        o = (*oldlist)->vector;
        // copy state vector
        // 1. reverse branch bit
        v[vbyte] = v[vbyte] ^ (1 << (7 - upper++));

        if (upper == 8) {
            upper = 0;
            vbyte++;
        }

        // 2. copy remaining vector
        if (upper) {
            lower = 8 - upper;
            obyte = 0;
            unsigned char vuppermask = ((1 << upper) - 1) << lower ;
            unsigned char vlowermask = (1 << lower) - 1;
            unsigned char ouppermask = ((1 << lower) - 1) << upper ;
            unsigned char olowermask = (1 << upper) - 1;

            do {
                /* 1. upper part of o --> lower part of v */
                v[vbyte] = (v[vbyte] & vuppermask) |((o[obyte] & ouppermask)
                        >> upper);
                vbyte++;
                if (8 * vbyte >= BitVectorSize) {
                    break;
                }

                /* 2. lower part of o --> upper part of v */
                v[vbyte] = (v[vbyte] & vlowermask) |((o[obyte++] & olowermask)
                        << lower);

                if ((*oldlist)->bitnr + 8 * obyte >= BitVectorSize) {
                    break;
                }
            } while (true);
        } else {
            for (obyte = 0; (*oldlist)->bitnr + 8 * obyte < BitVectorSize; obyte++) {
                v[vbyte++] = o[obyte];
            }
        }

        // 3. delete vector and set pointer in decision records
        delete[] (*oldlist)->vector;
        (*oldlist) = (*oldlist)->nextnew;
    } else {
        // top level state is last state in list
        delete[] (*Bucket)->vector;
        *Bucket = NULL;
    }
}


//! \brief DESCRIPTION
//! \param d DESCRIPTION
void binDeleteAll(binDecision* d) {
    if (!d) {
        return;
    }

    binDeleteAll(d->nextold);
    binDeleteAll(d->nextnew);
    delete d;
}


//! \brief DESCRIPTION
//! \param PN DESCRIPTION
//! \param h DESCRIPTION
void binDeleteAll(oWFN* PN, int h) {
    // delete all states in hash bucket h
    if (!PN->binHashTable[h]) {
        return;
    }

    binDeleteAll(PN->binHashTable[h]->nextold);
    delete[] PN->binHashTable[h]->vector;
    PN->binHashTable[h] = NULL;
}


//! \brief DESCRIPTION
//! \param PN DESCRIPTION
//! \return DESCRIPTION
State* binSearch(oWFN* PN) {
    TRACE(TRACE_5, "binSearch(oWFN* PN)\n");
    return binSearch(PN->binHashTable[PN->getPlaceHashValue()], PN);
}


//! \brief DESCRIPTION
//! \param Bucket DESCRIPTION
//! \param PN DESCRIPTION
//! \return DESCRIPTION
State* binSearch(binDecision* Bucket, oWFN* PN) {
    // cout << "search for marking "
    //      << PN->getMarkingAsString(PN->CurrentMarking) << endl;

    bin_dir = 0;

    // search the state given in CurrentMarking in the binHashTable
    if (!(fromdec = Bucket)) {
        // cout << "\t not found!" << endl;
        return NULL;
    }

    // convert CurrentMarking (int-vector) to bit-vector and
    // compare with bin_v (one byte at a time)
    bin_p = 0;                  // index of current place in CurrentMarking
    bin_pb = 0;                 // current bit in current place
    bin_s = 0;                  // start-bit of bin_byte
    bin_t = 0;                  // index in vector bin_v
    bin_b = 0;                  //
    bin_byte = 0;               // to become one byte in the resulting bit-vector

    bin_v = fromdec->vector;    // vector to compare
    todec = fromdec->nextold;   // next decision in bintree
    vectordec = fromdec;        // bindecision of bin_v

    while (true) {
        // - fill bin_byte starting at bin_s
        while (bin_s < 8 && bin_p < PN->getPlaceCount()) {
            // still more bits to go (space in bin_byte and place remaining)
            if (8 - bin_s < PN->getPlace(bin_p)->nrbits - bin_pb) {
                // bin_byte has NOT enough space to store remaining bits of
                // place bin_p

                // shift what fits, cut the rest, and add to bin_byte
                inttobits(&bin_byte, bin_s, 8 - bin_s,
                          (PN->CurrentMarking[bin_p] % (1 << (PN->getPlace(bin_p)->nrbits - bin_pb))) >> (PN->getPlace(bin_p)->nrbits + bin_s - (8 + bin_pb)));

                // compute new indizes
                bin_pb += 8 - bin_s;
                bin_s = 8;
                break;
            } else {
                // bin_byte has enough space to store remaining bits of
                // place bin_p

                // shift what fits (fill up with zeros), and add to bin_byte
                inttobits(&bin_byte, bin_s, PN->getPlace(bin_p)->nrbits - bin_pb, PN->CurrentMarking[bin_p] %(1
                        << (PN->getPlace(bin_p)->nrbits - bin_pb)));

                // compute new indizes
                bin_s += PN->getPlace(bin_p)->nrbits - bin_pb;
                bin_p++;      // next place
                bin_pb = 0;   // starts at next place at bit 0
            }
        } // while (still more bits to go)
        // converted one byte

        // - compare byte with bin_v[bin_t] and find first difference
        bin_d = 8 - logzwo(bin_v[bin_t] ^ bin_byte);

        // - conclude
        // case 0: length of vector reached without difference = state found
        if (bin_b + bin_d >= PN->BitVectorSize) {
            // cout << "\t state found at address: " << vectordec->state
            //      << endl;
            return vectordec->state;
        }

checkagain:
        if (todec) {
            if (bin_b + bin_d < todec->bitnr) {
                // case 1: no difference or difference before "to" branch =
                // state not found
                if (bin_d < 8) {
                    // cout << "\t not found!" << endl;
                    return NULL;
                } else {
                    bin_s = 0;
                    bin_t++;      // next byte from CurrentMarking
                    bin_b += 8;
                    bin_byte = 0; // build new byte from CurrentMarking
                    continue;
                }
            }

            if (bin_b + bin_d == todec->bitnr) {
                // case 2: no difference or difference equals "to" branch =
                // continue with nextnew

                // switch comparison to the new branch
                if (bin_d < 8) {
                    // re-compute indizes to fit the new branch
                    bin_byte = bin_byte << (bin_d + 1);
                    bin_s = 7 - bin_d;
                    bin_v = todec->vector;      // take vector from new branch
                    bin_t = 0;
                    bin_b = todec->bitnr + 1;   // continue comparison after branch
                    vectordec = fromdec = todec;
                    todec = fromdec->nextnew;   // get next branch in new vector
                    bin_dir = 1;    // now descending the "new" branch
                    continue;
                } else {
                    // no difference (bin_d == 8)
                    bin_s = 0;
                    bin_t++;
                    bin_b += 8;
                    bin_byte = 0;
                    continue;
                }
            }

            // case 3: difference beyond "to" branch = continue with nextold
            fromdec = todec;
            todec = fromdec->nextold;
            bin_dir = 0;
            goto checkagain;
        } else {
            // case 4: no difference or state not found
            if (bin_d < 8) {
                // cout << "\t not found!" << endl;
                return NULL;
            } else {
                bin_s = 0;
                bin_t++;
                bin_b += 8;
                bin_byte = 0;
                continue;
            }
        }
    }

    // cout << "\t state found at address: ???" << endl;
}
