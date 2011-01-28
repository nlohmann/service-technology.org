#include <iostream>
#include <set>
#include <cassert>
#include <sstream>
#include <pnapi/pnapi.h>
#include "util.h"
#include "Graph.h"
#include "cmdline.h"

extern gengetopt_args_info args_info;


#define TAU 0

using pnapi::Transition;
typedef std::pair<std::set<unsigned int>::iterator, bool> ii;

unsigned int Graph::r1 = 0;
unsigned int Graph::r2 = 0;
unsigned int Graph::r62 = 0;
unsigned int Graph::r63 = 0;

pnapi::PetriNet Graph::net;

void Graph::info() {
    unsigned int e = 0;
    unsigned int t = 0;

    FOREACH(n, nodes) {
        FOREACH(l, n->second->postset) {
            e += l->second.size();
        }
        t += n->second->postset[TAU].size();
    }

    std::cerr << "N=" << nodes.size() << ", E=" << e << ", TAU=" << t << ", R1=" << r1 << " R2=" << r2 << " R6.2=" << r62 << " R6.3=" << r63 << "\n";
}

void Graph::initLabels() {
    unsigned int event = 1;

    const std::set<pnapi::Label*> outputLabels(net.getInterface().getOutputLabels());
    for (std::set<pnapi::Label*>::const_iterator l = outputLabels.begin(); l != outputLabels.end(); ++l, ++event) {

        const std::set<Transition*> preset((**l).getTransitions());
        FOREACH(t, preset) {
            labels[(**t).getName()] = event;
            labels2[event] = (**t).getName();
        }
    }

    const std::set<pnapi::Label*> inputLabels(net.getInterface().getInputLabels());
    for (std::set<pnapi::Label*>::const_iterator l = inputLabels.begin(); l != inputLabels.end(); ++l, ++event) {
        const std::set<Transition*> postset((**l).getTransitions());
        FOREACH(t, postset) {
            labels[(**t).getName()] = event;
            labels2[event] = (**t).getName();
        }
    }

    const std::set<pnapi::Label*> sync_label_names(net.getInterface().getSynchronousLabels());
    FOREACH(l, sync_label_names) {
        ++event;

        FOREACH(t, (**l).getTransitions()) {
            labels[(**t).getName()] = event;
            labels2[event] = (**t).getName();
        }
    }

    if (args_info.verbose_flag) {
        FOREACH(t, net.getTransitions()) {
            std::cerr << (*t)->getName() << " " << labels[(*t)->getName()] << "\n";
        }
    }

    FOREACH(t, net.getTransitions()) {
        if (labels[(*t)->getName()] == 0) {
            labels2[0] = (*t)->getName();
            break;
        }
    }
}

void Graph::addEdge(unsigned int source, unsigned int target, const char* label) {
    const unsigned int labelNum = labels[label];

    Node* s = nodes[source];
    Node* t = nodes[target];

    if (not s) {
        s = nodes[source] = new Node();
    }
    if (not t) {
        t = nodes[target] = new Node();
    }

    ii ii1 = t->preset[labelNum].insert(source);
    s->postset[labelNum].insert(target);

    if (not ii1.second) {
        r1++;
    }
}

