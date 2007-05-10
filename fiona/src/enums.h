/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Robert Danitz      *
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
 * \file	enums.h
 *
 * \brief	all enums
 * 
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/forschung/projekte/tools4bpel
 *          for details.
 */

#ifndef ENUMS_H_
#define ENUMS_H_

#include "mynew.h"
#include <string>

enum vertexColor_enum {RED, BLUE};				//!< RED == bad vertex; BLUE == good one

class vertexColor
{
    private:
        vertexColor_enum color_;
    public:
        vertexColor();
        vertexColor(vertexColor_enum color);
        std::string toString() const;
        operator vertexColor_enum() const;
};

enum edgeType {sending, receiving}; 		//!< ENUM possible types of an edge
enum analysisResult {TERMINATE, CONTINUE};  //!< needed as feedback of the "analysis" function, whether this node is an end node or not
enum stateType {TRANS, DEADLOCK, FINALSTATE};   //!< TRANS == Transient
enum threeValueLogic {FALSE, TRUE, UNKNOWN}; //!< used, for instance, to remember if there is a final state in a node


#endif /*ENUMS_H_*/
