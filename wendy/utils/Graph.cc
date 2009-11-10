#include <cstdio>
#include <cstring>
#include <string>
#include <cassert>
#include <map>
#include <vector>
#include "Graph.h"
#include "verbose.h"

using std::map;
using std::string;
using std::vector;

/// set the root of the graph
void Graph::setRoot(Node *n) {
    assert(n);
    root = n;
}

void Graph::setType(Type t) {
    assert(t != NOTDEFINED);
    type = t;
}

/// add a node to the graph
void Graph::addNode(Node *n) {
    assert(n);
    nodes[n->id] = n;
}

/// minimize the graph by removing redundant nodes
void Graph::minimize() {
    // create formula/bit->node mapping
    map<string, vector<Node *> > form;
    for (map<unsigned int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
        if (type == FORMULA)
          form[(n->second)->dnf()].push_back(n->second);
        else
        {
          string bit;
          bit = (n->second)->bit == Node::NONE ? "" :
                (n->second)->bit == Node::S    ? "S" :
                (n->second)->bit == Node::F    ? "F" : "T";
          form[bit].push_back(n->second);
        }
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
    
    status("%d initial partitions (showing only non-trivial partions)\n", partitions.size());

    for (unsigned int i = 0; i < partitions.size(); ++i) {
        if (partitions[i].size() > 1) {
            status("partition %d [", i);
            for (unsigned j = 0; j < partitions[i].size(); ++j) {
                status(" %d", partitions[i][j]->id);
            }
            status(" ]\n");
        }
    }

    status("REFINING");

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
                        status("partition %d: node %d has no %s-successor\n", i, thisNode->id, labels[l].c_str());
                    } else {
                        reach[ partition[reachedNode] ].push_back(thisNode);
                        status("partition %d: node %d has a %s-successor in partition %d\n", i, thisNode->id, labels[l].c_str(), partition[reachedNode]);
                    }
                }

                // if more than one temporary partition is found, we can refine
                if (reach.size() > 1) {
                    status("[PASS %d] refining partition %d to %d sub-partitions using label %s\n", passes, i, reach.size(), labels[l].c_str());

                    for (map<int, vector<Node *> >::iterator np = reach.begin(); np != reach.end(); ++np) {
                        unsigned int newPartitionNumber;

                        if (np == reach.begin()) {
                            newPartitionNumber = i;
                            status("refined partition %d\n", newPartitionNumber);
                        } else {
                            newPartitionNumber = partitions.size();                        
                            status("added partition %d\n", newPartitionNumber);
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

    status("DONE PARTITIONING  (showing only non-trivial partions)\n");
    for (unsigned int i = 0; i < partitions.size(); i++) {
        if (partitions[i].size() > 1) {
            status("partition %d [", i);
            for (unsigned j = 0; j < partitions[i].size(); ++j) {
                status(" %d", partitions[i][j]->id);
            }
            status(" ]\n");
        }
    }

    status("%d partition passes\n", passes);
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
        
        status("%d nodes before reduction\n", nodesBefore);
        status("%d nodes after reduction\n", nodes.size());
    }
    
}

/// adds a label to the graphs
void Graph::addLabel(std::string l) {
    labels.push_back(l);
}

/// OG output format
void Graph::ogOut(FILE *out) const {
    if (!labels.empty())
    {
        Labels temp;
        fprintf(out, "INTERFACE\n");
        if (!(temp = input()).empty())
        {
            fprintf(out, "  INPUT\n");
            fprintf(out, "    ");
            for (int i = 0; i < (int) temp.size(); ++i)
              fprintf(out, i == 0 ? temp[i].c_str() : (string(", ")+temp[i]).c_str());
            fprintf(out, "\n");
        }
        if (!(temp = output()).empty())
        {
            fprintf(out, "  OUTPUT\n");
            fprintf(out, "    ");
            for (int i = 0; i < (int) temp.size(); ++i)
              fprintf(out, i == 0 ? temp[i].c_str() : (string(", ")+temp[i]).c_str());
            fprintf(out, "\n");
        }
        if (!(temp = synchronous()).empty())
        {
            fprintf(out, "  SYNCHRONOUS\n");
            fprintf(out, "    ");
            for (int i = 0; i < (int) temp.size(); ++i)
              fprintf(out, i == 0 ? temp[i].c_str() : (string(", ")+temp[i]).c_str());
            fprintf(out, "\n");
        }
        fprintf(out, "\n");
    } /* INTERFACE */

    fprintf(out, "NODES\n");
    std::string bit;
    fprintf(out, "  %d ", root->id);
    switch (type)
    {
    case FORMULA:
        fprintf(out, (string(": ")+root->formula->toString()).c_str());
        break;
    case BIT:
        bit = root->bitString();
        if (!bit.empty())
            fprintf(out, (string(":: ")+bit).c_str());
        break;
    default:
        assert(false);
        break;
    }
    fprintf(out, "\n");
    for (std::map<std::string, std::vector<Node *> >::const_iterator
          s = root->outEdges.begin(); s != root->outEdges.end(); ++s)
    {
        for (int i = 0; i < (int) s->second.size(); ++i)
            fprintf(out, "    %s -> %d\n", s->first.c_str(), s->second[i]->id);
    }

    for (std::map<unsigned int, Node *>::const_iterator n = nodes.begin();
        n != nodes.end(); ++n)
    {
        if (n->second == root)
            continue;
        fprintf(out, "  %d ", n->second->id);
        switch (type)
        {
        case FORMULA:
            fprintf(out, (string(": ")+n->second->formula->toString()).c_str());
            break;
        case BIT:
            bit = n->second->bitString();
            if (!bit.empty())
                fprintf(out, (string(":: ")+bit).c_str());
            break;
        default:
            assert(false);
            break;
        }
        fprintf(out, "\n");
        for (std::map<std::string, std::vector<Node *> >::const_iterator
              s = n->second->outEdges.begin(); s != n->second->outEdges.end(); ++s)
        {
            for (int i = 0; i < (int) s->second.size(); ++i)
                fprintf(out, "    %s -> %d\n", s->first.c_str(), s->second[i]->id);
        }
    } /* NODES */
}


Labels Graph::input() const {
    Labels result;
    for (int i = 0; i < (int) labels.size(); ++i)
        if (labels[i].c_str()[0] == '?')
            result.push_back(labels[i].substr(1, labels[i].size()));
    return result;
}


Labels Graph::output() const {
    Labels result;
    for (int i = 0; i < (int) labels.size(); ++i)
        if (labels[i].c_str()[0] == '!')
            result.push_back(labels[i].substr(1, labels[i].size()));
    return result;
}


Labels Graph::synchronous() const {
  Labels result;
  for (int i = 0; i < (int) labels.size(); ++i)
      if (labels[i].c_str()[0] == '#')
          result.push_back(labels[i].substr(1, labels[i].size()));
  return result;
}
