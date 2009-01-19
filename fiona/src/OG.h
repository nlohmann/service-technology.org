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
 * \file    OG.h
 *
 * \brief   functions for Operating Guidelines (OG)
 *
 * \author  responsible: Peter Massuthe <massuthe@informatik.hu-berlin.de>
 */

#ifndef OG_H_
#define OG_H_

#include "mynew.h"
#include "CommunicationGraph.h"
#include "BddRepresentation.h"
#include <map>

class oWFN;


class OG : public CommunicationGraph {
    protected:
        /// calculates the annotation (CNF) for the node
        void computeCNF(AnnotatedGraphNode* node) const;

        /// Builds the OG of the associated PN recursively starting at
        /// currentNode.
        virtual void buildGraph(AnnotatedGraphNode* currentNode, double progress_plus);

        /// calculates the set of successor states in case of an input message
        void calculateSuccStatesInput(unsigned int, AnnotatedGraphNode*, AnnotatedGraphNode*);
        
        /// calculates the set of successor states in case of an output message
        void calculateSuccStatesOutput(unsigned int, AnnotatedGraphNode*, AnnotatedGraphNode*);

        /// adds a node to the OG
        void addGraphNode(AnnotatedGraphNode*, AnnotatedGraphNode*); // for OG
        
        /// creates an edge in the OG
        void add(AnnotatedGraphNode*,
                          AnnotatedGraphNode*,
                          oWFN::Places_t::size_type,
                          GraphEdgeType); // for OG

    public:
        /// constructor
        OG(oWFN *);
        
        /// basic deconstructor
        virtual ~OG();

        BddRepresentation * bdd;

        /// Turns all blue nodes that should be red into red ones and
        /// simplifies their annotations by removing unneeded literals.
        void correctNodeColorsAndShortenAnnotations();

        /// Builds the OG of the associated PN
        virtual void buildGraph();

        /// converts an OG into its BDD representation
        void convertToBdd();


        // Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
        NEW_OPERATOR(OG)
#define new NEW_NEW
};


#endif /*OG_H_*/
