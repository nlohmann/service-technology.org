/*****************************************************************************\
 Rachel -- Reparing Service Choreographies

 Copyright (c) 2008, 2009 Niels Lohmann

 Rachel is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Rachel.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file values.h
 * \brief Node and label similarity (can be overridden by command line parameters)
 */

#ifndef __VALUES_H
#define __VALUES_H

#include "types.h"

class Graph;


/// node similarity
Value N(const Graph &g1, const Graph &g2, Node q1, Node q2);

/// label similarity
Value L(const Graph &g1, const Graph &g2, Label l1, Label l2);


#endif
