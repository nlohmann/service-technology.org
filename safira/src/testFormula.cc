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
#include <cstdio>
#include <map>
#include <set>
#include "testFormula.h"
#include "Formula.h"

#include "cmdline.h"

/// the command line parameters
extern gengetopt_args_info args_info;

using namespace std;
extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;

void testFormulaClass(){

	string s = intToString(23);
	assert (s == "23");

	testTRUE();
	testFALSE();
	testFINAL();
	testNUM();
	testLIT();
	testNOT();
	testAND();
	testOR();
	testAND_OR_NOT();

	testSat();

}

void testSat(){
    label2id.clear();
    id2label.clear();

    id2label[0] = "";
    label2id[""] = 0;

	id2label[1] = "true";
	label2id["true"] = 1;

	id2label[2] = "false";
	label2id["false"] = 2;

	id2label[3] = "final";
	label2id["final"] = 3;

	id2label[4] = "Z";
	label2id["Z"] = 4;

	// ~Z
	Formula * h = new FormulaNOT(new FormulaLit(4)); //~Z
	assert (h->toString() == "~(Z)");
	assert (h->isSatisfiable() == true);

	list<Clause> clauses = h->calculateCNF();
	assert(clauses.size() == 3);

	list<Clause>::const_iterator iter = clauses.begin(); assert(checkClause(*iter, 5,0,0));
	++iter; assert(checkClause(*iter, -5,-4,0));
	++iter; assert(checkClause(*iter, 5, 4,0));


	// (~Z * Z)
	Formula * f = new FormulaAND(new FormulaNOT(new FormulaLit(4)), new FormulaLit(4)); // ~Z*Z
	assert (f->toString() == "(~(Z) * Z)");
	assert (f->isSatisfiable() == false);

	clauses = f->calculateCNF();
	assert(clauses.size() == 4);

	iter = clauses.begin(); assert(checkClause(*iter, 5,0,0));
	++iter; assert(checkClause(*iter, 5, 4, -4));
	++iter; assert(checkClause(*iter,  -5, -4, 0));
	++iter; assert(checkClause(*iter,  -5, 0,4));


	// (Z * ~Z)
	Formula * g = new FormulaAND(new FormulaLit(4), new FormulaNOT(new FormulaLit(4))); // Z*~Z
	assert (g->toString() == "(Z * ~(Z))");
	assert (g->isSatisfiable() == false);

	clauses = g->calculateCNF();
	assert(clauses.size() == 4);

	iter = clauses.begin(); assert(checkClause(*iter, 5,0,0));
	++iter; assert(checkClause(*iter,  5, -4, 4));
	++iter; assert(checkClause(*iter,  -5, 4, 0));
	++iter; assert(checkClause(*iter,  -5, 0, -4));

	delete h;
	delete f;
	delete g;
}

void initFormulaClass(){
    label2id.clear();
    id2label.clear();
    inout.clear();

    id2label[0] = ""; //0 has a special meaning in minisat, therefore 0 cannot use as ID for labels
    label2id[""] = 0;
    inout[0] = ' ';

	id2label[1] = "final";
	label2id["final"] = 1;
	inout[1] = ' ';

	id2label[2] = "tau";
	label2id["tau"] = 2;
	inout[2] = ' ';

	id2label[3] = "A";
	label2id["A"] = 3;
	inout[3] = '?';

	id2label[4] = "R";
	label2id["R"] = 4;
	inout[4] = '?';

	id2label[5] = "I";
	label2id["I"] = 5;
	inout[5] = '!';

	id2label[6] = "O";
	label2id["O"] = 6;
	inout[6] = '!';

	//cout << "initFormulaClass... \t passed" << endl;
}

bool checkClause(const Clause& cl, const int l0, const int l1, const int l2){

	return ((cl.literal0 == l0) && (cl.literal1 == l1) && (cl.literal2 == l2));
}

//void printClause(const Clause& cl){
//	cout << "Clausel: " << cl.literal0 << ", " << cl.literal1 << ", " << cl.literal2 << endl;
//}

void testTRUE(){
	cout << "testTRUE... ";
	initFormulaClass();

	/****************
	 * f = TRUE
	 ****************/
	Formula* f = new FormulaTrue();
	Formula* f1 = f->moveNegation();

	assert (f->formulaType == TRUE);
	assert (f->toString() == "true");
	assert (f1->toString() == "true");
	assert (f->isSatisfiable() == true);

	Formula* f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	list<Clause> clauses = f->calculateCNF();
	assert(clauses.size() == 2);

	list<Clause>::const_iterator iter = clauses.begin(); //first clause
	assert(checkClause(*iter, 7,0,0));

	++iter; //second clause
	assert(checkClause(*iter, 7,0,0));


	/******************
	 * g = NOT(true)
	 ******************/
	Formula* g = new FormulaNOT(new FormulaTrue());
	Formula* g1 = g->moveNegation();

	assert (g->formulaType == NOT);
	assert(g->toString() == "~(true)");
	assert(g1->toString() == "false");
	assert(g->isSatisfiable() == false);

	Formula* g_clone = g->getCopy();
	assert(g_clone->toString() == g->toString());

	clauses = g1->calculateCNF();
	assert(clauses.size() == 2);

	iter = clauses.begin(); //first clause
	assert(checkClause(*iter, 7,0,0));

	++iter;	//second clause
	assert(checkClause(*iter, -7,0,0));


	delete f;
	delete f1;
	delete g1;
	delete g;
	delete f_clone;
	delete g_clone;

	cout << "\t passed." << endl;
}

