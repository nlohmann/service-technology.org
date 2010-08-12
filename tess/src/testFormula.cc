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
#include <list>
#include "testFormula.h"
#include "Formula.h"
#include "helpers.h"

#include "cmdline.h"

/// the command line parameters
//extern gengetopt_args_info args_info;

using namespace std;
extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;

void testFormulaClass(){

	string s = intToString(23);
	assert (s == "23");

	initFormulaClass();
	testPutIdIntoList();
	testMinimizeDnf();
	testTRUE();
	testFINAL();
	testLIT();
	testNUM();
	testAND();
	testOR();
	testAND_OR();

}



void initFormulaClass(){

	initGlobalVariables();
	addLabel("A",'!');
	addLabel("R",'!');
	addLabel("I",'?');
	addLabel("O",'?');

	//cout << "initFormulaClass... \t passed" << endl;
}

void testPutIdIntoList(){
	cout << "testPutIdIntoList... ";
	list< set<int> > l0 = putIdIntoList(0);
	assert(*((l0.begin())->begin()) == 0);
	list< set<int> > l1 = putIdIntoList(1);
	assert(*((l1.begin())->begin()) == 1);
	list< set<int> > l2 = putIdIntoList(2);
	assert(*((l2.begin())->begin()) == 2);
	list< set<int> > l3 = putIdIntoList(3);
	assert(*((l3.begin())->begin()) == 3);
	list< set<int> > l4 = putIdIntoList(4);
	assert(*((l4.begin())->begin()) == 4);

	cout << "\t passed." << endl;

}

void testMinimizeDnf(){
	cout << "testMinimizeDnf... ";
	initFormulaClass();

	list<set<int> > dnf;

	int ints1[]= {0,1,2,3};
	set<int> set1;
	set1.insert (ints1, ints1 + 4);
	dnf.push_back(set1);

	int ints2[]= {2,3};
	set<int> set2;
	set2.insert (ints2, ints2 + 2);
	dnf.push_back(set2);

	int ints3[]= {4,3};
	set<int> set3;
	set3.insert (ints3, ints3 + 2);
	dnf.push_back(set3);

	int ints4[]= {2,4};
	set<int> set4;
	set4.insert (ints4, ints4 + 2);
	dnf.push_back(set4);

	int ints5[]= {4,1,2};
	set<int> set5;
	set5.insert (ints5, ints5 + 3);
	dnf.push_back(set5);

	int ints6[]= {2,4};
	set<int> set6;
	set6.insert (ints6, ints6 + 2);
	dnf.push_back(set4);

	assert(dnfToString(dnf) == "(final * true * tau * A) + (tau * A) + (A * R) + (tau * R) + (true * tau * R) + (tau * R)");
	list<set<int> > minimal = minimizeDnf(dnf);
	assert(dnfToString(minimal) == "(tau * A) + (A * R) + (tau * R)");

	cout << "\t passed." << endl;

}

void testTRUE(){
	cout << "testTRUE... ";
	initFormulaClass();

	/****************
	 * f = TRUE
	 ****************/
	Formula* f = new FormulaTrue();
	assert (f->toString() == "true");
	list <set<int> > dnf_f = f->toDnf();

	assert (f->formulaType == TRUE);
	assert (dnfToString(dnf_f) == "(true)");

	delete f;

	cout << "\t passed." << endl;
}



void testFINAL(){
	cout << "testFINAL... ";
	initFormulaClass();

	/****************
	 * f = FINAL
	 ****************/
	Formula* f = new FormulaFinal();
	assert (f->toString() == "final");

	assert (f->formulaType == FINAL);
	assert (dnfToString(f->toDnf()) == "(final)");
	assert (dnfToString(f->toMinimalDnf()) == "(final)");

	delete f;

	cout << "\t passed." << endl;
}

void testNUM(){
	cout << "testINT... ";
	initFormulaClass();

	Formula* f = new FormulaNUM(42);
	assert (f->toString() == "42");

	delete f;

	cout << "\t passed." << endl;
}


void testLIT(){
	cout << "testLIT... ";
	initFormulaClass();

	Formula* f = new FormulaLit(label2id["I"]);
	assert (f->formulaType == LIT);
	assert (f->toString() == "I");

	assert (dnfToString(f->toDnf()) == "(I)");
	assert (dnfToString(f->toMinimalDnf()) == "(I)");

	delete f;

	Formula* g = new FormulaLit(label2id["A"]);
	assert (g->formulaType == LIT);
	assert (g->toString() == "A");

	assert (dnfToString(g->toDnf()) == "(A)");
	assert (dnfToString(g->toMinimalDnf()) == "(A)");

	delete g;
	cout << "\t passed." << endl;
}


