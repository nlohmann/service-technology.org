#include <config.h>
#include "testGraph.h"
#include "Graph.h"
#include <string>
#include <iostream>
#include "Formula.h"
#include "Node.h"
#include "cmdline.h"

/// the command line parameters
extern gengetopt_args_info args_info;

using std::map;
using std::string;
using std::vector;

extern map<string, unsigned int> label2id;
extern map<unsigned int, string> id2label;

extern unsigned int firstLabelId; //all labels including tau
extern unsigned int firstInputId; //input labels
extern unsigned int firstOutputId;//output labels

extern unsigned int lastLabelId;
extern unsigned int lastInputId;
extern unsigned int lastOutputId;

void initGraph(){
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

	firstLabelId = 4;
	firstInputId = 4;
	firstOutputId = 7;
	lastLabelId = 8;
	lastInputId = 6;
	lastOutputId = 8;

	cout << "initGraphClass... \t passed" << endl;

}


void testGraphClass(){

	initGraph();

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

	Graph* g = new Graph();
	g->nodes[n0->id] = n0;
	g->nodes[n1->id]  = n1;
	g->addInitialNode(n0->id);
	g->globalFormula = new FormulaAND(new FormulaNUM(n0->id), new FormulaNUM(n1->id)); // 0+1

	g->makeTotal();
	g->makeComplete();

	delete g;
	cout << "testGraphClass... \t passed" << endl;

}
