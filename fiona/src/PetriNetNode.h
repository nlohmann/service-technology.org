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
 * \file    petriNetNode.h
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

#ifndef PETRINETNODE_H_
#define PETRINETNODE_H_

#include "mynew.h"
#include <iostream>
#include <vector>

using namespace std;

class Arc;

class Node {
	// public typedefs first, then private members, then public methods.
	public:
		typedef std::vector<Arc*> Arcs_t;
	private:
		Arcs_t ArrivingArcs;
		Arcs_t LeavingArcs;
	public:
		std::string name;
		Node(const std::string&);
		ostream& operator << (ostream &);
		~Node();
		void addArrivingArc(Arc* arc);
		Arcs_t::size_type getArrivingArcsCount() const;
		Arc* getArrivingArc(Arcs_t::size_type i) const;
		void addLeavingArc(Arc* arc);
		Arcs_t::size_type getLeavingArcsCount() const;
		Arc* getLeavingArc(Arcs_t::size_type i) const;
};

class owfnTransition;
class owfnPlace;

class Arc
{
	public:
		Arc(owfnTransition *, owfnPlace *, bool ,unsigned int);
		
	//	static unsigned int cnt;
		Node * Source;
	  	Node * Destination;
	  	owfnPlace * pl;
	  	owfnTransition * tr;
	  	unsigned int Multiplicity;
	  	void operator += (unsigned int);

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(Arc)
#define new NEW_NEW
};

#endif /*PETRINETNODE_H_*/
