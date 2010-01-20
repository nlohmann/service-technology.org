#ifndef _TYPES_H
#define _TYPES_H

#include <string>
#include <vector>
#include <algorithm>

typedef std::vector<std::string> Labels;
typedef std::vector<std::string> Assignment;
typedef std::vector<Labels> Assignments;

enum formulaType {
    FORMULA_AND,    ///< a conjunction
    FORMULA_OR,     ///< a disjunction
    FORMULA_GEN     ///< any other formula
};

#endif
