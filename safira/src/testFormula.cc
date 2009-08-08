#include "config.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <map>
#include "testFormula.h"
#include "Formula.h"
#include "helpers.h"

using namespace std;
extern map<string, unsigned int> label2id;
extern map<unsigned int, string> id2label;

void testFormulaClass(){
	initFormulaClass();

	string s = intToString(23);
	assert (s == "23");


	testTRUE();
	testFALSE();
	testFINAL();
	testNUM();
	testLIT();
	testNOT();
	testAND();
	testAND_NOT();
	testOR();
	testAND_OR_NOT();
}

void initFormulaClass(){
    label2id.clear();
    id2label.clear();

    id2label[0] = ""; //0 has a special meaning in minisat, therefore 0 cannot use as ID for labels
    label2id[""] = 0;

	id2label[1] = "true";
	label2id["true"] = 1;

	id2label[2] = "false";
	label2id["false"] = 2;

	id2label[3] = "final";
	label2id["final"] = 3;

	id2label[4] = "tau";
	label2id["tau"] = 4;

	id2label[5] = "A";
	label2id["A"] = 5;

	id2label[6] = "R";
	label2id["R"] = 6;

	id2label[7] = "I";
	label2id["I"] = 7;

	id2label[8] = "O";
	label2id["O"] = 8;

	cout << "initFormulaClass... \t passed" << endl;
}

void testTRUE(){
	Formula* f = new FormulaTrue();
	Formula* f1 = f->moveNegation();

	assert (f->formulaType == TRUE);
	assert (f->toString() == "true");
	assert (f1->toString() == "true");
	assert (f1->toStringCNF() == "true");
	assert (f->isSatisfiable(label2id.size()) == true);

	list<Clause> clauses = f1->toCNF();
	assert(clauses.size() == 0);

	/*
	 * g = NOT(true)
	 */
	Formula* g = new FormulaNOT(new FormulaTrue());
	Formula* g1 = g->moveNegation();

	assert (g->formulaType == NOT);
	assert(g->toString() == "~(true)");
	assert(g1->toString() == "false");
	assert(g1->toStringCNF() == "false");
	assert(g->isSatisfiable(label2id.size()) == false);

	clauses = g1->toCNF();
	assert(clauses.size() == 2);

	assert(clauses.begin()->literal0 == -2);
	assert(clauses.begin()->literal1 == emptyLiteral());
	assert(clauses.begin()->literal2 == emptyLiteral());

	list<Clause>::const_iterator iter = clauses.begin(); //first clause
	++iter;
	assert(iter->literal0 == 2);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == emptyLiteral());

	delete f1;
	delete f;
	delete g1;
	delete g;

	cout << "testTRUE... \t passed." << endl;
}

void testFALSE(){
	/*
	 * f = false
	 */
	Formula* f = new FormulaFalse();
	Formula* f1 = f->moveNegation();

	assert (f->formulaType == FALSE);
	assert (f->toString() == "false");
	assert (f1->toString() == "false");
	assert (f1->toStringCNF() == "false");
	assert (f->isSatisfiable(label2id.size()) == false);

	list<Clause> clauses = f1->toCNF();
	assert(clauses.size() == 2);

	assert(clauses.begin()->literal0 != emptyLiteral());
	assert(clauses.begin()->literal1 == emptyLiteral());
	assert(clauses.begin()->literal2 == emptyLiteral());

	list<Clause>::const_iterator iter = clauses.begin(); //first clause
	++iter;
	assert(iter->literal0 != emptyLiteral());
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == emptyLiteral());


	/*
	 * g = NOT(false)
	 */
	Formula* g = new FormulaNOT(new FormulaFalse());
	Formula* g1 = g->moveNegation();

	assert(g->formulaType == NOT);
	assert(g->toString() == "~(false)");
	assert(g1->toString() == "true");
	assert(g1->toStringCNF() == "true");
	assert(g->isSatisfiable(label2id.size()) == true);

	clauses = g1->toCNF();
    assert(clauses.size() == 0);

    delete f1;
    delete f;
    delete g1;
	delete g;

    cout << "testFALSE... \t passed." << endl;
}

void testFINAL(){
	Formula* f = new FormulaFinal();
	Formula* f1 = f->moveNegation();

	assert (f->formulaType == FINAL);
	assert (f->toString() == "final");
	assert (f1->toString() == "final");
	assert (f1->toStringCNF() == "final");
	assert (f->isSatisfiable(label2id.size()) == true);

	list<Clause> clauses = f1->toCNF();
	assert(clauses.size() == 1);

	assert(clauses.begin()->literal0 == 3);
	assert(clauses.begin()->literal1 == emptyLiteral());
	assert(clauses.begin()->literal2 == emptyLiteral());

	delete f1;
	delete f;

	cout << "testFINAL... \t passed." << endl;
}

