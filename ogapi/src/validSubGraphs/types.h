#ifndef _TYPES_H
#define _TYPES_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>

typedef std::vector<int> Clause;
typedef std::vector<Clause> ClauseList;

typedef std::vector<std::string> Labels;
typedef std::vector<std::string> Assignment;
typedef std::vector<Labels> Assignments;

class Node;

typedef struct {
	Node *node;
	std::pair<std::string, Node*> outEdge;
} NodeAndOutEdge;

typedef struct {
	Node *node;
	std::pair<std::string, Node*> inEdge;
} NodeAndInEdge;

typedef struct {
	int id;
	std::vector<NodeAndInEdge> incomingEdges;
	std::vector<NodeAndOutEdge> cycleEdges;
} Cycle;

typedef std::vector<Cycle> Cycles;

typedef struct {
	unsigned int count;
	unsigned int subgraphcount;
} Sub;

typedef std::map<Node*, Sub> VisitedByNodes;

typedef struct {
	unsigned int original;
	unsigned int computed;
	std::vector<std::vector<Node*> > paths;
} Values;

typedef std::map<std::string, std::map<Node*, Values> > labelMap;

typedef struct {
	unsigned int multiply;
	Values surplus;
} AssignCount;

enum optionType {
    VAR1,
    VAR2,
    VAR1NOFORMULA,
    VAR2NOFORMULA
};

enum formulaType {
    FORMULA_AND,    ///< a conjunction
    FORMULA_OR,     ///< a disjunction
    FORMULA_GEN     ///< any other formula
};

#endif
