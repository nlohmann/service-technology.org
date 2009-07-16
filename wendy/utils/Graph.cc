#include <cstdio>
#include <string>
#include <cassert>
#include <map>
#include <vector>
#include "Graph.h"

using std::map;
using std::string;
using std::vector;

/// set the root of the graph
void Graph::setRoot(Node *n) {
    assert(n);
    root = n;
}

/// add a node to the graph
void Graph::addNode(Node *n) {
    assert(n);
    nodes[n->id] = n;
}

/// minimize the graph by removing redundant nodes
void Graph::minimize() {
    // create formula->node mapping
    map<string, vector<Node *> > form;
    for (map<unsigned int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
        form[(n->second)->dnf()].push_back(n->second);
    }
    
    // the partitions
    map<unsigned int, vector<Node *> > partitions;
    map<Node *, unsigned int> partition;
    
    // initial partition according to formulas
    unsigned int k = 0;
    for (map<string, vector<Node*> >::iterator it = form.begin(); it != form.end(); ++it, ++k) {
        partitions[k] = it->second;
        for (unsigned int i = 0; i < it->second.size(); ++i) {
            partition[it->second[i]] = k;
        }
    }
    
    fprintf(stderr, "%d initial partitions (showing only non-trivial partions)\n", partitions.size());

    for (unsigned int i = 0; i < partitions.size(); i++) {
        if (partitions[i].size() > 1) {
            fprintf(stderr, "partition %d [", i);
            for (unsigned j = 0; j < partitions[i].size(); ++j) {
                fprintf(stderr, " %d", partitions[i][j]->id);
            }
            fprintf(stderr, " ]\n");
        }
    }

    fprintf(stderr, "\nREFINING\n");

    bool done = false;
    unsigned int passes = 0;
    while (!done) {
        done = true;
        passes++;

        // iterate the partitions
        for (unsigned int i = 0; i < partitions.size(); i++) {

            // singleton partitions can't be refined
            if (partitions[i].size() < 2)
                continue;

            // iterate the labels
            for (unsigned l = 0; l != labels.size(); ++l) {

                // new (temporary) partitions
                map<int, vector<Node *> > reach;

                // iterate the nodes inside a partition
                for (unsigned j = 0; j != partitions[i].size(); ++j) {

                    Node *thisNode = partitions[i][j];
                    Node *reachedNode = thisNode->successor(labels[l]);

                    if (reachedNode == NULL) {
                        reach[-1].push_back(thisNode);
                        fprintf(stderr, "partition %d: node %d has no %s-successor\n", i, thisNode->id, labels[l].c_str());
                    } else {
                        reach[ partition[reachedNode] ].push_back(thisNode);
                        fprintf(stderr, "partition %d: node %d has a %s-successor in partition %d\n", i, thisNode->id, labels[l].c_str(), partition[reachedNode]);
                    }
                }

                // if more than one temporary partition is found, we can refine
                if (reach.size() > 1) {
                    fprintf(stderr, "[PASS %d] refining partition %d to %d sub-partitions using label %s\n", passes, i, reach.size(), labels[l].c_str());

                    for (map<int, vector<Node *> >::iterator np = reach.begin(); np != reach.end(); ++np) {
                        unsigned int newPartitionNumber;

                        if (np == reach.begin()) {
                            newPartitionNumber = i;
                            fprintf(stderr, "refined partition %d\n", newPartitionNumber);
                        } else {
                            newPartitionNumber = partitions.size();                        
                            fprintf(stderr, "added partition %d\n", newPartitionNumber);
                        }

                        partitions[newPartitionNumber] = np->second;

                        for (unsigned int nn = 0; nn < np->second.size(); ++nn) {
                            partition[np->second[nn]] = newPartitionNumber;
                        }
                    }

                    done = false;

                    break;
                }
            }
        }
    }

    fprintf(stderr, "\nDONE PARTITIONING  (showing only non-trivial partions)\n");
    for (unsigned int i = 0; i < partitions.size(); i++) {
        if (partitions[i].size() > 1) {
            fprintf(stderr, "partition %d [", i);
            for (unsigned j = 0; j < partitions[i].size(); ++j) {
                fprintf(stderr, " %d", partitions[i][j]->id);
            }
            fprintf(stderr, " ]\n");
        }
    }

    fprintf(stderr, "\n%d partition passes\n", passes);
    if (nodes.size() != partitions.size()) {
        unsigned int nodesBefore = nodes.size();
        
//        fprintf(stderr, "\nMERGING EQUIVALENT NODES\n");
        for (unsigned int i = 0; i < k; i++) {
            if (partitions[i].size() > 1) {
                for (unsigned j = 1; j < partitions[i].size(); ++j) {
//                    fprintf(stderr, "merging node %d with %d\n", partitions[i][j]->id, partitions[i][0]->id);
                    partitions[i][j]->replaceWith(partitions[i][0]);
                    nodes.erase(partitions[i][j]->id);
                }
            }
        }
        
        fprintf(stderr, "%d nodes before reduction\n", nodesBefore);
        fprintf(stderr, "%d nodes after reduction\n", nodes.size());
    }
    
}

/// adds a label to the graphs
void Graph::addLabel(std::string l) {
    labels.push_back(l);
}

/// Fiona OG output
void Graph::ogOut() const {
    fprintf(stdout, "INTERFACE\n");
    fprintf(stdout, "  INPUT\n");
    fprintf(stdout, "  OUTPUT\n");
    
    fprintf(stdout, "NODES\n");
    for (map<unsigned int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
        if (n != nodes.begin())
            fprintf(stdout, ",\n");

        fprintf(stdout, "  %d : %s : blue", n->second->id, n->second->formula->toString().c_str());        
    }
    fprintf(stdout, ";\n\n");


    fprintf(stdout, "INITIALNODE\n  %d;\n\n", root->id);


    fprintf(stdout, "TRANSITIONS\n");
    for (map<unsigned int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
        for (map<string, vector<Node*> >::iterator e = n->second->outEdges.begin(); e != n->second->outEdges.end(); ++e) {
            for (unsigned int i = 0; i < e->second.size(); ++i) {
                
                if (n != nodes.begin() || e != n->second->outEdges.begin() || i != 0)
                    fprintf(stdout, ",\n");
                
                fprintf(stdout, "  %d -> %d : %s", n->second->id, e->second[i]->id, e->first.c_str());
            }
        }
    }
    fprintf(stdout, ";\n");
    
}