void testNUM(){
	Formula* f = new FormulaNUM(42);
	assert (f->formulaType == INT);
	assert (f->toString() == "42");

	Formula* f1 = f->moveNegation();
	assert (f1->toString() == "42");
	assert (f1->toStringCNF() == "42");
	assert (f->isSatisfiable(label2id.size()) == true);

	list<Clause> clauses = f1->toCNF();
	assert(clauses.size() == 1);

	assert(clauses.begin()->literal0 == 42);
	assert(clauses.begin()->literal1 == emptyLiteral());
	assert(clauses.begin()->literal2 == emptyLiteral());

	delete f1;
	delete f;

	cout << "testINT... \t passed." << endl;
}

void testLIT(){
	Formula* f = new FormulaLit(label2id["I"]);
	Formula* f1 = f->moveNegation();

	assert (f->formulaType == LIT);
	assert (f->toString() == "I");
	assert (f1->toString() == "I");
	assert (f1->toStringCNF() == "7");
	assert (f->isSatisfiable(label2id.size()) == true);

	list<Clause> clauses = f1->toCNF();
	assert(clauses.size() == 1);

	assert(clauses.begin()->literal0 == 7);
	assert(clauses.begin()->literal1 == emptyLiteral());
	assert(clauses.begin()->literal2 == emptyLiteral());

	delete f1;
	delete f;

	cout << "testLIT... \t passed." << endl;
}


void testNOT(){

	/*simple NOT*/
	Formula* g = new FormulaLit(label2id["I"]);
	Formula* f = new FormulaNOT(g);
	Formula* f1 = f->moveNegation();

	assert (f->formulaType == NOT);
	assert (f->toString() == "~(I)");
	assert (f1->toString() == "~(I)");
	assert (f1->toStringCNF() == "-7");
	assert (f->isSatisfiable(label2id.size()) == true);

	list<Clause> clauses = f1->toCNF();
	assert(clauses.size() == 1);

	assert(clauses.begin()->literal0 == -7);
	assert(clauses.begin()->literal1 == emptyLiteral());
	assert(clauses.begin()->literal2 == emptyLiteral());

	delete f1;
	delete f;

	/*double NOT*/
	Formula *a = new FormulaLit(label2id["R"]);
	Formula *b = new FormulaNOT(a);
	Formula *c = new FormulaNOT(b);
	Formula *c1 = c->moveNegation();


	assert (c->toString() == "~(~(R))");
	assert (c1->toString() == "R");
	assert (c1->toStringCNF() == "6");
	assert (c->isSatisfiable(label2id.size()) == true);

	clauses = c1->toCNF();
	assert(clauses.size() == 1);

	assert(clauses.begin()->literal0 == 6);
	assert(clauses.begin()->literal1 == emptyLiteral());
	assert(clauses.begin()->literal2 == emptyLiteral());

	delete c1;
	delete c;

	cout << "testNOT... \t passed." << endl;
}

