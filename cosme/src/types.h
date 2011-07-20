/*****************************************************************************\
 Cosme -- Checking Simulation, Matching, and Equivalence

 Copyright (c) 2010 Andreas Lehmann

 Cosme is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Cosme is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Cosme.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef __TYPES_H
#define __TYPES_H

#include <limits>
#include <stdint.h>

#define RBTS_NODE_KEY_UNUSED 4294967295U

// the range of labels (max. 255)
typedef uint8_t label_id_t;

// the range of labels leaving a node
typedef label_id_t label_index_t;
// the range of nodes in graphs (max. 255)
typedef uint32_t og_service_index_t;

#endif //__TYPES_H
