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
	for (map<Intpair, Nodepair*>::const_iterator n = nodepairs.begin(); n != nodepairs.end(); ++n) {
		delete n->second;
	}
}

void GraphIntersection::intersection(Graph *g1, Graph *g2){
	Formula* f;
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
				initialNodes.push_back(qp->id);
				unproc.push(qp);
			}
		}
	}


	while (!unproc.empty()){
		Nodepair * top = unproc.top();
//		cout << "processing nodepair: " << top->node1->id << " " << top->node2->id << endl;
		unproc.pop();

		product(top);
	}

	//generate global Formula
	NumPrinterBase* printer1 = new NumPrinterDouble(&nodepairs, true);
	NumPrinterBase* printer2 = new NumPrinterDouble(&nodepairs, false);
	globalFormulaString = "("
			+ g1->globalFormula->toString(printer1)
			+ ") * ("
			+ g2->globalFormula->toString(printer2)
			+ ")";
}

void GraphIntersection::product(Nodepair* qp){
	Formula * f;

	//both graphs must have the same interface!!!
	assert(qp->node1->outEdges->size() == qp->node2->outEdges->size());
	for (int l = firstLabelId; l <= lastLabelId; ++l){	//for each label l
//		cout << "pocessing label: " << id2label[l] << endl;

		//for each l-successor in q    ==> q'
		for (list<Node*>::iterator e1 = qp->node1->outEdges[l].begin(); e1 != qp->node1->outEdges[l].end(); ++e1){


			//for each l-successor in p   ==> p'
			for (list<Node*>::iterator e2 = qp->node2->outEdges[l].begin(); e2 != qp->node2->outEdges[l].end(); ++e2){

				//f = new FormulaAND(qp->node1->formula, qp->node2->formula);
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
						qp->addEdge(l,qp_succ);
						unproc.push(qp_succ);
//						cout << "State " << qp_succ->node1->id << " " << qp_succ->node2->id << " added" << endl;
					}
					else{
						qp->addEdge(l,nodepairs[ipair]);
					}
				}
			}
		}
	}
//	cout << "End Product(): " << qp->node1->id << " " << qp->node2->id << endl;
}


//! \brief creates a dot output of the graph (result of intersection or union)
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void GraphIntersection::toDot(FILE* out, string title) const {

		fprintf(out, "digraph D {\n");
		fprintf(out, "graph [fontname=\"Helvetica\", label=\"");
		fprintf(out, "%s", title.c_str());
		fprintf(out, "\"];\n");
		fprintf(out, "node [fontname=\"Helvetica\" fontsize=10];\n");
		fprintf(out, "edge [fontname=\"Helvetica\" fontsize=10];\n");


		for (list<Intpair>::const_iterator n = initialNodepairs.begin(); n!= initialNodepairs.end(); ++n){
			// an arrow indicating the initial state
			//fprintf(out, "  q_%d [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"];\n", (*n).id1*100+(*n).id2);
			//fprintf(out, "  q_%d -> %d [minlen=\"0.5\"];\n", (*n).id1*100+(*n).id2, (*n).id1*100+(*n).id2);
			//Intpair ip = *n;
			//cout << ip.id1 << " " << ip.id2 << endl;
			//(nodepairs.find(*n))->second->id;
			fprintf(out, "  q_%d [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"];\n", (nodepairs.find(*n))->second->id);
			fprintf(out, "  q_%d -> %d [minlen=\"0.5\"];\n", (nodepairs.find(*n))->second->id, (nodepairs.find(*n))->second->id);
		}

		for (map<Intpair, Nodepair*>::const_iterator n = nodepairs.begin(); n != nodepairs.end(); ++n) {
//			fprintf(out, "  %d [label=\"%d\\n %s\"]\n", n->second->node1->id * 100 + n->second->node2->id, n->second->node1->id * 100 + n->second->node2->id, n->second->formula->toString().c_str());
			fprintf(out, "  %d [label=\"%d\\n %s\"]\n", n->second->id, n->second->id, n->second->formula->toString().c_str());

			for (int i = firstLabelId; i <= lastLabelId; ++i){
				for (list<Nodepair*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
//					fprintf(out, "  %d -> %d [label=\"%c%s\"]\n", n->second->node1->id * 100 + n->second->node2->id, (*s)->node1->id * 100 + (*s)->node2->id, inout[i] ,id2label[i].c_str());
					fprintf(out, "  %d -> %d [label=\"%c%s\"]\n", n->second->id, (*s)->id, inout[i] ,id2label[i].c_str());
				}
			}
		}

		fprintf(out, "}\n");
}


void GraphIntersection::printNodepairs(ostream& o) const{
	o << "\nNODES\n";

    //print all nodes
	for (map<Intpair, Nodepair*>::const_iterator n = nodepairs.begin(); n != nodepairs.end(); ++n){
		o << "  " << n->second->id << ": " << n->second->formula->toString() << endl;
		for (int i = firstLabelId; i <= lastLabelId; ++i){
			for (list<Nodepair*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				assert(id2label.find(i) != id2label.end());
				o << "    " << id2label[i] << " -> " << (*s)->id << endl;
			}
		}
		o << endl;
	}
}


/// prints the Intersection as eaa
void GraphIntersection::print(ostream& o) const{
	printInterface(o);
	printInitialNodes(o);
	//printGlobalFormula(o);
	printNodepairs(o);
}

void GraphIntersection::printGlobalFormula(ostream& o) const{

}
