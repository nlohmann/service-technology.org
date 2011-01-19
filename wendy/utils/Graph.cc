#include <iostream>
#include <set>
#include <cassert>
#include <pnapi/pnapi.h>
#include "util.h"
#include "Graph.h"

#define TAU 0
//#define DEBUG 1

extern pnapi::PetriNet net;
using pnapi::Transition;

unsigned int Graph::l = 0;

void Graph::info() {
    unsigned int e = 0;
    unsigned int t = 0;

    FOREACH(n, nodes) {
        FOREACH(l, n->second->postset) {
            e += l->second.size();
        }
        t += n->second->postset[TAU].size();
    }

    std::cerr << nodes.size() << " nodes, " << e << " edges, " << t << " tau edges\n";
}

void Graph::initLabels() {
    unsigned int event = 1;

    const std::set<pnapi::Label*> outputLabels(net.getInterface().getOutputLabels());
    for (std::set<pnapi::Label*>::const_iterator l = outputLabels.begin(); l != outputLabels.end(); ++l, ++event) {

        const std::set<Transition*> preset((**l).getTransitions());
        FOREACH(t, preset) {
            labels[(**t).getName()] = event;
        }
    }

    const std::set<pnapi::Label*> inputLabels(net.getInterface().getInputLabels());
    for (std::set<pnapi::Label*>::const_iterator l = inputLabels.begin(); l != inputLabels.end(); ++l, ++event) {
        const std::set<Transition*> postset((**l).getTransitions());
        FOREACH(t, postset) {
            labels[(**t).getName()] = event;
        }
    }

    const std::set<pnapi::Label*> sync_label_names(net.getInterface().getSynchronousLabels());
    FOREACH(l, sync_label_names) {
        ++event;

        FOREACH(t, (**l).getTransitions()) {
            labels[(**t).getName()] = event;
        }
    }
}

void Graph::addEdge(unsigned int source, unsigned int target, std::string label) {
    const unsigned int labelNum = labels[label];

    Node* s = nodes[source];
    Node* t = nodes[target];

    if (not s) {
        s = nodes[source] = new Node();
    }
    if (not t) {
        t = nodes[target] = new Node();
    }

    t->preset[labelNum].insert(source);
    s->postset[labelNum].insert(target);
}

void Graph::print() {
    std::cout << "digraph d {\n";

    FOREACH(n1, nodes) {
        FOREACH(l, n1->second->postset) {
            FOREACH(n2, l->second) {
                std::cout << "  " << n1->first << " -> " << *n2 << " [label=\"" << l->first << "\"";
                
                if (l->first == TAU) {
                    std::cout << " penwidth=5.0";
                }
                
                std::cout << "];\n";
            }
        }
    }

    std::cout << "}\n";
}

// one node has only one incoming tau edge
bool Graph::rule613() {
    FOREACH(n, nodes) {
        if (n->second->preset.size() == 1 and n->second->preset[TAU].size() == 1) {
            const unsigned int source = *(n->second->preset[TAU].begin());
            const unsigned int target = n->first;

#ifdef DEBUG
            std::cerr << "[R61] merge nodes " << source << " and " << target << std::endl;
#endif

            // remove the linking tau-edge
            removeEdge(source, target, TAU);

            // merge the nodes (target remains)
            merge(target, source);
            return true;
        }

        if (n->second->postset.size() == 1 and n->second->postset[TAU].size() == 1) {
            const unsigned int source = n->first;
            const unsigned int target = *(n->second->postset[TAU].begin());

#ifdef DEBUG
            std::cerr << "[R63] merge nodes " << source << " and " << target << std::endl;
#endif

            // remove the linking tau-edge
            removeEdge(source, target, TAU);

            // merge the nodes (target remains)
            merge(target, source);
            return true;
        }
    }

    return false;
}

bool Graph::r() {
    // look at all nodes...
    FOREACH(n1, nodes) {
        // ... and their TAU-successors
        FOREACH(n2, n1->second->postset[TAU]) {
            bool possible = true;
            
            // check the non-TAU successors of n1
            FOREACH(l, labels) {
                if (l->second == TAU) {
                    continue;
                }
                FOREACH(n3, n1->second->postset[l->second]) {
                    if (nodes[*n2]->postset[l->second].find(*n3) == nodes[*n2]->postset[l->second].end()) {
                        possible = false;
                        break;
                    }
                }
            }
            
            if (possible) {
                const unsigned int source = n1->first;
                const unsigned int target = *n2;

#ifdef DEBUG
                std::cerr << "[R] merge nodes " << source << " and " << target << std::endl;
#endif
                
                // remove the linking tau-edge
                removeEdge(source, target, TAU);

                // merge the nodes (target remains)
                merge(source, target);
                
                return true;
            }
        }
    }
    
    return false;
}

bool Graph::r2() {
    // look at all nodes...
    FOREACH(n1, nodes) {
        // ... and their TAU-successors
        FOREACH(n2, n1->second->postset[TAU]) {
            const unsigned int source = n1->first;
            const unsigned int target = *n2;
            if (source != target and nodes[*n2]->postset[TAU].find(n1->first) != nodes[*n2]->postset[TAU].end()) {

//#ifdef DEBUG
                std::cerr << "[R2] merge nodes " << source << " and " << target << std::endl;
//#endif

                // merge the nodes (target remains)
                merge(source, target);

                // add tau loop
                n1->second->preset[TAU].insert(source);
                n1->second->postset[TAU].insert(source);

                return true;
            }
        }
    }

    return false;
}

bool Graph::rule62() {
    FOREACH(n1, nodes) {
        FOREACH(n2, n1->second->postset[TAU]) {
            bool possible = true;

            FOREACH(l, labels) {
                if (l->second == TAU) {
                    continue;
                }
                if (n1->second->postset[l->second] != nodes[*n2]->postset[l->second]) {
                    possible = false;
                    break;
                }
            }

            if (possible) {
                const unsigned int source = n1->first;
                const unsigned int target = *n2;

#ifdef DEBUG
                std::cerr << "[R62] merge nodes " << source << " and " << target << std::endl;
#endif

                // remove the linking tau-edge
                removeEdge(source, target, TAU);

                // merge the nodes (target remains)
                merge(source, target);

                return true;
            }
        }
    }

    return false;
}

inline void Graph::removeEdge(unsigned int source, unsigned int target, unsigned int label) {
//    std::cerr << "removing edge " << source << " -> " << target << " with label " << label << "\n";

    nodes[source]->postset[label].erase(target);
    nodes[target]->preset[label].erase(source);
}

inline void Graph::removeNode(unsigned int node) {
//    std::cerr << "removing node " << node << "\n";

    FOREACH(n, nodes) {
        FOREACH(l, n->second->preset) {
            l->second.erase(node);
        }
        FOREACH(l, n->second->postset) {
            l->second.erase(node);
        }
    }

    // remove
    delete nodes[node];
    nodes.erase(nodes.find(node));
}

// node1 will remain
inline void Graph::merge(unsigned int node1, unsigned int node2) {
    Node* const n1 = nodes[node1];
    Node* const n2 = nodes[node2];

    // copy the postset of n2 to n1
    FOREACH(l, n2->postset) {
        FOREACH(t, l->second) {
            n1->postset[l->first].insert(*t);
            nodes[*t]->preset[l->first].insert(node1);
        }
    }

    FOREACH(l, n2->preset) {
        FOREACH(t, l->second) {
            n1->preset[l->first].insert(*t);
            nodes[*t]->postset[l->first].insert(node1);
        }
    }

    // remove node2
    removeNode(node2);
}