void testFALSE(){
	cout << "testFALSE... ";
	initFormulaClass();

	/**************
	 * f = false
	 **************/
	Formula* f = new FormulaFalse();
	Formula* f1 = f->moveNegation();

	assert (f->formulaType == FALSE);
	assert (f->toString() == "false");
	assert (f1->toString() == "false");
	assert (f->isSatisfiable() == false);

	Formula* f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	list<Clause> clauses = f1->calculateCNF();
	assert(clauses.size() == 2);

	list<Clause>::const_iterator iter = clauses.begin(); //first clause
	assert(checkClause(*iter, 7,0,0));

	++iter;	//second clause
	assert(checkClause(*iter, -7,0,0));

	/*******************
	 * g = NOT(false)
	 *******************/
	Formula* g = new FormulaNOT(new FormulaFalse());
	Formula* g1 = g->moveNegation();

	assert(g->formulaType == NOT);
	assert(g->toString() == "~(false)");
	assert(g1->toString() == "true");
	assert(g->isSatisfiable() == true);

	Formula* g_clone = g->getCopy();
	assert(g_clone->toString() == g->toString());

	clauses = g1->calculateCNF();
    assert(clauses.size() == 2);

	iter = clauses.begin(); //first clause
	assert(checkClause(*iter, 7,0,0));

	++iter; //second clause
	assert(checkClause(*iter, 7,0,0));

    delete f1;
    delete f;
    delete g1;
	delete g;
	delete f_clone;
	delete g_clone;

    cout << "\t passed." << endl;
}

void testFINAL(){
	cout << "testFINAL... ";
	initFormulaClass();

	Formula* f = new FormulaFinal();
	Formula* f1 = f->moveNegation();

	assert (f->formulaType == FINAL);
	assert (f->toString() == "final");
	assert (f1->toString() == "final");
	assert (f->isSatisfiable() == true);

	Formula* f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	list<Clause> clauses = f->calculateCNF();
	assert(clauses.size() == 3);

	list<Clause>::const_iterator iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter;	assert(checkClause(*iter, -7,1,0));
	++iter; assert(checkClause(*iter, 7,-1,0));

	delete f1;
	delete f;
	delete f_clone;

	cout << "\t passed." << endl;
}

void testNUM(){
	cout << "testINT... ";
	initFormulaClass();

	Formula* f = new FormulaNUM(42);
	assert (f->toString() == "42");

	Formula* f1 = f->moveNegation();
	assert (f->isSatisfiable() == true);

	Formula* f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	list<Clause> clauses = f->calculateCNF();
	assert(clauses.size() == 3);

	list<Clause>::const_iterator iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,42,0));
	++iter; assert(checkClause(*iter, 7,-42,0));

	delete f1;
	delete f;
	delete f_clone;

	Formula* g = new FormulaNOT(new FormulaNUM(42)); //-42
	assert (g->formulaType == NOT);
	assert (g->toString() == "~(42)");

	Formula* g1 = g->moveNegation();
	assert (g1->toString() == "~(42)");
	assert (g->isSatisfiable() == true);

	Formula* g_clone = g->getCopy();
	assert(g_clone->toString() == g->toString());

	clauses = g->calculateCNF();
	assert(clauses.size() == 3);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,-42,0));
	++iter; assert(checkClause(*iter, 7,42,0));

	delete g1;
	delete g;
	delete g_clone;

	Formula* h = new FormulaNOT(new FormulaNOT(new FormulaNUM(42))); //--42
	assert (h->formulaType == NOT);
	assert (h->toString() == "~(~(42))");

	Formula* h1 = h->moveNegation();
	assert (h1->toString() == "42");
	assert (h->isSatisfiable() == true);

	Formula* h_clone = h->getCopy();
	assert(h_clone->toString() == h->toString());

	clauses = h->calculateCNF();
	assert(clauses.size() == 3);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,42,0));
	++iter; assert(checkClause(*iter, 7,-42,0));

	delete h1;
	delete h;
	delete h_clone;

	cout << "\t passed." << endl;
}

void testLIT(){
	cout << "testLIT... ";
	initFormulaClass();

	Formula* f = new FormulaLit(label2id["I"]);
	Formula* f1 = f->moveNegation();

	assert (f->formulaType == LIT);
	assert (f->toString() == "I");
	assert (f1->toString() == "I");
	assert (f->isSatisfiable() == true);

	Formula* f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	list<Clause> clauses = f->calculateCNF();
	assert(clauses.size() == 3);

	list<Clause>::const_iterator iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,5,0));
	++iter; assert(checkClause(*iter, 7,-5,0));

	delete f1;
	delete f;
	delete f_clone;

	cout << "\t passed." << endl;
}

