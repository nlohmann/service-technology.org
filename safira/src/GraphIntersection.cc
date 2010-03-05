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
#include <cstdio>
#include <string>

#include <algorithm>
#include <sstream>
//#include "cmdline.h"
#include "GraphIntersection.h"
//#include "helpers.h"
#include <time.h>
//#include <iostream>
#include "NumPrinterDouble.h"
#include "helpers.h"

using std::map;
using std::string;
using std::stack;

extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;

extern int firstLabelId; //all labels including tau
extern int lastLabelId;

using namespace std;

///constructor
GraphIntersection::GraphIntersection(){
	assert(unproc.empty());
}

///destructor
GraphIntersection::~GraphIntersection(){
//	for (map<Intpair, Nodepair*>::const_iterator n = nodepairs.begin(); n != nodepairs.end(); ++n) {
//		delete n->second;
//	}
}

void GraphIntersection::intersection(Graph *g1, Graph *g2){
	Formula* f;
	Node::init();
	for (list<int>::const_iterator q = g1->initialNodes.begin(); q != g1->initialNodes.end(); ++q){
		for (list<int>::const_iterator p = g2->initialNodes.begin(); p != g2->initialNodes.end(); ++p){
			f = new FormulaAND(g1->nodes[*q]->formula, g2->nodes[*p]->formula);
			if(f->isSatisfiable()){
				Intpair ipair;
				ipair.id1 = *q;
				ipair.id2 = *p;

				Nodepair* qp = new Nodepair(f, g1->nodes[*q], g2->nodes[*p]);
				nodepairs[ipair] = qp;
				initialNodepairs.push_back(ipair);
				initialNodes.push_back(qp->node->id);
				unproc.push(qp);
			}
		}
	}


	while (!unproc.empty()){
		Nodepair * top = unproc.top();
//		cout << "processing nodepair: " << top->n1->id << " " << top->n2->id << endl;
		unproc.pop();

		product(top);
	}

	//generate global Formula
	//NumPrinterBase* printer1 = new NumPrinterDouble(&nodepairs, true);
	//NumPrinterBase* printer2 = new NumPrinterDouble(&nodepairs, false);

	generateGlobalFormula(g1, g2);

//	globalFormulaString = "("
//			+ g1->globalFormula->toString(printer1)
//			+ ") * ("
//			+ g2->globalFormula->toString(printer2)
//			+ ")";

	convertTheMap();
}


void GraphIntersection::generateGlobalFormula(const Graph *g1, const Graph *g2){
	//TODO: hier nicht Kopieren, sondern Kopiervorgan in die modify-Funktion verlagern und damit die Probleme mit const umgehen.
	//Formula *f1_copy = g1->globalFormula->getCopy();
	//Formula *f2_copy = g2->globalFormula->getCopy();

	map<int,Formula*> formulaMap1;
	map<int,string> formulaStringMap1;

	//TODO: Schleifen umdrehen, das sollte schneller sein
	for (set<int>::const_iterator s = g1->lits.begin(); s != g1->lits.end(); ++s){
//		cout << "*s: " << *s << endl;
		for (map<Intpair, Nodepair*>::const_iterator n = nodepairs.begin(); n != nodepairs.end(); ++n){
			if (n->first.id1 == *s){

				Formula *f;

				if(formulaMap1.find(*s) == formulaMap1.end()){  //first nodepair
					f = new FormulaNUM(n->second->node->id);
					formulaStringMap1[*s] = intToString(n->second->node->id);
				}
				else{
					f = new FormulaOR(formulaMap1[*s], new FormulaNUM(n->second->node->id));
					string str = formulaStringMap1[*s] + " + " + intToString(n->second->node->id);
					formulaStringMap1[*s] = str;
				}

				formulaMap1[*s] = f;
//				cout << "node: [" << n->first.id1 << "," << n->first.id2 << "] -> id: " << n->second->node->id << ": " << f->toString() << endl;
			}
		}
	}

	map<int,Formula*> formulaMap2;
	map<int,string> formulaStringMap2;

	//TODO: Schleifen umdrehen, das sollte schneller sein
	for (set<int>::const_iterator s = g2->lits.begin(); s != g2->lits.end(); ++s){
//		cout << "*s: " << *s << endl;

		for (map<Intpair, Nodepair*>::const_iterator n = nodepairs.begin(); n != nodepairs.end(); ++n){
//			cout << "nodepair: [" << n->first.id1 << "," << n->first.id2 << "] -> " << n->second->node->id << ": " << endl;

			if (n->first.id2 == *s){

				Formula *f;

				if(formulaMap2.find(*s) == formulaMap2.end()){  //first nodepair
					f = new FormulaNUM(n->second->node->id);
					formulaStringMap2[*s] = intToString(n->second->node->id);
				}
				else{
					f = new FormulaOR(formulaMap2[*s], new FormulaNUM(n->second->node->id));
					string str = formulaStringMap2[*s] + " + " + intToString(n->second->node->id);
					formulaStringMap2[*s] = str;
				}

				formulaMap2[*s] = f;
//				cout << "node: [" << n->first.id1 << "," << n->first.id2 << "] -> " << n->second->node->id << ": " << f->toString() << endl;
			}
		}
	}

	//f1_copy->modify(formulaMap1);
	//f2_copy->modify(formulaMap2);

	globalFormula = new FormulaAND(g1->globalFormula->substitute(formulaMap1), g2->globalFormula->substitute(formulaMap2));
	globalFormulaAsString = globalFormula->toString();

//	cout << "Global Formula: " << globalFormula->toString() << endl;
//	cout << "Global Formula as String: " << globalFormulaAsString << endl;
}


