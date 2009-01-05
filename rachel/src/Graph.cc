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
#include <set>
#include "config.h"
#include "Graph.h"
#include "Formula.h"
#include "helpers.h"
#include "costfunction.h"

using std::vector;
using std::map;
using std::max;
using std::string;
using std::set;


/**************************************
 * implementation of member functions *
 **************************************/

/**************
 * class Edge *
 **************/

/*!
 * \brief comparison operator: equality
 *
 * Compares edges for equality.
 */
bool Edge::operator == (const Edge &e) const {
    return (source == e.source &&
            target == e.target &&
            label == e.label);
}


/*!
 * \brief comparison operator: less
 *
 * Compares edges using a lexicographical order.
 */
bool Edge::operator < (const Edge &e) const {
    if (label < e.label) {
        return true;
    }

    if (label == e.label && source < e.source) {
        return true;
    }

    if (label == e.label && source == e.source && target < e.target) {
        return true;
    }
    
    return false;
}


/*!
 * \brief constructor
 *
 * Constructs an edge given source, target, and label.
 */
Edge::Edge(Node _source, Node _target, const Label _label) :
    source(_source), target(_target), label(_label), type() {
}


/*!
 * \brief constructor
 *
 * Constructs an empty edge, labeled with "" (epsilon).
 */
Edge::Edge() :
  source(0), target(0), label(""), type() {
}


/***************
 * class Graph *
 ***************/

/// returns outgoing edges of a node (not newly added nodes!)
Edges Graph::outEdges(Node q) {
    Edges result;
    
    for (size_t i = 0; i < edges[q].size(); ++i) {
        if ( !addedNodes[edges[q][i].target] )
            result.push_back(edges[q][i]);
    }
    
    return result;
}


/// returns outgoing send edges of a node (not newly added nodes!)
std::set<Label> Graph::sendLabels(Node q) {
    std::set<Label> result;
    
    for (size_t i = 0; i < edges[q].size(); ++i) {
        if ( !addedNodes[edges[q][i].target] && edges[q][i].label.substr(0,1) == "!" )
            result.insert(edges[q][i].label);
    }    
    
    return result;
}


/// returns outgoing receive edges of a node (not newly added nodes!)
std::set<Label> Graph::receiveLabels(Node q) {
    std::set<Label> result;
    
    for (size_t i = 0; i < edges[q].size(); ++i) {
        if ( !addedNodes[edges[q][i].target] && edges[q][i].label.substr(0,1) == "?" )
            result.insert(edges[q][i].label);
    }    
    
    return result;
}


/// returns (at most 1!) successor of a node with a given label
Edge Graph::successor(Node q, const Label &a) {
    for (size_t i = 0; i < edges[q].size(); ++i) {
        if (edges[q][i].label == a && addedNodes[edges[q][i].source] == false)
            return edges[q][i];
    }
    
    return Edge();
}


/*!
 * \brief add an edge to the graph
 *
 * \param q1   the source node of the edge
 * \param q2   the target node of the edge
 * \param l    the label of the edge
 * \param type the type of the edge if the graph represents the edit distance
 *             (initialized to NONE if none is given)
 */
Edge Graph::addEdge(Node q1, Node q2, const Label l, action_type type) {
    Edge result = Edge(q1,q2,l);
    result.type = type;
    edges[q1].push_back(result);

    return result;
}


/*!
 * \brief add formula to a node
 *
 * \param q  a node
 * \param f  a formula
 *
 * \pre The pointer f is not NULL.
 */
void Graph::addFormula(Node q, Formula *f) {
    assert(f != NULL);    
    formulas[q] = f;
}


/// returns all combinations of edges that satisfy a node's annotation
Assignments Graph::sat(Node q) {
    Assignments result;
    
    // collect labels satisfying the annotation
    vector<Labels> satisfyingLabels = checkSat(q);
    
    // for each label, find the respective edge
    for (size_t i = 0; i < satisfyingLabels.size(); ++i) {
        Assignment temp;
        for (size_t j = 0; j < satisfyingLabels[i].size(); ++j) {
            temp.push_back(successor(q, satisfyingLabels[i][j]));
        }
        result.push_back(temp);
    }
    
    return result;    
}


/*!
 * \brief constructor
 *
 * Constructs a graph with a given identifyer.
 *
 * \param id  identifier of the graph
 *
 */
Graph::Graph(const std::string _id) :
  edges(), root(), max_value(0), insertionValue(), deletionValue(),
  formulas(), id(_id),  nodes(), addedNodes() {
}


