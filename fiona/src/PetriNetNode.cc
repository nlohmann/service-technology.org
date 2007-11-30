/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Karsten Wolf, Jan Bretschneider                                 *
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
 * \file    PetriNetNode.cc
 *
 * \brief   functions for Petri net elements
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
#include "debug.h"


//! \brief adds an arriving arc to this node
//! \param arc arriving arc to be added
void Node::addArrivingArc(Arc* arc) {
    ArrivingArcs.push_back(arc);
}


//! \brief adds an leaving arc to this node
//! \param arc leaving arc to be added
void Node::addLeavingArc(Arc* arc) {
    LeavingArcs.push_back(arc);
}


//! \brief returns the arriving art at the given iterator position
//! \param i iterator position of the arc
//! \returns the arc at the iterator position
Arc* Node::getArrivingArc(Arcs_t::size_type i) const {
    return ArrivingArcs[i];
}


//! \brief returns the leaving art at the given iterator position
//! \param i iterator position of the arc
//! \returns the arc at the iterator position
Arc* Node::getLeavingArc(Arcs_t::size_type i) const {
    return LeavingArcs[i];
}


//! \brief returns the number of arriving arcs
//! \return number of arrivng arcs
Node::Arcs_t::size_type Node::getArrivingArcsCount() const {
    return ArrivingArcs.size();
}


//! \brief returns the number of leaving arcs
//! \return number of leaving arcs
Node::Arcs_t::size_type Node::getLeavingArcsCount() const {
    return LeavingArcs.size();
}


//! \brief destructor
Node::~Node() {
    trace(TRACE_5, "Node::~Node() : start\n");

    // Transitions and places share pointers to the same arcs because every
    // arriving arc of a transition is a leaving arc of a place and vice versa.
    // Therefore we only have to delete all arcs in ArrivingArcs. This way
    // all arcs in LeavingArcs will also eventually be deleted.
    for (Arcs_t::size_type i = 0; i < getArrivingArcsCount(); ++i)
        delete getArrivingArc(i);

    trace(TRACE_5, "Node::~Node() : end\n");
}


//! \brief constructor
//! \param n name of the node
Node::Node(const std::string& n) :
    name(n) {
}


//! \brief outputstream operator for nodes
//! \param str stream to be written to
//! \param n node to be written to stream
//! \return updated stream
inline ostream& operator <<(ostream & str, Node n) {
    str << n.name;
    return str;
}


//! \brief constructor
//! \param t transition of the arc
//! \param p place of the arc
//! \param totrans true if this arc has a transition as destination, else false
//! \param mult multiplicity of the arc
Arc::Arc(owfnTransition * t, owfnPlace * p, bool totrans, unsigned int mult) {
    tr = t;
    pl = p;
    Source = totrans ? (Node *) p : (Node *) t;
    Destination = totrans ? (Node *) t : (Node *) p;
    Multiplicity = mult;
    //	cnt++;
}


//! \brief addition operator for arc multiplicities
//! \param incr multuplicity incrementation
void Arc::operator += (unsigned int incr) {
    Multiplicity += incr;
}
