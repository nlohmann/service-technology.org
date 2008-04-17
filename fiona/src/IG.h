/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg               *
 *                                                                           *
 * Copyright 2008             Peter Massuthe, Daniela Weinberg               *
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
#include "CommunicationGraph.h"

class oWFN;


class interactionGraph : public CommunicationGraph {

    private:
        /// adds a node and its connecting edge
        bool addGraphNode(AnnotatedGraphNode*,
                          AnnotatedGraphNode*,
                          messageMultiSet,
                          GraphEdgeType,
                          double); 
        
    public:
        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(interactionGraph)
#define new NEW_NEW

        /// constructor
        interactionGraph(oWFN*);
        
        /// basic destructor
        virtual ~interactionGraph();

        /// checks whether the set of input messages contains at least one input message
        /// that has been sent at its maximum
        bool checkMaximalEvents(messageMultiSet, AnnotatedGraphNode*, GraphEdgeType);

        /// Builds the IG of the associated PN starting with the root node
        void buildGraph();

        /// Builds the IG of the associated PN recursively
        void buildGraph(AnnotatedGraphNode*, double);

        /// calculates the set of successor states in case of an input message
        void calculateSuccStatesSendingEvent(messageMultiSet, AnnotatedGraphNode*, AnnotatedGraphNode*);

        /// calculates the set of successor states in case of an output message
        void calculateSuccStatesReceivingEvent(messageMultiSet, AnnotatedGraphNode*, AnnotatedGraphNode*);

        /// Calculate sending and receiving events using the reduction rules as specified. 
        void calculateSendingAndReceivingEvents(AnnotatedGraphNode*, setOfMessages&, setOfMessages&);
        
        /// Calculates the set of receiving events without applying reduction rules.
        void getReceivingEvents(StateSet::iterator&, GraphFormulaMultiaryOr*, setOfMessages&, AnnotatedGraphNode*);
        
        /// Calculates the set of sending events without applying reduction rules.
        void getSendingEvents(StateSet::iterator&, GraphFormulaMultiaryOr*, setOfMessages&);
        
        // reduction rules
        
        /// Calculates the set of receiving events using the "combine receiving events" rule
        void combineReceivingEvents(StateSet::iterator&, GraphFormulaMultiaryOr*, setOfMessages&, AnnotatedGraphNode*);
        
        /// Calculates the set of sending events using the "receive before sending" rule
        void receivingBeforeSending(StateSet::iterator&, GraphFormulaMultiaryOr*, setOfMessages&);

};

#endif /*IG_H_*/
