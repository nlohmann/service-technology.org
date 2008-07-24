/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
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

#ifndef __MAIN_H
#define __MAIN_H

#include <cassert>

#include "graph.h"


/// calculate the faculty function
unsigned int fac(unsigned int n);

/// return maximum of two values
Value max(Value v1, Value v2);

/// converts unsigned int to string
string toString(Node q);

/// updates an index
void next_index(vector<unsigned int> &current_index, const vector<unsigned int> &max_index);


#endif
