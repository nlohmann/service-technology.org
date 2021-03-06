#include <cstdio>
#include <cstring>
#include <string>
#include <cassert>
#include <map>
#include <vector>
#include <sstream>
#include "Graph.h"
#include "verbose.h"
#include "helpers.h"


using std::map;
using std::string;
using std::vector;

/// constructor
Graph::Graph() :
  root(NULL), type(NOTDEFINED), lastSearchId(0)
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
    ++graphformula.lastCycleIndex;
    nodes[n->id] = n;
}

ClauseList* Graph::calculateGraphFormula(int noCycle_flag, optionType option) {
	ClauseList *result = new ClauseList();

	graphformula.lastLabelIndex = graphformula.lastCycleIndex;

	if (noCycle_flag) {
		status("Not searching for cyles other than self-loops!");
	} else {
		status("Searching for cyles...");
		std::vector<NodeAndOutEdge> dummy;
		findCycles(root, dummy);
		if (option == VAR1 || option == VAR1NOFORMULA) {
			graphformula.lastLabelIndex += cycles.size();
		}

		computeIncomingEdgesForCycles();

//		printCycles(&cycles);
	}

	graphformula.checktime();

	status("computing formula...");

	// root node ID (the root is always reachable!)
	int rootvarId = graphformula.getNodeVarId(root->id);
	Clause root_reachable;
	root_reachable.push_back(rootvarId);
	result->push_back(root_reachable);

	for (std::map<unsigned int, Node*>::const_iterator node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
		ClauseList *f1 = addFormulaNotReachable(node_it->second);
		result->insert(result->end(), f1->begin(), f1->end());
		delete f1;

		graphformula.checktime();
	}

	if (option == VAR1 || option == VAR1NOFORMULA) {
		for (std::map<unsigned int, Node*>::const_iterator node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
			ClauseList *f2 = addFormulaReachable(node_it->second);
			result->insert(result->end(), f2->begin(), f2->end());
			delete f2;

			graphformula.checktime();
		}

		std::vector<int> cycleSeen;

		for (Cycles::iterator it_cycles = cycles.begin(); it_cycles != cycles.end(); ++it_cycles) {
			int cycleVarId = graphformula.getCycleVarId(it_cycles->id);
			//		status("cycle id: %i", it_cycles->id);
			ClauseList *f4 = addFormulaCycleReachable(&(*it_cycles), cycleSeen);
			ClauseList *f5 = new ClauseList(*f4);

			for (ClauseList::iterator it = f4->begin(); it != f4->end(); ++it) {
				it->push_back(cycleVarId);
			}

			result->insert(result->end(), f4->begin(), f4->end());
			delete f4;

			f5 = addFormulaCycleNotReachable(f5);
			for (ClauseList::iterator it = f5->begin(); it != f5->end(); ++it) {
				it->push_back(-cycleVarId);
			}

			result->insert(result->end(), f5->begin(), f5->end());
			delete f5;

			graphformula.checktime();
		}
	} else if (option == VAR2 || option == VAR2NOFORMULA) {
		for (std::map<unsigned int, Node*>::const_iterator node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
			ClauseList *f2 = addFormulaReachable2(node_it->second);
			result->insert(result->end(), f2->begin(), f2->end());
			delete f2;

			graphformula.checktime();
		}
	}

	if (option != VAR1NOFORMULA && option != VAR2NOFORMULA) {
		for (std::map<unsigned int, Node*>::const_iterator node_it = nodes.begin(); node_it != nodes.end(); ++node_it) {
			// the node's formula
			ClauseList *f3 = node_it->second->formula->toClauseList();
			ClauseList *temp;
			//graphformula.printClauseList(f3);

			// if node K is reachable then the node's formula has to be fulfilled
			// <=>	K_reach => formula(K)
			// <=>	not(K_reach) v formula(K)
			temp = new ClauseList();
			int varId = graphformula.getNodeVarId(node_it->second->id);
			Clause tempclause;
			tempclause.push_back(-varId);
			temp->push_back(tempclause);
			f3 = combineClauses(temp, f3);

			result->insert(result->end(), f3->begin(), f3->end());
			delete f3;

			graphformula.checktime();
		}
	}

	return result;
}

