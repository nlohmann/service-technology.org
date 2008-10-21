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
 * \file    searchTrace.h
 *
 * \brief   functions for Petri net states
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef SEARCHTRACE_H
#define SEARCHTRACE_H

using namespace std;

class SearchTrace;

extern SearchTrace * Trace;

extern int bin_p;               // (=place); index in CurrentMarking
extern unsigned char bin_byte;  // byte to be matched against tree vector; constructed from CurrentMarking
extern int bin_t;               // index in tree vector
extern unsigned char * bin_v;   // current tree vector
extern int bin_s;               // nr of bits pending in byte from previous iteration
extern int bin_dir;             // did we go "old" or "new" in last decision?
extern int bin_b;               // bit nr at start of byte
extern binDecision * fromdec, * todec;
extern binDecision * vectordec;


class SearchTrace {
    public:
        unsigned char bn_byte;  // byte to be matched against tree vector; constructed from MARKINGVECTOR
        int bn_t;               // index in tree vector
        unsigned char * bn_v;   // current tree vector
        int bn_s;               // nr of bits pending in byte from previous iteration
        int bn_dir;             // did we go "old" or "new" in last decision?
        int bn_b;               // bit nr at start of byte
        binDecision * frmdec, * tdec;
        binDecision * vectrdec;
};


inline void trace() {
    // bin_p = index, bin_pb = 0!
    Trace[bin_p].bn_byte = bin_byte;
    Trace[bin_p].bn_t = bin_t;
    Trace[bin_p].bn_v = bin_v;
    Trace[bin_p].bn_s = bin_s;
    Trace[bin_p].bn_dir = bin_dir;
    Trace[bin_p].bn_b = bin_b;
    Trace[bin_p].frmdec = fromdec;
    Trace[bin_p].vectrdec = vectordec;
    Trace[bin_p].tdec = todec;
}

#endif
