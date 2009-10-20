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

#ifndef __EDITDISTANCE_H
#define __EDITDISTANCE_H

#include "types.h"

class Graph;


/*!
 * \brief basic class for edit distances
 *
 * \note The usage of an C-style multidimensional array has the advantage that
 *       the access and insertion to the previously stored values is constant,
 *       whereas an STL map would have logarithmic time complexity.
 *       Furthermore, no overhead (16 bytes per element for STL maps) is
 *       needed. The C-style array can only be used, because its size is known
 *       in advance. 
 */ 
class EditDistance {
    protected:
        /// result cache for dynamic programming
        static Value **cache;

        /// value could be read from cache
        static unsigned int cache_hit;

        /// value had to be calculated
        static unsigned int cache_miss;

        /// allocate memory for cache and initialize it
        static void initializeCache(const Graph&, const Graph&);

        /// allocate memory for cache and initialize it
        static void emptyCache(const Graph&);

        /// destructor
        virtual ~EditDistance();
};

#endif