void testAND(){
	unsigned int maxId = label2id.size()-1;

	/**************
	 * simple AND: (I*R)
	 **************/
	Formula *a = new FormulaLit(label2id["I"]);
	Formula *b = new FormulaLit(label2id["R"]);
	Formula *c = new FormulaAND(a,b);
	Formula *c1 = c->moveNegation();

	assert (c->formulaType == AND);
	assert (c->toString() == "(I * R)");
	assert (c1->toString() == "(I * R)");
	assert (c1->toStringCNF(maxId+1,maxId+1) == "(9<->7*6)");
	assert (c->isSatisfiable(label2id.size()) == true);

	list<Clause> clauses = c1->toCNF(maxId+1,maxId+1);
	assert(clauses.size() == 3);

	list<Clause>::const_iterator iter = clauses.begin(); //first clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 7);
	assert(iter->literal2 == 6);
	++iter; //second clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -7);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == -6);

	delete c1;
	delete c;

	/*****************
	 * complex AND 1:  (I*R)*A
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b);           //I*R
	Formula *d = new FormulaLit(label2id["A"]);
	Formula *e = new FormulaAND(c, d); //(I*R)*A
	Formula *e1 = e->moveNegation();

	assert (e->formulaType == AND);
	assert (e->toString() == "((I * R) * A)");
	assert (e1->toString() == "((I * R) * A)");
	assert (e1->toStringCNF(maxId+1,maxId+1) == "(9<->10*5) * (10<->7*6)");
	assert (e->isSatisfiable(label2id.size()) == true);

	clauses = e1->toCNF(maxId+1,maxId+1);

	assert(clauses.size() == 6);

	iter = clauses.begin(); //first clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == 7);
	assert(iter->literal2 == 6);
	++iter; //second clause
	assert(iter->literal0 == 10);
	assert(iter->literal1 == -7);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == 10);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == -6);
	++iter; //fourth clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 10);
	assert(iter->literal2 == 5);
	++iter; //5. clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -10);
	assert(iter->literal2 == emptyLiteral());
	++iter; //1. clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == -5);

	delete e1;
	delete e;

	/*****************
	 * complex AND 2:  O*(I*R)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b); //I*R
	d = new FormulaLit(label2id["O"]);
	e = new FormulaAND(d, c);
	e1 = e->moveNegation();

	assert (e->formulaType == AND);
	assert (e->toString() == "(O * (I * R))");
	assert (e1->toString() == "(O * (I * R))");
	assert (e1->toStringCNF(maxId+1,maxId+1) == "(9<->8*10) * (10<->7*6)");
	assert (e->isSatisfiable(label2id.size()) == true);

	clauses = e1->toCNF(maxId+1,maxId+1);

	assert(clauses.size() == 6);

	//cout << "AUSGABE: " << (e->moveNegation()->toCNF(maxId+1,maxId+1)).begin()->literal0 << endl;
	iter = clauses.begin(); //first clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == 7);
	assert(iter->literal2 == 6);
	++iter; //second clause
	assert(iter->literal0 == 10);
	assert(iter->literal1 == -7);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == 10);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == -6);
	++iter; //fourth clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 8);
	assert(iter->literal2 == 10);
	++iter; //5. clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -8);
	assert(iter->literal2 == emptyLiteral());
	++iter; //1. clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == -10);

	delete e1;
	delete e;

	/*****************
	 * complex AND 2:  (O*A)*(I*R)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b); //I*R
	d = new FormulaLit(label2id["O"]);
	e = new FormulaLit(label2id["A"]);
	Formula *f = new FormulaAND(d,e);
	Formula *h = new FormulaAND(f,c);
	Formula *h1 = h->moveNegation();

	assert (h->formulaType == AND);
	assert (h->toString() == "((O * A) * (I * R))");
	assert (h1->toString() == "((O * A) * (I * R))");
	assert (h1->toStringCNF(maxId+1,maxId+1) == "(9<->10*11) * (10<->8*5) * (11<->7*6)");
	assert (h->isSatisfiable(label2id.size()) == true);

	clauses = h1->toCNF(maxId+1,maxId+1);

	assert(clauses.size() == 9);

	iter = clauses.begin(); //1. clause
	assert(iter->literal0 == -11);
	assert(iter->literal1 == 7);
	assert(iter->literal2 == 6);
	++iter; //second clause
	assert(iter->literal0 == 11);
	assert(iter->literal1 == -7);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == 11);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == -6);
	++iter; //4. clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == 8);
	assert(iter->literal2 == 5);
	++iter; //5. clause
	assert(iter->literal0 == 10);
	assert(iter->literal1 == -8);
	assert(iter->literal2 == emptyLiteral());
	++iter; //6. clause
	assert(iter->literal0 == 10);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == -5);
	++iter; //7. clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 10);
	assert(iter->literal2 == 11);
	++iter; //8. clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -10);
	assert(iter->literal2 == emptyLiteral());
	++iter; //9. clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == -11);

	delete h1;
	delete h;
	cout << "testAND... \t passed." << endl;

}

void testAND_NOT(){
	unsigned int maxId = label2id.size()-1;

	/**************
	 * ~(I*R)
	 **************/
	Formula *a = new FormulaLit(label2id["I"]);
	Formula *b = new FormulaLit(label2id["R"]);
	Formula *c = new FormulaAND(a,b);
	Formula *d = new FormulaNOT(c);
	Formula *d1 = d->moveNegation();

	assert (d->formulaType == NOT);
	assert (d->toString() == "~((I * R))");
	assert (d1->toString() == "(~(I) + ~(R))");
	assert (d1->toStringCNF(maxId+1,maxId+1) == "(9<->-7+-6)");
	assert (d->isSatisfiable(label2id.size()) == true);

	list<Clause> clauses = d1->toCNF(maxId+1,maxId+1);
	assert(clauses.size() == 3);

	list<Clause>::const_iterator iter = clauses.begin(); //first clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == 7);
	assert(iter->literal2 == 6);
	++iter; //second clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == -7);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == -6);

	delete d1;
	delete d;

	/**************
	 * ~(~(I*R))
	 **************/
	Formula *e = new FormulaLit(label2id["I"]);
	Formula *f = new FormulaLit(label2id["R"]);
	Formula *g = new FormulaAND(e,f);
	Formula *h = new FormulaNOT(g);
	Formula *k = new FormulaNOT(h);
	Formula *k1 = k->moveNegation();

	assert (k->formulaType == NOT);
	assert (k->toString() == "~(~((I * R)))");
	assert (k1->toString() == "(I * R)");
	assert (k1->toStringCNF(maxId+1,maxId+1) == "(9<->7*6)");
	assert (k->isSatisfiable(label2id.size()) == true);

	clauses = k1->toCNF(maxId+1,maxId+1);
	assert(clauses.size() == 3);

	iter = clauses.begin(); //first clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 7);
	assert(iter->literal2 == 6);
	++iter; //second clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -7);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == -6);

	delete k1;
	delete k;

	cout << "testAND_NOT... \t passed."<< endl;
}