ClauseList* Graph::addFormulaNotReachable(Node* node) {
	// node ID (K)
	int varId = graphformula.getNodeVarId(node->id);
	ClauseList *clauses = new ClauseList();

	if (node != root) {
		// 		if K != root is not reachable then for all edges K->V the edges' variables are set to false
		// <=>	not(K_reach) => &&(K->V)( not(label_(K->V)) )
		// <=>	&&(K->V)( K_reach v not(label_(K->V)) )
		for (Labels::const_iterator it = node->outLabels.begin(); it != node->outLabels.end(); ++it) {
			int labelVarId = graphformula.getLabelVarId(*it, node);

			// K_reach v not(label_(K->V))
			Clause temp;
			temp.push_back(varId);
			temp.push_back(-labelVarId);
			clauses->push_back(temp);
		}

		// furthermore if K != root is not reachable then K's final variable is set to false!
		if (node->formula->hasFinal()) {
			int finalVarId = graphformula.getLabelVarId("final", node);
			Clause temp;
			temp.push_back(varId);
			temp.push_back(-finalVarId);
			clauses->push_back(temp);
		}
	}
	return clauses;
}

ClauseList* Graph::addFormulaReachable(Node* node) {
	// if the current node is the root then just return an empty clause list
	if (node == root) {
		return new ClauseList();
	}

	// node ID (K)
	int varId = graphformula.getNodeVarId(node->id);
	int labelVarId;

	ClauseList *clauses = new ClauseList();
	// new with cycles:
	// <=>	if there exists an edge x from V to K and all cycles containing x are reachable then K is reachable
	// <=>	||(V -x-> K)( x & Cx_reach ) => K_reach
	// <=>	&&(V -x-> K)( K_reach v not(Cx_reach) v not(x) )

	ClauseList *clauses2 = new ClauseList();
	// new with cycles:
	//		If for a node K != root all incoming edges V-x->K the edges' variables are set to false
	//		or the cycles K is in are not reachable then node K is not reachable
	// <=>	&&(V-x->K)( not(x) v not(Cx_reach) ) => not(K_reach)
	// <=>	||(V-x->K)( x & Cx_reach ) v not(K_reach)

	// add not(K_reach) for part 2
	Clause tempclause2;
	tempclause2.push_back(-varId);
	clauses2->push_back(tempclause2);

	// iterate over incoming edges (labels)
	for (std::map<std::string, std::vector<Node*> >::const_iterator it_inedges = node->inEdges.begin(); it_inedges != node->inEdges.end(); ++it_inedges) {
		// iterate over predecessor nodes (V)
		for (std::vector<Node*>::const_iterator it_nodes = it_inedges->second.begin(); it_nodes != it_inedges->second.end(); ++it_nodes) {
			// ignore
			if (node->id != (*it_nodes)->id) {
				//int prenodeVarId = graphformula.getNodeVarId((*it_nodes)->id);
				int labelVarId = graphformula.getLabelVarId(it_inedges->first, *it_nodes);

				// K_reach v not(x)
				// for part 1
				Clause tempclause;
				tempclause.push_back(varId);
				tempclause.push_back(-labelVarId);
				//tempclause.push_back(-prenodeVarId); //not needed!!!?!

				// for part 2
				ClauseList *tempclauses2 = new ClauseList();
				Clause incclause2;
				incclause2.push_back(labelVarId);
				tempclauses2->push_back(incclause2);

				for (std::vector<int>::const_iterator it_cycles = (*it_nodes)->cycleList[it_inedges->first].begin(); it_cycles != (*it_nodes)->cycleList[it_inedges->first].end(); ++it_cycles) {
					int cycleVarId = graphformula.getCycleVarId(*it_cycles);
					// for part 1
					tempclause.push_back(-cycleVarId);

					// for part 2
					Clause tempclause2;
					tempclause2.push_back(cycleVarId);
					tempclauses2->push_back(tempclause2);
				}

				// part 1
				clauses->push_back(tempclause);

				// part 2
				clauses2 = combineClauses(tempclauses2, clauses2);
			}
		}
	}

	clauses2->insert(clauses2->end(), clauses->begin(), clauses->end());
	delete clauses;

	return clauses2;
}

