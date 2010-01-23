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
#include "vector"

#include "testFormula.h"

/// the old "main()" of Minisat with adjusted exit codes
extern vector<bool>* minisat2(vector< vector <int> > &);
extern int minisat(vector< vector <int> > &);

/// the command line parameters
extern gengetopt_args_info args_info;

extern map<int, string> id2label;
extern map<string, int> label2id;

double Formula::full_time;

/****************************************************************************
 * constructors
 ***************************************************************************/
Formula::Formula() : formulaType(UNDEF){
	//cout << "Konstruktor: Formula" << endl;
}

FormulaAND::FormulaAND(const Formula *_left, const Formula *_right) :
    left(_left), right(_right)
{
	//cout << "Konstruktor: FormulaAnd" << endl;
    assert(_left);
    assert(_right);
    formulaType = AND;
}

FormulaOR::FormulaOR(const Formula *_left, const Formula *_right) :
    left(_left), right(_right)
{
    assert(_left);
    assert(_right);
    formulaType = OR;
}

FormulaNOT::FormulaNOT(const Formula *_f) :
    f(_f)
{
    assert(_f);
    formulaType = NOT;
}

FormulaLit::FormulaLit(const int _number) :
    number(_number) {
	formulaType = LIT;
}

FormulaNUM::FormulaNUM(const int _number) :
    number(_number) {
	//cout << "Konstruktor: FormulaNUM" << endl;
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

/****************************************************************************
 * copy constructors
 ***************************************************************************/
Formula::Formula(const Formula &formula) : 	formulaType(formula.formulaType){
	//cout << "formulaType: " << formulaType << endl;
}


FormulaAND::FormulaAND(const FormulaAND &formula):
    Formula(formula) ,
    left(formula.left->getCopy()),
    right(formula.right->getCopy()){
}

FormulaOR::FormulaOR(const FormulaOR &formula) :
	Formula(formula),
	left(formula.left->getCopy()),
	right(formula.right->getCopy()){
}

FormulaNOT::FormulaNOT(const FormulaNOT &formula) :
	Formula(formula),
	f(formula.f->getCopy()){
}

FormulaLit::FormulaLit(const FormulaLit &formula) :
	Formula(formula), number(formula.number){
}

FormulaNUM::FormulaNUM(const FormulaNUM &formula) :
	Formula(formula), number(formula.number){
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


list<Clause> Formula::calculateCNF(){

	int maxId = label2id.size();

	Clause cl;
	cl.literal0 = maxId;
	cl.literal1 = emptyLiteral();
	cl.literal2 = emptyLiteral();

	Formula* h = moveNegation();
	list<Clause> clauses = h->toCNF(maxId,maxId);
	assert(clauses.size() >= 1);

	//append the cl to clauses
	clauses.push_front(cl);

	delete h;
	return clauses;
}



bool Formula::isSatisfiable(){
	//cout << toString() << endl;
	list<Clause> clauses = calculateCNF();
    vector<vector<int> > clausesVector;

    for(list<Clause>::const_iterator n = clauses.begin(); n != clauses.end(); ++n){
    	//printClause(*n);
        vector<int> currentClause(clauseToIntVector(*n));
        currentClause.push_back(0);
        clausesVector.push_back(currentClause);
    }

//	clock_t start_clock = clock();
    vector<bool> *assignment = minisat2(clausesVector);
//	full_time += (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC;

    if(assignment){
    	return true;
    }
    else {
    	return false;
    }
}


/****************************************************************************
 * string output
 ***************************************************************************/

string FormulaAND::toString() const {
    return ("(" + left->toString() + " * " + right->toString() + ")");
}

string FormulaOR::toString() const {
    return ("(" + left->toString() + " + " + right->toString() + ")");
}

string FormulaNOT::toString() const {
    return ("~(" + f->toString() + ")");
}

string FormulaLit::toString() const {
	assert(id2label.find(number) != id2label.end());
    return id2label[number];
}

string FormulaNUM::toString() const {
    return intToString(number);
}

string FormulaTrue::toString() const {
    return ("true");
}

string FormulaFalse::toString() const {
    return ("false");
}

string FormulaFinal::toString() const {
    return ("final");
}

/****************************************************************************
 * move negation into the inner
 ****************************************************************************/

Formula* FormulaAND::moveNegation(bool leadingNot) const{
	if (leadingNot == true){
		return new FormulaOR(left->moveNegation(true), right->moveNegation(true));
	}
	else{
		assert(leadingNot == false);
		return new FormulaAND(left->moveNegation(false), right->moveNegation(false));
	}
}

Formula* FormulaOR::moveNegation(bool leadingNot) const{
	if (leadingNot == true){
		return new FormulaAND(left->moveNegation(true),right->moveNegation(true));
	}
	else{
		assert(leadingNot == false);
		return new FormulaOR(left->moveNegation(false), right->moveNegation(false));
	}
}

Formula* FormulaNOT::moveNegation(bool leadingNot) const {
	if (leadingNot == true){
		return f->moveNegation(false);
	}
	else{
		assert(leadingNot == false);
		return f->moveNegation(true);
	}
}

Formula* FormulaLit::moveNegation(bool leadingNot) const {
	if (leadingNot == true){
		return new FormulaNOT(new FormulaLit(number));
	}
	else {
		assert(leadingNot == false);
		return new FormulaLit(number);
	}
}

Formula* FormulaNUM::moveNegation(bool leadingNot) const {
	if (leadingNot == true){
		return new FormulaNOT(new FormulaNUM(number));
	}
	else {
		assert(leadingNot == false);
		return new FormulaNUM(number);
	}
}

Formula* FormulaTrue::moveNegation(bool leadingNot) const {
	if (leadingNot == true){
		return new FormulaFalse();
	}
	else {
		assert(leadingNot == false);
		return new FormulaTrue();
	}
}

Formula* FormulaFalse::moveNegation(bool leadingNot) const {
	if (leadingNot == true){
		return new FormulaTrue();
	}
	else {
		assert(leadingNot == false);
		return new FormulaFalse();
	}
}

Formula* FormulaFinal::moveNegation(bool leadingNot) const {
	if (leadingNot == true){
		return new FormulaNOT(new FormulaFinal());
	}
	else {
		assert(leadingNot == false);
		return new FormulaFinal();
	}
}

/***********************************************************************
 * formula to CNF
 ***********************************************************************/
//maxId: bisher h�chste ID in Formel
list<Clause> FormulaAND::toCNF(int id, int& maxId) const {

    bool leftLit = false;
    bool rightLit = false;
    bool tf = false; //none of the subformulas is equal to (T)RUE or (F)ALSE

    if (left->formulaType == TRUE || left->formulaType == FALSE || right->formulaType == TRUE || right->formulaType == FALSE){
		tf = true;
	}

	if (left->formulaType == INT || left->formulaType == LIT || left->formulaType == NOT || left->formulaType == FINAL){
		leftLit = true;
	}

	if (right->formulaType == INT || right->formulaType == LIT || right->formulaType == NOT || right->formulaType == FINAL){
		rightLit = true;
	}

	list<Clause> clausesLeft;
	list<Clause> clausesRight;

	if(tf == false){	//none of the two subformuals ist equivalent to TRUE or FALSE

		if (leftLit == true){
			// to get the number of the left side
			clausesLeft = left->toCNF(id, maxId);
			assert(clausesLeft.size() == 2);
			assert(clausesLeft.begin()->literal0 == -1 * id);
			assert(clausesLeft.begin()->literal1 != emptyLiteral());
			assert(clausesLeft.begin()->literal2 == emptyLiteral());
		}

		if (rightLit == true) {
			//to get the number of the right side
			clausesRight = right->toCNF(id, maxId);
			assert(clausesRight.size() == 2);
			assert(clausesRight.begin()->literal0 == -1 * id);
			assert(clausesRight.begin()->literal1 != emptyLiteral());
			assert(clausesRight.begin()->literal2 == emptyLiteral());
		}


		if (leftLit == true && rightLit == true){ //CASE 1.1
			//cout << "case1.1" << endl;

			return (tripleAND(id, (clausesLeft.begin())->literal1, (clausesRight.begin())->literal1));
		}


		if (leftLit == true ){	// CASE 1.2
			//cout << "case1.2" << endl;
			assert (rightLit == false);

			++maxId;
			list<Clause> templist = tripleAND(id, (clausesLeft.begin())->literal1, maxId);

			list<Clause> clauses = right->toCNF(maxId, maxId);
			list<Clause>::iterator iter = clauses.begin();
			clauses.splice(iter, templist); //add the clauses of the left child

			return clauses;
		}


		if (rightLit == true){ // CASE 1.3
			//cout << "case1.3" << endl;
			assert(leftLit == false);

			++maxId;
			list<Clause> templist = tripleAND(id, maxId, (clausesRight.begin())->literal1);

			list<Clause> clauses = left->toCNF(maxId, maxId);
			list<Clause>::iterator iter = clauses.begin();
			clauses.splice(iter, templist); //append the clauses of the left child

			return clauses;
		}


		assert(leftLit == false && rightLit == false); //CASE 1.4
		//cout << "case1.4" << endl;

		int x1 = ++maxId;
		list<Clause> clauses = left->toCNF(maxId, maxId);

		int x2 = ++maxId;
		list<Clause> tempRight = right->toCNF(maxId, maxId);
		//append the clauses of the right child
		list<Clause>::iterator iter = clauses.end();
		clauses.splice(iter, tempRight);

		//express id<->x1*x2 as set of clauses
		list<Clause> tempRoot = tripleAND(id, x1, x2);
		iter = clauses.begin();
		clauses.splice(iter, tempRoot);

		return clauses;
	}

    else {
    	assert(tf == true);

    	if (left->formulaType == FALSE || right->formulaType == FALSE){ // CASE 2.1
    		//cout << "case 2.1" << endl;

    		// "x <-> <arbitrary> * false" is equivalent to x <-> false
    		return (xEqualsFalse(id));
    	}

    	if (left->formulaType == TRUE && right->formulaType == TRUE){ //CASE 2.2
    		//cout << "case 2.2" << endl;

    		//x <-> true * true  is equal to x<->true
    		return (xEqualsTrue(id));
    	}

    	/*
    	 * CASE 2.3: right->formulaType == INT/LIT/FINAL/NOT
    	 * CASE 2.4: right->formulaType == AND/OR
    	 */
    	if(left->formulaType == TRUE){
    		//cout << "case2.3/case2.4" << endl;

    		assert(right->formulaType != TRUE);
    		assert(right->formulaType != FALSE);
    		return xEqualsFormula(id, maxId, right);
    	}

    	/*
    	 * CASE 2.5: left->formulaType == INT/LIT/FINAL/NOT
    	 * CASE 2.6: left->formulaType == AND/OR
    	 */
    	if(right->formulaType == TRUE){
    		//cout << "case2.5/case 2.6" << endl;

    		assert(left->formulaType != TRUE);
    		assert(left->formulaType != FALSE);
    		return (xEqualsFormula(id, maxId, left));
    	}
    }
    assert(false);
}

list<Clause> FormulaOR::toCNF(int id, int& maxId) const {

    bool leftLit = false;
    bool rightLit = false;
    bool tf = false; //none of the subformulas is equal to (T)RUE or (F)ALSE

    if (left->formulaType == TRUE || left->formulaType == FALSE || right->formulaType == TRUE || right->formulaType == FALSE){
    	tf = true;
    }

    if (left->formulaType == INT || left->formulaType == LIT || left->formulaType == NOT || left->formulaType == FINAL){
    	leftLit = true;
    }

    if (right->formulaType == INT || right->formulaType == LIT || right->formulaType == NOT || right->formulaType == FINAL){
    	rightLit = true;
    }

    list<Clause> clausesLeft;
    list<Clause> clausesRight;

    if(tf == false){	//none of the two subformuals ist equivalent to TRUE or FALSE

    	if (leftLit == true){
    		// to get the number of the left side
    		clausesLeft = left->toCNF(id, maxId);
    		assert(clausesLeft.size() == 2);
    		assert(clausesLeft.begin()->literal0 == -1 * id);
    		assert(clausesLeft.begin()->literal1 != emptyLiteral());
    		assert(clausesLeft.begin()->literal2 == emptyLiteral());
    	}

    	if (rightLit == true) {
    		//to get the number of the right side
    		clausesRight = right->toCNF(id, maxId);
    		assert(clausesRight.size() == 2);
    		assert(clausesRight.begin()->literal0 == -1 * id);
    		assert(clausesRight.begin()->literal1 != emptyLiteral());
    		assert(clausesRight.begin()->literal2 == emptyLiteral());
    	}


    	if (leftLit == true && rightLit == true){ //CASE 1.1
    		//cout << "case1.1" << endl;

			return (tripleOR(id, (clausesLeft.begin())->literal1, (clausesRight.begin())->literal1));
    	}

    	if (leftLit == true){ // CASE 1.2
    		//cout << "case1.2" << endl;
			assert(rightLit == false);

			++maxId;
			list<Clause> templist = tripleOR(id, (clausesLeft.begin())->literal1, maxId);

			list<Clause> clauses = right->toCNF(maxId, maxId);
			list<Clause>::iterator iter = clauses.begin();
			clauses.splice(iter, templist); //append the clauses of the left child

			return clauses;
		}

		if (rightLit == true){ // CASE 1.3
			//cout << "case1.3" << endl;
			assert(leftLit == false);

			++maxId;
			list<Clause> templist = tripleOR(id, maxId, (clausesRight.begin())->literal1);

			list<Clause> clauses = left->toCNF(maxId, maxId);
			list<Clause>::iterator iter = clauses.begin();
			clauses.splice(iter, templist); //append the clauses of the left child

			return clauses;
		}

		assert(leftLit == false && rightLit == false); //CASE 1.4
		//cout << "case1.4" << endl;

		int x1 = ++maxId;
		list<Clause> clauses = left->toCNF(maxId, maxId);

		int x2 = ++maxId;
		list<Clause> tempRight = right->toCNF(maxId, maxId);

		//append the clauses of the right child
		list<Clause>::iterator iter = clauses.end();
		clauses.splice(iter, tempRight);


		//express id<->x1+x2 as set of clauses
		list<Clause> tempRoot = tripleOR(id, x1, x2);
		iter = clauses.begin();
		clauses.splice(iter, tempRoot);

		return clauses;

    }
    else {
    	assert(tf == true);

    	if (left->formulaType == FALSE && right->formulaType == FALSE){  // CASE 2.1
    		//cout << "case 2.1" << endl;

    		//x <-> false + false" is equivalent to x <-> false
    		return (xEqualsFalse(id));
    	}

    	/*
    	 * CASE 2.2: x <-> <arbitrary> + true   is equal to x <-> true
    	 */
    	if (left->formulaType == TRUE || right->formulaType == TRUE){
    		//cout << "case 2.2" << endl;

    		//x <-> true + formula  is equal to x<->true
    		return (xEqualsTrue(id));
    	}

    	/*
    	 * CASE 2.3:  right->formulaType == INT/LIT/FINAL/NOT
    	 * CASE 2.4:  right->formulaType == AND/OR
    	 */
    	if(left->formulaType == FALSE){
    		//cout << "case2.3/case2.4" << endl;

    		assert(right->formulaType != TRUE);
    		assert(right->formulaType != FALSE);
   			return (xEqualsFormula(id, maxId, right));
    	}

    	/*
    	 * CASE 2.5:  left->formulaType == INT/LIT/FINAL/NOT
    	 * CASE 2.6:  left->formulaType == AND/OR
    	 */
    	if(right->formulaType == FALSE){
    		//cout << "case2.5/case 2.6" << endl;

    		assert(left->formulaType != TRUE);
    		assert(left->formulaType != FALSE);

    		return (xEqualsFormula(id, maxId, left));
    	}
    }
    assert(false);
}

list<Clause> FormulaNOT::toCNF(int varId, int& maxId) const{
    list<Clause> clauses = f->toCNF(varId,maxId);
    assert(clauses.size() == 2);

    list<Clause>::iterator iter = clauses.begin(); //first clause
    iter->literal1 = negateLiteral(iter->literal1);
    ++iter;
    iter->literal1 = negateLiteral(iter->literal1);

    return clauses;

	//return ("~" + f->toCNF(varId, maxId));


}

list<Clause> FormulaLit::toCNF(int x, int&) const{

//    list<Clause> clauses;
//	Clause cl;
//	cl.literal0 = number;
//	cl.literal1 = emptyLiteral();
//	cl.literal2 = emptyLiteral();
//	clauses.push_back(cl);
//	return clauses;

	return (xEqualsI(x, number));

}

list<Clause> FormulaNUM::toCNF(int x, int&) const{

//	list<Clause> clauses;
//	Clause cl;
//	cl.literal0 = number;
//	cl.literal1 = emptyLiteral();
//	cl.literal2 = emptyLiteral();
//	clauses.push_back(cl);
//	return clauses;

	return (xEqualsI(x, number));

}

list<Clause> FormulaTrue::toCNF(int x, int&) const {
	assert(label2id.find("true") == label2id.end());
	list<Clause> clauses = xEqualsTrue(x);
	return clauses;
}

list<Clause> FormulaFalse::toCNF(int x, int&) const{
	assert(label2id.find("false") == label2id.end());
	list<Clause> clauses = xEqualsFalse(x);
	return clauses;
}

list<Clause> FormulaFinal::toCNF(int x, int&) const{

	assert(label2id.find("final") != label2id.end());

//	list<Clause> clauses;
//	Clause cl;
//
//	cl.literal0 = label2id["final"];
//	cl.literal1 = emptyLiteral();
//	cl.literal2 = emptyLiteral();
//	clauses.push_back(cl);
//	return clauses;

	return (xEqualsI(x, label2id["final"]));

}