/// returns a Dot representation of the graph
string Graph::toDot() {
    string result;

    for (size_t i = 0; i != nodes.size(); ++i) {
        result += "  q_" + id + "_" + toString(nodes[i]) + " [label=\"q" + toString(nodes[i]);
                
        if (formulas[nodes[i]] != NULL)
            result += ": " + formulas[nodes[i]]->toString();
        
        result += "\"";
        
        if (addedNodes[nodes[i]])
            result += " style=dashed";
        
        result += "];\n";

        for (size_t j = 0; j < edges[nodes[i]].size(); ++j) {
            result += "  q_"  + id + "_" + toString(edges[nodes[i]][j].source);
            result += " -> q_" + id + "_" + toString(edges[nodes[i]][j].target);
            result += " [label=\"" + edges[nodes[i]][j].label + "\"";

            if (addedNodes[edges[nodes[i]][j].source] || addedNodes[edges[nodes[i]][j].target])
                result += " style=dashed";

            switch (edges[nodes[i]][j].type) {
                case(DELETE): result += " color=red"; break;
                case(INSERT): result += " color=green"; break;
                case(MODIFY): result += " color=orange"; break;
                case(NONE):
                case(KEEP):
                default: break;
            }
            
            result += "]";
            result += ";\n";
        }
    }
    
    result += "label=\"" + id + "\"\n";
    
    return result;
}


/// returns all satisfying label vectors
vector<Labels> Graph::checkSat(Node q) {
    vector<Labels> result;
    
    vector<unsigned int> index;
    vector<unsigned int> bounds;
    unsigned int count = 1;
    unsigned int degree = edges[q].size();
    for (size_t i = 0; i < degree; ++i) {
        index.push_back(0);
        bounds.push_back(2);
        count *= 2;
    }
    
    for (size_t j = 0; j < count; ++j) {
        set<string> tempSet;
        Labels tempLabels;
        for (size_t i = 0; i < degree; ++i) {
            if (index[i] != 0) {
                tempSet.insert(edges[q][i].label);
                tempLabels.push_back(edges[q][i].label);
            }
        }
        
        if (formulas[q] != NULL) {
            if (formulas[q]->sat(tempSet)) {
                result.push_back(tempLabels);
            }
        }
        
        // update the indices
        next_index(index, bounds);
    }
    
    return result;
}


/// returns true iff the node's annotation is fulfilled by "final"
bool Graph::isFinal(Node q) {
    set<Label> test;
    test.insert("FINAL");

    if (formulas[q] != NULL) {
        if (formulas[q]->sat(test))
            return true;
    }
    
    return false;
}


/// gets the root node
Node Graph::getRoot() const {
    return root;
}


/// sets the root node
void Graph::setRoot(Node q) {
    root = q;
}


/// add a node to the graph
Node Graph::addNode(Node q) {
    // if node is present, return it
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i] == q)
            return q;
    }
    
    this->max_value = max(q, max_value);
    this->nodes.push_back(q);
    this->addedNodes[q] = false;
    
    return q;
}


/*!
 * \brief add a new node and connect it with a new edge
 *
 * \note If the node to add is already present (either already inserted or
 *       present from the start) that node is returned instead.
 *
 * \todo Remove this function: adding of nodes is not necessary to calculate
 *       simulation or matching.
 */
Node Graph::addNewNode(Node q, const Label l) {
    // if node was already added here, return it
    for (size_t i = 0; i < edges[q].size(); ++i) {
        if ( edges[q][i].label == l ) //&& addedNodes[ edges[q][i].target ] )
            return edges[q][i].target;
    }
    
    // add a new node and a corresponding edge
    Node new_q = this->max_value + 1;
    addNode(new_q);
    addedNodes[new_q] = true;
    addEdge(q, new_q, l);
    
    return new_q;
}


/// preprocess the graph with deletion values (helper)
Value Graph::preprocessDeletionRecursively(Node q) {
    if (deletionValue[q] != 0)
        return deletionValue[q];

    if (edges[q].empty()) {
        deletionValue[q] = 1;
        return 1;
    }
    
    Value result = 0;

    // iterate the successor nodes and collect their preprocess value
    for (unsigned i  = 0; i < edges[q].size(); ++i) {
        result += L(edges[q][i].label, "") * preprocessDeletionRecursively(edges[q][i].target);
    }
    
    return result;
}