void testNOT(){
	cout << "testNOT... ";
	initFormulaClass();

	/*simple NOT: ~I   */
	Formula* g = new FormulaLit(label2id["I"]);
	Formula* f = new FormulaNOT(g);
	Formula* f1 = f->moveNegation();

	assert (f->formulaType == NOT);
	assert (f->toString() == "~(I)");
	assert (f1->toString() == "~(I)");
	assert (f->isSatisfiable() == true);

	Formula* f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	list<Clause> clauses = f->calculateCNF();
	assert(clauses.size() == 3);

	list<Clause>::const_iterator iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,-5,0));
	++iter;	assert(checkClause(*iter, 7,5,0));

	delete f1;
	delete f;
	delete f_clone;

	/*double NOT: ~(~R)   */
	Formula *a = new FormulaLit(label2id["R"]);
	Formula *b = new FormulaNOT(a);
	Formula *c = new FormulaNOT(b);
	Formula *c1 = c->moveNegation();

	Formula* c_clone = c->getCopy();
	assert(c_clone->toString() == c->toString());

	assert (c->toString() == "~(~(R))");
	assert (c1->toString() == "R");
	assert (c->isSatisfiable() == true);

	clauses = c->calculateCNF();
	assert(clauses.size() == 3);

	iter = clauses.begin();	assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,4,0));
	++iter; assert(checkClause(*iter, 7,-4,0));

	delete c1;
	delete c;
	delete c_clone;

	cout << "\t passed." << endl;
}

