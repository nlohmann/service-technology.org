#include <cassert>
#include "helpers.h"
#include "verbose.h"

#include <sstream>

/*!
 * The function increases the indices in the vector and propagates resulting
 * carries. For example, if the index vector [3,3] is increased and the maximal
 * bounds are [5,4] the resulting vector is [4,0].
 *
 * \param[in,out] current_index  vector holding the current indices
 * \param[in]     max_index      vector holding the upper bounds of the indices
 *
 * \post Index vector is increased according to the described rules.
 *
 * \invariant Each index lies between 0 and its maximal value minus 1.
 */
void next_index(std::vector<unsigned int> &current_index,
                const std::vector<unsigned int> &max_index)
{
    assert(current_index.size() == max_index.size());
    
    for (unsigned int i = 0; i < current_index.size(); ++i) {
        if (current_index[i] < max_index[i]-1) {
            current_index[i]++;
            break;
        } else {
            current_index[i] = 0;
        }
    }
}

std::string printvector(std::vector<std::string> v) {
	std::string result = "<< ";
	for (std::vector<std::string>::const_iterator it = v.begin(); it != v.end();) {
		result += *it;
		if (++it != v.end())
			result += ", ";
	}
	result += " >>";
	return result;
}

void printCycles(Cycles *cycles) {
	std::string result;

	for (Cycles::const_iterator it_outer = cycles->begin(); it_outer != cycles->end(); ++it_outer) {
		result += "<< ";
		for (std::vector<NodeAndOutEdge>::const_iterator it = it_outer->cycleEdges.begin(); it != it_outer->cycleEdges.end();) {
			std::stringstream ss (std::stringstream::in | std::stringstream::out);
			ss << "(" << (*it).node->id << ", " << (*it).outEdge.first << ", " << (*it).outEdge.second->id << ")";
			result += ss.str();
			if (++it != it_outer->cycleEdges.end())
				result += ", ";
		}
		result += " >>";
	}

	status("cycles: %s", result.c_str());
}

//void insertMapInMap(std::map<Node*, Values> insertMap, std::string label, std::map<Node*, Values> intoMap) {
//	for (std::map<Node*, Values>::const_iterator it = insertMap.begin(); it != insertMap.end(); ++it) {
//		intoMap[it->first] = it->second;
//		intoMap[it->first].fromLabel = label;
//	}
//}

/// constructor
GraphFormula::GraphFormula() :
  lastNodeIndex(0), lastLabelIndex(0), lastCycleIndex(0)
{
}

int GraphFormula::getNodeVarId(unsigned int nodeId) {
	if (NodeVarId.count(nodeId) > 0) {
		return NodeVarId[nodeId];
	} else {
		++lastNodeIndex;
		NodeVarId[nodeId] = lastNodeIndex;
		return lastNodeIndex;
	}
}

int GraphFormula::getLabelVarId(std::string label, Node *node) {
	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	ss << label << node->id;
	if (LabelVarId.count(ss.str()) > 0) {
		return LabelVarId[ss.str()];
	} else {
		++lastLabelIndex;
		LabelVarId[ss.str()] = lastLabelIndex;
		return lastLabelIndex;
	}
}

int GraphFormula::getCycleVarId(int cycleID) {
	if (CycleVarId.count(cycleID) > 0) {
		return CycleVarId[cycleID];
	} else {
		++lastCycleIndex;
		CycleVarId[cycleID] = lastCycleIndex;
		return lastCycleIndex;
	}
}

std::string GraphFormula::getStringfromId(int id) {
	bool ispositive = true;

	if (id < 0) {
		ispositive = false;
		id *= -1;
	}

	if (id <= lastNodeIndex) {
		for (std::map<unsigned int, int>::const_iterator it = NodeVarId.begin(); it != NodeVarId.end(); ++it) {
			if (it->second == id) {
				std::stringstream ss (std::stringstream::in | std::stringstream::out);
				if (ispositive)
					ss << it->first << "_r";
				else
					ss << "-" << it->first << "_r";

				return ss.str();
			}
		}
	} else if (id <= lastCycleIndex) {
		for (std::map<int, int>::const_iterator it = CycleVarId.begin(); it != CycleVarId.end(); ++it) {
			if (it->second == id) {
				std::stringstream ss (std::stringstream::in | std::stringstream::out);
				if (ispositive)
					ss << it->first << "_cr";
				else
					ss << "-" << it->first << "_cr";

				return ss.str();
			}
		}
	} else if (id <= lastLabelIndex) {
		for (std::map<std::string, int>::const_iterator it = LabelVarId.begin(); it != LabelVarId.end(); ++it) {
			if (it->second == id) {
				if (ispositive)
					return it->first;
				else
					return "-" + it->first;
			}
		}
	}

	return "unknown";
}

void GraphFormula::printClauseList(ClauseList *clauses) {

	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	for (ClauseList::const_iterator it = clauses->begin(); it != clauses->end(); ++it) {
		ss << "(" << *(it->begin());
		for (Clause::const_iterator it_inner = it->begin()+1; it_inner != it->end(); ++it_inner) {
			ss << " + " << *it_inner;
		}
		if (it + 1 != clauses->end())
			ss << ") * ";
		else
			ss << ")";
	}
	status("Graph formula CNF = %s", ss.str().c_str());
}

void GraphFormula::printClauseListExpr(ClauseList *clauses) {

	std::stringstream ss (std::stringstream::in | std::stringstream::out);
	for (ClauseList::const_iterator it = clauses->begin(); it != clauses->end(); ++it) {
		ss << "(" << getStringfromId(*(it->begin()));
		for (Clause::const_iterator it_inner = it->begin()+1; it_inner != it->end(); ++it_inner) {
			ss << " + " << getStringfromId(*it_inner);
		}
		if (it + 1 != clauses->end())
			ss << ") * ";
		else
			ss << ")";
	}
	status("Graph formula CNF = %s", ss.str().c_str());
}

/// OG output format
void GraphFormula::cnfOut(std::ostream &o, std::string filename) const {

	o << "c  graph formula corresponding to the graph of " << filename << "\n";
	o << "c\n";
	o << "p cnf " << lastLabelIndex << " " << cnfClauses->size() << "\n";

	for (ClauseList::const_iterator it = cnfClauses->begin(); it != cnfClauses->end(); ++it) {
		for (Clause::const_iterator it_inner = it->begin(); it_inner != it->end(); ++it_inner) {
			o << *it_inner << " ";
		}
		o << "0\n";
	}

    o << std::flush;
}
