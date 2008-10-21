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
 * \file    GraphEdge.cc
 *
 * \brief   functions for traversing IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 */

#include <string>
#include <stdexcept>
#include <cassert>
#include "mynew.h"
#include "GraphEdge.h"
#include "GraphNode.h"

using namespace std;

//! \brief basic constructor
//! \param dstNodeP Points to the destination of this edge.
//! \param labelP label of this edge.
GraphEdge::GraphEdge(GraphNode* dstNodeP, const std::string& labelP) :
    dstNode(dstNodeP), label(labelP) {
}


//! \brief returns the label of the edge
//! \return label of the edge
std::string GraphEdge::getLabel() const {
    return label;
}


//! \brief returns the label of the edge without its prefix
//! \return label of the edge
std::string GraphEdge::getLabelWithoutPrefix() const {
    assert(label[0] == '?' || label[0] == '!');
    return label.substr(1);
}


//! \brief checks the type of the edge and returns it
//! \return type of the edge
GraphEdgeType GraphEdge::getType() const {
    assert(label.size() != 0);
// CODE FROM PL
    if (label == GraphFormulaLiteral::TAU) {
    	return SILENT;
    } else {
// END OF CODE FROM PL
	    switch (label[0]) {
	        case '?':
	            return RECEIVING;
	        case '!':
	            return SENDING;
	        default:
	            // This should never happen.
	            assert(false);
	            throw new std::invalid_argument("Cannot determine type of this GraphEdge with label '" + label + "'.");
	    }
// CODE FROM PL
    }
// END OF CODE FROM PL
}

// CODE FROM PL
//! \brief modifies the edge according to its type
void GraphEdge::toggleType() {
	switch (this->getType()) {
		case RECEIVING:
			// switch to SENDING by relabeling
			label[0] = '!';
			break;
		case SENDING:
			// switch to RECEIVING by relabeling
			label[0] = '?';
			break;
		default:
			// type is either SILENT or unknown
			// transitions of this type cannot be toggled
			break;
	}
}
// END OF CODE FROM PL

//! \brief returns the destination node of the dge
//! \return destination node
GraphNode* GraphEdge::getDstNode() const {
    return dstNode;
}
