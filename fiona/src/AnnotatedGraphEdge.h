/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg,              *
 *           Jan Bretschneider, Leonard Kern                                 *
 *                                                                           *
 * Copyright 2008                                                            *
 *   Peter Massuthe, Daniela Weinberg                                        *
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
 * \file    AnnotatedGraphEdge.h
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

#ifndef ANNOTATEDGRAPHEDGE_H
#define ANNOTATEDGRAPHEDGE_H

#include "mynew.h"
#include <string>
#include <stdexcept>
#include <cassert>
#include "GraphEdge.h"

class AnnotatedGraphNode;


class AnnotatedGraphEdge : public GraphEdge {
private:
    /// Points to the destination node of this edge.
    AnnotatedGraphNode* dstNode;

public:
    /// Constructs a GraphEdge.
    AnnotatedGraphEdge(AnnotatedGraphNode* dstNodeP, const std::string& labelP);

    /// Returns the node this edge points to.
    AnnotatedGraphNode* getDstNode() const;

    /// Sets the node this edge points to.
    void setDstNode(AnnotatedGraphNode* newDst);

#undef new
    /// Provides user defined operator new. Needed to trace all new operations
    /// on this class.
    NEW_OPERATOR(AnnotatedGraphEdge)
#define new NEW_NEW
};

#endif //ANNOTATEDGRAPHEDGE_H