void Graph::dot() {
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

/*!
\note Rule 6.3: Source node has exactly one outgoing edge, and that is a TAU edge.
*/
bool Graph::rule63() {
    FOREACH(n, nodes) {
        // n (source) has exactly one outgoing edge, and that is a TAU edge
        if (n->second->postset.size() == 1 and n->second->postset[TAU].size() == 1) {
            unsigned int source = n->first;
            unsigned int target = *(n->second->postset[TAU].begin());

            if (args_info.verbose_flag) {
                std::cerr << "[R6.3] merge nodes " << source << " and " << target << std::endl;
            }

            // remove the linking tau-edge
            removeEdge(source, target, TAU);

            // merge the nodes (target remains)
            mergeNode(target, source);

            r63++;
            return true;
        }
    }

    return false;
}

bool Graph::rule62() {
    // look at all nodes...
    FOREACH(n1, nodes) {
        // ... and their TAU-successors
        FOREACH(n2, n1->second->postset[TAU]) {
            bool possible = true;

            FOREACH(l, labels) {
                FOREACH(n3, n1->second->postset[l->second]) {
                    if (*n3 != *n2) {
                        if (nodes[*n2]->postset[l->second].find(*n3) == nodes[*n2]->postset[l->second].end()) {
                            possible = false;
                            break;
                        }
                    }
                }
            }

            if (possible) {
                unsigned int source = n1->first;
                unsigned int target = *n2;

                if (args_info.verbose_flag) {
                    std::cerr << "[R6.2] merge nodes " << source << " and " << target << std::endl;
                }

                // remove the linking tau-edge
                removeEdge(source, target, TAU);

                // merge the nodes (source remains)
                mergeNode(source, target);

                r62++;
                return true;
            }
        }
    }

    return false;
}

// only implemented for trivial loops
bool Graph::rule2() {
    // look at all nodes...
    FOREACH(n1, nodes) {
        // ... and their TAU-successors
        FOREACH(n2, n1->second->postset[TAU]) {
            unsigned int source = n1->first;
            unsigned int target = *n2;
            if (source != target and nodes[*n2]->postset[TAU].find(n1->first) != nodes[*n2]->postset[TAU].end()) {

                if (args_info.verbose_flag) {
                    std::cerr << "[R2] merge nodes " << source << " and " << target << std::endl;
                }

                // merge the nodes (source remains)
                mergeNode(source, target);

                // add tau loop
                ii ii1 = n1->second->preset[TAU].insert(source);
                n1->second->postset[TAU].insert(source);

                if (not ii1.second) {
                    r1++;
                }

                r2++;
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
// we may change node1 and node2 to assert 0 remains the initial node
inline void Graph::mergeNode(unsigned int& node1, unsigned int& node2) {
    // make sure 0 remains the initial node
    if (node2 == 0) {
        unsigned int temp = node2;
        node1 = node2;
        node2 = temp;
    }

    Node* const n1 = nodes[node1];
    Node* const n2 = nodes[node2];

    // copy the postset of n2 to n1
    FOREACH(l, n2->postset) {
        FOREACH(t, l->second) {
            ii ii1 = n1->postset[l->first].insert(*t);
            nodes[*t]->preset[l->first].insert(node1);

            if (not ii1.second) {
                r1++;
            }
        }
    }

    FOREACH(l, n2->preset) {
        FOREACH(t, l->second) {
            ii ii1 = n1->preset[l->first].insert(*t);
            nodes[*t]->postset[l->first].insert(node1);

            if (not ii1.second) {
                r1++;
            }
        }
    }

    // remove node2
    removeNode(node2);
}

void Graph::tarjan(unsigned int v, bool firstCall) {
    static std::stack<unsigned int> Tarj;
    static std::set<unsigned int> Tarj_set;
    static unsigned int maxdfs = 0;
    static std::set<unsigned int> weiss;

    if (firstCall) {
        FOREACH(n, nodes) {
            weiss.insert(n->first);
        }
    }

    Node* const n = nodes[v];
    n->dfs = maxdfs;
    n->lowlink = maxdfs;
    maxdfs += 1;

    Tarj.push(v);
    Tarj_set.insert(v);
    weiss.erase(v);

    FOREACH(l, n->postset) {
        FOREACH(vprime, n->postset[l->first]) {
            Node* const nprime = nodes[*vprime];
            if (weiss.find(*vprime) != weiss.end()) {
                tarjan(*vprime, false);
                n->lowlink = MINIMUM(n->lowlink, nprime->lowlink);
            } else {
                if (Tarj_set.find(*vprime) != Tarj_set.end()) {
                    n->lowlink = MINIMUM(n->lowlink, nprime->dfs);
                }
            }
        }
    }


    /* done with state v */
    std::cout << "STATE " << v << " Lowlink: " << n->lowlink;

//    std::cerr << "node " << v << " has lowlink " << n->lowlink << "\n";


    if (n->lowlink == n->dfs) {
        unsigned int count = 0;
        std::stringstream ss;
        unsigned int vstar;
//        std::cerr << " -> SCC: [";
        do {
            ++count;
            vstar = Tarj.top();
            Tarj_set.erase(vstar);
            Tarj.pop();
            if (vstar != v) {
            ss << vstar << " ";
        }
//            std::cerr << vstar << " ";
        } while (vstar != v);
        if (count > 1) {
            std::cout << " SCC: " << ss.str();
        }
//        std::cerr << "]\n";
    }

    std::cout << "\n";

    bool comma = false;
    FOREACH(m, n->markings) {
        std::cout << (comma ? ",\n" : "");
        std::cout << m->first << " : " << m->second;
        if (not comma) {
            comma = true;
        }
    }
    std::cout << "\n\n";

    FOREACH(l, n->postset) {
        FOREACH(vprime, n->postset[l->first]) {
            std::cout << labels2[l->first] << " -> " << *vprime << "\n";
        }
    }
    std::cout << "\n";
}

void Graph::addMarking(unsigned int n, const char* place, unsigned int tokens) {
    Node* state = nodes[n];
    if (not state) {
        state = nodes[n] = new Node();
    }
    state->addMarking(place, tokens);
}

inline void Node::addMarking(const char* place, unsigned int tokens) {
    markings[place] = tokens;
}

void Graph::reenumerate() {
    // collect a sorted list of the nodenames
    std::vector<unsigned int> nodeNames;
    FOREACH(n, nodes) {
        nodeNames.push_back(n->first);
    }

    std::map<unsigned int, unsigned int> translate;

    for (size_t i = 0; i < nodeNames.size(); ++i) {
        if (nodeNames[i] != i) {
            assert(nodes[i] == NULL);

            // rename node in list
            translate[nodeNames[i]] = i;
            nodes[i] = nodes[nodeNames[i]];
            nodes.erase(nodeNames[i]);
        }
    }

    // rename edges
    FOREACH(n1, nodes) {
        FOREACH(l, n1->second->postset) {
            FOREACH(a, translate) {
                if (l->second.erase(a->first) != 0) {
                    l->second.insert(a->second);
                }
            }
        }
        FOREACH(l, n1->second->preset) {
            FOREACH(a, translate) {
                if (l->second.erase(a->first) != 0) {
                    l->second.insert(a->second);
                }
            }
        }
    }
}
