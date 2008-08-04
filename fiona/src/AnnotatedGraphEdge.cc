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
 * \file    AnnotatedGraphEdge.cc
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
#include "AnnotatedGraphEdge.h"
#include "AnnotatedGraphNode.h"

using namespace std;


//! \brief basic constructor
//! \param dstNodeP Points to the destination of this edge.
//! \param labelP label of this edge.
AnnotatedGraphEdge::AnnotatedGraphEdge(AnnotatedGraphNode* dstNodeP,
                                       const std::string& labelP) :
    GraphEdge(NULL,labelP),
    dstNode(dstNodeP) {
}


//! \brief returns the destination node of the edge
//! \return destination node
AnnotatedGraphNode* AnnotatedGraphEdge::getDstNode() const {
    return dstNode;
}


//! \brief sets the destination node of the edge
//! \param newNode the new destination of the edge
void AnnotatedGraphEdge::setDstNode(AnnotatedGraphNode* newNode) {
    dstNode = newNode;
}