ClauseList* Graph::addFormulaReachable2(Node* node) {
	// if the current node is the root then just return an empty clause list
	if (node == root) {
		return new ClauseList();
	}

	// node ID (K)
	int varId = graphformula.getNodeVarId(node->id);
	int labelVarId;

	ClauseList *clauses = new ClauseList();
	// <=>	if there exists an edge x from V to K then K is reachable
	// <=>	||(V -x->* K)( x & ||(x' \in conn(x)\x)( x' ) ) => K_reach
	// <=>	&&(V -x->* K)( K_reach v not(x) v &&(x' \in conn(x)\x)( not(x') ) )

	// <=>  &&(V -x->* K)( &&(x' \in conn(x)\x)( K_reach v not(x) v not(x') ) )

	ClauseList *clauses2 = new ClauseList();
	//		if K is reachable then there has to exist a transition x to K and a connection to the 'outside'...
	// <=>	K_reach => ||(V -x->* K)( x & ||(x' \in conn(x)\x)(x') )
	// <=>	not(K_reach) v ||(V -x->* K)( x & ||(x' \in conn(x)\x)(x') )

	// add not(K_reach) for part 2
	Clause tempclause2;
	tempclause2.push_back(-varId);
	clauses2->push_back(tempclause2);

	// iterate over incoming edges (labels)
	for (std::map<std::string, std::vector<Node*> >::const_iterator it_inedges = node->inEdges.begin(); it_inedges != node->inEdges.end(); ++it_inedges) {
		// iterate over predecessor nodes (V)
		for (std::vector<Node*>::const_iterator it_prenodes = it_inedges->second.begin(); it_prenodes != it_inedges->second.end(); ++it_prenodes) {
			// ignore self-loops
			if (node->id != (*it_prenodes)->id) {
				// compute conn(x)!
				// only look at edges V -x-> K where C(V -x-> K) != C(K)
				//if (!cyclesEqual(&(node->cycleList), it_inedges->first, &((*it_prenodes)->cycleList))) {
				if ((*it_prenodes)->cycleList.count(it_inedges->first) > 0
						&& !(node->inEdges.size() == 1 && it_inedges->second.size() == 1)) {
					++lastSearchId;
					// remember to delete?
					//status("start");
					Clause* conn = computeConnectionSet(node, it_inedges->first, *it_prenodes);

					if (conn->empty()) {
						delete conn;
						break;
					}

					// x
					int labelVarId = graphformula.getLabelVarId(it_inedges->first, *it_prenodes);

					// K_reach v not(x)
					// for part 1
					Clause tempclause;
					tempclause.push_back(varId);
					tempclause.push_back(-labelVarId);

					for (Clause::const_iterator it_conn = conn->begin(); it_conn != conn->end(); ++it_conn) {
						// not(x')
						// for part 1
						Clause tt = tempclause;
						tt.push_back(-(*it_conn));
						clauses->push_back(tt);
					}

					// x & ||(x')
					// for part 2
					ClauseList *tempclauses2 = new ClauseList();
					Clause incclause2;
					incclause2.push_back(labelVarId);
					tempclauses2->push_back(incclause2);
					tempclauses2->push_back(*conn);
					clauses2 = combineClauses(tempclauses2, clauses2);

					delete conn;
				} else {
					// x
					int labelVarId = graphformula.getLabelVarId(it_inedges->first, *it_prenodes);

					// K_reach v not(x)
					// for part 1
					Clause tempclause;
					tempclause.push_back(varId);
					tempclause.push_back(-labelVarId);
					clauses->push_back(tempclause);

					// x
					// for part 2
					ClauseList *tempclauses2 = new ClauseList();
					Clause incclause2;
					incclause2.push_back(labelVarId);
					tempclauses2->push_back(incclause2);
					clauses2 = combineClauses(tempclauses2, clauses2);
				}
			}
		}
	}

	clauses2->insert(clauses2->end(), clauses->begin(), clauses->end());
	delete clauses;

	return clauses2;
}

