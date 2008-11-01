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
        static void initializeCache(Graph &g1, Graph &g2);

        /// allocate memory for cache and initialize it
        static void emptyCache(Graph &g1, Graph &g2);
    
        /// destructor
        virtual ~EditDistance() {};
};

#endif
