#ifndef _HELPERS_H
#define _HELPERS_H

#include <vector>
#include <string>
#include <map>
#include <ctime>
#include "types.h"
#include "Node.h"

/// updates an index
void next_index(std::vector<unsigned int> &current_index,
                const std::vector<unsigned int> &max_index);

std::string printvector(std::vector<std::string> v);


void printCycles(Cycles *cycles);

//void insertMapInMap(std::map<Node*, AssignCount> insertMap, std::string label, std::map<Node*, AssignCount> intoMap);

class GraphFormula {
public:
	GraphFormula();

	int lastNodeIndex;

	int lastLabelIndex;

	int lastCycleIndex;

	ClauseList *cnfClauses;

	/// the logical graph variables connected to an id
	std::map<unsigned int, int> NodeVarId;

	/// the logical graph variables connected to an id
	std::map<std::string, int> LabelVarId;

	/// the logical graph variables connected to an id
	std::map<int, int> CycleVarId;

	clock_t start_time;

	int timebound;

	void settime(int timebound);

	void checktime();

	int getNodeVarId(unsigned int nodeId);

	int getLabelVarId(std::string label, Node *node);

	int getCycleVarId(int cycleID);

	std::string getStringfromId(int id);

	void printClauseList(ClauseList *clauses);

	void printClauseListExpr(ClauseList *clauses);

	void booleanOut(std::ostream &o, std::string filename, ClauseList *clauses, Cycles *cycles);

	/// CNF output
	void cnfOut(std::ostream &, std::string filename, double time, int noCycle_flag, Cycles *cycles) const;
};

#endif