Clause* Graph::computeConnectionSet(Node* node, std::string label, Node* source) {
	Clause* result = new Clause();

	if (source->lastSearchId < lastSearchId) {
		//status("(%u, %s, %u), %i", node->id, label.c_str(), source->id, source->cycleList.count(label));
		if (source != node) {
			if (source->cycleList.count(label) == 0 || source == root) {
				int labelVarId = graphformula.getLabelVarId(label, source);
				result->push_back(labelVarId);
				//status("found");
			} else {
				node->lastSearchId = lastSearchId;

				for (std::map<std::string, std::vector<Node*> >::const_iterator it = source->inEdges.begin(); it != source->inEdges.end(); ++it) {
					for (std::vector<Node*>::const_iterator it_inner = it->second.begin(); it_inner != it->second.end(); ++it_inner) {
						Clause* temp = computeConnectionSet(source, it->first, *it_inner);
						result->insert(result->end(), temp->begin(), temp->end());
						delete temp;
					}
				}
			}
		}
	}

	return result;
}

bool Graph::cyclesEqual(std::map<std::string, std::vector<int> >* list1, std::string label, std::map<std::string, std::vector<int> >* list2) {
	if (list2->count(label) == 0) {
		if (list1->empty())
			return true;
		else
			return false;
	}

	bool found = false;

	// iterate over all the node's cycles
	for (std::map<std::string, std::vector<int> >::const_iterator it1 = list1->begin(); it1 != list1->end(); ++it1) {
		for (std::vector<int>::const_iterator it_trans2 = it1->second.begin(); it_trans2 != it1->second.end(); ++it_trans2) {
			found = false;
			// iterate over incoming transition's cycles
			for (std::vector<int>::const_iterator it_trans = (*list2)[label].begin(); it_trans != (*list2)[label].end(); ++it_trans) {
				if (*it_trans == *it_trans2) {
					found = true;
					break;
				}
			}
			if (found) {
				break;
			}
		}
		// if node's cycle is not in transition's cycles then return false
		if (!found) {
			return false;
		}
	}
	return true;
}

ClauseList* Graph::addFormulaCycleReachable(Cycle *cycle, std::vector<int> cycleSeen) {
	ClauseList *result = new ClauseList();

	int cycleVarId = graphformula.getCycleVarId(cycle->id);

	// 		(...) v (...) v ... => Cycle_reach
	// <=>	( not(...) & not(...) & ... ) v Cycle_reach

	cycleSeen.push_back(cycle->id);

	// if the root is in the current cycle then the cycle is reachable
	for (std::vector<NodeAndOutEdge>::const_iterator it = cycle->cycleEdges.begin(); it != cycle->cycleEdges.end(); ++it) {
		if (it->node == root) {
			Clause empty;
			result->push_back(empty);
			return result;
		}
	}

	// iterate over the incoming edges that don't belong to the cycle
	for (std::vector<NodeAndInEdge>::const_iterator it_edges = cycle->incomingEdges.begin(); it_edges != cycle->incomingEdges.end(); ++it_edges) {
		std::vector<int>* c = edgeBelongsToCycles(*it_edges);
		int labelVarId = graphformula.getLabelVarId(it_edges->inEdge.first, it_edges->inEdge.second);
		// if the edge doesn't belong to another cycle then return the label's variable
		if (c == NULL || c->empty()) {
//			status("edge doesn't belong to a cycle");
			Clause temp;
			temp.push_back(-labelVarId);
			result->push_back(temp);
		} else {
//			status("iterate over cycles");
			for (std::vector<int>::const_iterator it_cycles = c->begin(); it_cycles != c->end(); ++it_cycles) {
//				status("now in cycle %i", *it_cycles);
				// if the cycle was already seen then discard the current part of the formula
				bool found = false;
				for (std::vector<int>::const_iterator it_f = cycleSeen.begin(); it_f != cycleSeen.end(); ++it_f) {
					if (*it_cycles == *it_f) {
						found = true;
						break;
					}
				}

				if (!found) {
//					status("cycle not seen");
					ClauseList * tempclauses = addFormulaCycleReachable(&cycles[*it_cycles], cycleSeen);

					for (ClauseList::iterator it = tempclauses->begin(); it != tempclauses->end(); ++it) {
						it->push_back(-labelVarId);
					}

					result->insert(result->end(), tempclauses->begin(), tempclauses->end());
					delete tempclauses;
				}
			}

		}
	}

	return result;
}

ClauseList* Graph::addFormulaCycleNotReachable(ClauseList* input) {
	ClauseList *result = new ClauseList();
	Clause empty;
	result->push_back(empty);

	// input == (-x v -y) & (...) & (...) ...

	//					   ====== -input ======
	// 		Cycle_reach => (...) v (...) v ...
	// <=>	not(Cycle_reach) v (...) v (...) v ...

	for (ClauseList::const_iterator it_clauses = input->begin(); it_clauses != input->end(); ++it_clauses) {
		result = combineClauses(ClauseIntoNegClauseList(*it_clauses), result);
	}

	return result;
}

