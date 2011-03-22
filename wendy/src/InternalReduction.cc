#include <iostream>
#include <set>
#include <cassert>
#include <sstream>
#include <pnapi/pnapi.h>
#include "InnerMarking.h"
#include "util.h"
#include "InternalReduction.h"
#include "cmdline.h"
#include "verbose.h"

extern gengetopt_args_info args_info;

extern int reducedgraph_parse();
extern int reducedgraph_lex_destroy();
extern FILE* reducedgraph_in;


#define TAU 0

using pnapi::Transition;
typedef std::pair<std::set<unsigned int>::iterator, bool> ii;

Graph *Graph::g;
unsigned int Graph::r1 = 0;
unsigned int Graph::r2 = 0;
unsigned int Graph::r62 = 0;
unsigned int Graph::r63 = 0;


void Graph::shortInfo() {
    message("   N=%d, R1=%d, R2=%d, R6.2=%d, R6.3=%d", nodes.size(), r1, r2, r62, r63);
}

void Graph::info() {
    unsigned int e = 0;
    unsigned int t = 0;

    FOREACH(n, nodes) {
        FOREACH(l, n->second->postset) {
            e += l->second.size();
        }
        t += n->second->postset[TAU].size();
    }

    message("   N=%d, E=%d, τ=%d, R1=%d, R2=%d, R6.2=%d, R6.3=%d", nodes.size(), e, t, r1, r2, r62, r63);
}

void Graph::initLabels() {
    unsigned int event = 1;

    const std::set<pnapi::Label*> outputLabels(InnerMarking::net->getInterface().getOutputLabels());
    for (std::set<pnapi::Label*>::const_iterator l = outputLabels.begin(); l != outputLabels.end(); ++l, ++event) {

        const std::set<Transition*> preset((**l).getTransitions());
        FOREACH(t, preset) {
            labels[(**t).getName()] = event;
            labels2[event] = (**t).getName();
        }
    }

    const std::set<pnapi::Label*> inputLabels(InnerMarking::net->getInterface().getInputLabels());
    for (std::set<pnapi::Label*>::const_iterator l = inputLabels.begin(); l != inputLabels.end(); ++l, ++event) {
        const std::set<Transition*> postset((**l).getTransitions());
        FOREACH(t, postset) {
            labels[(**t).getName()] = event;
            labels2[event] = (**t).getName();
        }
    }

    const std::set<pnapi::Label*> sync_label_names(InnerMarking::net->getInterface().getSynchronousLabels());
    FOREACH(l, sync_label_names) {
        ++event;

        FOREACH(t, (**l).getTransitions()) {
            labels[(**t).getName()] = event;
            labels2[event] = (**t).getName();
        }
    }

//    if (args_info.debug_flag) {
//        FOREACH(t, net.getTransitions()) {
//            std::cerr << (*t)->getName() << " " << labels[(*t)->getName()] << "\n";
//        }
//    }

    FOREACH(t, InnerMarking::net->getTransitions()) {
        if (labels[(*t)->getName()] == 0) {
            labels2[0] = (*t)->getName();
            break;
        }
    }
}

void Graph::addEdge(unsigned int source, unsigned int target, const char* label) {
    const unsigned int labelNum = labels[label];

    GraphNode* s = nodes[source];
    GraphNode* t = nodes[target];

    if (not s) {
        s = nodes[source] = new GraphNode();
    }
    if (not t) {
        t = nodes[target] = new GraphNode();
    }

    ii ii1 = t->preset[labelNum].insert(source);
    s->postset[labelNum].insert(target);

    if (not ii1.second) {
        r1++;
    }
}

void Graph::addFinal(unsigned int node) {
    GraphNode* n = nodes[node];
    assert(n);
    n->isFinal = true;
}

Output* Graph::out() {
    Output* temp = new Output();

    FOREACH(n1, orderedNodes) {
        const GraphNode* v1 = nodes[*n1];

        temp->stream() << "STATE " << v1->dfs << " Lowlink: " << v1->lowlink << " " << v1->scc << "\n";

        bool comma = false;
        FOREACH(m, v1->markings) {
            temp->stream() << (comma ? ",\n" : "");
            temp->stream() << m->first << " : " << m->second;
            if (not comma) {
                comma = true;
            }
        }
        temp->stream() << "\n\n";


        FOREACH(l, v1->postset) {
            FOREACH(n2, l->second) {
                const GraphNode* v2 = nodes[*n2];

                temp->stream() << labels2[l->first] << " -> " << v2->dfs << "\n";
            }
        }
        temp->stream() << "\n";
    }

    temp->stream() << std::flush;

    return temp;
}



