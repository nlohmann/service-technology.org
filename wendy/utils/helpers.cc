#include <cassert>
#include "helpers.h"

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
                const std::vector<unsigned int> &max_index)
{
    assert(current_index.size() == max_index.size());
    
    for (unsigned int i = 0; i < current_index.size(); ++i) {
        if (current_index[i] < max_index[i]-1) {
            current_index[i]++;
            break;
        } else {
            current_index[i] = 0;
        }
    }
}
