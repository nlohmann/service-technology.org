/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg               *
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
                          double); // for IG

    public:
        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(interactionGraph)
#define new NEW_NEW

        /// conrtructor
        interactionGraph(oWFN*);
        
        /// basic deconstructor
        ~interactionGraph();

        /// checks whether the set of input messages contains at least one input message
        /// that has been sent at its maximum
        bool checkMaximalEvents(messageMultiSet, AnnotatedGraphNode*, GraphEdgeType);

        /// Builds the IG of the associated PN starting with the root node
        void buildGraph();

        /// Builds the IG of the associated PN recursively
        void buildGraph(AnnotatedGraphNode*, double);

        /// creates a list of all activated sending and receiving events (input messages 
        /// and output messages) of the current node
        void getActivatedEventsComputeCNF(AnnotatedGraphNode*, setOfMessages&, setOfMessages&);

        /// calculates the set of successor states in case of an input message
        void calculateSuccStatesSendingEvent(messageMultiSet, AnnotatedGraphNode*, AnnotatedGraphNode*);

        /// calculates the set of successor states in case of an output message
        void calculateSuccStatesReceivingEvent(messageMultiSet, AnnotatedGraphNode*, AnnotatedGraphNode*);

        /// creates a list of all receiving events of the current node and creates the set of
        /// sending events applies the reduction rules: "combine receiving events" and 
        /// "receiving before sending"
        setOfMessages combineReceivingEvents(AnnotatedGraphNode*, setOfMessages&);
        
        /// not yet implemented
        setOfMessages receivingBeforeSending(AnnotatedGraphNode*);

};

#endif /*IG_H_*/
