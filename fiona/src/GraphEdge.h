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
 * \file    GraphEdge.h
 *
 * \brief   functions for traversing IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#ifndef GRAPHEDGE_H
#define GRAPHEDGE_H

#include "mynew.h"
#include <string>
#include <stdexcept>
#include <cassert>

//! Possible types of a GraphEdge.
enum GraphEdgeType {SENDING, RECEIVING, SILENT};

class GraphNode;

class GraphEdge {
    private:
        /// Points to the destination node of this edge.
        GraphNode* dstNode;
    
    protected:
        /// Label of the edge.
        std::string label;

    public:
        /// Constructs a GraphEdge.
        GraphEdge(GraphNode* dstNodeP, const std::string& labelP);

        /// Returns the label of this edge.
        std::string getLabel() const;

        /// Returns the label of this edge without its prefix denoting whether
        /// it is a sending or receiving edge.
        std::string getLabelWithoutPrefix() const;

        /// Returns the type if this edge. (RECEIVING or SENDING or SILENT)
        GraphEdgeType getType() const;

// CODE FROM PL
        /// Switches the type of this edge between RECEIVING and SENDING 
        void toggleType();
// END OF CODE FROM PL

        /// Returns the node this edge points to.
        GraphNode* getDstNode() const;

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphEdge)
#define new NEW_NEW
};

#endif //GRAPHEDGE_H
