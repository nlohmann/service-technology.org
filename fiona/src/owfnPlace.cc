/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf       *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    owfnPlace.cc
 *
 * \brief   functions for Petri net places
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "mynew.h"
#include "PetriNetNode.h"
#include "owfn.h"
#include "dimensions.h"
#include "owfnPlace.h"
#include "debug.h"
#include <cassert>


//! \param name
//! \param _type
//! \param _net
//! \brief constructor
owfnPlace::owfnPlace(char * name, placeType _type, oWFN * _net) :
    Node(name),
    type(_type),
    index(0),
    capacity(0),
    nrbits(0),
    max_occurence(1),
    cardprop(0),
    proposition(NULL) {

    references = initial_marking = hash_factor = 0;
    net = _net;
    port = "";
}


//! \brief constructor
owfnPlace::~owfnPlace() {
    trace(TRACE_5, "owfnPlace::~owfnPlace() : start\n");

    // delete the array with all propositions (atomicformulas) which mention
    // this place. The propositions themselves are deleted by the class oWFN
    // that recursively deletes its FinalCondition.
    delete[] proposition;

    trace(TRACE_5, "owfnPlace::~owfnPlace() : end\n");
}


std::string owfnPlace::getLabelForCommGraph() const {

    string label;
    switch (type) {
        case INPUT:
            label  = '!';
            break;
        case OUTPUT:
            label = '?';
            break;
        default:
            assert(false);
            break;
    }
    return (label + name);
}


std::string owfnPlace::getLabelForMatching() const {

    string label;
    switch (type) {
        case INPUT:
            label  = '?';
            break;
        case OUTPUT:
            label = '!';
            break;
        default:
            assert(false);
            break;
    }
    return (label + name);
}


void owfnPlace::operator += (unsigned int i) {
    initial_marking += i;
    net->placeHashValue += i*hash_factor;
    net->placeHashValue %= HASHSIZE;
}


void owfnPlace::operator -= (unsigned int i) {
    initial_marking -= i;

    net->placeHashValue -= i*hash_factor;
    net->placeHashValue %= HASHSIZE;
}


bool owfnPlace::operator >= (unsigned int i) {
    return((initial_marking >= i) ? 1 : 0);
}


void owfnPlace::set_hash(unsigned int i) {
    net->placeHashValue -= hash_factor * initial_marking;
    hash_factor = i;
    net->placeHashValue += hash_factor * initial_marking;
    net->placeHashValue %= HASHSIZE;
}


//! \brief set the port of the oWFN place
void owfnPlace::set_port(std::string my_port) {
    port = my_port;
}
