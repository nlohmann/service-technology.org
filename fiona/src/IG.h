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
 * \file    IG.h
 *
 * \brief   functions for Interaction Graphs (IG)
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef IG_H_
#define IG_H_

#include "mynew.h"
#include "communicationGraph.h"

class oWFN;

class interactionGraph : public communicationGraph {
	public:
		interactionGraph(oWFN *);
		~interactionGraph();
	
		bool checkMaximalEvents(messageMultiSet, vertex *, edgeType);	
	
	    void buildGraph();
    	void buildGraph(vertex * );
    	void buildReducedGraph(vertex * );
    
    	void getActivatedEventsComputeCNF(vertex *, setOfMessages &, setOfMessages &);
    
		setOfMessages combineReceivingEvents(vertex *, setOfMessages &);
		setOfMessages receivingBeforeSending(vertex *);

    	bool terminateBuildingGraph(vertex *);
    	
    	void calculateSuccStatesOutputSet(messageMultiSet, vertex *);
    	void calculateSuccStatesInputReduced(messageMultiSet, vertex *);
        
        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(interactionGraph)
#define new NEW_NEW
};

#endif /*IG_H_*/