/// preprocess the graph with insertion values (helper)
Value Graph::preprocessInsertionRecursively(Node q) {
    if (insertionValue[q] != 0)
        return insertionValue[q];
    
    // If the node's formula can be satisfied with "FINAL", any subsequent
    // addition of nodes would be sub-optimal.
    // What about nodes without successors?
    if (isFinal(q)) {
        insertionValue[q] = 1;
        return 1;
    }    
    
    Value bestValue = 0;
    
    // get and iterate the satisfying assignments
    Assignments assignments = sat(q);
    for (size_t k = 0; k < assignments.size(); ++k) {
        
        Assignment beta = assignments[k];
        
        Value newValue = 0;
        
        // iterate the successor nodes and collect their preprocess value
        for (unsigned i  = 0; i < beta.size(); ++i) {
            newValue += L("", beta[i].label) * preprocessInsertionRecursively(beta[i].target);
        }
        
        // divide by size of the assignment and discount
        newValue *= (discount() / beta.size());       
        
        bestValue = max(bestValue, newValue);
    }
    
    bestValue += (1-discount());    
    insertionValue[q] = bestValue;
    
    return bestValue;
}


/// preprocess the graph with deletion values (for service automata)
void Graph::preprocessDeletion() {
    preprocessDeletionRecursively(root);
}


/// preprocess the graph with insertion values (for OGs)
void Graph::preprocessInsertion() {
    preprocessInsertionRecursively(root);
}


/// return deletion value (for service automata)
Value Graph::getDeletionValue(Node q) {
    return deletionValue[q];
}


/// return insertion value (for OGs)
Value Graph::getInsertionValue(Node q) {
    return insertionValue[q];
}


/*!
 * \brief calculate the number of characterized services (helper)
 *
 * The algorithm traverses the satisfying assignments of the given node's
 * formula and recursively checks the successor nodes. A mapping implements a
 * cache for dynammic programming to avoid repetitive calculations of already
 * seen nodes.
 *
 * \param q  a node
 *
 * \return   the number of services represented by the subgraph starting here
 */
long double Graph::countServicesRecursively(Node q) {
    // a cache for dynamic programming
    static map<Node, long double> cache;
    if (cache[q] != 0)
        return cache[q];
    
    Assignments assignments = sat(q);
    
    // the number of services represented by the subgraph starting here
    long double result = 0;
    
    // traverse the satisfying assignments of the node's formula
    for (size_t k = 0; k < assignments.size(); ++k) {
        Assignment beta = assignments[k];

        // the number of services represented by this assignment
        long double temp = 1;

        // multiply all recursively reachable services
        for (size_t i = 0; i < beta.size(); ++i) {
            temp *= countServicesRecursively(beta[i].target);
        }

        // add up the numbers of each satisfying assignment        
        result += temp;
    }

    // in case no assignment was found, this node would be red!
    assert (result != 0);
    
    cache[q] = result;
    return result;
}


/*!
 * \brief calculate the number of characterized services
 *
 * Calculates the number of the acyclic deterministic services represented by
 * the graph (up to tree isomorphism).
 *
 * \return the number of characterized services or 'inf' if this number is
 *         greater than 10**4932
 */
long double Graph::countServices() {
    return countServicesRecursively(root);
}


/// check if graph is cyclic (helper)
bool Graph::isCyclicRecursively(Node q) {
    // call stack to detect cyclic calls
    static vector<Node> call_stack;
    
    // the result so far
    static bool result = false;
    
    if (result)
        return true;
    
    // check if this combination is already on the call stack
    for (size_t i = 0; i < call_stack.size(); ++i) {
        if (call_stack[i] == q) {
            result = true;
            return true;
        }
    }
    
    call_stack.push_back(q);

    for (size_t i = 0; i < edges[q].size(); ++i) {
        result = result || isCyclicRecursively(edges[q][i].target);
    }
    
    call_stack.pop_back();

    return result;
}


/// check if graph is cyclic
bool Graph::isCyclic() {
    return isCyclicRecursively(root);
}


/// coherently re-enumerate the graph's nodes
void Graph::reenumerate() {
    map<Node, Node> translation1;
    map<Node, Node> translation2;
    std::map<Node, Edges> new_edges;
    std::map<Node, Formula*> new_formulas;
    Nodes new_nodes;
    
    addedNodes.clear();

    // re-indexing nodes
    for (size_t i = 0; i < nodes.size(); ++i) {
        translation1[i] = nodes[i];
        translation2[nodes[i]] = i;
        new_formulas[i] = formulas[nodes[i]];
        addedNodes[i] = false;
        new_nodes.push_back(i);
    }
    
    // using the new index for the edges
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = 0; j < edges[nodes[i]].size(); ++j) {
            Edge e;
            e.source = translation2[ edges[nodes[i]][j].source ];
            e.target = translation2[ edges[nodes[i]][j].target ];
            e.label = edges[nodes[i]][j].label;
            new_edges[i].push_back(e);
        }
    }
    
    root = translation2[root];
    max_value = nodes.size()-1;
    
    nodes = new_nodes;
    edges = new_edges;
    formulas = new_formulas;
}