void testAND(){
	cout << "testAND... ";
	initFormulaClass();

	/**************
	 * simple AND 1.1: (I*R)   (CASE1.1)
	 **************/
	Formula *a = new FormulaLit(label2id["I"]);
	Formula *b = new FormulaLit(label2id["R"]);
	Formula *c = new FormulaAND(a,b);
	Formula *c1 = c->moveNegation();

	assert (c->formulaType == AND);
	assert (c->toString() == "(I * R)");
	assert (c1->toString() == "(I * R)");
	assert (c->isSatisfiable() == true);

	Formula* c_clone = c->getCopy();
	assert(c_clone->toString() == c->toString());

	list<Clause> clauses = c->calculateCNF();
	assert(clauses.size() == 4);

	list<Clause>::const_iterator iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, 7,-5,-4));
	++iter; assert(checkClause(*iter, -7,5,0));
	++iter; assert(checkClause(*iter, -7,0,4));

	delete c1;
	delete c;
	delete c_clone;


	/*****************
	 * complex AND 1.2:  O*(I*R)   (CASE1.2 and CASE1.1)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b); //I*R
	Formula *d = new FormulaLit(label2id["O"]);
	Formula *e = new FormulaAND(d, c);
	Formula *e1 = e->moveNegation();

	assert (e->formulaType == AND);
	assert (e->toString() == "(O * (I * R))");
	assert (e1->toString() == "(O * (I * R))");
	assert (e->isSatisfiable() == true);

	Formula *e_clone = e->getCopy();
	assert(e_clone->toString() == e->toString());

	clauses = e->calculateCNF();
	assert(clauses.size() == 7);

	iter = clauses.begin();	assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, 7,-6,-8));
	++iter; assert(checkClause(*iter, -7,6,0));
	++iter; assert(checkClause(*iter, -7,0,8));
	++iter; assert(checkClause(*iter, 8,-5,-4));
	++iter; assert(checkClause(*iter, -8,5,0));
	++iter; assert(checkClause(*iter, -8,0,4));

	delete e1;
	delete e;
	delete e_clone;


	/*****************
	 * complex AND 1.3:  (I*R)*A    (CASE1.3 and CASE1.1)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b);           //I*R
	d = new FormulaLit(label2id["A"]);
	e = new FormulaAND(c, d); //(I*R)*A
	e1 = e->moveNegation();

	assert (e->formulaType == AND);
	assert (e->toString() == "((I * R) * A)");
	assert (e1->toString() == "((I * R) * A)");
	assert (e->isSatisfiable() == true);

	e_clone = e->getCopy();
	assert(e_clone->toString() == e->toString());

	clauses = e->calculateCNF();
	assert(clauses.size() == 7);

	iter = clauses.begin();	assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, 7,-8,-3));
	++iter; assert(checkClause(*iter, -7,8,0));
	++iter; assert(checkClause(*iter, -7,0,3));
	++iter; assert(checkClause(*iter, 8,-5,-4));
	++iter; assert(checkClause(*iter, -8,5,0));
	++iter; assert(checkClause(*iter, -8,0,4));

	delete e1;
	delete e;
	delete e_clone;


	/*****************
	 * complex AND 1.4:  (O*A)*(I*R)  (CASE1.4 and CASE1.1)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b);          // I*R
	d = new FormulaLit(label2id["O"]);
	e = new FormulaLit(label2id["A"]);
	Formula *f = new FormulaAND(d,e); // O*A
	Formula *h = new FormulaAND(f,c); // (O*A)*(I*R)
	Formula *h1 = h->moveNegation();

	assert (h->formulaType == AND);
	assert (h->toString() == "((O * A) * (I * R))");
	assert (h1->toString() == "((O * A) * (I * R))");
	assert (h->isSatisfiable() == true);

	Formula* h_clone = h->getCopy();
	assert(h_clone->toString() == h->toString());

	clauses = h->calculateCNF();
	assert(clauses.size() == 10);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, 7,-8,-9));
	++iter; assert(checkClause(*iter, -7,8,0));
	++iter; assert(checkClause(*iter, -7,0,9));
	++iter; assert(checkClause(*iter, 8,-6,-3));
	++iter; assert(checkClause(*iter, -8,6,0));
	++iter; assert(checkClause(*iter, -8,0,3));
	++iter; assert(checkClause(*iter, 9,-5,-4));
	++iter; assert(checkClause(*iter, -9,5,0));
	++iter; assert(checkClause(*iter, -9,0,4));

	delete h1;
	delete h;
	delete h_clone;

	/*****************
	 * complex AND2.1 :  false*(I*R)    (CASE2.1 and CASE1.1)
	 *****************/

	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b);           //I*R
	d = new FormulaFalse();
	f = new FormulaAND(d,c);

	Formula *f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "(false * (I * R))");
	assert (f1->toString() == "(false * (I * R))");
	assert (f->isSatisfiable() == false);

	Formula* f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	clauses = f->calculateCNF();
	assert(clauses.size() == 2);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,0,0));

	delete f1;
	delete f;
	delete f_clone;

	/*****************
	 * complex AND2.1 :  (I*R)*false    (CASE2.1 and CASE1.1)
	 *****************/

	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b);           //I*R
	d = new FormulaFalse();
	f = new FormulaAND(c,d);

	f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "((I * R) * false)");
	assert (f1->toString() == "((I * R) * false)");
	assert (f->isSatisfiable() == false);

	f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	clauses = f->calculateCNF();
	assert(clauses.size() == 2);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,0,0));

	delete f1;
	delete f;
	delete f_clone;

	/*****************
	 * complex AND2.1 :  true*false (Case2.1)
	 *****************/

	a = new FormulaTrue();
	b = new FormulaFalse();
	f = new FormulaAND(a,b); //true*false

	f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "(true * false)");
	assert (f1->toString() == "(true * false)");
	assert (f->isSatisfiable() == false);

	f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	clauses = f->calculateCNF();
	assert(clauses.size() == 2);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,0,0));

	delete f1;
	delete f;
	delete f_clone;

	/*****************
	 * complex AND2.2 :  true*true    (CASE2.2 and CASE1.1)
	 *****************/

	a = new FormulaTrue();
	b = new FormulaTrue();
	f = new FormulaAND(a,b); //true*true

	f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "(true * true)");
	assert (f1->toString() == "(true * true)");
	assert (f->isSatisfiable() == true);

	f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());
	clauses = f->calculateCNF();
	assert(clauses.size() == 2);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, 7,0,0));

	delete f1;
	delete f;
	delete f_clone;


	/*****************
	 * complex AND2.3 :  true*A    (CASE2.3)
	 *****************/

	a = new FormulaTrue();
	b = new FormulaLit(label2id["A"]);
	f = new FormulaAND(a,b); //true*A

	f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "(true * A)");
	assert (f1->toString() == "(true * A)");
	assert (f->isSatisfiable() == true);

	f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	clauses = f->calculateCNF();
	assert(clauses.size() == 3);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,3,0));
	++iter; assert(checkClause(*iter, 7,-3,0));

	delete f1;
	delete f;
	delete f_clone;

	/*****************
	 * complex AND2.4 :  true*(A*I)    (CASE2.3, CASE 1.1)
	 *****************/

	a = new FormulaTrue();
	b = new FormulaLit(label2id["A"]);
	c = new FormulaLit(label2id["I"]);
	f = new FormulaAND(a, new FormulaAND(b,c)); //true*(A*I)

	f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "(true * (A * I))");
	assert (f1->toString() == "(true * (A * I))");
	assert (f->isSatisfiable() == true);

	f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	clauses = f->calculateCNF();
	assert(clauses.size() == 6);

	iter = clauses.begin();  assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,8,0));
	++iter; assert(checkClause(*iter, 7,-8,0));
	++iter; assert(checkClause(*iter, 8,-3,-5));
	++iter; assert(checkClause(*iter, -8,3,0));
	++iter; assert(checkClause(*iter, -8,0,5));

	delete f1;
	delete f;
	delete f_clone;



	/*****************
	 * complex AND2.5 :  A*true    (CASE2.5)
	 *****************/

	a = new FormulaTrue();
	b = new FormulaLit(label2id["A"]);
	f = new FormulaAND(b, a); //true*A

	f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "(A * true)");
	assert (f1->toString() == "(A * true)");
	assert (f->isSatisfiable() == true);

	f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	clauses = f->calculateCNF();
	assert(clauses.size() == 3);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,3,0));
	++iter; assert(checkClause(*iter, 7,-3,0));

	delete f1;
	delete f;
	delete f_clone;


	/*****************
	 * complex AND2.6 :  (A*I)*true    (CASE2.6 CASE1.1)
	 *****************/

	a = new FormulaTrue();
	b = new FormulaLit(label2id["A"]);
	c = new FormulaLit(label2id["I"]);
	f = new FormulaAND(new FormulaAND(b,c), a); // (A*I)*true

	f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "((A * I) * true)");
	assert (f1->toString() == "((A * I) * true)");
	assert (f->isSatisfiable() == true);

	f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	clauses = f->calculateCNF();
	assert(clauses.size() == 6);

	iter = clauses.begin();  assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,8,0));
	++iter; assert(checkClause(*iter, 7,-8,0));
	++iter; assert(checkClause(*iter, 8,-3,-5));
	++iter; assert(checkClause(*iter, -8,3,0));
	++iter; assert(checkClause(*iter, -8,0,5));

	delete f1;
	delete f;
	delete f_clone;

	/*****************
	 * long AND:  ((A*I)*(I*R))*((O*R)*(O*I))
	 *****************/
	a = new FormulaLit(label2id["A"]);
	b = new FormulaLit(label2id["I"]);
	c = new FormulaLit(label2id["I"]);
	d = new FormulaLit(label2id["R"]);
	Formula *v = new FormulaLit(label2id["O"]);
	Formula *x = new FormulaLit(label2id["R"]);
	Formula *y = new FormulaLit(label2id["O"]);
	Formula *z = new FormulaLit(label2id["I"]);

	Formula *n = new FormulaAND(new FormulaAND(a,b), new FormulaAND(c,d));
	Formula *m = new FormulaAND(new FormulaAND(v,x), new FormulaAND(y,z));

	f = new FormulaAND(n,m);
	f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "(((A * I) * (I * R)) * ((O * R) * (O * I)))");
	assert (f1->toString() == "(((A * I) * (I * R)) * ((O * R) * (O * I)))");
	assert (f->isSatisfiable() == true);

	f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	clauses = f->calculateCNF();
	assert(clauses.size() == 22);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, 7,-8,-11));
	++iter; assert(checkClause(*iter, -7,8,0));
	++iter; assert(checkClause(*iter, -7,0,11));
	++iter; assert(checkClause(*iter, 8,-9,-10));
	++iter; assert(checkClause(*iter, -8,9,0));
	++iter; assert(checkClause(*iter, -8,0,10));
	++iter; assert(checkClause(*iter, 9,-3,-5));
	++iter; assert(checkClause(*iter, -9,3,0));
	++iter; assert(checkClause(*iter, -9,0,5));
	++iter; assert(checkClause(*iter, 10,-5,-4));
	++iter; assert(checkClause(*iter, -10,5,0));
	++iter; assert(checkClause(*iter, -10,0,4));
	++iter; assert(checkClause(*iter, 11,-12,-13));
	++iter; assert(checkClause(*iter, -11,12,0));
	++iter; assert(checkClause(*iter, -11,0,13));
	++iter; assert(checkClause(*iter, 12,-6,-4));
	++iter; assert(checkClause(*iter, -12,6,0));
	++iter; assert(checkClause(*iter, -12,0,4));
	++iter; assert(checkClause(*iter, 13,-6,-5));
	++iter; assert(checkClause(*iter, -13,6,0));
	++iter; assert(checkClause(*iter, -13,0,5));

	delete f;
	delete f1;
	delete f_clone;

	cout << "\t passed." << endl;

}

