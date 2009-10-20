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

#include <config.h>
#include <string>
#include <map>
#include <sstream>
#include "Graph.h"
#include "helpers.h"

using std::map;


/*!
 * The function increases the indices in the vector and propagates resulting
 * carries. For example, if the index vector [3,3] is increased and the maximal
 * bounds are [5,4] the resulting vector is [4,0].
 *
 * \param[in,out] current_index  vector holding the current indices
 * \param[in]     max_index      vector holding the upper bounds of the indices
 *
 * \post Index vector is increased according to the described rules.
 *
 * \invariant Each index lies between 0 and its maximal value minus 1.
 */
void next_index(std::vector<unsigned int> &current_index,
                const std::vector<unsigned int> &max_index) {
    assert(current_index.size() == max_index.size());

    for (unsigned int i = 0; i < current_index.size(); ++i) {
        if (current_index[i] < max_index[i]-1) {
            ++current_index[i];
            break;
        } else {
            current_index[i] = 0;
        }
    }
}
