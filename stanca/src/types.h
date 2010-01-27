#ifndef _TYPES_H
#define _TYPES_H

#include <string>
#include <vector>

typedef std::vector<std::string> Labels;

enum FormulaType {UNDEF, AND, OR, NOT, LIT, INT, TRUE, FALSE, FINAL,IMP,EQUIV};

//struct Clause {
//	std::string literal0;
//	std::string literal1;
//	std::string literal2;
//};

struct Clause {
	int literal0;
	int literal1;
	int literal2;
};

#endif