/// return the average size of the satisfying assignments
double Graph::averageSatSize() {
    double result = 0;
    
    for (size_t i = 0; i < nodes.size(); ++i) {
        result += sat(nodes[i]).size();
    }
    
    return (result / static_cast<double>(nodes.size()));
}


/// calculate a compact represenation of the OG's formulae
void Graph::calculateCompactAnnotations() {
    unsigned int F_count = 0;
    unsigned int F_prime_count = 0;
    unsigned int S_count = 0;
    unsigned int S1_count = 0;
    unsigned int S2_count = 0;
    
    // traverse the OG's nodes
    for (size_t i = 0; i < nodes.size(); ++i) {
        bool in_F = false;
        bool in_S = false;
        
        // STEP 1: treat the "final" assignment
        // if "final" satisfies the formula, this node belongs to F
        if (formulas[nodes[i]]->hasFinal()) {
            F_count++;
            in_F = true;
            fprintf(stderr, "node %d is in F\n", nodes[i]);
            
            // if the node has no outgoing edges, its implicitly in F
            if (!edges[nodes[i]].empty()) {
                F_prime_count++;
                fprintf(stderr, "node %d is in F'\n", nodes[i]);
            }
        }
        

        // STEP 2: check if annotation can only be fulfilled by sending
        // (i.e., not by receiving or final)
        if (formulas[nodes[i]] != NULL) {
            if (!formulas[nodes[i]]->sat(receiveLabels(nodes[i]))) {
                S_count++;
                in_S = true;
                fprintf(stderr, "node %d is in S\n", nodes[i]);
            }
        }

        // check for an alternative representation of S
        // additional check?: !edges[nodes[i]].empty()
        if (receiveLabels(nodes[i]).empty()) {
            if (!in_S && !in_F) {
                S2_count++;
                fprintf(stderr, "node %d is in S2\n", nodes[i]);
            }
        } else {
            if (in_S) {
                S1_count++;
                fprintf(stderr, "node %d is in S1\n", nodes[i]);
            }
        }        
    }
        
    fprintf(stderr, "\n");
    fprintf(stderr, "# nodes with final annotation:     %5d = |F|\n", F_count);
    fprintf(stderr, "  ~ without successor:             %5d\n", F_count - F_prime_count);
    fprintf(stderr, "  ~ with successor:                %5d = |F'|\n", F_prime_count);
    fprintf(stderr, "  ~ effect of implicit storage:    %5.2f %%\n",
        (double(F_prime_count) / double(F_count)) * 100.0);
    fprintf(stderr, "# nodes with must send annotation: %5d = |S|\n", S_count);
    fprintf(stderr, "  ~ with ?-successor:              %5d = |S1|\n", S1_count);
    fprintf(stderr, "  * without ?-successor:           %5d = |S2|\n", S2_count);
    fprintf(stderr, "  ~ effect of implicit storage:    %5.2f %%\n",
        (double(S1_count + S2_count) / double(S_count)) * 100.0);
    fprintf(stderr, "# nodes of OG:                     %5d\n", nodes.size());
    fprintf(stderr, "  ~ with set bits (explicit)       %5d (%6.2f %%)\n",
        F_count + S_count, ((double(F_count + S_count) / double(nodes.size())) * 100.0) );
    fprintf(stderr, "  ~ with set bits (implicit)       %5d (%6.2f %%)\n",
        F_prime_count + S1_count + S2_count,
        ((double(F_prime_count + S1_count + S2_count) / double(nodes.size())) * 100.0) );    
}


