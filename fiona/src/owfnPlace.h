// *****************************************************************************\
// * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf       *
// *                                                                           *
// * This file is part of Fiona.                                               *
// *                                                                           *
// * Fiona is free software; you can redistribute it and/or modify it          *
// * under the terms of the GNU General Public License as published by the     *
// * Free Software Foundation; either version 2 of the License, or (at your    *
// * option) any later version.                                                *
// *                                                                           *
// * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
// * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
// * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
// * more details.                                                             *
// *                                                                           *
// * You should have received a copy of the GNU General Public License along   *
// * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
// * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
// *****************************************************************************/

/*!
 * \file    owfnPlace.h
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

#ifndef OWFNPLACE_H_
#define OWFNPLACE_H_

#include "mynew.h"
#include "petriNetNode.h"
#include "formula.h"


class oWFN;
class formula;

enum placeType {INPUT, OUTPUT, INTERNAL};	//< type of place

class owfnPlace : public Node {
	private :
		oWFN * net;			// pointer to underlying petri net (needed for hash value)
		
 	public:
	  	owfnPlace(char *, placeType, oWFN *);
	  	~owfnPlace();

		placeType type;		// type of place (input, output, internal, initial, final)  
		
		unsigned int initial_marking;
	  	unsigned int hash_factor;
	  	void operator += (unsigned int);  // increment marking of place
	  	void operator -= (unsigned int);  // decrement marking of place
	  	bool operator >= (unsigned int);  // test enabledness with respect to place
	  	void set_marking(unsigned int);   // set initial 1marking of place;
	  	void set_cmarking(unsigned int);   // set current marking of place;
	  	void set_hash(unsigned int);      // define a factor for hash value calculation
	  	// hash(m) = sum(p in P) p.hash_factor*CurrentMarking[p]
	 	unsigned int index; // index in place array, necessary for symmetries
	  	unsigned int references; // we remove isolated places 
	  	int capacity;   // maximum capacity
	  	int nrbits;     // nr of bits required for storing its marking (= log capacity)
		int startbit;   // first bit representing this place in bit vector
		int max_occurence;
	//	placeType getType();
		unsigned int cardprop; // number of propositions in final condition that mention this place
		formula ** proposition; // array of propositions in final condition that mention this place
					// used for quick re-evaluation of condition

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(owfnPlace)
#define new NEW_NEW
};

#endif /*OWFNPLACE_H_*/
