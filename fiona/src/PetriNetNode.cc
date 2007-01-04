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
 * \file    petriNetNode.cc
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
#include "petriNetNode.h"
#include "debug.h"

inline void Node::NewArriving(Arc & a) {
  Arc ** Old = ArrivingArcs;
  NrOfArriving++;
  ArrivingArcs = new Arc* [NrOfArriving];
  for(int i = 0; i < NrOfArriving -1;i++)
  {
    ArrivingArcs[i] = Old[i];
  }
  ArrivingArcs[NrOfArriving -1] = & a;
  delete [] Old;
}

inline void Node::NewLeaving(Arc & a) {
  Arc ** Old = LeavingArcs;
  NrOfLeaving++;
  LeavingArcs = new Arc* [NrOfLeaving];
  for(int i = 0; i < NrOfLeaving -1;i++)
  {
    LeavingArcs[i] = Old[i];
  }
  LeavingArcs[NrOfLeaving -1] = & a;
  delete [] Old;
}

Node::~Node() {
	trace(TRACE_5, "Node::~Node() : start\n");
	
  delete [] name;

  // Transitions and places share pointers to the same arcs because every
  // arriving arc of a transition is a leaving arc of a place and vice versa.
  // Therefore we only have to delete all arcs in ArrivingArcs[]. This way all
  // arcs in LeavingArcs[] will also eventually be deleted.
  for(int i = 0; i < NrOfArriving; i++) {
    delete ArrivingArcs[i];
    ArrivingArcs[i] = NULL;
  }

  delete [] ArrivingArcs;

  delete [] LeavingArcs;

	trace(TRACE_5, "Node::~Node() : end\n");
}

Node::Node(char * n) : NrOfArriving(0), NrOfLeaving(0)
{
  name = new char [strlen(n) + 1];
  strcpy(name, n);
}

inline ostream& operator << (ostream & str,Node n)
{
  str << n.name;
  return str;
}

Arc::Arc(owfnTransition * t, owfnPlace * p, bool totrans, unsigned int mult)
{
  tr = t;
  pl = p;
  Source = totrans ? (Node *) p : (Node *) t;
  Destination = totrans ? (Node *) t : (Node *) p;
  Multiplicity = mult;
//	cnt++;
}

inline Node * Arc::Get(bool dest)
{
  return dest? Destination : Source;
}

void Arc::operator += (unsigned int incr)
{
  Multiplicity += incr;
}