ClauseList* Graph::ClauseIntoNegClauseList(Clause clause) {
	ClauseList *result = new ClauseList();

	for (Clause::const_iterator it = clause.begin(); it != clause.end(); ++it) {
		Clause temp;
		temp.push_back(-*it);
		result->push_back(temp);
	}

	return result;
}

std::vector<int>* Graph::edgeBelongsToCycles(NodeAndInEdge edge) {
//	status("edge (%i, %s, %i) belongs to which cycles?", edge.inEdge.second->id, edge.inEdge.first.c_str(), edge.node->id);
	if (edge.inEdge.second->cycleList.count(edge.inEdge.first) > 0)
		return &(edge.inEdge.second->cycleList[edge.inEdge.first]);
	else
		return NULL;
}

/**
 * combines two lists of clauses (CNF) via "or". list2 should be greater than list1.
 *
 * returns result = list1 or list2 in CNF
 */
ClauseList* Graph::combineClauses(ClauseList *list1, ClauseList *list2) {
	ClauseList *result = new ClauseList();

	for (ClauseList::const_iterator it_left = list1->begin(); it_left != list1->end(); ++it_left) {
		for (ClauseList::const_iterator it_right = list2->begin(); it_right != list2->end(); ++it_right) {
			Clause temp;
			temp.insert(temp.end(), it_left->begin(), it_left->end());
			temp.insert(temp.end(), it_right->begin(), it_right->end());
			result->push_back(temp);
		}
	}

	delete list1;
	delete list2;

	return result;
}

bool Graph::findCycles(Node *node, std::vector<NodeAndOutEdge> pathToNode) {
	graphformula.checktime();

	if (node->isActive) {
//		status("found cycle");
		//printCycles(&cycles);
		bool temp = insertCycleWithoutDuplicates(restVector(node, pathToNode));
		return temp;
	} else if (!node->hasCycles) {
		return false;
	} else {
		node->isActive = true;
		NodeAndOutEdge temp;
		temp.node = node;
		pathToNode.push_back(temp);

		node->hasCycles = false;
//		status("next node: %u", node->id);
		for (std::map<std::string, std::vector<Node*> >::const_iterator it = node->outEdges.begin(); it != node->outEdges.end(); ++it) {
			//status("node: %i, label: %s, dest node: %i", node->id, it->first.c_str(), (*it->second.begin())->id);
			// the graph is considered to be deterministic! \todo
			std::pair<std::string, Node*> edge;
			edge.first = it->first;
			edge.second = *it->second.begin();
			pathToNode.back().outEdge = edge;
			bool tempbool = findCycles(*it->second.begin(), pathToNode);
			if(tempbool) {
				node->hasCycles = true;
			}
		}
		node->isActive = false;

		return node->hasCycles;
	}
}

std::vector<NodeAndOutEdge> Graph::restVector(Node *node, std::vector<NodeAndOutEdge> nodeVector) {
	for (std::vector<NodeAndOutEdge>::iterator it = nodeVector.begin(); it != nodeVector.end();) {
		if (it->node == node) {
			return nodeVector;
		}
		nodeVector.erase(it);
	}
	return nodeVector;
}

bool Graph::insertCycleWithoutDuplicates(std::vector<NodeAndOutEdge> nodeVector) {
	int length = nodeVector.size();
	// ignore loops
	if (length == 1)
		return false;
	// iterate over all cycles
	for (Cycles::const_iterator it_cycles = cycles.begin(); it_cycles != cycles.end(); ++it_cycles) {
		// only makes sense if vectors are of even length!
		if (it_cycles->cycleEdges.size() == length) {
			std::vector<NodeAndOutEdge> temp = nodeVector;
			// iterate over the cycle's edges
			for (int i = 0; i < length; ++i) {
//				status("%i",i);
//				status("%u, %u",(*it_cycles)[i]->id, cycle[i]->id);
				// iterate over the cycle to be inserted and check if the cycles are equal
				for (std::vector<NodeAndOutEdge>::iterator it = temp.begin(); it != temp.end(); ++it) {
					if (it_cycles->cycleEdges[i].node->id == it->node->id
						&& it_cycles->cycleEdges[i].outEdge.first == it->outEdge.first) {
						temp.erase(it);
						break;
					}
				}
			}
			// if all elements have been found then temp is empty and the cycle is already in the list
			if (temp.size() == 0) {
				return true;
			}
		}
	}
	Cycle cycle;
	cycle.cycleEdges = nodeVector;

	cycles.push_back(cycle);
	return true;
}

