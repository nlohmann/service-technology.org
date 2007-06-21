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
 * \file    successorNodeList.h
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

#ifndef SUCCESSORNODELIST_H
#define SUCCESSORNODELIST_H

#include "mynew.h"
#include "graphEdge.h"
#include <stddef.h>
#include <string>

using namespace std;

class GraphNode;

class successorNodeList {

private:    
    GraphEdge * firstElement;	//!< pointer to the first element of the list
    GraphEdge * nextElement;	//!< pointer to the next element in the list while iterating through the list
public:    
    bool duringIteration;			/*!< we have reached the end of the iterating process, or not */
    
//public:
    successorNodeList();    
    ~successorNodeList(); 
    
    void deleteList(GraphEdge *);   
    void addNextNode(GraphEdge *);
    
    void resetIterating();
    GraphEdge * getNextElement();
    
// Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
    NEW_OPERATOR(successorNodeList)
#define new NEW_NEW
};
#endif //SUCCESSORNODELIST_H