/// BPMN output of a service automaton
void Graph::bpmnOutput() {
    char *xor_generic = "shape=\"diamond\" regular=\"true\" height=\"0.7\" label=\"\" image=\"bpmn/l_xor.png\"";
    char *mul_generic = "shape=\"diamond\" regular=\"true\" height=\"0.7\" label=\"\" image=\"bpmn/l_multiple.png\"";
    char *start_generic = "shape=\"circle\" label=\"\" width=\"0.6\" height=\"0.6\"";
    char *stop_generic = "shape=\"circle\" label=\"\" penwidth=\"4\" width=\"0.6\" height=\"0.6\"";
    char *send_generic = "shape=\"circle\" peripheries=\"2\" width=\"0.5\" height=\"0.5\" image=\"bpmn/l_send.png\"";
    char *receive_generic = "shape=\"circle\" peripheries=\"2\" width=\"0.5\" height=\"0.5\" image=\"bpmn/l_receive.png\"";    
    
    fprintf(stdout, "digraph G {\n");
    fprintf(stdout, "  graph [rankdir=\"LR\" fontname=\"Helvetica\" fontsize=\"12\"];\n");
    fprintf(stdout, "  node [width=\"0.6\" height=\"0.6\" fixedsize=\"true\", fontname=\"Helvetica\" fontsize=\"10\"];\n");
    fprintf(stdout, "  edge [fontname=\"Helvetica\" fontsize=\"10\"];\n\n");
    
    std::map<Node, std::string> leaving;
    
    // traverse the SA's nodes and collect predecessors
    std::map<Node, Nodes> pred;
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = 0; j < edges[i].size(); ++j) {
            pred[edges[i][j].target].push_back(edges[i][j].source);
            
            if (edges[i][j].label.substr(0,1) == "!") {
                fprintf(stdout, "  n_%d_%d [%s label=\"\\n\\n\\n\\n\\n%s\"];\n",
                    edges[i][j].source, edges[i][j].target, send_generic, edges[i][j].label.c_str());
            } else {
                fprintf(stdout, "  n_%d_%d [%s label=\"\\n\\n\\n\\n\\n%s\"];\n",
                    edges[i][j].source, edges[i][j].target, receive_generic, edges[i][j].label.c_str());
            }
        }
    }
    
    fprintf(stdout, "\n");
    
    // traverse the SA's nodes again to create splits and joins
    for (size_t i = 0; i < nodes.size(); ++i) {
        // this should be an error of Fiona's IG-OG-output
        if (pred[i].empty() && edges[i].empty())
            continue;


        // START NODES
        if (pred[i].empty()) {
            fprintf(stderr, "%d is the initial node\n", i);
            fprintf(stdout, "  n_S_%d [%s];\n", i, start_generic);
        } else if (pred[i].size() > 1) {
            // JOIN GATEWAY
            fprintf(stdout, "  n_j_%d [%s];\n", i, xor_generic);
            fprintf(stderr, "%d needs a preceeding XOR join: 'n_j_%d'\n", i, i);

            // collect incoming arcs
            for (size_t j = 0; j < pred[i].size(); j++) {
                fprintf(stdout, "  n_%d_%d -> n_j_%d;\n", pred[i][j], i, i);
            }
        }
        

        // SPLIT GATEWAYS
        if (edges[i].size() > 1) {        
            if (!receiveLabels(i).empty() && !sendLabels(i).empty()) {
                fprintf(stderr, "%d is a mixed split -- CANNOT HANDLE THIS YET!\n", i);
                continue;
            }
            else if (receiveLabels(i).empty()) {
                fprintf(stdout, "  n_%d_s [%s];\n", i, xor_generic);
                fprintf(stderr, "%d is an XOR split\n", i);                
            }
            else if (sendLabels(i).empty()) {
                fprintf(stderr, "%d is a pick split\n", i);                
                fprintf(stdout, "  n_%d_s [%s];\n", i, mul_generic);
            }
            
            // incoming arcs
            for (size_t j = 0; j < pred[i].size(); j++) {
                fprintf(stdout, "  n_%d_%d -> n_%d_s\n", pred[i][j], i, i);
            }
            
            // outgoing arcs
            for (size_t j = 0; j < edges[i].size(); j++) {    
                fprintf(stdout, "  n_%d_s -> n_%d_%d;\n", i, i, edges[i][j].target);
            }            
        }
        
        // STOP NODES
        if (edges[i].size() == 0) {
            fprintf(stdout, "  n_%d_E [%s];\n", i, stop_generic);
            fprintf(stderr, "%d is a sink\n", i);
            
            // collect incoming arcs
            for (size_t j = 0; j < pred[i].size(); j++) {
                if (pred[i].size() == 1)
                    fprintf(stdout, "  n_%d_%d -> n_%d_E\n", pred[i][j], i, i);
                else {
                    fprintf(stdout, "  n_j_%d -> n_%d_E\n", i, i);
                    break;
                }
            }
        }             
    }
    
    fprintf(stdout, "\n  label=\"\\n\"\n");    
    fprintf(stdout, "}\n");
}
