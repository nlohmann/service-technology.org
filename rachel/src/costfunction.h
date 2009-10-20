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

#ifndef __VALUES_H
#define __VALUES_H

#include "types.h"
#include "cmdline.h"


/// the command line options
extern gengetopt_args_info args_info;



/*!
 * \brief discount value
 *
 * The discont value describes the "local unimportance" of dissimilarities
 * compared with the future. If it is set to a value close to 1,
 * dissimilarities are punished very severely.
 *
 * \note  The value can be controlled with the command line option "--discount".
 *        Alternatively, they can be defined in a config file.
 */
inline Value discount() {
    return args_info.discount_arg;
}



/*!
 * \brief   node similarity
 *
 * The node similarity can be used to compare nodes by labels. It is currently
 * not used and always returns a constant value for any pair of nodes.
 *
 * \param   q1  a node
 * \param   q2  a node
 *
 * \return  the similarity between node n1 and n2
 *
 * \note    The value can be controlled with the command line option
 *          "--node-similarity". Alternatively, it can be defined in a config
 *          file.
 */
inline Value N(const Node q1, const Node q2) {
    // all nodes are equally similar
    return args_info.node_similarity_arg;
}


/*!
 * \brief   label similarity
 *
 * The label similarity describes similarities between edge labels and is used
 * to set the benefit or payoff for edge operations such as keeping, changing,
 * inserting or deleting. Values close to 0 make actions infeasible. For
 * example setting the modification payoff to a low value would result in
 * subsequent removal and insertion of edges with different labels.
 *
 * \param   l1  a label
 * \param   l2  a label
 *
 * \return  the similarity between label l1 and l2
 *
 * \note    The values can be controlled with the command line options
 *          "--benefit-keep", "--benefit-change", "--benefit-insert", and
 *          "--benefit-delete". Alternatively, they can be defined in a config
 *          file.
 */
inline Value L(const Label l1, const Label l2) {
    // avoid stuttering in both graphs
    if (l1.empty() and l2.empty())
        return 0;

    // first graph stutters: insert
    if (l1.empty() and not l2.empty())
        return args_info.benefit_insert_arg;

    // second graph stutters: delete
    if (not l1.empty() and l2.empty())
        return args_info.benefit_delete_arg;

    // both labels are equal: keep
    if (l1 == l2)
        return args_info.benefit_keep_arg;

    // label has to be changed: modify
    return args_info.benefit_change_arg;
}

#endif
