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
#include "Formula.h"

extern map<int, string> id2label;
extern map<string, int> label2id;


/***********************************************************************
 * substitute the literals/numbers in a formula with a formula (given by the map lits)
 ***********************************************************************/
const Formula* Formula::substitute(map<int, Formula*> ) const{
	return this->getCopy();
}


const Formula* FormulaAND::substitute(map<int,Formula*> lits) const{
	const Formula* newLeft = left->substitute(lits);
	const Formula* newRight = right->substitute(lits);

	if (newLeft->formulaType == FALSE || newRight->formulaType == FALSE){
		Formula* f = new FormulaFalse();
		return f;
	}

//	*((const Formula**)&left) = newLeft; //hack: left = newLeft (due to const)
//	*((const Formula**)&right) = newRight; //right = newRight (due to const)
//	return this;

	return new FormulaAND(newLeft, newRight);
}


const Formula* FormulaOR::substitute(map<int,Formula*> lits) const{
	Formula* newLeft = (Formula*) left->substitute(lits);
	Formula* newRight = (Formula*) right->substitute(lits);

	if (newLeft->formulaType == FALSE && newRight->formulaType == FALSE){
		Formula* f = new FormulaFalse();
		return f;
	}

//	*((Formula**)&left) = newLeft; //hack: left = newLeft (due to const)
//	*((Formula**)&right) = newRight; //right = newRight (due to const)
//	return this;

	return new FormulaOR(newLeft, newRight);

}


const Formula* FormulaNOT::substitute(map<int,Formula*> lits) const{
	const Formula* newf = f->substitute(lits);

	if (newf->formulaType == FALSE){
		Formula* f = new FormulaFalse();
		return f;
	}
	else {
		//*((const Formula**)&f) = newf; //hack: f = newf (due to const)
		//return this;
		return new FormulaNOT(newf);
	}
}


const Formula* FormulaNUM::substitute(map<int,Formula*> lits) const{

	if (lits.find(number) == lits.end()){
		Formula* f = new FormulaFalse();
		return f;
	}
	else {
		return new FormulaDummy(lits[number]);
	}
}

//const Formula* FormulaDummy::substitute(map<int,Formula*> lits) const{
//	const Formula* newf = f->substitute(lits);
//
//	return new FormulaDummy(newf);
//}
