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
 * \file    AnnotatedGraphEdge.cc
 *
 * \brief   functions for traversing IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
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
