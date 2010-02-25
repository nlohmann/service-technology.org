/*****************************************************************************\
 Safira -- Implementing a Set Algebra for Service Behavior

 Copyright (c) 2010 Kathrin Kaschner

 Safira is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Safira is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Safira.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

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
