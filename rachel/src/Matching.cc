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

#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <map>

#include "Matching.h"
#include "costfunction.h"
#include "Graph.h"
#include "helpers.h"

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


/**********************
 * initialize statics *
 **********************/

unsigned int Matching::permuteEdges_calls = 0;
unsigned int Matching::calcPermutations_calls = 0;
unsigned int Matching::w_calls = 0;
unsigned int Matching::matching_calls = 0;


/*************
 * functions *
 *************/

/*!
 * \brief create a permutation
 *
 * \note An optimization rules out all permutations in which the same label is
 *       deleted and inserted, e.g. ("a","") and ("","a"). Instead of removing
 *       these pairs and adding a pair ("a","a"), we discard the whole
 *       permutation, because this permutation is generated anyway.
 */
Permutation Matching::permuteEdges(Edges &e1, Edges &e2) {
    permuteEdges_calls++;
    
    assert(e1.size() == e2.size());
    
    // set to store labels of inserted or deleted edges
    set<string> insert_delete;

    // create pairs without ("","")-pairs or duplicates
    Permutation result;
    for (size_t i = 0; i < e1.size(); i++) {
        if (e1[i].label != "" || e2[i].label != "") {

            // Optimization: The variable already is set to true if the current
            // label was already used for deletion or insertion. In this case,
            // the function is directly ended, and an empty permutation is
            // returned.
            bool already = false;

            if (e2[i].label == "")
                already = already || !(insert_delete.insert(e1[i].label).second);
            if (e1[i].label == "")
                already = already || !(insert_delete.insert(e2[i].label).second);

            if (already) {
                permuteEdges_calls--;
                return Permutation();
            }
            
            result.insert( EdgePair(e1[i],e2[i]) );
        }
    }

    return result;
}


/*!
 * \brief calculates permutations
 *
 * \param   q1    a node of graph A
 * \param   q2    a node of graph B
 * \param   beta  an assignment to q2's formula
 *
 * \return  the set of all edge permutations
 *
 * \note The next_permutation function has one additional feature that is quite
 *       useful. It properly generates permutations when some of the members of
 *       the input sequence have identical values. For example, when I generate
 *       all the permutations of "ABCDE", I will get 120 unique character
 *       sequences. But when I generate all the permutations of "AAABB", I only
 *       get 10. This is because there are 6 different identical permutations
 *       of "AAA", and 2 identical permutations of "BB".
 *       (from http://marknelson.us/2002/03/01/next-permutation)
 *
 * \todo Understand the abortion criterion of the while loop: Why is the
 *       counter really necessary?
 */
Permutations Matching::calcPermutations(Node q1, Node q2, Assignment &beta) {
    calcPermutations_calls++;
    
    // collect outgoing edges
    Edges e1 = A.outEdges(q1);
    Edges e2 = beta;

    // add epsilon-edges to fill edge vectors to equal size
    unsigned int e1size = e1.size();
    unsigned int e2size = e2.size();
    for (unsigned int i = 0; i < e2size; i++) {
        e1.push_back( Edge(q1,0,"") );
    }
    for (unsigned int i = 0; i < e1size; i++) {
        e2.push_back( Edge(q2,q2,"") );
    }


    // Create a permutation of the current edge arrays. Then get the next
    // permutation using the next_permutation algorithm from the STL.
    // Eventually, this function returns false. As this alone is not sufficient
    // as abort criterion (WHY?), variable count assures that next_permutation
    // returns false twice before the loop ends.
    bool perm_result;
    unsigned int count = 0;
    Permutations result;
    do {
        Permutation p = permuteEdges(e1, e2);
        if (!p.empty())
            result.insert(p);
        
        perm_result = std::next_permutation(e2.begin(), e2.end());
        count++;
    } while (perm_result || count < 2);
    
    
    return result;
}


