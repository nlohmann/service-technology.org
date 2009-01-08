/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008, 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
 Rachel is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Rachel (see file COPYING); if not, see http://www.gnu.org/licenses or write to
 the Free Software Foundation,Inc., 51 Franklin Street, Fifth
 Floor, Boston, MA 02110-1301  USA.
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
Value N(Graph &g1, Graph &g2, Node q1, Node q2);

/// label similarity
Value L(Graph &g1, Graph &g2, Label l1, Label l2);


#endif
