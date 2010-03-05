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
#include <ctime>
#include <zlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include "Formula.h"
#include "helpers.h"



/// the old "main()" of Minisat with adjusted exit codes
extern vector<bool>* minisat2(vector< vector <int> > &);
extern int minisat(vector< vector <int> > &);


extern map<int, string> id2label;
extern map<string, int> label2id;

double Formula::full_time;


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

bool Formula::isSatisfiable(vector<bool> * assignment){
//	cout << "isSatisfiable(...): " << toString() << endl;
	assert(assignment->size() >= label2id.size());

	list<Clause> clauses = calculateCNF();

	Clause cl;
	for(int i = 1; i < label2id.size(); ++i){
		if(assignment->at(i) == true){
			cl.literal0 = i;
		}
		else{
			assignment->at(i) == false;
			cl.literal0 = i * -1;
		}
		cl.literal1 = emptyLiteral();
		cl.literal2 = emptyLiteral();
		clauses.push_back(cl);
	}

	vector<vector<int> > clausesVector;
	for(list<Clause>::const_iterator n = clauses.begin(); n != clauses.end(); ++n){
//		cout << "isSatisfiable(...): "; printClause(*n);
		vector<int> currentClause(clauseToIntVector(*n));
		currentClause.push_back(0);
		clausesVector.push_back(currentClause);
	}

	//	clock_t start_clock = clock();
	vector<bool>* satAssignment = minisat2(clausesVector); // assignmet is NULL if Formula is not satifiable
	//	full_time += (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC;



	if(satAssignment){
		delete satAssignment;
		return true;
	}
	else {
		delete satAssignment;
		return false;
	}
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
    return minisat(clausesVector);
    //	full_time += (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC;
}

vector<bool> * Formula::getSatisfyingAssignment(){
//	cout << "isSatisfiable(): " << toString() << endl;
	list<Clause> clauses = calculateCNF();
    vector<vector<int> > clausesVector;
    //cout << "Number of Clauses: " << clauses.size() << endl;
    for(list<Clause>::const_iterator n = clauses.begin(); n != clauses.end(); ++n){
    	//cout << "isSatisfiable(): "; printClause(*n);
        vector<int> currentClause(clauseToIntVector(*n));
        currentClause.push_back(0);
        clausesVector.push_back(currentClause);
    }

//	clock_t start_clock = clock();
//	vector<bool>* satAssignment = minisat2(clausesVector); // satAssignmet is NULL if Formula is not satisfiable
//	cout << "isSatisfiable(): satisfying assignment returned by minisat2: " << assignmentToString(satAssignment) << endl;
//	return satAssignment;
    return minisat2(clausesVector);

//	full_time += (static_cast<double>(clock()) - static_cast<double>(start_clock)) / CLOCKS_PER_SEC;
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
}

list<Clause> FormulaLit::toCNF(int x, int&) const{
	return (xEqualsI(x, number));
}

list<Clause> FormulaNUM::toCNF(int x, int&) const{
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
	return (xEqualsI(x, label2id["final"]));
}

list<Clause> FormulaDummy::toCNF(int varId, int& maxId) const{
    list<Clause> clauses = f->toCNF(varId,maxId);
    return clauses;
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

Formula* FormulaDummy::moveNegation(bool leadingNot) const {
	return f->moveNegation(leadingNot);
}