void testAND(){
	cout << "testAND... ";
	initFormulaClass();

	/**************
	 * simple AND 1.1: (I*R)
	 **************/
	Formula *a = new FormulaLit(label2id["I"]);
	Formula *b = new FormulaLit(label2id["R"]);
	Formula *c = new FormulaAND(a,b);

	assert (c->formulaType == AND);
	assert (c->toString() == "(I * R)");

	assert (dnfToString(c->toDnf()) == "(R * I)");
	assert (dnfToString(c->toMinimalDnf()) == "(R * I)");

	delete c;


	/*****************
	 * complex AND 1.2:  O*(I*R)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b); //I*R
	Formula *d = new FormulaLit(label2id["O"]);
	Formula *e = new FormulaAND(d, c);

	assert (e->formulaType == AND);
	assert (e->toString() == "(O * (I * R))");

	assert(dnfToString(e->toDnf()) == "(R * I * O)");
	assert(dnfToString(e->toMinimalDnf()) == "(R * I * O)");

	delete e;


	/*****************
	 * complex AND 1.3:  (I*R)*A
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b);           //I*R
	d = new FormulaLit(label2id["A"]);
	e = new FormulaAND(c, d); //(I*R)*A

	assert (e->formulaType == AND);
	assert (e->toString() == "((I * R) * A)");

	assert(dnfToString(e->toDnf()) == "(A * R * I)");
	assert(dnfToString(e->toMinimalDnf()) == "(A * R * I)");

	delete e;



	/*****************
	 * complex AND 1.4:  (O*A)*(I*R)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b);          // I*R
	d = new FormulaLit(label2id["O"]);
	e = new FormulaLit(label2id["A"]);
	Formula *f = new FormulaAND(d,e); // O*A
	Formula *h = new FormulaAND(f,c); // (O*A)*(I*R)

	assert (h->formulaType == AND);
	assert (h->toString() == "((O * A) * (I * R))");

    assert(dnfToString(h->toDnf()) == "(A * R * I * O)");
    assert(dnfToString(h->toMinimalDnf()) == "(A * R * I * O)");

	delete h;

	/*****************
	 * complex AND 1.5:  (O*(A*(I*R))
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b);          // I*R
	d = new FormulaAND(new FormulaLit(label2id["A"]), c); // A*(I*R)
	e = new FormulaAND(new FormulaLit(label2id["O"]), d); // O*(A*(I*R))

	assert (e->formulaType == AND);
	assert (e->toString() == "(O * (A * (I * R)))");

	assert(dnfToString(e->toDnf()) == "(A * R * I * O)");
	assert(dnfToString(e->toMinimalDnf()) == "(A * R * I * O)");

	delete e;

	/*****************
	 * complex AND 1.6:  (O*(O*(I*O))
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["O"]);
	c = new FormulaAND(a,b);          // I*O
	d = new FormulaAND(new FormulaLit(label2id["O"]), c); // O*(I*O)
	e = new FormulaAND(new FormulaLit(label2id["O"]), d); // O*(O*(I*O))

	assert (e->formulaType == AND);
	assert (e->toString() == "(O * (O * (I * O)))");


	assert(dnfToString(e->toDnf()) == "(I * O)");
	assert(dnfToString(e->toMinimalDnf()) == "(I * O)");

	delete e;


	/*****************
	 * complex AND1.7 :  final*(I*R)
	 *****************/

	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaAND(a,b);           //I*R
	d = new FormulaFinal();
	f = new FormulaAND(d,c);

	assert (f->formulaType == AND);
	assert (f->toString() == "(final * (I * R))");

	assert(dnfToString(f->toDnf()) == "(final * R * I)");
	assert(dnfToString(f->toMinimalDnf()) == "(final * R * I)");

	delete f;


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

	assert (f->formulaType == AND);
	assert (f->toString() == "(((A * I) * (I * R)) * ((O * R) * (O * I)))");

	assert(dnfToString(f->toDnf()) == "(A * R * I * O)");
	assert(dnfToString(f->toMinimalDnf()) == "(A * R * I * O)");

	delete f;

	cout << "\t passed." << endl;

}