void GraphIntersection::product(Nodepair* qp){
	Formula * f;

	//both graphs must have the same interface!!!
	assert(qp->n1->outEdges->size() == qp->n2->outEdges->size());
	for (int l = firstLabelId; l <= lastLabelId; ++l){	//for each label l
//		cout << "pocessing label: " << id2label[l] << endl;

		//for each l-successor in q    ==> q'
		for (list<Node*>::iterator e1 = qp->n1->outEdges[l].begin(); e1 != qp->n1->outEdges[l].end(); ++e1){


			//for each l-successor in p   ==> p'
			for (list<Node*>::iterator e2 = qp->n2->outEdges[l].begin(); e2 != qp->n2->outEdges[l].end(); ++e2){

				//f = new FormulaAND(qp->n1->formula, qp->n2->formula);
				assert((*e1)->formula != NULL);
				assert((*e2)->formula != NULL);

				f = new FormulaAND((*e1)->formula, (*e2)->formula);
//				cout << f->toString() << endl;
				if (f->isSatisfiable()){

					Intpair ipair;
					ipair.id1 = (*e1)->id;
					ipair.id2 = (*e2)->id;

					//if [q',p'] not yet in the graph
					if (nodepairs.find(ipair) == nodepairs.end()){
						Nodepair* qp_succ = new Nodepair(f, *e1, *e2);
						nodepairs[ipair] = qp_succ;
						qp->node->addEdge(l,qp_succ->node);
						unproc.push(qp_succ);
//						cout << "State " << qp_succ->n1->id << " " << qp_succ->n2->id << " added" << endl;
					}
					else{
						qp->node->addEdge(l,nodepairs[ipair]->node);
					}
				}
			}
		}
	}
//	cout << "End Product(): [" << qp->n1->id << "," << qp->n2->id << "]-> " << qp->node->id << endl;
}


///converts "this" to a Graph object g
void GraphIntersection::convertToGraph(Graph *g){
	for (list<int>::iterator i = initialNodes.begin(); i != initialNodes.end(); ++i ){
		g->initialNodes.push_back(*i);
	}

	g->nodes.insert(nodes.begin(), nodes.end());
	g->globalFormula = globalFormula;
	g->globalFormulaAsString = globalFormulaAsString;

	for (set<int>::iterator i = lits.begin(); i != lits.end(); ++i ){
		g->lits.insert(*i);
	}


}

///converts the map of Nodepairs to a map of Nodes
void GraphIntersection::convertTheMap(){
	assert(nodes.size() == 0);
	for (map<Intpair, Nodepair*>::const_iterator n = nodepairs.begin(); n != nodepairs.end(); ++n){
		assert(nodes.find(n->second->node->id) == nodes.end());
		nodes[n->second->node->id] = n->second->node;
	}
}


//! \brief creates a dot output of the graph (result of intersection or union)
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
//void GraphIntersection::toDot(FILE* out, string title) const {
//
//		fprintf(out, "digraph D {\n");
//		fprintf(out, "graph [fontname=\"Helvetica\", label=\"");
//		fprintf(out, "%s", title.c_str());
//		fprintf(out, "\"];\n");
//		fprintf(out, "node [fontname=\"Helvetica\" fontsize=10];\n");
//		fprintf(out, "edge [fontname=\"Helvetica\" fontsize=10];\n");
//
//
//		for (list<Intpair>::const_iterator n = initialNodepairs.begin(); n!= initialNodepairs.end(); ++n){
//			// an arrow indicating the initial state
//			//fprintf(out, "  q_%d [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"];\n", (*n).id1*100+(*n).id2);
//			//fprintf(out, "  q_%d -> %d [minlen=\"0.5\"];\n", (*n).id1*100+(*n).id2, (*n).id1*100+(*n).id2);
//			//Intpair ip = *n;
//			//cout << ip.id1 << " " << ip.id2 << endl;
//			//(nodepairs.find(*n))->second->id;
//			fprintf(out, "  q_%d [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"];\n", (nodepairs.find(*n))->second->node->id);
//			fprintf(out, "  q_%d -> %d [minlen=\"0.5\"];\n", (nodepairs.find(*n))->second->node->id, (nodepairs.find(*n))->second->node->id);
//		}
//
//		for (map<Intpair, Nodepair*>::const_iterator n = nodepairs.begin(); n != nodepairs.end(); ++n) {
////			fprintf(out, "  %d [label=\"%d\\n %s\"]\n", n->second->n1->id * 100 + n->second->n2->id, n->second->n1->id * 100 + n->second->n2->id, n->second->formula->toString().c_str());
//			fprintf(out, "  %d [label=\"%d\\n %s\"]\n", n->second->node->id, n->second->node->id, n->second->node->formula->toString().c_str());
//
//			for (int i = firstLabelId; i <= lastLabelId; ++i){
//				for (list<Node*>::iterator s = n->second->node->outEdges[i].begin(); s != n->second->node->outEdges[i].end(); ++s){
////					fprintf(out, "  %d -> %d [label=\"%c%s\"]\n", n->second->n1->id * 100 + n->second->n2->id, (*s)->n1->id * 100 + (*s)->n2->id, inout[i] ,id2label[i].c_str());
//					fprintf(out, "  %d -> %d [label=\"%c%s\"]\n", n->second->node->id, (*s)->id, inout[i], id2label[i].c_str());
//				}
//			}
//		}
//
//		fprintf(out, "}\n");
//}
