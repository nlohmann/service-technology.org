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
#include "testGraph.h"
#include "Graph.h"
#include "GraphComplement.h"
#include <string>
#include <iostream>
#include "Formula.h"
#include "Node.h"
#include "cmdline.h"
#include "testFormula.h"

//#include "cmdline.h"
//#include "verbose.h"
//#include "config-log.h"

#include "Output.h"

/// the command line parameters
gengetopt_args_info args_info;

using std::map;
using std::string;
using std::vector;

extern map<string,  int> label2id;
extern map< int, string> id2label;

extern  int firstLabelId; //all labels including tau
extern  int firstInputId; //input labels
extern  int firstOutputId;//output labels

extern  int lastLabelId;
extern  int lastInputId;
extern  int lastOutputId;

void TestGraphComplement::initGraph(){
	initFormulaClass();
	Node::init();

	firstLabelId = 2;
	firstInputId = 3;
	lastInputId = 4;
	firstOutputId = 5;
	lastOutputId = 6;
	lastLabelId = 6;

	Formula* f0 = new FormulaOR(new FormulaLit(label2id["I"]), new FormulaLit(label2id["O"])); //f1 = I+O
	Formula* f1 = new FormulaOR(new FormulaLit(label2id["O"]), new FormulaFinal()); //f2 = O + final


	Node* n0 = new Node(f0, 0);
	Node* n1 = new Node(f1, 1);

	assert(n0->id == 0);
	assert(n1->id == 1);

	assert(n0->formula->toString() == "(I + O)");
	assert(n1->formula->toString() == "(O + final)");

	n0->addEdge(label2id["I"], n0); //n0--I->n0
	n0->addEdge(label2id["I"], n1); //n0--I->n1
	n0->addEdge(label2id["O"], n1); //n0--O->n1
	n1->addEdge(label2id["O"], n0); //n1--O->n1

	nodes[n0->id] = n0;
	nodes[n1->id]  = n1;
	addInitialNode(n0->id);
	globalFormula = new FormulaOR(new FormulaNUM(n0->id), new FormulaNUM(n1->id)); // 0+1
    globalFormulaAsString = globalFormula->toString();

	cout << "initGraphClass... \t passed" << endl;

}


/* computes the complement of a given annotated Graph
 * the correctness of the result is checked by the testsuite.at
 * the Function make complete is also tested by this method
 */
void TestGraphComplement::testComplement(){
	initGraph();



    complement();
	Output o("testGraph_complement.eaa", "complement testGraph");
	print(o);
}
