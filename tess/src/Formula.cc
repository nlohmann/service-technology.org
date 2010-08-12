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
#include <sstream>
#include <cstdlib>
#include <set>
#include <map>
#include <ctime>
#include <sys/types.h>
#include "Formula.h"
#include "types.h"
#include "helpers.h"
#include "cmdline.h"
#include "verbose.h"


extern map<int, string> id2label;
extern map<string, int> label2id;


/****************************************************************************
 * constructors
 ***************************************************************************/
Formula::Formula() : formulaType(UNDEF), merged(false){
}

FormulaAND::FormulaAND(Formula *_left, Formula *_right) : left(_left), right(_right) {
    assert(_left);
    assert(_right);

    formulaType = AND;
}

FormulaOR::FormulaOR(Formula *_left, Formula *_right) : left(_left), right(_right) {
    assert(_left);
    assert(_right);

    formulaType = OR;
}

FormulaLit::FormulaLit(const int _id) : id(_id) {
	formulaType = LIT;
}

FormulaNUM::FormulaNUM(const int _id) : id(_id) {
	formulaType = INT;
}

FormulaTrue::FormulaTrue() : id(label2id["true"]){
	formulaType = TRUE;
}

FormulaFinal::FormulaFinal() : id(label2id["final"]){
	formulaType = FINAL;
}


/****************************************************************************
 * deconstructors
 ***************************************************************************/
Formula::~Formula() {}

FormulaAND::~FormulaAND() {
//	if (!merged){
		delete left;
		delete right;
//	}
//	else {
//		for (list<Formula*>::const_iterator iter = formulas.begin(); iter!= formulas.end(); ++iter){
//			Formula* f = *iter;
//			delete f;
//		}
//	}
}

FormulaOR::~FormulaOR(){
//	if (!merged){
		delete left;
		delete right;
//	}
//	else {
//		for (list<Formula*>::const_iterator iter = formulas.begin(); iter!= formulas.end(); ++iter){
//			Formula* f = *iter;
//			delete f;
//		}
//	}
}


///// deletes Tau from the formula
//void Formula::deleteTauChild(){
//
//}
//
///// deletes Tau from the formula
//void FormulaAND::deleteTauChild(){
//
//}
//
///// deletes Tau from the formula
//void FormulaOR::deleteTauChild(){
//	assert(right->formulaType == LIT);
//	assert(right->toString() == Tau);
//	this = left;
//}

