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

/// constructor
Graph::Graph() :
  root(NULL), type(NOTDEFINED)
{
  
}

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

int Graph::countValidSubgraphs() {
	AssignCount result = countFromNode(root, NULL);
	return result.multiply;
}

//// test function... not finished!!
//int Graph::testAllAssignments(std::map<unsigned int, Labels> AssignmentMap, std::vector<Node*> nodes) {
//	Node *node = *(--nodes.end());
//	Assignments *assign = node->Sat();
//	nodes.pop_back();
//
//	if (!nodes.empty()) {
//		for (Assignments::const_iterator it = assign->begin(); it != assign->end(); ++it) {
//			AssignmentMap[node->id] = *it;
//			testAllAssignments(AssignmentMap, nodes);
//		}
//	} else {
//		return assign->size();
//	}
//	return -1;
//}

AssignCount Graph::countFromNode(Node *node, Node *from) {

	AssignCount result = {0,0};
	if (node->isActive) {
		// the last edge would lead to a cycle, return 1
		result.multiply = 1;
	} else if (node->visitedBy.find(from) != node->visitedBy.end()) {
		// we have already calculated the result from here!
		++(node->visitedBy[from].count);
		result.multiply = node->visitedBy[from].subgraphcount;

//	} else if (node->visitedBy.node != NULL) {
//		if (node->formula->hasFinal()) {
//			result.add = (node->subgraphcount-1) * wasVisitedByNode(node, from);
//		} else {
//			result.add = (node->subgraphcount) * wasVisitedByNode(node, from);
//		}

	} else {
		// this is a new node for the current search path
		node->isActive = true;
		node->visitedBy[from].count = 1;

		Assignments *assigns = node->Sat();

		int counter = 1;

		// iterate through assignments satisfying the node's formula
		for (std::vector<Labels>::const_iterator it = assigns->begin(); it != assigns->end(); ++it) {

			if (it->empty()) {
				// formula contains final or true
				status("node id: %i, assignment: %i, final or true, done!", node->id, counter);
				status("...added 1 to %i possibilities.", result.multiply);
				++result.multiply;
			} else {
				AssignCount temp = {1,0};
				// iterate through the labels in the current assignment
				for (std::vector<std::string>::const_iterator itLabel = it->begin(); itLabel != it->end(); ++itLabel) {
					AssignCount temp2 = {1,0};
					// \todo: only one successor per output label!!!!
					status("node id: %i, assignment: %i, label: %s, starting...", node->id, counter, itLabel->c_str());
					// if the function returns '0' (no possible subgraphs) then temp gets the value '0'
					temp2 = countFromNode(*(node->outEdges[*itLabel].begin()), node);

					temp.multiply *= temp2.multiply;
//					temp.add += temp2.add;

					status("node id: %i, assignment: %i, label: %s, done!", node->id, counter, itLabel->c_str());
					if (temp.multiply == 0) {
						status("node id: %i, assignment: %i, no valid subgraph possible. aborting loop...", node->id, counter);
						break;
					}
				}
				status("node id: %i, last label of assignment %i. added %i to %i possibilities.", node->id, counter, temp.multiply, result.multiply);
				result.multiply += temp.multiply;
			}
			++counter;
		}

		if (node->formula->hasFinal()) {
			status("node %i contains final", node->id);
		} else {
			status("node %i doesn't contain final, multiplying possible subgraphs by 2...", node->id);
			result.multiply *= 2;
		}

		node->visitedBy[from].subgraphcount = result.multiply;
		node->isActive = false;
	}

	status("leaving node id: %i with %i possibilities...", node->id, result.multiply);
	return result;
}

//int Graph::wasVisitedByNode(Node *node, Node *from) {
//	Node *nodePointer = node;
//	Node *fromPointer = from;
//	int counter = 0;
//
//	while (nodePointer !=  NULL) {
//		while (fromPointer !=  NULL) {
//			if (nodePointer->visitedBy.node == fromPointer) {
//				return counter;
//			} else {
//				fromPointer = fromPointer->visitedBy.node;
//			}
//		}
//		fromPointer = from;
//		counter += nodePointer->visitedBy.node->Sat()->size() - nodePointer->visitedBy.count;
//		nodePointer = nodePointer->visitedBy.node;
//	}
//
//	return 0;
//}

/// adds a label to the graphs
void Graph::addLabel(std::string l) {
    labels.push_back(l);
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
