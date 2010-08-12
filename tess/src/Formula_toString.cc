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

#include <config.h>
#include <iostream>
#include "Formula.h"
#include <map>
#include <string>
#include "helpers.h"

extern map<int, string> id2label;
extern map<string, int> label2id;


/****************************************************************************
 * string output
 ***************************************************************************/

string FormulaAND::toString() const {
	if (merged){
		string s = "(";
		bool first = true;
		for (list<Formula*>::const_iterator iter = formulas.begin(); iter!= formulas.end(); ++iter){
			assert(formulas.size() > 0);
			Formula* f = *iter;

			if (first){
				first = false;
			}
			else {
				s = s + " * ";
			}
			s = s + f->toString();
		}
		s = s + ")";
		return s;
	}
	else {
		return ("(" + left->toString() + " * " + right->toString() + ")");
	}
}

string FormulaOR::toString() const {
	if (merged){
		string s = "(";
		bool first = true;
		for (list<Formula*>::const_iterator iter = formulas.begin(); iter!= formulas.end(); ++iter){
			assert(formulas.size() > 0);
			Formula* f = *iter;

			if (first){
				first = false;
			}
			else {
				s = s + " + ";
			}
			s = s + f->toString();
		}
		s = s + ")";
		return s;
	}
	else {
		return ("(" + left->toString() + " + " + right->toString() + ")");
	}
}

string FormulaLit::toString() const {
	assert(id2label.find(id) != id2label.end());
    return id2label[id];
}

string FormulaNUM::toString() const {
    return intToString(id);
}

string FormulaTrue::toString() const {
    return ("true");
}

string FormulaFinal::toString() const {
    return ("final");
}