void testOR(){
	unsigned int maxId = label2id.size()-1;

	/**************
	 * simple OR: (I+R)
	 **************/
	Formula *a = new FormulaLit(label2id["I"]);
	Formula *b = new FormulaLit(label2id["R"]);
	Formula *c = new FormulaOR(a,b); //I+R
	Formula *c1 = c->moveNegation();

	assert (c->formulaType == OR);
	assert (c->toString() == "(I + R)");
	assert (c1->toString() == "(I + R)");
	assert (c1->toStringCNF(maxId+1,maxId+1) == "(9<->7+6)");

	list<Clause> clauses = c1->toCNF(maxId+1,maxId+1);
	assert(clauses.size() == 3);

	list<Clause>::const_iterator iter = clauses.begin(); //first clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -7);
	assert(iter->literal2 == -6);
	++iter; //second clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 7);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == 6);

	delete c1;
	delete c;

	/*****************
	 * complex AND 1:  (I+R)+A
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b); //I+R
	Formula *d = new FormulaLit(label2id["A"]);
	Formula *e = new FormulaOR(c, d);
	Formula *e1 = e->moveNegation();

	assert (e->formulaType == OR);
	assert (e->toString() == "((I + R) + A)");
	assert (e1->toString() == "((I + R) + A)");
	assert (e1->toStringCNF(maxId+1,maxId+1) == "(9<->10+5) * (10<->7+6)");

	clauses = e1->toCNF(maxId+1,maxId+1);

	assert(clauses.size() == 6);
    //cout << "AUSGABE: " << (e->moveNegation()->toCNF(maxId+1,maxId+1)).begin()->literal1 << endl;
	iter = clauses.begin(); //first clause
	assert(iter->literal0 == 10);
	assert(iter->literal1 == -7);
	assert(iter->literal2 == -6);
	++iter; //second clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == 7);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == 6);
	++iter; //fourth clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -10);
	assert(iter->literal2 == -5);
	++iter; //5. clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 10);
	assert(iter->literal2 == emptyLiteral());
	++iter; //6. clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == 5);

	delete e1;
	delete e;

	/*****************
	 * complex OR 2:  O+(I+R)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b); //I+R
	d = new FormulaLit(label2id["O"]);
	e = new FormulaOR(d, c);
	e1 = e->moveNegation();

	assert (e->formulaType == OR);
	assert (e->toString() == "(O + (I + R))");
	assert (e1->toString() == "(O + (I + R))");
	assert (e1->toStringCNF(maxId+1,maxId+1) == "(9<->8+10) * (10<->7+6)");

	clauses = e1->toCNF(maxId+1,maxId+1);

	assert(clauses.size() == 6);

	iter = clauses.begin(); //first clause
	assert(iter->literal0 == 10);
	assert(iter->literal1 == -7);
	assert(iter->literal2 == -6);
	++iter; //second clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == 7);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == 6);
	++iter; //fourth clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -8);
	assert(iter->literal2 == -10);
	++iter; //5. clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 8);
	assert(iter->literal2 == emptyLiteral());
	++iter; //1. clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == 10);

	delete e1;
	delete e;

	/*****************
	 * complex AND 2:  (O+A)+(I+R)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b); //I+R
	d = new FormulaLit(label2id["O"]);
	e = new FormulaLit(label2id["A"]);
	Formula *f = new FormulaOR(d,e);
	Formula *h = new FormulaOR(f,c);
	Formula *h1 = h->moveNegation();

	assert (h->formulaType == OR);
	assert (h->toString() == "((O + A) + (I + R))");
	assert (h1->toString() == "((O + A) + (I + R))");
	assert (h1->toStringCNF(maxId+1,maxId+1) == "(9<->10+11) * (10<->8+5) * (11<->7+6)");

	clauses = h1->toCNF(maxId+1,maxId+1);

	assert(clauses.size() == 9);

	iter = clauses.begin(); //1. clause
	assert(iter->literal0 == 11);
	assert(iter->literal1 == -7);
	assert(iter->literal2 == -6);
	++iter; //second clause
	assert(iter->literal0 == -11);
	assert(iter->literal1 == 7);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == -11);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == 6);
	++iter; //4. clause
	assert(iter->literal0 == 10);
	assert(iter->literal1 == -8);
	assert(iter->literal2 == -5);
	++iter; //5. clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == 8);
	assert(iter->literal2 == emptyLiteral());
	++iter; //6. clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == 5);
	++iter; //7. clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -10);
	assert(iter->literal2 == -11);
	++iter; //8. clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 10);
	assert(iter->literal2 == emptyLiteral());
	++iter; //9. clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == 11);

	cout << "testOR... \t passed." << endl;
	delete h1;
	delete h;

}

void testAND_OR_NOT(){
	/*
	 * ~((A * ~R) + ~~A)
	 */
	unsigned int maxId = label2id.size()-1;
	Formula *a = new FormulaLit(label2id["A"]);
	Formula *b = new FormulaLit(label2id["R"]);
	Formula *c = new FormulaNOT(b);
	Formula *d = new FormulaAND(a,c); //A*~R
	Formula *e = new FormulaLit(label2id["A"]);
	Formula *f = new FormulaNOT(e);
	Formula *g = new FormulaNOT(f); //~~A
	Formula *h = new FormulaOR(d,g);
	Formula *i = new FormulaNOT(h); //~((A * ~R) + ~~A)
	Formula *i1 = i->moveNegation();

	assert (i->formulaType == NOT);
	assert (i->toString() == "~(((A * ~(R)) + ~(~(A))))");
	assert (i1->toString() == "((~(A) + R) * ~(A))");
	assert (i1->toStringCNF(maxId+1,maxId+1) == "(9<->10*-5) * (10<->-5+6)");

	list<Clause> clauses = i1->toCNF(maxId+1,maxId+1);
	assert(clauses.size() == 6);

	list<Clause>::const_iterator iter = clauses.begin(); //first clause
	assert(iter->literal0 == 10);
	assert(iter->literal1 == 5);
	assert(iter->literal2 == -6);
	++iter; //second clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == -5);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == -10);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == 6);
	++iter; //4. clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 10);
	assert(iter->literal2 == -5);
	++iter; //second clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -10);
	assert(iter->literal2 == emptyLiteral());
	++iter; //third clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == emptyLiteral());
	assert(iter->literal2 == 5);

	delete i1;
	delete i;

	/*
	 * (final + true) * ~false
	 */

	Formula *k = new FormulaFinal();
	Formula *l = new FormulaTrue();
	Formula *m = new FormulaOR(k,l); //final+true
	Formula *n = new FormulaFalse();
	Formula *o = new FormulaNOT(n); //~false
	Formula *p = new FormulaAND(m,o);
	Formula *p1 = p->moveNegation();

	assert (p->toString() == "((final + true) * ~(false))");
	assert (p1->toString() == "((final + true) * true)");
	assert (p1->toStringCNF(maxId+1,maxId+1) == "(9<->10*true) * (10<->final+true)");

	clauses = p1->toCNF(maxId+1,maxId+1);
	assert(clauses.size() == 2);

	iter = clauses.begin(); //first clause
	assert(iter->literal0 == -9);
	assert(iter->literal1 == 10);
	assert(iter->literal2 == emptyLiteral());
	++iter; //second clause
	assert(iter->literal0 == 9);
	assert(iter->literal1 == -10);
	assert(iter->literal2 == emptyLiteral());

	delete p1;
	delete p;
	
	//false * f/A
	//f/A* false
	//false*false
	
	// true * f/A
	// f/A * true
	// true * true
	
        // Analog für OR
        
        // clauseToString
        // test isSatisfiable für true
        // test isSatisfiable für false
        // test isSatisfiable für x + y ...
	
	cout << "testAND_OR_NOT... \t passed." << endl;
}


