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

#include <config.h>
#include <libgen.h>
#include <set>
#include <fstream>
#include <algorithm>
#include "Graph.h"
#include "Formula.h"
#include "helpers.h"
#include "costfunction.h"
#include "Output.h"
#include "verbose.h"

using std::vector;
using std::map;
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
    return (source == e.source and
            target == e.target and
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

    if (label == e.label and source < e.source) {
        return true;
    }

    if (label == e.label and source == e.source and target < e.target) {
        return true;
    }

    return false;
}


/*!
 * \brief constructor
 *
 * Constructs an edge given source, target, and label.
 */
Edge::Edge(Node _source, Node _target, const Label& _label)
  : source(_source), target(_target), label(_label), type() {
}


/*!
 * \brief constructor
 *
 * Constructs an empty edge, labeled with "" (epsilon).
 */
Edge::Edge()
  : source(0), target(0), label(""), type() {
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
Edge Graph::addEdge(Node q1, Node q2, const Label& l, action_type type) {
    Edge result = Edge(q1, q2, l);
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
    assert(f);
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
 * \param _id  identifier of the graph
 *
 */
Graph::Graph(const std::string& _id)
  : edges(), root(), finalNode(), max_value(0), insertionValue(),
    deletionValue(), formulas(), id(_id),  nodes(), addedNodes() {
}


void Graph::createDotFile() {
    string dot_filename;

    // if no filename is given via command line, create it
    if (args_info.dot_arg == NULL) {
        switch (args_info.mode_arg) {
            case(mode_arg_matching):
            case(mode_arg_simulation): {
                dot_filename = std::string(basename(args_info.automaton_arg)) + "_" +
                    std::string(basename(args_info.og_arg)) + "_" +
                    cmdline_parser_mode_values[args_info.mode_arg] + ".dot";
                break;
            }

            case(mode_arg_og): {
                dot_filename = std::string(basename(args_info.og_arg)) + ".dot";
                break;
            }

            case(mode_arg_sa): {
                dot_filename = std::string(basename(args_info.automaton_arg)) + ".dot";
                break;
            }
        }
    } else {
        dot_filename = args_info.dot_arg;
    }

    {
        // try to open the dot file for writing
        Output dot_file(dot_filename, "dot representation");

        // write header
        dot_file.stream() << "digraph G {\n";
        dot_file.stream() << "edge [fontname=Helvetica fontsize=10]\n";
        dot_file.stream() << "node [fontname=Helvetica fontsize=10]\n";


        // write dot graph according to chosen mode
        switch (args_info.mode_arg) {
            case(mode_arg_og):
            case(mode_arg_matching):
            case(mode_arg_simulation): {
                toDot(dot_file);
                break;
            }

            case(mode_arg_sa): {
                toDot(dot_file, false);
                break;
            }
        }


        // write footer
        dot_file.stream() << "\n}\n";
    }

    // if dot found during configuration, executed it to create a PNG
    if (args_info.png_flag && !std::string(CONFIG_DOT).empty()) {
        std::string command = std::string(CONFIG_DOT) + " " + dot_filename + " -Tpng -O";
        status("executing '%s'", command.c_str());
        system(command.c_str());
    }
}


/// returns a Dot representation of the graph
void Graph::toDot(std::ostream& o, bool drawFormulae) {
    // an arrow indicating the initial state
    o << "  q0 [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"];\n";
    o << "  q0 -> q_" << nodes[root] << " [minlen=\"0.5\"];\n";

    for (size_t i = 0; i != nodes.size(); ++i) {
        o << "  q_" << nodes[i] << " [label=";

        if (drawFormulae && formulas[nodes[i]]) {
            o << "<<FONT>";
            o << formulas[nodes[i]]->toDot();
            o << "</FONT>>";
        } else {
            o << "\"\"";
        }

        if (addedNodes[nodes[i]])
            o << " style=dashed";

        if (finalNode[nodes[i]] && !drawFormulae) {
            o << " peripheries=\"2\"";
        }

        o << "];\n";

        for (size_t j = 0; j < edges[nodes[i]].size(); ++j) {
            o << "  q_" << edges[nodes[i]][j].source;
            o << " -> q_" << edges[nodes[i]][j].target;
            o << " [label=\"" << edges[nodes[i]][j].label << "\"";

            if (addedNodes[edges[nodes[i]][j].source] || addedNodes[edges[nodes[i]][j].target])
                o << " style=dashed";

            switch (edges[nodes[i]][j].type) {
                case(DELETE): o << " color=red"; break;
                case(INSERT): o << " color=green"; break;
                case(MODIFY): o << " color=orange"; break;
                case(NONE):
                case(KEEP):
                default: break;
            }

            o << "];";
        }
    }
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

        if (formulas[q]) {
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

    if (formulas[q]) {
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


/// adds this node to the final nodes (IG only)
void Graph::setFinal(Node q) {
    finalNode[q] = true;
}


/// add a node to the graph
Node Graph::addNode(Node q) {
    // if node is present, return it
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i] == q)
            return q;
    }

    this->max_value = std::max(q, max_value);
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
Node Graph::addNewNode(Node q, const Label& l) {
    // if node was already added here, return it
    for (size_t i = 0; i < edges[q].size(); ++i) {
        if (edges[q][i].label == l) {
            return edges[q][i].target;
        }
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

        bestValue = std::max(bestValue, newValue);
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
    assert(result);

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
