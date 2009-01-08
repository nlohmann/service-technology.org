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
 * \file Simulation.cc
 * \brief Implements functions for the simulation-based graph similarity
 */

#include <vector>
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include "helpers.h"
#include "cmdline.h"
#include "costfunction.h"
#include "Simulation.h"
#include "Action.h"
#include "Graph.h"

using std::pair;
using std::map;


/*************
 * externals *
 *************/

/// a map caching the best script for all action pairs
extern map<Node, map<Node, ActionScript> > G_script_cache;

/// the command line options
extern gengetopt_args_info args_info;

extern Graph A;
extern Graph B;


/*************
 * functions *
 *************/


/*!
 * \brief helper function 1 for simulation: insertion
 *
 * \note  Caching is not necessary, because the function is only called once
 *        for every pair (q1,q2) of states.
 *
 */
ActionScript Simulation::w1(Node q1, Node q2) {
    Action action_insert(INSERT, 0);
    
    // traverse B's edges and find edge with highest value if A stutters
    for (size_t i = 0; i < B.outEdges(q2).size(); ++i)  {
        Value currentvalue = L("", B.outEdges(q2)[i].label) *
            simulation_recursively(q1, B.outEdges(q2)[i].target);
 
        if (currentvalue > action_insert.value) {
            action_insert.value = currentvalue;
            action_insert.stateA = q1;
            action_insert.stateB = B.outEdges(q2)[i].target;
            action_insert.label_new = B.outEdges(q2)[i].label;
        }
    }
    
    // create a singleton script out of the action and return it
    return ActionScript(action_insert);
}


/*!
 * \brief helper function 2 for simulation: keep, modify, or delete
 *
 * \note  Caching is not necessary, because the function is only called once
 *        for every pair (q1,q2) of states.
 *
 */
ActionScript Simulation::w2(Node q1, Node q2) {
    ActionScript script;
    
    // traverse A's edges
    for (size_t i = 0; i < A.outEdges(q1).size(); ++i) {
        Action action_modify(MODIFY, 0);
        
        // traverse B's edges: find edge with highest value if no-one stutters
        for (size_t j = 0; j < B.outEdges(q2).size(); ++j) {
            Value currentvalue = L(A.outEdges(q1)[i].label, B.outEdges(q2)[j].label) *
                simulation_recursively(A.outEdges(q1)[i].target, B.outEdges(q2)[j].target);            

            if (currentvalue > action_modify.value) {
                action_modify.value = currentvalue;
                action_modify.stateA = A.outEdges(q1)[i].target;
                action_modify.stateB = B.outEdges(q2)[j].target;
                action_modify.label_old = A.outEdges(q1)[i].label;
                action_modify.label_new = B.outEdges(q2)[j].label;
            }
        }
        
        // B stutters
        Value value_delete = L(A.outEdges(q1)[i].label, "") *
            simulation_recursively(A.outEdges(q1)[i].target, q2);
        Action action_delete(DELETE, value_delete, A.outEdges(q1)[i].target, q2);
        action_delete.label_old = A.outEdges(q1)[i].label;
        
        // add best action to script
        if (action_delete.value > action_modify.value) {
            script.add(action_delete);
        } else {
            script.add(action_modify);
        }
    }
    
    // discount value of all actions using the sum of actions
    script.value *= (discount() / A.outEdges(q1).size());
    
    return script;
}


/*!
 * \brief   weighted weak quantitative simulation (helper)
 *
 * \param q1  a node of graph A
 * \param q2  a node of graph B
 *
 * \return    the weighted simulation between node q1 and node q2
 */
Value Simulation::simulation_recursively(Node q1, Node q2) {
    // recycle previously calculated values
    if (cache[q1][q2] != 0) {
        cache_hit++;
        return cache[q1][q2];
    }
        
    // value was not in the cache: calculate it
    cache_miss++;
    
    // statistical output
    if (args_info.verbose_flag && cache_miss > 1 && (cache_miss % 1000) == 0) {
        fprintf(stderr, "%8u\n", cache_miss);
    }
    
    // calculate successor values
    if (A.outEdges(q1).empty()) {
        cache[q1][q2] = N(q1,q2);
    } else {
        // collect helper values and choose maximum
        ActionScript script1 = w1(q1,q2);
        ActionScript script2 = w2(q1,q2); 
        ActionScript bestScript = (script1.value > script2.value) ? script1 : script2;
        
        // cache best action
        G_script_cache[q1][q2] = bestScript;
        
        // calculate and cache value
        cache[q1][q2] = (((1-discount()) * N(q1,q2)) + bestScript.value);
    }
    
    
    return cache[q1][q2];
}


/*!
 * \brief weighted weak quantitative simulation
 *
 * The function calculates the "weighted weak quantitative simulation" as it is
 * defined in the paper "Simulation-based graph matching" by Sokolsky et al.
 * In addition to returning a single value, the function also creates an edit
 * script that describes how graph A has to be changed to acheive simulation
 * to B.
 * 
 * \return    the weighted weak quantitative simulation between graph A and
 *            graph B
 *
 * \pre  Both graphs have to be acyclic, because otherwise the algorithm does
 *       not terminate.
 *
 * \post Cache is cleared.
 */ 
Value Simulation::simulation() {
    // reset static values
    cache_hit = 0;
    cache_miss = 0;
    
    // allocate memory for cache
    initializeCache(A, B);
    
    // calculate the matching between the root nodes
    Value result = simulation_recursively(A.getRoot(), B.getRoot());
    
    // statistical output
    double hit_rate = 1 - (static_cast<double>(cache_miss) / static_cast<double>(cache_hit));
    fprintf(stderr, "cache: %u hits, %u misses, hit rate %.2f%%\n",
            cache_hit, cache_miss, hit_rate * 100);
    
    // delete cache
    emptyCache(A, B);    
    
    return result;
}
