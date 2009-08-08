#include "config.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <set>
#include <map>
#include <list>
#include "Formula.h"
#include "types.h"
#include "helpers.h"


extern map<int, string> id2label;
extern map<string, int> label2id;

/****************************************************************************
 * constructors
 ***************************************************************************/

FormulaAND::FormulaAND(const Formula *_left, const Formula *_right) :
    left(_left), right(_right)
{
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



//fVar: number of Variables in the formula to be checked
bool Formula::isSatisfiable(int fVar){
	list<Clause> clauses = moveNegation()->toCNF(fVar+1,fVar+1);

	int cntVar = fVar + clauses.size(); //TODO: das müsste eine Abschätzung nach oben sein für die Anzahl der Variablen
	string sVar_Clauses;
	stringstream out;
	out << cntVar;
	sVar_Clauses = out.str();

	int cntClauses = clauses.size();
	out << "  " << cntClauses;
	sVar_Clauses = out.str();

	string s = "echo 'p cnf " + sVar_Clauses;

	string test;
	for(list<Clause>::const_iterator n = clauses.begin(); n != clauses.end(); ++n){
			s = s + clauseToString(*n) + "0 ";
	}
//	s = s +  " ' | /Users/kathrin/5_Projekte/minisat/core/minisat &> /dev/null";
	s = s +  " ' | /Users/niels/Documents/6Tools/service-tech/safira/minisat/src/minisat &> /dev/null";

//	cout << "in function isSatisfiable: \n";
//	cout << "max. number of the Variables in the formula: " << fVar << endl;
//	cout << "formula: " << toString() << endl;
//	cout << "moveNegation(): " << moveNegation()->toString() << endl;
//	cout << "CNF-String: " << moveNegation()->toStringCNF(fVar+1,fVar+1) << endl;
//	cout << "Clauses:\n";
//	for(list<Clause>::const_iterator n = clauses.begin(); n != clauses.end(); ++n){
//		cout << n->literal0 << "\t" << n->literal1 << "\t" << n->literal2 << endl;
//	}
//	cout << s << endl;

	int result = system(s.c_str());
	//int result = system("echo 'p cnf 1 2 -1 0 -1 0' | /Users/kathrin/5_Projekte/minisat/core/minisat &> /dev/null");

	//FILE *fp;
	//fp = popen("/Users/kathrin/5_Projekte/minisat/core/minisat", "w");
	//fprintf(fp, s.c_str());
	//pclose(fp);

	result = result>>8;
	assert(result != 3); //3 == unexpected char
	if (20 == result){   // Unsatisfiable
		//cout << "Formula is NOT satisfiable" << endl;
		return false;
	}
	else {
		assert(result == 0); //Satisfiable
		//cout << "Formula is satisfiable" << endl;
		return true;
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
	string s = intToString(number);
    return s;
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

list<Clause> FormulaAND::toCNF(int varId, int maxId) const {
//	string sId = intToString(varId);
//	string sMax1 = intToString(maxId+1);
//	string sMax2 = intToString(maxId+2);

	int sId = varId;
	int sMax1 = maxId + 1;
	int sMax2 = maxId + 2;

    bool leftLit = false;
    bool rightLit = false;
    bool tf = false; //at least one subformula is equal to (t)rue or (f)alse
    list<Clause> clauses;

    if (left->formulaType == TRUE || left->formulaType == FALSE || right->formulaType == TRUE || right->formulaType == FALSE){
		tf = true;
	}

	if (left->formulaType == INT || left->formulaType == LIT || left->formulaType == NOT || left->formulaType == FINAL){
		leftLit = true;
	}

	if (right->formulaType == INT || right->formulaType == LIT || right->formulaType == NOT || right->formulaType == FINAL){
		rightLit = true;
	}

	if(tf == false){

		if (leftLit == true && rightLit == true){ //CASE 1.1
			//cout << "case1.1" << endl;
			list<Clause> clausesLeft = left->toCNF();
			assert(clausesLeft.size() == 1);
			assert(clausesLeft.begin()->literal1 == emptyLiteral());
			assert(clausesLeft.begin()->literal2 == emptyLiteral());
			assert(clauses.size() == 0);

			list<Clause> clausesRight = right->toCNF();
			assert(clausesRight.size() == 1);
			assert(clausesRight.begin()->literal1 == emptyLiteral());
			assert(clausesRight.begin()->literal2 == emptyLiteral());

			clauses = tripleAND(sId, (clausesLeft.begin())->literal0, (clausesRight.begin())->literal0);

			return clauses;
			//return ("(" + Id + "<-> " + left->toCNF() + "*" + right->toCNF() + ")");
		}

		if (leftLit == true){ // CASE 1.2
			//cout << "case1.2" << endl;
			assert(rightLit == false);
			assert(clauses.size() == 0);

			list<Clause> clausesLeft = left->toCNF();
			assert(clausesLeft.size() == 1);
			assert(clausesLeft.begin()->literal1 == emptyLiteral());
			assert(clausesLeft.begin()->literal2 == emptyLiteral());

			clauses = tripleAND(sId, (clausesLeft.begin())->literal0, sMax1);

			list<Clause>::iterator iter = clauses.begin();
			list<Clause> templist = right->toCNF(maxId+1, maxId+1);
			clauses.splice(iter, templist); //append the clauses of the right child

			return clauses;
			//return ("(" + sId + "<->" + left->toCNF() + "*" + sMax1 + ")" + " * " + right->toCNF(maxId+1, maxId+1));
		}

		if (rightLit == true){ // CASE 1.3
			//cout << "case1.3" << endl;
			assert(leftLit == false);
			assert(clauses.size() == 0);

			list<Clause> clausesRight = right->toCNF();
			assert(clausesRight.size() == 1);
			assert(clausesRight.begin()->literal1 == emptyLiteral());
			assert(clausesRight.begin()->literal2 == emptyLiteral());

			clauses = tripleAND(sId, sMax1, (clausesRight.begin())->literal0);

			list<Clause>::iterator iter = clauses.begin();
			list<Clause> templist = left->toCNF(maxId+1, maxId+1);
			clauses.splice(iter, templist); //append the clauses of the left child

			return clauses;
			//return ("(" + sId + "<->" + sMax1 + "*" + right->toCNF() + ")" + " * " + left->toCNF(maxId+1, maxId+1));
		}

		if (leftLit == false && rightLit == false){ //CASE 1.4
			//cout << "case1.4" << endl;
			//express sId<->sMax1*sMax2 as set of clauses
			clauses = tripleAND(sId, sMax1, sMax2);

			//append the clauses of the left child
			list<Clause>::iterator iter = clauses.begin();
			list<Clause> templeft = left->toCNF(maxId+1, maxId+2);
			clauses.splice(iter, templeft);

			//append the clauses of the right child
			iter = clauses.begin();
			list<Clause> tempright = right->toCNF(maxId+2, maxId+2);
			clauses.splice(iter, tempright);

			return clauses;
			//return ("(" + sId + " <-> " + sMax1 + "*" + sMax2 + ")" + " * " + left->toCNF(maxId+1, maxId+2) + "*" + right->toCNF(maxId+2, maxId+2));
		}
		assert(false);
	}
    else {
    	assert(tf == true);

    	if (left->formulaType == FALSE || right->formulaType == FALSE){ // CASE 2.1
    		//cout << "case 2.1" << endl;
    		// "x <-> <arbitrary> * false" is equivalent to x <-> false
    		assert(clauses.size() == 0);
    		clauses = xEqualsFalse(varId);
    		return clauses;

    	}

    	if (left->formulaType == TRUE && right->formulaType == TRUE){ //CASE 2.2
    		//cout << "case 2.2" << endl;
    		//x <-> true * true  is equal to x<->true
    		clauses = xEqualsTrue();
    		return clauses;
    	}

    	/*
    	 * CASE 2.3: right->formulaType == INT/LIT/FINAL/NOT
    	 * CASE 2.4: right->formulaType == AND/OR
    	 */
    	if(left->formulaType == TRUE){
    		//cout << "case2.3" << endl;
    		assert(clauses.size() == 0);
    		assert(right->formulaType != TRUE);
    		assert(right->formulaType != FALSE);

    		clauses = xEqualsFormula(varId, maxId, right);
    	}

    	/*
    	 * CASE 2.5: left->formulaType == INT/LIT/FINAL/NOT
    	 * CASE 2.6: left->formulaType == AND/OR
    	 */
    	if(right->formulaType == TRUE){
    		//cout << "case2.5" << endl;
    		assert(clauses.size() == 0);
			clauses = xEqualsFormula(varId, maxId, left);
    		return clauses;
    	}
    }
    assert(false);
}

list<Clause> FormulaOR::toCNF(int varId, int maxId) const {
//	string sId = intToString(varId);
//	string sMax1 = intToString(maxId+1);
//	string sMax2 = intToString(maxId+2);

	int sId = varId;
	int sMax1 = maxId + 1;
	int sMax2 = maxId + 2;

    bool leftLit = false;
    bool rightLit = false;
    bool tf = false; //at least one subformula is equal to (t)rue or (f)alse
    list<Clause> clauses;

    if (left->formulaType == TRUE || left->formulaType == FALSE || right->formulaType == TRUE || right->formulaType == FALSE){
    	tf = true;
    }

    if (left->formulaType == INT || left->formulaType == LIT || left->formulaType == NOT || left->formulaType == FINAL){
    	leftLit = true;
    }

    if (right->formulaType == INT || right->formulaType == LIT || right->formulaType == NOT || right->formulaType == FINAL){
    	rightLit = true;
    }

    if(tf == false){
    	if (leftLit == true && rightLit == true){ //CASE 1.1
			list<Clause> clausesLeft = left->toCNF();
			assert(clausesLeft.size() == 1);
			assert(clausesLeft.begin()->literal1 == emptyLiteral());
			assert(clausesLeft.begin()->literal2 == emptyLiteral());
			assert(clauses.size() == 0);

			list<Clause> clausesRight = right->toCNF();
			assert(clausesRight.size() == 1);
			assert(clausesRight.begin()->literal1 == emptyLiteral());
			assert(clausesRight.begin()->literal2 == emptyLiteral());

			clauses = tripleOR(sId, (clausesLeft.begin())->literal0, (clausesRight.begin())->literal0);

			return clauses;
			//return ("(" + sId + "<->" + left->toCNF() + "+" + right->toCNF() + ")");
    }

    	if (leftLit == true){ // CASE 1.2
			assert(rightLit == false);
			assert(clauses.size() == 0);

			list<Clause> clausesLeft = left->toCNF();
			assert(clausesLeft.size() == 1);
			assert(clausesLeft.begin()->literal1 == emptyLiteral());
			assert(clausesLeft.begin()->literal2 == emptyLiteral());

			clauses = tripleOR(sId, (clausesLeft.begin())->literal0, sMax1);

			list<Clause>::iterator iter = clauses.begin();
			list<Clause> templist = right->toCNF(maxId+1, maxId+1);
			clauses.splice(iter, templist); //append the clauses of the right child
			return clauses;

			//return ("(" + sId + "<->" + left->toCNF() + "+" + sMax1 + ")" + " * " + right->toCNF(maxId+1, maxId+1));
		}

		if (rightLit == true){ // CASE 1.3
			assert(leftLit == false);
			assert(clauses.size() == 0);

			list<Clause> clausesRight = right->toCNF();
			assert(clausesRight.size() == 1);
			assert(clausesRight.begin()->literal1 == emptyLiteral());
			assert(clausesRight.begin()->literal2 == emptyLiteral());

			clauses = tripleOR(sId, sMax1, (clausesRight.begin())->literal0);

			list<Clause>::iterator iter = clauses.begin();
			list<Clause> templist = left->toCNF(maxId+1, maxId+1);
			clauses.splice(iter, templist); //append the clauses of the left child

			return clauses;
			//return ("(" + sId + "<->" + sMax1 + "+" + right->toCNF() + ")" + " * " + left->toCNF(maxId+1, maxId+1));
		}

		if (leftLit == false && rightLit == false){ // CASE 1.4
			assert(clauses.size() == 0);

			//express sId<->sMax1+sMax2 as set of clauses
			clauses = tripleOR(sId, sMax1, sMax2);

			//append the clauses of the left child
			list<Clause>::iterator iter = clauses.begin();
			list<Clause> templeft = left->toCNF(maxId+1, maxId+2);
			clauses.splice(iter, templeft);

			//append the clauses of the right child
			iter = clauses.begin();
			list<Clause> tempright = right->toCNF(maxId+2, maxId+2);
			clauses.splice(iter, tempright);

			return clauses;
			//return ("(" + sId + " <-> " + sMax1 + "+" + sMax2 + ")" + " * " + left->toCNF(maxId+1, maxId+2) + "*" + right->toCNF(maxId+2, maxId+2));
		}
		assert(false);
    }
    else {
    	assert(tf == true);

    	/*
    	 * CASE 2.1: x <-> false + false" is equivalent to x <-> false
    	 */
    	if (left->formulaType == FALSE && right->formulaType == FALSE){ //CASE 2.1
    		clauses = xEqualsFalse(varId);
			return clauses;
    	}

    	/*
    	 * CASE 2.2: x <-> <arbitrary> + true   is equal to x <-> true
    	 */
    	if (left->formulaType == TRUE || right->formulaType == TRUE){
    		clauses = xEqualsTrue(varId);
    		return clauses;
    	}

    	/* x <-> false + formula  is equal to x <-> formula
    	 * CASE 2.3:  right->formulaType == INT/LIT/FINAL/NOT
    	 * CASE 2.4:  right->formulaType == AND/OR
    	 */
    	if(left->formulaType == FALSE){
    		assert(right->formulaType != TRUE);
    		assert(right->formulaType != FALSE);
    		assert(clauses.size() == 0);

   			clauses = xEqualsFormula(varId, maxId, right);
			return clauses;
    	}

    	/* x <-> formula + false  is equal to x <-> formula
    	 * CASE 2.5:  left->formulaType == INT/LIT/FINAL/NOT
    	 * CASE 2.6:  left->formulaType == AND/OR
    	 */
    	if(right->formulaType == FALSE){
    		assert(clauses.size() == 0);
    		assert(left->formulaType != TRUE);
    		assert(left->formulaType != FALSE);

    		clauses = xEqualsFormula(varId, maxId, left);
    		return clauses;
    	}
    }
    assert(false);
}

list<Clause> FormulaNOT::toCNF(int varId, int maxId) const{
    list<Clause> clauses = f->toCNF(varId,maxId);
    assert(clauses.size() == 1);
    assert(clauses.begin()->literal1 == emptyLiteral());
    assert(clauses.begin()->literal2 == emptyLiteral());
    clauses.begin()->literal0 = negateLiteral(clauses.begin()->literal0);
    return clauses;

	//return ("~" + f->toCNF(varId, maxId));


}

list<Clause> FormulaLit::toCNF(int varId, int maxId) const{

    list<Clause> clauses;
	Clause cl;
	cl.literal0 = number;
	cl.literal1 = emptyLiteral();
	cl.literal2 = emptyLiteral();
	clauses.push_back(cl);
	return clauses;
}

list<Clause> FormulaNUM::toCNF(int varId, int maxId) const{

    list<Clause> clauses;
	Clause cl;
	cl.literal0 = number;
	cl.literal1 = emptyLiteral();
	cl.literal2 = emptyLiteral();
	clauses.push_back(cl);
	return clauses;
}

list<Clause> FormulaTrue::toCNF(int varId, int maxId) const {
	assert(label2id.find("true") != label2id.end());
	list<Clause> clauses = xEqualsTrue(label2id["true"]);
	return clauses;
}

list<Clause> FormulaFalse::toCNF(int varId, int maxId) const{
	assert(label2id.find("false") != label2id.end());
	list<Clause> clauses = xEqualsFalse(label2id["false"]);
	return clauses;
}

list<Clause> FormulaFinal::toCNF(int varId, int maxId) const{

	assert(label2id.find("final") != label2id.end());

	list<Clause> clauses;
	Clause cl;

	cl.literal0 = label2id["final"];
	cl.literal1 = emptyLiteral();
	cl.literal2 = emptyLiteral();
	clauses.push_back(cl);
	return clauses;
}


string FormulaAND::toStringCNF(int varId, int maxId) const{
	string sId = intToString(varId);
	string sMax1 = intToString(maxId+1);
	string sMax2 = intToString(maxId + 2);

	bool leftLit = false;
	bool rightLit = false;

	if (left->formulaType == INT || left->formulaType == LIT || left->formulaType == NOT ||
			left->formulaType == TRUE || left->formulaType == FALSE || left->formulaType == FINAL){
		leftLit = true;
	}

	if (right->formulaType == INT || right->formulaType == LIT || right->formulaType == NOT ||
			right->formulaType == TRUE || right->formulaType == FALSE || right->formulaType == FINAL){
		rightLit = true;
	}

	if (leftLit == true && rightLit == true){
		return ("(" + sId + "<->" + left->toStringCNF() + "*" + right->toStringCNF() + ")");
	}

	if (leftLit == true){
		assert(rightLit == false);
		return ("(" + sId + "<->" + left->toStringCNF() + "*" + sMax1 + ")" + " * "
				+ right->toStringCNF(maxId+1, maxId+1));
	}

	if (rightLit == true){
		assert(leftLit == false);
		return ("(" + sId + "<->" + sMax1 + "*" + right->toStringCNF() + ")" + " * "
				+ left->toStringCNF(maxId+1, maxId+1));
	}

	assert(leftLit == false && rightLit == false);
	return ("(" + sId + "<->" + sMax1 + "*" + sMax2 + ")" + " * "
			+ left->toStringCNF(maxId+1, maxId+2) + " * " + right->toStringCNF(maxId+2, maxId+2));
}

string FormulaOR::toStringCNF(int varId, int maxId) const{
	string sId = intToString(varId);
	string sMax1 = intToString(maxId+1);
	string sMax2 = intToString(maxId + 2);

    bool leftLit = false;
    bool rightLit = false;

    if (left->formulaType == INT || left->formulaType == LIT || left->formulaType == NOT ||
            left->formulaType == TRUE || left->formulaType == FALSE || left->formulaType == FINAL){
    	leftLit = true;
    }

    if (right->formulaType == INT || right->formulaType == LIT || right->formulaType == NOT ||
            right->formulaType == TRUE || right->formulaType == FALSE || right->formulaType == FINAL){
    	rightLit = true;
    }

    if (leftLit == true && rightLit == true){
    	return ("(" + sId + "<->" + left->toStringCNF() + "+" + right->toStringCNF() + ")");
    }

    if (leftLit == true){
    	assert(rightLit == false);
        return ("(" + sId + "<->" + left->toStringCNF() + "+" + sMax1 + ")" + " * "
    	+ right->toStringCNF(maxId+1, maxId+1));
    }

    if (rightLit == true){
    	assert(leftLit == false);
    	return ("(" + sId + "<->" + sMax1 + "+" + right->toStringCNF() + ")" + " * "
    	+ left->toStringCNF(maxId+1, maxId+1));
    }

    assert(leftLit == false && rightLit == false);
	return ("(" + sId + "<->" + sMax1 + "+" + sMax2 + ")" + " * "
			+ left->toStringCNF(maxId+1, maxId+2) + " * " + right->toStringCNF(maxId+2, maxId+2));

}

string FormulaNOT::toStringCNF(int varId, int maxId) const{
	return ("-" + f->toStringCNF(varId, maxId));
}

string FormulaLit::toStringCNF(int varId, int maxId) const{
	string sNumber = intToString(number);
	return (sNumber);

}

string FormulaNUM::toStringCNF(int varId, int maxId) const{
	string sNumber = intToString(number);
	return (sNumber);
}

string FormulaTrue::toStringCNF(int varId, int maxId) const{
	return ("true");
}

string FormulaFalse::toStringCNF(int varId, int maxId) const{
	return ("false");
}

string FormulaFinal::toStringCNF(int varId, int maxId) const{
	return ("final");
}