void testOR(){
	cout << "testOR... ";
	initFormulaClass();

	/**************
	 * simple ORb1.1: (I+R)
	 **************/
	Formula *a = new FormulaLit(label2id["I"]);
	Formula *b = new FormulaLit(label2id["R"]);
	Formula *c = new FormulaOR(a,b); //I+R
	Formula *c1 = c->moveNegation();

	assert (c->formulaType == OR);
	assert (c->toString() == "(I + R)");
	assert (c1->toString() == "(I + R)");

	Formula* c_clone = c->getCopy();
	assert(c_clone->toString() == c->toString());

	assert (c->isSatisfiable() == true);
	list<Clause> clauses = c->calculateCNF();
	assert(clauses.size() == 4);

	list<Clause>::const_iterator iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,5,4));
	++iter; assert(checkClause(*iter, 7,-5,0));
	++iter; assert(checkClause(*iter, 7,0,-4));

	delete c1;
	delete c;
	delete c_clone;

	/*****************
	 * OR 1.2:  O+(I+R)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b); //I+R
	Formula *d = new FormulaLit(label2id["O"]);
	Formula *e = new FormulaOR(d, c);
	Formula *e1 = e->moveNegation();

	assert (e->formulaType == OR);
	assert (e->toString() == "(O + (I + R))");
	assert (e1->toString() == "(O + (I + R))");

	Formula *e_clone = e->getCopy();
	assert(e_clone->toString() == e->toString());

	assert (e->isSatisfiable() == true);
	clauses = e->calculateCNF();

	assert(clauses.size() == 7);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7, 6, 8));
	++iter; assert(checkClause(*iter, 7, -6, 0));
	++iter; assert(checkClause(*iter, 7, 0, -8));
	++iter; assert(checkClause(*iter, -8, 5, 4));
	++iter; assert(checkClause(*iter, 8, -5, 0));
	++iter; assert(checkClause(*iter, 8, 0, -4));

	delete e1;
	delete e;
	delete e_clone;

	/*****************
	 * OR 1.3:  (I+R)+A
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b); //I+R
	d = new FormulaLit(label2id["A"]);
	e = new FormulaOR(c, d);
	e1 = e->moveNegation();

	assert (e->formulaType == OR);
	assert (e->toString() == "((I + R) + A)");
	assert (e1->toString() == "((I + R) + A)");

	e_clone = e->getCopy();
	assert(e_clone->toString() == e->toString());

	assert (e->isSatisfiable() == true);
	clauses = e->calculateCNF();

	assert(clauses.size() == 7);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter;  assert(checkClause(*iter, -7, 8, 3));
	++iter;  assert(checkClause(*iter, 7, -8, 0));
	++iter;  assert(checkClause(*iter, 7, 0, -3));
	++iter;  assert(checkClause(*iter, -8, 5, 4));
	++iter;  assert(checkClause(*iter, 8, -5, 0));
	++iter;  assert(checkClause(*iter, 8, 0, -4));

	delete e1;
	delete e;
	delete e_clone;

	/*****************
	 * OR 1.4:  (O+A)+(I+R)
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

	Formula* h_clone = h->getCopy();
	assert(h_clone->toString() == h->toString());

	assert (h->isSatisfiable() == true);
	clauses = h->calculateCNF();

	assert(clauses.size() == 10);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7, 8, 9));
	++iter; assert(checkClause(*iter, 7, -8, 0));
	++iter; assert(checkClause(*iter, 7, 0, -9));
	++iter; assert(checkClause(*iter, -8, 6, 3));
	++iter; assert(checkClause(*iter, 8, -6, 0));
	++iter; assert(checkClause(*iter, 8, 0, -3));
	++iter; assert(checkClause(*iter, -9, 5, 4));
	++iter; assert(checkClause(*iter, 9, -5, 0));
	++iter; assert(checkClause(*iter, 9, 0, -4));

	delete h1;
	delete h;
	delete h_clone;

	/*****************
	 * OR 2.1:  false + false
	 *****************/

	h = new FormulaOR(new FormulaFalse(), new FormulaFalse());
	h1 = h->moveNegation();

	assert (h->formulaType == OR);
	assert (h->toString() == "(false + false)");
	assert (h1->toString() == "(false + false)");

	h_clone = h->getCopy();
	assert(h_clone->toString() == h->toString());

	assert (h->isSatisfiable() == false);
	clauses = h->calculateCNF();

	assert(clauses.size() == 2);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7, 0, 0));

	delete h1;
	delete h;
	delete h_clone;

	/*****************
	 * OR 2.2:  true + false // result should be equal to true + arbitrary formula
	 *****************/

	h = new FormulaOR(new FormulaTrue(), new FormulaFalse());
	h1 = h->moveNegation();

	assert (h->formulaType == OR);
	assert (h->toString() == "(true + false)");
	assert (h1->toString() == "(true + false)");

	h_clone = h->getCopy();
	assert(h_clone->toString() == h->toString());

	assert (h->isSatisfiable() == true);
	clauses = h->calculateCNF();

	assert(clauses.size() == 2);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, 7, 0, 0));

	delete h1;
	delete h;
	delete h_clone;

	/*****************
	 * OR 2.2:  false + true      // result should be equal to true + arbitrary formula
	 *****************/

	h = new FormulaOR(new FormulaFalse(), new FormulaTrue());
	h1 = h->moveNegation();

	assert (h->formulaType == OR);
	assert (h->toString() == "(false + true)");
	assert (h1->toString() == "(false + true)");

	h_clone = h->getCopy();
	assert(h_clone->toString() == h->toString());

	assert (h->isSatisfiable() == true);
	clauses = h->calculateCNF();

	assert(clauses.size() == 2);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, 7, 0, 0));

	delete h1;
	delete h;
	delete h_clone;

	/*****************
	 * OR 2.3/2.4:  false + (I+R)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b); //I+R
	h = new FormulaOR(new FormulaFalse(), c);
	h1 = h->moveNegation();

	assert (h->formulaType == OR);
	assert (h->toString() == "(false + (I + R))");
	assert (h1->toString() == "(false + (I + R))");

	h_clone = h->getCopy();
	assert(h_clone->toString() == h->toString());

	assert (h->isSatisfiable() == true);
	clauses = h->calculateCNF();
	assert(clauses.size() == 6);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7, 8, 0));
	++iter; assert(checkClause(*iter,  7, -8, 0));
	++iter; assert(checkClause(*iter,  -8, 5, 4));
	++iter; assert(checkClause(*iter, 8, -5, 0));
	++iter; assert(checkClause(*iter, 8, 0, -4));

	delete h1;
	delete h;
	delete h_clone;

	/*****************
	 * OR 2.5/2.6:  (I+R) + false
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b); //I+R
	h = new FormulaOR(c, new FormulaFalse());
	h1 = h->moveNegation();

	assert (h->formulaType == OR);
	assert (h->toString() == "((I + R) + false)");
	assert (h1->toString() == "((I + R) + false)");

	h_clone = h->getCopy();
	assert(h_clone->toString() == h->toString());

	assert (h->isSatisfiable() == true);
	clauses = h->calculateCNF();
	assert(clauses.size() == 6);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7, 8, 0));
	++iter; assert(checkClause(*iter,  7, -8, 0));
	++iter; assert(checkClause(*iter,  -8, 5, 4));
	++iter; assert(checkClause(*iter, 8, -5, 0));
	++iter; assert(checkClause(*iter, 8, 0, -4));

	delete h1;
	delete h;
	delete h_clone;

	/*****************
	 * long AND:  ((A*I)*(I*R))*((O*R)*(O*I))
	 *****************/
	a = new FormulaLit(label2id["A"]);
	b = new FormulaLit(label2id["I"]);
	c = new FormulaLit(label2id["I"]);
	d = new FormulaLit(label2id["R"]);
	Formula *v = new FormulaLit(label2id["O"]);
	Formula *x = new FormulaLit(label2id["R"]);
	Formula *y = new FormulaLit(label2id["O"]);
	Formula *z = new FormulaLit(label2id["I"]);

	Formula *n = new FormulaOR(new FormulaOR(a,b), new FormulaOR(c,d));
	Formula *m = new FormulaOR(new FormulaOR(v,x), new FormulaOR(y,z));

	f = new FormulaOR(n,m);
	Formula *f1 = f->moveNegation();

	assert (f->formulaType == OR);
	assert (f->toString() == "(((A + I) + (I + R)) + ((O + R) + (O + I)))");
	assert (f1->toString() == "(((A + I) + (I + R)) + ((O + R) + (O + I)))");
	assert (f->isSatisfiable() == true);

	Formula *f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	assert (f->isSatisfiable() == true);
	clauses = f->calculateCNF();
	assert(clauses.size() == 22);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7, 8, 11));
	++iter; assert(checkClause(*iter, 7, -8, 0));
	++iter; assert(checkClause(*iter, 7, 0, -11));
	++iter; assert(checkClause(*iter, -8, 9, 10));
	++iter; assert(checkClause(*iter, 8, -9, 0));
	++iter; assert(checkClause(*iter, 8, 0, -10));
	++iter; assert(checkClause(*iter, -9, 3, 5));
	++iter; assert(checkClause(*iter, 9, -3, 0));
	++iter; assert(checkClause(*iter, 9, 0, -5));
	++iter; assert(checkClause(*iter, -10, 5, 4));
	++iter; assert(checkClause(*iter, 10, -5, 0));
	++iter; assert(checkClause(*iter, 10, 0, -4));
	++iter; assert(checkClause(*iter, -11, 12, 13));
	++iter; assert(checkClause(*iter,  11, -12, 0));
	++iter; assert(checkClause(*iter,  11, 0, -13));
	++iter; assert(checkClause(*iter, -12, 6, 4));
	++iter; assert(checkClause(*iter, 12, -6, 0));
	++iter; assert(checkClause(*iter, 12, 0, -4));
	++iter; assert(checkClause(*iter, -13, 6, 5));
	++iter; assert(checkClause(*iter, 13, -6, 0));
	++iter; assert(checkClause(*iter, 13, 0, -5));

	delete f;
	delete f1;
	delete f_clone;

	cout << "\t passed." << endl;
}

