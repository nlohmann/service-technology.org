/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
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
 * \file    graphEdge.h
 *
 * \brief   functions for traversing IG / OG
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef GRAPHEDGE_H
#define GRAPHEDGE_H

#include "mynew.h"
#include "enums.h"
#include <stddef.h>
#include <string>

using namespace std;

class vertex;

class graphEdge {

private:
    vertex * node;				//!< pointer to the node this arc is pointing to
    std::string label;			//!< label of the arc (usually the name of the event)
    edgeType type;  			//!< type of the arc (sending, receiving)
    graphEdge * nextElement;	//!< pointer to the next element in list

public:
	graphEdge(vertex *, const string&, edgeType);
	~graphEdge();
	void setNextElement(graphEdge *);
	graphEdge * getNextElement();
	string getLabel() const;
	edgeType getType();
	vertex * getNode();
	void setNode(vertex *);

// Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
    NEW_OPERATOR(graphEdge)
#define new NEW_NEW
};
#endif //GRAPHEDGE_H

