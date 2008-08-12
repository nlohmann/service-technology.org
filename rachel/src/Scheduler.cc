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

#include "Scheduler.h"

using std::pair;
using std::string;
using std::map;


void Scheduler::addNode(Node q1, Node q2) {
    nodes.insert(pair<Node,Node>(q1, q2));
}


void Scheduler::addEdge(Node q11, Node q12, Node q21, Node q22) {
    edges[ pair<Node, Node>(q11, q12) ].insert( pair<Node, Node>(q21, q22) );
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


void Scheduler::initialize(Graph &g1, Graph &g2, Node q1, Node q2) {
    // add current node pair
    addNode(q1, q2);
    
    // dynamic programming cache
    static map<Node, map<Node, map<Node, map<Node, bool> > > > seen;
    
    Edges out1 = g1.outEdges(q1);
    Edges out2 = g2.outEdges(q2);
    
    for (size_t i = 0; i < out1.size(); i++) {
        for (size_t j = 0; j < out2.size(); j++) {
            // add edge to successor
            addEdge(q1, q2, out1[i].target, out2[j].target);
            
            // use dynamic programming cache
            if (seen[q1][q2][out1[i].target][out2[j].target])
                continue;
            seen[q1][q2][out1[i].target][out2[j].target] = true;
            
            // recursive call
            initialize(g1, g2, out1[i].target, out2[j].target);
        }
    }
}


void Scheduler::schedule() {
    bool done;
    set<pair<Node, Node> > seen;
    unsigned int max_width = 0;
    unsigned int current_width = 0;
    unsigned int max_height = 0;
    
    do {
        done = true;
        current_width = 0;
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
        
        // remove printed nodes
        for(set<pair<Node, Node> >::iterator n = seen.begin(); n!= seen.end(); n++) {
            // remove respective edges
            for (set<pair<Node, Node> >::iterator n2 = nodes.begin(); n2 != nodes.end(); n2++) {
                done = false;
                edges[*n2].erase(*n);
            }
            nodes.erase(*n);            
        }
    } while (!done);
    
    fprintf(stderr, "dependency width:   %4u\n", max_width);
    fprintf(stderr, "depdendency height: %4u\n\n", max_height);
}
