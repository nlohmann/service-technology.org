/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Jan Bretschneider                                               *
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
 * \file    GraphEdge.cc
 *
 * \brief   functions for traversing IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universitt zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include <string>
#include <stdexcept>
#include <cassert>
#include "mynew.h"
#include "GraphEdge.h"
#include "GraphNode.h"

using namespace std;

GraphEdge::GraphEdge(GraphNode* dstNodeP, const std::string& labelP) :
    dstNode(dstNodeP), label(labelP) {
}


std::string GraphEdge::getLabel() const {
    return label;
}


std::string GraphEdge::getLabelWithoutPrefix() const {
    assert(label[0] == '?' || label[0] == '!');
    return label.substr(1);
}


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

GraphNode* GraphEdge::getDstNode() const {
    return dstNode;
}