bool Graph::metaRule() {
    // look at all nodes...
    FOREACH(n1, nodes) {
        assert(nodes[n1->first]);

        /**********\
        * RULE 6.3 *
        \**********/

        // n1 (source) has exactly one outgoing edge, and that is a TAU edge
        if (n1->second->postset.size() == 1 and n1->second->postset[TAU].size() == 1) {
            unsigned int source = n1->first;
            unsigned int target = *(n1->second->postset[TAU].begin());
            assert(nodes[target]);

//            if (args_info.debug_flag) {
//                std::cerr << "[R6.3] merge nodes " << source << " and " << target << std::endl;
//            }

            // remove the linking tau-edge
            removeEdge(source, target, TAU);

            // merge the nodes (target remains)
            mergeNode(target, source);

            r63++;
            return true;
        }


        // ... and their TAU-successors
        FOREACH(n2, n1->second->postset[TAU]) {
            unsigned int source = n1->first;
            unsigned int target = *n2;
            assert(nodes[target]);

            /********\
            * RULE 2 *
            \********/

            if (source != target and nodes[*n2]->postset[TAU].find(n1->first) != nodes[*n2]->postset[TAU].end()) {

//                if (args_info.debug_flag) {
//                    std::cerr << "[R2] merge nodes " << source << " and " << target << std::endl;
//                }

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


            /**********\
            * RULE 6.2 *
            \**********/

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
//                if (args_info.debug_flag) {
//                    std::cerr << "[R6.2] merge nodes " << source << " and " << target << std::endl;
//                }

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



/*!
\note Rule 6.3: Source node has exactly one outgoing edge, and that is a TAU edge.
*/
bool Graph::rule63() {
    FOREACH(n, nodes) {
        // n (source) has exactly one outgoing edge, and that is a TAU edge
        if (n->second->postset.size() == 1 and n->second->postset[TAU].size() == 1) {
            unsigned int source = n->first;
            unsigned int target = *(n->second->postset[TAU].begin());

//            if (args_info.debug_flag) {
//                std::cerr << "[R6.3] merge nodes " << source << " and " << target << std::endl;
//            }

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

//                if (args_info.debug_flag) {
//                    std::cerr << "[R6.2] merge nodes " << source << " and " << target << std::endl;
//                }

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

//                if (args_info.debug_flag) {
//                    std::cerr << "[R2] merge nodes " << source << " and " << target << std::endl;
//                }

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
    nodes[source]->postset[label].erase(target);
    nodes[target]->preset[label].erase(source);
}

inline void Graph::removeNode(unsigned int node) {
    FOREACH(n, nodes) {
        FOREACH(l, n->second->preset) {
            l->second.erase(node);
        }
        FOREACH(l, n->second->postset) {
            l->second.erase(node);
        }
    }

    // remove
//    delete nodes[node]; // would be expensive and useless
    nodes[node] = NULL;
    nodes.erase(nodes.find(node));
    deleted.insert(node);
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

    GraphNode* const n1 = nodes[node1];
    GraphNode* const n2 = nodes[node2];
    assert(n1);
    assert(n2);

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

    // don't reduce away final states
    if (n2->isFinal) {
        n1->markings = n2->markings;
        n1->isFinal = true;
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
        status("detecting SCCs");
        FOREACH(n, nodes) {
            weiss.insert(n->first);
        }
    }

    GraphNode* const n = nodes[v];
    assert(n);
    n->dfs = maxdfs;
    n->lowlink = maxdfs;
    maxdfs += 1;

    Tarj.push(v);
    Tarj_set.insert(v);
    weiss.erase(v);

    FOREACH(l, n->postset) {
        FOREACH(vprime, n->postset[l->first]) {
            GraphNode* const nprime = nodes[*vprime];
            assert(nprime);
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

    orderedNodes.push_back(v);

    if (n->lowlink == n->dfs) {
        unsigned int count = 0;
        std::stringstream ss;
        unsigned int vstar;
        do {
            ++count;
            vstar = Tarj.top();
            Tarj_set.erase(vstar);
            Tarj.pop();
            if (vstar != v) {
                ss << vstar << " ";
            }
        } while (vstar != v);

        if (count > 1) {
            n->scc = "SCC: " + ss.str();
        }
    }
}

void Graph::addMarking(unsigned int n, const char* place, unsigned int tokens) {
    GraphNode* state = nodes[n];
    if (not state) {
        state = nodes[n] = new GraphNode();
    }
    state->addMarking(place, tokens);
}

inline void GraphNode::addMarking(const char* place, unsigned int tokens) {
    markings[place] = tokens;
}

void Graph::reenumerate() {
    // collect a sorted list of the nodenames
    std::vector<unsigned int> nodeNames;
    FOREACH(n, nodes) {
        nodeNames.push_back(n->first);
    }

    for (size_t i = 0; i < nodeNames.size(); ++i) {
        if (nodeNames[i] != i) {
            assert(nodes[i] == NULL);

            // rename node in list
            translate[nodeNames[i]] = i;
            nodes[i] = nodes[nodeNames[i]];
            nodes.erase(nodeNames[i]);
        }
    }

    status("reenumerating edges");

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


void Graph::reenumerate2() {
    // collect a sorted list of the nodenames
    std::vector<unsigned int> nodeNames;
    FOREACH(n, nodes) {
        nodeNames.push_back(n->first);
    }

    std::set<unsigned int>::iterator freeSpace = deleted.begin();
    for (size_t i = nodeNames.size(); i > 0; --i) {
//        printf("checking %d\n", nodeNames[i-1]);
        if (nodeNames[i - 1] != i and nodeNames[i - 1] > nodeNames.size()) {
//            printf(" -> move to %d\n", *freeSpace);
            translate[nodeNames[i - 1]] = *freeSpace;


            // rename node in list
            nodes[*freeSpace] = nodes[nodeNames[i - 1]];
            nodes.erase(nodeNames[i - 1]);
            freeSpace++;

        }
    }

    status("reenumerating edges (%lu/%lu)", translate.size(), nodes.size());

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

GraphNode::GraphNode() : isFinal(false) {}


Output *Graph::internalReduction(FILE *fullGraph) {
    g = new Graph();
    g->initLabels();

    reducedgraph_in = fullGraph;
    reducedgraph_parse();
    pclose(reducedgraph_in);
    reducedgraph_lex_destroy();

    g->info();

    static unsigned int i = 0;
    static const unsigned int j = 1000;

    while (g->metaRule()) {
        if (++i % j == 0) {
            g->shortInfo();
        }
    }
    g->info();

    g->reenumerate();
    g->tarjan();

    Output *result = g->out();
    delete g;
    return result;
}

Graph::~Graph() {
    FOREACH(node, nodes) {
        delete node->second;
        node->second = NULL;
    }
}