/// helper function for matching
ActionScript Matching::w(Node q1, Node q2) {
    w_calls++;
    
    ActionScript result;
    
    // get and iterate the satisfying assignments
    Assignments assignments = B.sat(q2);
    for (unsigned int k = 0; k < assignments.size(); k++) {
        Permutations permutations = calcPermutations(q1,q2,assignments[k]);

        ActionScript assignment_script;

        unsigned int perm_size = 0;
        
        // iterate the permutations
        for (Permutations::iterator perms = permutations.begin();
             perms != permutations.end(); perms++) {

            ActionScript permutation_script;
            
            // iterate the current permutation
            for (Permutation::iterator perm = perms->begin();
                 perm != perms->end(); perm++) {
                Value current_value = 0;
                
                Edge e1 = perm->first;
                Edge e2 = perm->second;
                  
                // add a node
                if (e1.label == "") {
                    Node q_new = A.addNewNode(q1, e2.label);
                    e1.target = q_new;
                }

                if (e2.label == "") {
                    current_value = L(e1.label, "") * A.getDeletionValue(q1);
                } else {
                    current_value = L(e1.label, e2.label) *
                                        matching_recursively(e1.target, e2.target);
                }

                // if a node was added, use the new label for the action
                if (e1.label == "") {
                    e1.label = e2.label;
                }

                
                Action current_action(MODIFY, current_value, e1.target, e2.target);
                current_action.label_old = e1.label;
                current_action.label_new = e2.label;
                if (e1.label == "" || A.addedNodes[e1.target])
                    current_action.setType(INSERT);
                if (e2.label == "")
                    current_action.setType(DELETE);
                
                permutation_script.add(current_action);
            }
            
            // store value for best permutation we found for this assignment
            if (permutation_script.value > assignment_script.value) {
                assignment_script = permutation_script;
                perm_size = perms->size();
            }
        }
        
        // distcount value
        assignment_script.value *= (discount() / perm_size);
        
        // store value for best assignment we found
        if (assignment_script.value > result.value) {
            result = assignment_script;
        }
    }

    return result;
}


/*!
 * \brief weighted matching (helper)
 *
 * \param q1  a node of graph A
 * \param q2  a node of graph B
 *
 * \return    the weighted matching between node q1 and node q2
 */
Value Matching::matching_recursively(Node q1, Node q2) {
    matching_calls++;
        
    if (A.addedNodes[q1]) {
        cache_hit++;
        return B.getInsertionValue(q2);
    }
    
    // recycle previously calculated values
    if (cache[q1][q2] != 0) {
        cache_hit++;
        return cache[q1][q2];
    }
        
    // value was not in the cache: calculate it
    cache_miss++;

    // statistical output
    if (args_info.verbose_flag && cache_miss > 0 && (cache_miss % 1000) == 0) {
        fprintf(stderr, "%8u\n", cache_miss);
    }
    
    // calculate successor values
    if (A.outEdges(q1).empty() && B.isFinal(q2)) {
        cache[q1][q2] = N(q1,q2);
    } else {
        // recursive call
        ActionScript s = w(q1,q2);
        cache[q1][q2] = (1-discount()) * N(q1,q2) + s.value;
        G_script_cache[q1][q2] = s;
    }
    
    
    // probably the cache access can be avoided with a local variable
    return cache[q1][q2];
}


/*!
 * \brief weighted matching
 *
 * \return    the weighted matching between graph A and graph B
 *
 * \pre  Both graphs have to be acyclic, because otherwise the algorithm does
 *       not terminate.
 *
 * \bug  Cache clearing currently crashes.
 */
Value Matching::matching() {
    // reset static values
    permuteEdges_calls = 0;
    calcPermutations_calls = 0;
    w_calls = 0;
    matching_calls = 0;    
    cache_hit = 0;
    cache_miss = 0;
    
    // allocate memory for cache
    initializeCache(A, B);
    
    // preprocess OG to pre-calculate values for insertion
    B.preprocessInsertion();
    
    // calculate the matching between the root nodes
    Value result = matching_recursively(A.getRoot(), B.getRoot());
    
    // statistical output
    double hit_rate= 1 - (static_cast<double>(cache_miss) / static_cast<double>(cache_hit));
    fprintf(stderr, "cache: %u hits, %u misses, hit rate %.2f%%\n",
            cache_hit, cache_miss, hit_rate * 100);
    fprintf(stderr, "%u added nodes\n",
            static_cast<unsigned int>(A.addedNodes.size()));
    fprintf(stderr, "permuteEdges: %u, calcPermutations: %u, w: %u, matching: %u\n",
            permuteEdges_calls, calcPermutations_calls, w_calls, matching_calls);

    // delete cache
    //emptyCache(A, B);    
        
    return result;
}
