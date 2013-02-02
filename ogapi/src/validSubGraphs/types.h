#ifndef _TYPES_H
#define _TYPES_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>

typedef std::vector<std::string> Labels;
typedef std::vector<std::string> Assignment;
typedef std::vector<Labels> Assignments;

class Node;

typedef struct {
	unsigned int count;
	unsigned int subgraphcount;
} Sub;

typedef std::map<Node*, Sub> VisitedByNodes;

typedef struct {
	unsigned int multiply;
	std::map<Node*, unsigned int> surplus;
} AssignCount;

enum formulaType {
    FORMULA_AND,    ///< a conjunction
    FORMULA_OR,     ///< a disjunction
    FORMULA_GEN     ///< any other formula
};

#endif