void testOR(){
	cout << "testOR... ";
	initFormulaClass();

	/**************
	 * simple OR 1.1: (I+R)
	 **************/
	Formula *a = new FormulaLit(label2id["I"]);
	Formula *b = new FormulaLit(label2id["R"]);
	Formula *c = new FormulaOR(a,b);

	assert (c->formulaType == OR);
	assert (c->toString() == "(I + R)");

	assert (dnfToString(c->toDnf()) == "(I) + (R)");
	assert (dnfToString(c->toMinimalDnf()) == "(I) + (R)");

	delete c;


	/*****************
	 * complex OR 1.2:  O*(I*R)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b); //I+R
	Formula *d = new FormulaLit(label2id["O"]);
	Formula *e = new FormulaOR(d, c);

	assert (e->formulaType == OR);
	assert (e->toString() == "(O + (I + R))");

	assert(dnfToString(e->toDnf()) == "(O) + (I) + (R)");
	assert(dnfToString(e->toMinimalDnf()) == "(O) + (I) + (R)");

	delete e;


	/*****************
	 * complex OR 1.3:  (I*R)*A
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b);           //I+R
	d = new FormulaLit(label2id["A"]);
	e = new FormulaOR(c, d); //(I+R)+A

	assert (e->formulaType == OR);
	assert (e->toString() == "((I + R) + A)");

	assert(dnfToString(e->toDnf()) == "(I) + (R) + (A)");
	assert(dnfToString(e->toMinimalDnf()) == "(I) + (R) + (A)");

	delete e;



	/*****************
	 * complex OR 1.4:  (O*A)*(I*R)
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b);          // I+R
	d = new FormulaLit(label2id["O"]);
	e = new FormulaLit(label2id["A"]);
	Formula *f = new FormulaOR(d,e); // O+A
	Formula *h = new FormulaOR(f,c); // (O+A)+(I+R)

	assert (h->formulaType == OR);
	assert (h->toString() == "((O + A) + (I + R))");

    assert(dnfToString(h->toDnf()) == "(O) + (A) + (I) + (R)");
    assert(dnfToString(h->toMinimalDnf()) == "(O) + (A) + (I) + (R)");

	delete h;

	/*****************
	 * complex OR 1.5:  (O*(A*(I*R))
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b);          // I+R
	d = new FormulaOR(new FormulaLit(label2id["A"]), c); // A+(I+R)
	e = new FormulaOR(new FormulaLit(label2id["O"]), d); // O+(A+(I+R))

	assert (e->formulaType == OR);
	assert (e->toString() == "(O + (A + (I + R)))");

	assert(dnfToString(e->toDnf()) == "(O) + (A) + (I) + (R)");
	assert(dnfToString(e->toMinimalDnf()) == "(O) + (A) + (I) + (R)");

	delete e;

	/*****************
	 * complex OR 1.6:  (O*(O*(I*O))
	 *****************/
	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["O"]);
	c = new FormulaOR(a,b);          // I+O
	d = new FormulaOR(new FormulaLit(label2id["O"]), c); // O+(I+O)
	e = new FormulaOR(new FormulaLit(label2id["O"]), d); // O+(O+(I+O))

	assert (e->formulaType == OR);
	assert (e->toString() == "(O + (O + (I + O)))");

	assert(dnfToString(e->toDnf()) == "(O) + (O) + (I) + (O)");
	assert(dnfToString(e->toMinimalDnf()) == "(I) + (O)");

	delete e;

	/*****************
	 * complex OR1.7 :  final*(I*R)
	 *****************/

	a = new FormulaLit(label2id["I"]);
	b = new FormulaLit(label2id["R"]);
	c = new FormulaOR(a,b);           //I+R
	d = new FormulaFinal();
	f = new FormulaOR(d,c);

	assert (f->formulaType == OR);
	assert (f->toString() == "(final + (I + R))");

	assert(dnfToString(f->toDnf()) == "(final) + (I) + (R)");
	assert(dnfToString(f->toMinimalDnf()) == "(final) + (I) + (R)");

	delete f;


	/*****************
	 * long OR:  ((A+I)+(I+R))+((O+R)+(O+I))
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

	assert (f->formulaType == OR);
	assert (f->toString() == "(((A + I) + (I + R)) + ((O + R) + (O + I)))");

	assert(dnfToString(f->toDnf()) == "(A) + (I) + (I) + (R) + (O) + (R) + (O) + (I)");
	assert(dnfToString(f->toMinimalDnf()) == "(A) + (R) + (O) + (I)");

	delete f;

	cout << "\t passed." << endl;
}

void testAND_OR(){
	cout << "testAND_OR... ";
	initFormulaClass();

	/*****************
	 * ((A+I)*(I+R))*((O+R)*(O*I))
	 *****************/
	Formula *a = new FormulaLit(label2id["A"]);
	Formula *b = new FormulaLit(label2id["I"]);
	Formula *c = new FormulaLit(label2id["I"]);
	Formula *d = new FormulaLit(label2id["R"]);
	Formula *v = new FormulaLit(label2id["O"]);
	Formula *x = new FormulaLit(label2id["R"]);
	Formula *y = new FormulaLit(label2id["O"]);
	Formula *z = new FormulaLit(label2id["I"]);

	Formula *n = new FormulaAND(new FormulaOR(a,b), new FormulaOR(c,d)); //((A+I)*(I+R))
	Formula *m = new FormulaAND(new FormulaOR(v,x), new FormulaAND(y,z)); //((O+R)*(O*I))

	Formula *f = new FormulaAND(n,m);

	assert (f->formulaType == AND);
	assert (f->toString() == "(((A + I) * (I + R)) * ((O + R) * (O * I)))");

	assert(dnfToString(f->toDnf()) == "(A * I * O) + (A * R * I * O) + (I * O) + (R * I * O) + (A * R * I * O) + (A * R * I * O) + (R * I * O) + (R * I * O)");
	assert(dnfToString(f->toMinimalDnf()) == "(I * O)");

	delete f;

	cout << "\t passed." << endl;
}