void testAND_OR_NOT(){
	cout << "testAND_OR_NOT... ";
	initFormulaClass();

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

	Formula* d_clone = d->getCopy();
	assert(d_clone->toString() == d->toString());
	Formula* d1_clone = d1->getCopy();
	assert(d1_clone->toString() == d1->toString());

	assert (d->isSatisfiable() == true);
	list<Clause> clauses = d->calculateCNF();
	assert(clauses.size() == 4);

	list<Clause>::const_iterator iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7, -5, -4));
	++iter; assert(checkClause(*iter, 7, 5, 0));
	++iter; assert(checkClause(*iter, 7, 0, 4));

	delete d1;
	delete d;
	delete d_clone;
	delete d1_clone;

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
	assert (k->isSatisfiable() == true);

	Formula* k_clone = k->getCopy();
	assert(k_clone->toString() == k->toString());

	clauses = k->calculateCNF();
	assert(clauses.size() == 4);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, 7, -5, -4));
	++iter; assert(checkClause(*iter, -7, 5, 0));
	++iter; assert(checkClause(*iter, -7, 0, 4));

	delete k1;
	delete k;
	delete k_clone;


	/********************
	 * ~((A * ~R) + ~~A)
	 ********************/

	a = new FormulaLit(label2id["A"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaNOT(b);
	d = new FormulaAND(a,c); //A*~R
	e = new FormulaLit(label2id["A"]);
	f = new FormulaNOT(e);
	g = new FormulaNOT(f); //~~A
	h = new FormulaOR(d,g);
	Formula *i = new FormulaNOT(h); //~((A * ~R) + ~~A)
	Formula *i1 = i->moveNegation();

	assert (i->formulaType == NOT);
	assert (i->toString() == "~(((A * ~(R)) + ~(~(A))))");
	assert (i1->toString() == "((~(A) + R) * ~(A))");

	Formula* i_clone = i->getCopy();
	assert(i_clone->toString() == i->toString());

	assert (i->isSatisfiable() == true);
	clauses = i->calculateCNF();
	assert(clauses.size() == 7);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, 7, -8, 3));
	++iter; assert(checkClause(*iter, -7, 8, 0));
	++iter; assert(checkClause(*iter, -7, 0, -3));
	++iter; assert(checkClause(*iter, -8, -3, 4));
	++iter; assert(checkClause(*iter, 8, 3, 0));
	++iter; assert(checkClause(*iter, 8, 0, -4));


	delete i1;
	delete i;
	delete i_clone;

	/*****************
	 * true * ~(A*I)
	 *****************/

	a = new FormulaTrue();
	b = new FormulaLit(label2id["A"]);
	c = new FormulaLit(label2id["I"]);
	f = new FormulaAND(a, new FormulaNOT(new FormulaAND(b,c))); //true * (~(A*I))

	Formula *f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "(true * ~((A * I)))");
	assert (f1->toString() == "(true * (~(A) + ~(I)))");
	assert (f->isSatisfiable() == true);

	Formula *f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	clauses = f->calculateCNF();
	assert(clauses.size() == 6);

	iter = clauses.begin();  assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7,8,0));
	++iter; assert(checkClause(*iter, 7,-8,0));
	++iter; assert(checkClause(*iter, -8, -3, -5));
	++iter; assert(checkClause(*iter, 8, 3, 0));
	++iter; assert(checkClause(*iter, 8, 0, 5));

	delete f1;
	delete f;
	delete f_clone;

	/*****************
	 * true * ~(A)
	 *****************/

	a = new FormulaTrue();
	b = new FormulaLit(label2id["A"]);
	f = new FormulaAND(a, new FormulaNOT(b)); //true * (~(A))

	f1 = f->moveNegation();

	assert (f->formulaType == AND);
	assert (f->toString() == "(true * ~(A))");
	assert (f1->toString() == "(true * ~(A))");
	assert (f->isSatisfiable() == true);

	f_clone = f->getCopy();
	assert(f_clone->toString() == f->toString());

	clauses = f->calculateCNF();
	assert(clauses.size() == 3);

	iter = clauses.begin();  assert(checkClause(*iter, 7,0,0));
	++iter;  assert(checkClause(*iter, -7, -3, 0));
	++iter;  assert(checkClause(*iter, 7, 3, 0));

	delete f1;
	delete f;
	delete f_clone;

	/******************************
	 * (final + ~true) * ~false
	 *****************************/

	k = new FormulaFinal();
	Formula *l = new FormulaNOT(new FormulaTrue());
	Formula *m = new FormulaOR(k,l); //final + ~true
	Formula *n = new FormulaFalse();
	Formula *o = new FormulaNOT(n); //~false
	Formula *p = new FormulaAND(m,o);
	Formula *p1 = p->moveNegation();

	assert (p->toString() == "((final + ~(true)) * ~(false))");
	assert (p1->toString() == "((final + false) * true)");

	Formula* p_clone = p->getCopy();
	assert(p_clone->toString() == p->toString());

	assert (p->isSatisfiable() == true);
	clauses = p->calculateCNF();
	assert(clauses.size() == 5);

	iter = clauses.begin(); assert(checkClause(*iter, 7,0,0));
	++iter; assert(checkClause(*iter, -7, 8, 0));
	++iter; assert(checkClause(*iter, 7, -8, 0));
	++iter; assert(checkClause(*iter, -8, 1, 0));
	++iter; assert(checkClause(*iter, 8, -1, 0));


	delete p1;
	delete p;
	delete p_clone;
	
	cout << "\t passed." << endl;
}


