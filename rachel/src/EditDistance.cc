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

#include "EditDistance.h"
#include "Graph.h"

unsigned int EditDistance::cache_hit = 0;
unsigned int EditDistance::cache_miss = 0;
Value **EditDistance::cache = NULL;


/*!
 * \brief allocate memory for cache and initialize it
 *
 * \note Assertions about NULL pointers are not needed, because "new" would
 *       throw an execption if allocation does not work.
 */
void EditDistance::initializeCache(const Graph& g1, const Graph& g2) {
    cache = new Value*[g1.nodes.size()];

    for (size_t i = 0; i < g1.nodes.size(); ++i) {
        cache[i] = new Value[g2.nodes.size()];

        // initialize cache entries to 0
        for (size_t j = 0; j < g2.nodes.size(); ++j) {
            cache[i][j] = 0;
        }
    }
}


/*!
 * \brief de-allocate the cache
 *
 * \bug This function crashes for reasons I don't understand if it is called
 *      after calculating the matching.
 */
void EditDistance::emptyCache(const Graph& g1, const Graph& g2) {
    for (size_t i = 0; i < g1.nodes.size(); ++i) {
        delete [] cache[i];
    }

    delete [] cache;
}
