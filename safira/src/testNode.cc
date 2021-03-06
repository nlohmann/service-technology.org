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
#include "helpers.h"
#include "cassert"
#include "testNode.h"
#include "testFormula.h"
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include "Formula.h"
#include "Node.h"
#include "cmdline.h"

/// the command line parameters
extern gengetopt_args_info args_info;

using namespace std;
extern map<string, int> label2id;
extern map<int, string> id2label;

void testNodeClass(){
	initFormulaClass();
	int firstLabelId = 2;
	int lastLabelId = label2id.size()-1;
	assert(label2id.size() == id2label.size());

	/**************
	 * simple AND: (I*R)
	 **************/
	Formula *a = new FormulaLit(label2id["I"]);
	Formula *b = new FormulaLit(label2id["R"]);
	Formula *c = new FormulaAND(a,b);

	Formula *d = new FormulaLit(label2id["I"]);
	Formula *e = new FormulaLit(label2id["A"]);

	Node* n1 = new Node(c);
	assert (n1->id == 1);
	assert (n1->formula->toString() == "(I * R)");

	Node* n2 = new Node(d);
	assert (n2->id == 2);
	assert (n2->formula->toString() == "I");

	Node* n3 = new Node(e, 23);
	assert (n3->id == 23);
	assert (n3->formula->toString() == "A");

	n1->addEdge(label2id["tau"], n1);// n1-tau->n1
	n1->addEdge(label2id["I"], n2);  // n1--I-->n2
	n1->addEdge(label2id["I"], n3);  // n1--I-->n3
	n1->addEdge((label2id["O"]), n2);// n1--O-->n2

	//trap state
    Formula *t = new FormulaTrue();
	Node* trap = new Node(t);
	assert (trap->id == 24);
	assert (trap->formula->toString() == "true");
	//add for each label a self loop
    for (int l = firstLabelId; l <= lastLabelId; ++l){
    	trap->addEdge(l,trap); //self loop
    }

	string s1;
	stringstream out1;

    for (int i = firstLabelId; i <= lastLabelId; ++i){
    	if (n1->outEdges[i].size() > 0){
    		for (list<Node*>::iterator s = n1->outEdges[i].begin(); s != n1->outEdges[i].end(); ++s){

    			out1 << " " << n1->id << "--" << id2label[i] << "->" << (*s)->id; // n1--label-->successor
    		}
    	}
    }
    s1 = out1.str();
    assert (s1 ==" 1--tau->1 1--I->2 1--I->23 1--O->2");

    string s2;
    stringstream out2;

    for (int i = firstLabelId; i <= lastLabelId; ++i){
    	if (trap->outEdges[i].size() > 0){
    		for (list<Node*>::iterator s = trap->outEdges[i].begin(); s != trap->outEdges[i].end(); ++s){

    			out2 << " " << trap->id << "--" << id2label[i] << "->" << (*s)->id; // n1--label-->successor
    		}
    	}
    }

    s2 = out2.str();
    assert (s2 ==" 24--tau->24 24--A->24 24--R->24 24--I->24 24--O->24");

    delete n1;
    delete n2;
    delete n3;
    delete trap;


	cout << "testNodeClass... \t passed." << endl;
}