void Graph::computeIncomingEdgesForCycles() {
	int cycleID = 0;
	// iterate over all cycles
	for (Cycles::iterator it_cycles = cycles.begin(); it_cycles != cycles.end(); ++it_cycles) {
		it_cycles->id = cycleID;
		// iterate over all edges of the current cycle
		for (std::vector<NodeAndOutEdge>::iterator it_inner = it_cycles->cycleEdges.begin(); it_inner != it_cycles->cycleEdges.end(); ++it_inner) {
			// add id to list of cycle-ids that the node is in
			//status("node: %i, label: %s, push back cycle id: %i", it_inner->node->id, it_inner->outEdge.first.c_str(), cycleID);
			it_inner->node->cycleList[it_inner->outEdge.first].push_back(cycleID);
			// iterate over all incoming edges of destination node of the current edge
			for (std::map<std::string, std::vector<Node*> >::iterator it_edges = it_inner->outEdge.second->inEdges.begin(); it_edges != it_inner->outEdge.second->inEdges.end(); ++it_edges) {
				// if the label of the edge that is in the cycle isn't equal to to the incoming edge
				// then that edge is an incoming edge that doesn't belong to the cycle
				if (it_inner->outEdge.first != it_edges->first) {
					for (std::vector<Node*>::iterator it_nodes = it_edges->second.begin(); it_nodes != it_edges->second.end(); ++it_nodes) {
						// ignore self-loops
						if (it_inner->outEdge.second->id != (*it_nodes)->id) {
							NodeAndInEdge temp;
							temp.node = it_inner->outEdge.second;
							std::pair<std::string, Node*> edge;
							edge.first = it_edges->first;
							edge.second = *it_nodes;
							temp.inEdge = edge;
							it_cycles->incomingEdges.push_back(temp);
						}
					}
				}
			}
		}
		++cycleID;
	}
}

void Graph::printFormulas() {
	for (std::map<unsigned int, Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		status("node %u, f = %s",it->first, it->second->formula->toString().c_str());
	}
}

void Graph::printFormulasCNF() {
	for (std::map<unsigned int, Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		std::stringstream ss (std::stringstream::in | std::stringstream::out);
		std::vector<std::vector<std::string> > *temp = it->second->formula->cnf();
		for (std::vector<std::vector<std::string> >::const_iterator it = temp->begin(); it != temp->end(); ++it) {
			ss << "(" << it->begin()->c_str();
			for (std::vector<std::string>::const_iterator it_inner = it->begin()+1; it_inner != it->end(); ++it_inner) {
				ss << " + " << it_inner->c_str();
			}
			if (it + 1 != temp->end())
				ss << ") * ";
			else
				ss << ")";
		}
		status("node %u, f.cnf = %s", it->first, ss.str().c_str());
	}
}

void Graph::printFormulasDNF() {
	for (std::map<unsigned int, Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
		status("node %u, f.dnf = %s",it->first, it->second->formula->dnf()->toString().c_str());
	}
}

