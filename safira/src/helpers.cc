#include <config.h>
#include "helpers.h"
#include <iostream>
#include <sstream>
#include <map>

extern map<string, int> label2id;
extern map<int, string> id2label;

void addLabel(string label, int id){
	assert(label2id.find(label) == label2id.end());
	assert(id2label.find(id) == id2label.end());
	id2label[id] = label;
	label2id[label] = id;
}

std::vector<int> clauseToIntVector(const Clause &c) {
  std::vector<int> result;

	if (c.literal0 != emptyLiteral()){
    result.push_back(c.literal0);
	}
	if (c.literal1 != emptyLiteral()){
    result.push_back(c.literal1);
	}
	if (c.literal2 != emptyLiteral()){
    result.push_back(c.literal2);
	}
  
  return result;
}

string clauseToString(Clause c){
	string s = " ";
	if (c.literal0 != emptyLiteral()){
		s = s + intToString(c.literal0);
	}
	if (c.literal1 != emptyLiteral()){
		s = s + " " + intToString(c.literal1);
	}
	if (c.literal2 != emptyLiteral()){
		s = s + " " + intToString(c.literal2);
	}

	s = s + " ";

	return s;  // s = " " + c->literal0 + " " + c->literal1 + " " + c->literal2 + " "
}

string intToString(int i){

	stringstream ssout;
	ssout <<  i;
        string s (ssout.str());
	return s;
}

int negateLiteral(int i){
	return (-1 * i);
}

int emptyLiteral(){
	return 0;
}

//x <-> true  is equal to the empty clause
list<Clause> xEqualsTrue(int){
	list<Clause> clauses;
	assert(clauses.size() == 0);
	return clauses;
}

/* "x <-> false" is equivalent to "x * ~x" */
list<Clause> xEqualsFalse(int x){

	Clause cl1;
	cl1.literal0 = negateLiteral(x);
	cl1.literal1 = emptyLiteral();
	cl1.literal2 = emptyLiteral();

	Clause cl2;
	cl2.literal0 = x;
	cl2.literal1 = emptyLiteral();
	cl2.literal2 = emptyLiteral();

	list<Clause> clauses;
	clauses.push_back(cl1);
	clauses.push_back(cl2);
	return clauses;
}


//max: largest ID in the formula f
list<Clause> xEqualsFormula(int x, int max, const Formula* f){

	if (f->formulaType == INT || f->formulaType == LIT || f->formulaType == NOT || f->formulaType == FINAL){

		// if f is NUM/LIT/FINAL/NOT then x <-> f is equivalent to "(~x + f) * (x + f)"
		list<Clause> tmpClauses = f->toCNF();
		assert(tmpClauses.size() == 1);
		assert(tmpClauses.begin()->literal1 == emptyLiteral());
		assert(tmpClauses.begin()->literal2 == emptyLiteral());

		Clause cl1;
		cl1.literal0 = negateLiteral(x);
		cl1.literal1 = tmpClauses.begin()->literal0;
		cl1.literal2 = emptyLiteral();

		Clause cl2;
		cl2.literal0 = x;
		cl2.literal1 = negateLiteral(tmpClauses.begin()->literal0);
		cl2.literal2 = emptyLiteral();

		list<Clause> clauses;
		clauses.push_back(cl1);
		clauses.push_back(cl2);
		return clauses;
	}
	else{
		assert (f->formulaType == AND || f->formulaType == OR);
		// if f is an AND/OR-Formula the x <-> f is equivalent to "(~x + sMax1) * (x + ~sMax1) * (sMax1 <-> f)

		/* (~x + maxId) */
		Clause cl1;
		cl1.literal0 = negateLiteral(x);
		cl1.literal1 = max+1;
		cl1.literal2 = emptyLiteral();

		/* (x + ~maxId) */
		Clause cl2;
		cl2.literal0 = x;
		cl2.literal1 = negateLiteral(max+1);
		cl2.literal2 = emptyLiteral();

		list<Clause> clauses;
		clauses.push_back(cl1);
		clauses.push_back(cl2);

		/* (maxId <-> f) */
		list<Clause> tmp = f->toCNF(max+1, max+1);

		//append the clauses of (sMax1 <-> f)
		list<Clause>::iterator iter = clauses.begin();
		clauses.splice(iter, tmp);
		return clauses;
	}

}


// x <-> l*r  is equals to the Conjunction of the three Clauses: (~x,l,r) (x,~l) (x, ~r)
list<Clause> tripleAND(int x, int l, int r){
	list<Clause> clauses;

	Clause cl1;
	cl1.literal0 = negateLiteral(x);
	cl1.literal1 = l;
	cl1.literal2 = r;
	clauses.push_back(cl1);

	Clause cl2;
	cl2.literal0 = x;
	cl2.literal1 = negateLiteral(l);
	cl2.literal2 = emptyLiteral();
	clauses.push_back(cl2);

	Clause cl3;
	cl3.literal0 = x;
	cl3.literal1 = emptyLiteral();
	cl3.literal2 = negateLiteral(r);
	clauses.push_back(cl3);

	return clauses;
}

// x <-> l+r  is equals to the Conjunction of the three Clauses: (x,~l,~r) (~x,l) (~x, r)
list<Clause> tripleOR(int x, int l, int r){
	assert(x != emptyLiteral() && l != emptyLiteral() && r != emptyLiteral());
	list<Clause> clauses;

	Clause cl1;
	cl1.literal0 = x;
	cl1.literal1 = negateLiteral(l);
	cl1.literal2 = negateLiteral(r);
	clauses.push_back(cl1);

	Clause cl2;
	cl2.literal0 = negateLiteral(x);
	cl2.literal1 = l;
	cl2.literal2 = emptyLiteral();
	clauses.push_back(cl2);

	Clause cl3;
	cl3.literal0 = negateLiteral(x);
	cl3.literal1 = emptyLiteral();
	cl3.literal2 = r;
	clauses.push_back(cl3);

	return clauses;
}
