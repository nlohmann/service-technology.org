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
 * \file values.cc
 * \brief Implements functions for the node and label similarity
 */
 
#include "values.h"
#include "cmdline.h"


/*************
 * externals *
 *************/

extern gengetopt_args_info args_info;


/*************
 * functions *
 *************/

/// node similarity
Value N(Graph &g1, Graph &g2, Node q1, Node q2) {
    // all nodes are equally similar
    return args_info.node_similarity_arg;
}


/*!
 * \brief   label similarity
 *
 * \param   g1  a graph
 * \param   g2  a graph
 * \param   l1  a label
 * \param   l2  a label
 *
 * \return  the similarity between label l1 and l2
 */
Value L(Graph &g1, Graph &g2, Label l1, Label l2) {
    // avoid stuttering in both graphs
    if (l1 == "" && l2 == "")
        return 0;
    
    // first graph stutters: insertion
    if (l1 == "" && l2 != "")
        return args_info.benefit_insert_arg;
    
    // second graph stutters: deletion
    if (l1 != "" && l2 == "")
        return args_info.benefit_delete_arg;
    
    // both labels are equal
    if (l1 == l2)
        return args_info.benefit_keep_arg;
    
    // label has to be changed
    return args_info.benefit_change_arg;
}