unsigned int Graph::countValidSubgraphs() {
	/************
	std::vector<Node*> path;
	AssignCount result = countFromNode(root, path);
	return result.multiply;
	***********/
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

AssignCount Graph::countFromNode(Node *node, std::vector<Node*> path) {
/***********
	AssignCount result;
	result.multiply = 0;
	result.surplus = NULL;

	if (node->isActive) {
		// the last edge would lead to a cycle, return 1
		result.multiply = 1;
//	} else if (node->visitedBy.find(from) != node->visitedBy.end()) {
//		// we have already calculated the result from here!
//		++(node->visitedBy[from].count);
//		result.multiply = node->visitedBy[from].subgraphcount;
	} else if (!node->visitedBy.empty()) {
		// \todo: further requirements...
		result.multiply = node->visitedBy.begin()->second.subgraphcount; // \todo
		Values temp;
		temp.original = result.multiply;
		temp.computed = result.multiply - 1;
		temp.paths = node->pathsToNode;

		result.surplus = temp;

		path.push_back(node);
		node->pathsToNode.push_back(path);

//	} else if (node->visitedBy.node != NULL) {
//		if (node->formula->hasFinal()) {
//			result.add = (node->subgraphcount-1) * wasVisitedByNode(node, from);
//		} else {
//			result.add = (node->subgraphcount) * wasVisitedByNode(node, from);
//		}

	} else {
		// this is a new node \todo: for the current search path...?
		node->isActive = true;
		Node *from = path.back();
//		node->visitedBy[from].count = 1;

		node->pathsToNode.push_back(path);

		Assignments *assigns = node->Sat();

		unsigned int counter = 1;

		// iterate through assignments satisfying the node's formula
		for (std::vector<Labels>::const_iterator it = assigns->begin(); it != assigns->end(); ++it) {

			AssignCount temp;
			temp.multiply = 1;
			labelMap labelSurplus;

			status("node id: %u, assignment: %u: %s", node->id, counter, printvector(*it).c_str());
			if (it->empty()) {
				// formula contains final or true
				status("node id: %u, assignment: %u, final or true...", node->id, counter);
			} else {
				// iterate through the labels in the current assignment
				for (std::vector<std::string>::const_iterator itLabel = it->begin(); itLabel != it->end(); ++itLabel) {
					// \todo: only one successor per output label!!!!
					status("node id: %u, assignment: %u, label: %s, starting...", node->id, counter, itLabel->c_str());
					// if the function returns '0' (no possible subgraphs) then temp2 gets the value '0'
					AssignCount temp2;
					if (node->results.find(*itLabel) != node->results.end()) {
						temp2 = node->results[*itLabel];
					} else {
						temp2 = node->results[*itLabel] = countFromNode(*(node->outEdges[*itLabel].begin()), path);
					}

					temp.multiply *= temp2.multiply;

					status("node id: %u, assignment: %u, label: %s, done! return value: %u", node->id, counter, itLabel->c_str(), temp2.multiply);
					if (temp.multiply == 0) {
						status("node id: %u, assignment: %u, no valid subgraph possible. aborting loop...", node->id, counter);
						break;
					}

					if (!temp2.surplus != NULL)
						labelSurplus[*itLabel] = temp2.surplus;
				}

				// iterate through current assignment's labels that have surpluses
				for (labelMap::const_iterator itLabelSurplus = labelSurplus.begin(); itLabelSurplus != labelSurplus.end(); ++itLabelSurplus) {
					// iterate through surpluses from current label
					for (std::map<Node*, Values>::const_iterator itSurplus = itLabelSurplus->second.begin(); itSurplus != itLabelSurplus->second.end(); ++itSurplus) {
						if (cycleStartedHere(node, itSurplus->first)) {

						} else {
							uint temp = 1;
							// iterate through current assignment's labels
							for (std::vector<std::string>::const_iterator itLabel = it->begin(); itLabel != it->end(); ++itLabel) {
								if (itLabelSurplus->first == *itLabel) {
									temp *= itSurplus->second.computed;
								} else {
									temp *= node->results[*itLabel];
								}
							}
						}
					}
				}
			}

			status("\t  node id: %u, assignment: %u: %s", node->id, counter, printvector(*it).c_str());
			status("\t  assignment done! added %u to %u possibilities.", temp.multiply, result.multiply);
			result.multiply += temp.multiply;
			++counter;
		}

		if (node->formula->hasFinal()) {
			status("node id: %u contains final", node->id);
		} else {
			status("node id: %u doesn't contain final, multiplying possible subgraphs by 2...", node->id);
			result.multiply *= 2;
		}

		node->visitedBy[from].subgraphcount = result.multiply;
		node->isActive = false;
	}

	status("leaving node id: %u with %u possibilities...", node->id, result.multiply);
	return result;
********/
}

bool cycleStartedHere(Node *current, Node *origin) {
/*******
	for (std::vector<Node*>::const_reverse_iterator it = origin->pathToNode.rbegin();
			it != origin->pathToNode.rend(); ++it) {
		if (*it == current)
			return true;
	}
	return false;
**********/
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
