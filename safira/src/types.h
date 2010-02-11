#ifndef _TYPES_H
#define _TYPES_H

#include <string>
#include <vector>
#include <iostream>

typedef std::vector<std::string> Labels;

enum FormulaType {UNDEF, AND, OR, NOT, LIT, INT, TRUE, FALSE, FINAL};

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

struct Intpair {
	int id1;
	int id2;
};

struct Cmp {
	bool operator()( const Intpair i1, const Intpair i2 ) const {
	//	std::cout << "1)" << i1.id1 << " " << i1.id2 << std::endl;
	//	std::cout << "2)" << i2.id1 << " " << i2.id2 << std::endl;
		if (i1.id1 < i2.id1) {
		//	std::cout << "return true1\n";
			return true;
		}
		if (i1.id1 == i2.id1 && i1.id2 < i2.id2) {
		//	std::cout << "return true2\n";
			return true;
		}
	//	std::cout << "return false\n";
		return false;
	}
};

#endif
