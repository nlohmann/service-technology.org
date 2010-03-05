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
#include <list>
#include <ctime>
#include <zlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Formula.h"
#include "types.h"
#include "helpers.h"
#include "cmdline.h"
#include "verbose.h"


/// the old "main()" of Minisat with adjusted exit codes
extern vector<bool>* minisat2(vector< vector <int> > &);
extern int minisat(vector< vector <int> > &);

extern map<int, string> id2label;
extern map<string, int> label2id;


/****************************************************************************
 * constructors
 ***************************************************************************/
Formula::Formula() : formulaType(UNDEF){
}

FormulaAND::FormulaAND(const Formula *_left, const Formula *_right) : left(_left), right(_right) {
    assert(_left);
    assert(_right);

    formulaType = AND;
}

FormulaOR::FormulaOR(const Formula *_left, const Formula *_right) : left(_left), right(_right) {
    assert(_left);
    assert(_right);

    formulaType = OR;
}

FormulaNOT::FormulaNOT(const Formula *_f) : f(_f) {
    assert(_f);

    formulaType = NOT;
}

FormulaLit::FormulaLit(const int _number) : number(_number) {
	formulaType = LIT;
}

FormulaNUM::FormulaNUM(const int _number) : number(_number) {
	formulaType = INT;
}

FormulaTrue::FormulaTrue(){
	formulaType = TRUE;
}

FormulaFalse::FormulaFalse(){
	formulaType = FALSE;
}

FormulaFinal::FormulaFinal(){
	formulaType = FINAL;
}

FormulaDummy::FormulaDummy(const Formula *_f) : f(_f) {
    assert(_f);

	formulaType = DUMMY;
}


Formula* FormulaAND::getCopy() const {
	return (new FormulaAND(*this));
}

Formula* FormulaOR::getCopy() const {
	return (new FormulaOR(*this));
}

Formula* FormulaNOT::getCopy() const {
	return (new FormulaNOT(*this));
}

Formula* FormulaLit::getCopy() const {
	return (new FormulaLit(*this));
}

Formula* FormulaNUM::getCopy() const {
	return (new FormulaNUM(*this));
}

Formula* FormulaTrue::getCopy() const {
	return (new FormulaTrue(*this));
}

Formula* FormulaFalse::getCopy() const {
	return (new FormulaFalse(*this));
}

Formula* FormulaFinal::getCopy() const {
	return (new FormulaFinal(*this));
}

Formula* FormulaDummy::getCopy() const {
	return (new FormulaDummy(*this));
}

/****************************************************************************
 * copy constructors
 ***************************************************************************/
Formula::Formula(const Formula &formula) :
	formulaType(formula.formulaType) {

}


FormulaAND::FormulaAND(const FormulaAND &formula):
    Formula(formula) ,
    left(formula.left->getCopy()),
    right(formula.right->getCopy()) {

}

FormulaOR::FormulaOR(const FormulaOR &formula) :
	Formula(formula),
	left(formula.left->getCopy()),
	right(formula.right->getCopy()) {

}

FormulaNOT::FormulaNOT(const FormulaNOT &formula) :
	Formula(formula),
	f(formula.f->getCopy()) {

}

FormulaLit::FormulaLit(const FormulaLit &formula) :
	Formula(formula), number(formula.number) {

}

FormulaNUM::FormulaNUM(const FormulaNUM &formula) :
	Formula(formula), number(formula.number) {

}

FormulaDummy::FormulaDummy(const FormulaDummy &formula) :
	Formula(formula),
	f(formula.f->getCopy()) {

}

/****************************************************************************
 * operator =
 ***************************************************************************/
//FormulaAND& FormulaAND::operator = (const FormulaAND &formula){
//	left = formula.left;
//	right = formula.right;
//}
//
//FormulaOR& FormulaOR::operator = (const FormulaOR &formula){
//	left = formula.left;
//	right = formula.right;
//}
//
//FormulaNOT& FormulaNOT::operator = (const FormulaNOT &formula){
//	f = formula.f;
//}


/****************************************************************************
 * deconstructors
 ***************************************************************************/
Formula::~Formula() {}

FormulaAND::~FormulaAND() {
	delete left;
	delete right;
}

FormulaOR::~FormulaOR(){
	delete left;
	delete right;
}

FormulaNOT::~FormulaNOT() {
	delete f;
}

FormulaDummy::~FormulaDummy() {

}
