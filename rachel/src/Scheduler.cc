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

#include <map>
#include <set>
#include <algorithm>
#include <string>
#include <cassert>

#include "Scheduler.h"

using std::pair;
using std::string;
using std::map;

extern Graph A;
extern Graph B;


void Scheduler::setThreads(unsigned int _threads) {
    assert(_threads > 1);
    threads = _threads;
    fprintf(stderr, "created a scheduler for %d threads\n", threads);
}


void Scheduler::toDot() {
    fprintf(stdout, "digraph D {\n");
    
    // print nodes
    for (set<pair<Node, Node> >::iterator n = nodes.begin(); n != nodes.end(); n++) {
        fprintf(stdout, "\"%u,%u\"\n", n->first, n->second);
        
        // print edges
        for(set<pair<Node, Node> >::iterator n2 = edges[*n].begin(); n2 != edges[*n].end(); n2++) {
            fprintf(stdout, "\"%u,%u\" -> \"%u,%u\"\n", n->first, n->second, n2->first, n2->second);
        }
    }
    
    fprintf(stdout, "}\n");
}


void Scheduler::initialize(Node q1, Node q2) {
    // add current node pair
    nodes.insert(pair<Node,Node>(q1, q2));
    
    // dynamic programming cache
    static map<Node, map<Node, map<Node, map<Node, bool> > > > seen;
    
    Edges out1 = A.outEdges(q1);
    Edges out2 = B.outEdges(q2);
    
    for (size_t i = 0; i < out1.size(); i++) {
        for (size_t j = 0; j < out2.size(); j++) {
            // add edge to successor
            edges[ pair<Node, Node>(q1, q2) ].insert( pair<Node, Node>(out1[i].target, out2[j].target) );
            
            // use dynamic programming cache
            if (seen[q1][q2][out1[i].target][out2[j].target])
                continue;
            seen[q1][q2][out1[i].target][out2[j].target] = true;
            
            // recursive call
            initialize(out1[i].target, out2[j].target);
        }
    }
}


void Scheduler::schedule() {   
    bool done;
    unsigned int max_width = 0;
    int max_height = -1; // loop is repeated once after max heigt is reached
    unsigned int task_id = 0;
    
    do {
        done = true;
        set<pair<Node, Node> > seen;
        unsigned int current_width = 0;
        max_height++;
        
        // iterate nodes
        for (set<pair<Node, Node> >::iterator n = nodes.begin(); n != nodes.end(); n++) {
            if (edges[*n].empty()) {
                ++current_width;
                seen.insert(*n);
            }
        }

        // store maximal width
        if (current_width > max_width)
            max_width = current_width;
        
        // remove seen nodes
        for(set<pair<Node, Node> >::iterator n = seen.begin(); n!= seen.end(); n++) {
            // remove respective edges
            for (set<pair<Node, Node> >::iterator n2 = nodes.begin(); n2 != nodes.end(); n2++) {
                done = false;
                edges[*n2].erase(*n);
            }
            
            // put state pair on the task stack
            tasks.push_back(pair<unsigned int, pair<Node, Node> >(task_id++, *n));
            //fprintf(stderr, "pushed task #%5d (%3d,%3d) for thread %2d at height %3\n", task_id-1, n->first, n->second, (task_id-1) % threads, max_height+1);
            
            nodes.erase(*n);
        }
        
    } while (!done);
    
    
    fprintf(stderr, "\ndependency width:   %4u\n", max_width);
    fprintf(stderr, "depdendency height: %4u\n", max_height);
    fprintf(stderr, "%d tasks on the stack\n\n", tasks.size());
}
