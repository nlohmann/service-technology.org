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
 * \file    IG.h
 *
 * \brief   functions for Interaction Graphs (IG)
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef IG_H_
#define IG_H_

#include "mynew.h"
#include "CommunicationGraph.h"

class oWFN;


class InteractionGraph : public CommunicationGraph {

    private:
        /// adds a node and its connecting edge
        bool addGraphNode(AnnotatedGraphNode*,
                          AnnotatedGraphNode*,
                          messageMultiSet,
                          GraphEdgeType,
                          double);

        /// Builds the IG of the associated PN recursively (helper function)
        void buildGraph(AnnotatedGraphNode*, double);

        /// calculates the set of successor states in case of an input message
        void calculateSuccStatesSendingEvent(messageMultiSet, AnnotatedGraphNode*, AnnotatedGraphNode*);

        /// calculates the set of successor states in case of an output message
        void calculateSuccStatesReceivingEvent(messageMultiSet, AnnotatedGraphNode*, AnnotatedGraphNode*);

        /// Calculate sending and receiving events using the reduction rules as specified.
        void calculateSendingAndReceivingEvents(AnnotatedGraphNode*, setOfMessages&, setOfMessages&);

        /// Calculates the set of receiving events without applying reduction rules.
        void getReceivingEvents(State*, GraphFormulaMultiaryOr*, setOfMessages&, AnnotatedGraphNode*);

        /// Calculates the set of sending events without applying reduction rules.
        void getSendingEvents(State*, GraphFormulaMultiaryOr*, setOfMessages&);

        // reduction rules

        /// Calculates the set of receiving events using the "combine receiving events" rule
        void combineReceivingEvents(State *, GraphFormulaMultiaryOr*, setOfMessages&, AnnotatedGraphNode*);

        /// Calculates the set of sending events using the "receive before sending" rule
        void receivingBeforeSending(State*, GraphFormulaMultiaryOr*, setOfMessages&);



    public:
        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(InteractionGraph)
#define new NEW_NEW

        /// constructor
        InteractionGraph(oWFN*);

        /// basic destructor
        virtual ~InteractionGraph();

        /// checks whether the set of input messages contains at least one input message
        /// that has been sent at its maximum
        bool checkMaximalEvents(messageMultiSet, AnnotatedGraphNode*, GraphEdgeType);

        /// Builds the IG of the associated PN starting with the root node
        void buildGraph();

};

#endif /*IG_H_*/
