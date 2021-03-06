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
 * \file    owfnPlace.cc
 *
 * \brief   functions for Petri net places
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#include "mynew.h"
#include "PetriNetNode.h"
#include "owfn.h"
#include "dimensions.h"
#include "owfnPlace.h"
#include "owfnTransition.h"
#include "debug.h"
#include <cassert>


//! \brief constructor
//! \param name
//! \param _type
//! \param _net
owfnPlace::owfnPlace(char * name, placeType _type, oWFN * _net) :
    Node(name), type(_type), capacity(0), nrbits(0),
            max_occurrence(1), cardprop(0), proposition(NULL),propsize(0) {

    references = initial_marking = hash_factor = 0;
    net = _net;
    port = "";
}


//! \brief constructor
//! \param name
//! \param _type
//! \param _net
owfnPlace::owfnPlace(const string& name, placeType _type, oWFN * _net) :
    Node(name), type(_type), capacity(0), nrbits(0),
            max_occurrence(1), cardprop(0), proposition(NULL) {

    references = initial_marking = hash_factor = 0;
    net = _net;
    port = "";
}


//! \brief copy constructor
//! \param original pointer to a owfnPlace which serves as original
//! \param _net pointer to a OWFN
owfnPlace::owfnPlace(owfnPlace * original, oWFN * _net) :
    Node( original->getName() ),
    type( original->type ),
    initial_marking( original->initial_marking ),
    hash_factor( original->hash_factor ),
    references( original->references ),
    capacity( original->capacity ),
    nrbits( original->nrbits ),
    startbit( 0 ),       // startbit is set correctly in oWFN::initialize
    max_occurrence( original->max_occurrence ),
    cardprop(0),         // is set by formula::setstatic()
    proposition(NULL),    // is set by formula::setstatic()
    propsize(0)
{
    net = _net;
    port = original->getPort();
}


//! \brief destructor
owfnPlace::~owfnPlace() {
    TRACE(TRACE_5, "owfnPlace::~owfnPlace() : start\n");

/*    // delete the array with all propositions (atomicformulas) which mention
    // this place. The propositions themselves are deleted by the class oWFN
    // that recursively deletes its FinalCondition.

    cerr << "deleting proposition of owfn place " << this->name << endl;

    cerr << "Number of props " << this->propsize << " and cardprop is: " << this->propsize << endl;

    for (int i = 0; i < this->cardprop; ++i) {
    	cerr << "deleting prop " << i << ": " << proposition[i] << endl;
    	delete proposition[i];
    }


    delete[] proposition;
*/
    delete proposition;
    TRACE(TRACE_5, "owfnPlace::~owfnPlace() : end\n");
}


//! \brief return the type of this place
//! \return type of this place
placeType owfnPlace::getType() const {
    return type;
}


//! \brief return the label of the place for use in a communication graph
//! \return label of the node
std::string owfnPlace::getLabelForCommGraph() const {

    string label;
    switch (type) {
        case INPUT:
            label = '!';
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


//! \brief return the label of the place for use in matching
//! \return label of the node
std::string owfnPlace::getLabelForMatching() const {

    string label;
    switch (type) {
        case INPUT:
            label = '?';
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


//! \brief return the net this place is used in
//! \return owfn
oWFN* owfnPlace::getUnderlyingOWFN() const {
    return net;
}


//! \brief increment marking of place by the given value
//! \param i increment value
void owfnPlace::operator += (unsigned int i) {
    initial_marking += i;
    net->placeHashValue += i*hash_factor;
    net->placeHashValue %= HASHSIZE;
}


//! \brief decrement marking of place by the given value
//! \param i decrement value
void owfnPlace::operator -= (unsigned int i) {
    initial_marking -= i;

    net->placeHashValue -= i*hash_factor;
    net->placeHashValue %= HASHSIZE;
}


//! \brief check whether the marking of a place is greater or equal to a given i
//! \param i relation value
//! \return true if the makring is greater or equal to i, else false
bool owfnPlace::operator >= (unsigned int i) {
    return((initial_marking >= i) ? 1 : 0);
}


//! \brief sets the hash value of the place to the given value
//! \param i new hash value
void owfnPlace::set_hash(unsigned int i) {
    net->placeHashValue -= hash_factor * initial_marking;
    hash_factor = i;
    net->placeHashValue += hash_factor * initial_marking;
    net->placeHashValue %= HASHSIZE;
}


//! \brief set the port of the oWFN place
//! \param my_port port to be set
void owfnPlace::setPort(const std::string my_port) {
    port = my_port;
}

//! \brief get the port of the oWFN place
//! \return a string representing the port
std::string owfnPlace::getPort() {
    return port;
}

//! \brief returns the cost of this place if its and interface place
//! \return cost
unsigned int owfnPlace::getCost() {

    if ( type == INPUT && getLeavingArcsCount() > 0 ) {
        unsigned int max = (getLeavingArc(0)->tr)->cost;
        for (int i = 1; i < getLeavingArcsCount(); ++i) {
            unsigned int currentCost = (getLeavingArc(i)->tr)->cost;
            max = (currentCost > max) ? currentCost : max;
        }
        return max;
    }

    if ( type == OUTPUT && getArrivingArcsCount() > 0 ) {
        unsigned int max = (getArrivingArc(0)->tr)->cost;
        for (int i = 1; i < getArrivingArcsCount(); ++i) {
            unsigned int currentCost = (getArrivingArc(i)->tr)->cost;
            max = (currentCost > max) ? currentCost : max;
        }
        return max;
    }

    return 0;
}
