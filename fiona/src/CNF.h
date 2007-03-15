/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
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
 * \file    CNF.h
 *
 * \brief   functions for node annotations and analysis of IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef CNF_H_
#define CNF_H_


#include "mynew.h"
#include "enums.h"
#include <string>
#include <set>		// set

using namespace std;

class graphEdge;
class State;


/*!
 * 	\class clause
    \brief class for storing a clause (set of literals)

	\author Daniela Weinberg
	\par History
		- 2006-07-11 creation
 */
class clause {
public:
	clause();
	clause(graphEdge *);
	~clause();
	
	bool fake;			//!< this literal has no real edge assigned to it yet (-> fake edge)
		
	graphEdge * edge;	//!< edge associated with this literal, does not need to be a real one in the graph
	clause * nextElement; //!< pointer to the next literal of this clause

	void addLiteral(const std::string&);  //!< add another literal to this clause
	
	void setEdge(graphEdge *);	//!< set the edge of this literal to the edge given
	void setEdges(graphEdge *); //!< parse the whole clause and look for the appropriate fake edge, which is to be substituted
	
	string getClauseString();  //!< generates the string represented by this clause
	
	vertexColor getColor();	//!< calculates the color of this clause

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(clause)
#define new NEW_NEW
};


/*!
 * 	\class CNF
    \brief class for storing a CNF (conjunction of clauses)

	\author Daniela Weinberg
	\par History
		- 2006-07-11 creation
 */
class CNF {
public:	
	CNF();
	~CNF();

	clause * cl;					//!< pointer to the clause
	CNF * nextElement;				//!< next element in CNF
	bool isFinalState;				//!< indicates whether this clause belongs to a final state or not
		
	vertexColor getColor();			//!< returns the color of the CNF
	vertexColor calcClauseColor();	//!< calculates the color of the clauses and thus of the CNF
	
	void addClause(clause *);		//!< adds a new clause to the CNF
	void setEdge(graphEdge *);		//!< sets the real edge in all those literals of all clauses of this CNF if needed

	int numberOfElements();			//!< returns the number of elements of this CNF
	
	string getCNFString();			//!< returns the CNF's string

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(CNF)
#define new NEW_NEW
};

#endif /* CNF_H_ */
