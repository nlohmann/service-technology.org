#include <config.h>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
//#include "cmdline.h"
#include "Graph.h"
#include "helpers.h"
#include <time.h>

/// the command line parameters
//extern gengetopt_args_info args_info;

using std::map;
using std::string;
using std::vector;
using std::stack;

extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;

extern int firstLabelId; //all labels including tau
//extern int firstInputId; //input labels
//extern int firstOutputId;//output labels

extern int lastLabelId;
//extern int lastInputId;
//extern int lastOutputId;

using namespace std;

int start;
///constructor
Graph::Graph() : trap(NULL), globalFormula(NULL){
	assert(unproc.empty());
}

///destructor
Graph::~Graph(){

	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
		delete n->second;
	}

	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
		delete n->second;
	}

	delete globalFormula;

}

void Graph::intersection(Graph *g1, Graph *g2){
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
	//TODO: global Formula
}

void Graph::product(Nodepair* qp){
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

/// add a node to the graph
void Graph::addInitialNode(int id){
	initialNodes.push_back(id);
}

void Graph::complement(){
	makeTotal();
	makeComplete();
}

void Graph::makeTotal(){
//	map<int, Node*> addedNodes;

	if (NULL == trap){
		//add trap state (with formula true and a self loop for every label) to the graph
		Formula *t = new FormulaTrue();
		trap = new Node(t);
	    addedNodes[trap->id] = trap;  //the trap state is already total and complete
	    for (int l = firstLabelId; l <= lastLabelId; ++l){
	    	trap->addEdge(l,trap); //self loop
	    }
	}

	/* for each node q and each label x such that there is no q' with q-x->q':
	 * (1) insert an edge q-x->t and
	 * (2) replace annotation f of q with f = AND(f,NOT(x))
	 */
	Formula *f = NULL;
	Formula *g = NULL;
	Formula *h = NULL;

	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
		for (int i = firstLabelId; i <= lastLabelId; ++i){
			if (n->second->outEdges[i].size() == 0){
				n->second->addEdge(i,trap);
				//h = new FormulaLit(id2label[i], i);
				h = new FormulaLit(i);
				g = new FormulaNOT(h);
				f = new FormulaAND(n->second->formula,g);
				n->second->formula = f;
			}

		}
		//printf("new formula for node %d: %s\n", n->second->id, n->second->formula->toString().c_str());
	}
}



void Graph::makeComplete() {

	// dieser fall kann nicht (mehr) eintreten, da makeComplete immer nach makeTotal aufgerufen wird
//	if (NULL == trap){
//		//add trap state (with formula true and a self loop for every label) to the graph
//		Formula *t = new FormulaTrue();
//		trap = new Node(t);
//	    addedNodes[trap->id] = trap; //the trap state is already total and complete
//	    for (int l = firstLabelId; l <= lastLabelId; ++l){
//	    	trap->addEdge(l,trap); //self loop
//	    }
//	}

	assert(NULL != trap);

	/* complete the initial nodes:
	 * if the formula of a initial node p is not equal true:
	 * (1) insert an new initial node q with successors in the trap state and
	 * (2) replace the global formula chi with chi = AND(chi,NOT(q))
	 */
	list<int> addedInitialNodes;
	for (list<int>::const_iterator n = initialNodes.begin(); n!= initialNodes.end(); ++n){
		Formula *g = nodes[*n]->formula->getCopy();
		Formula *f = new FormulaNOT(g);

		if(f->isSatisfiable() == true){
			Node *q = new Node(f);
			addedNodes[q->id] = q;
			addedInitialNodes.push_back(q->id);
			for (int l = firstLabelId; l <= lastLabelId; ++l){
				q->addEdge(l,trap); //edge from q to trap state
			}
		}

		else {
			delete f;
		}
	}

	//add new initial nodes to initialnodes-list
	for (list<int>::const_iterator n = addedInitialNodes.begin(); n != addedInitialNodes.end(); ++n) {
		assert(find(initialNodes.begin(), initialNodes.end(), *n) == initialNodes.end()); //elements in addedNodes-map are not yet in intialNodes-map
		initialNodes.push_back(*n);
		start = *n;
	}

	//start = addedInitialNodes.begin()

	/* complete the other nodes:
	 * for all nodes p and labels x:
	 * if the disjunction over all x successors of p is not equal true:
	 * (1) insert a new successor node q reachable from p over an x-edge
	 * (2) for each label x add an edge from node q to the trap state t
	 * (3) replace the global formula chi with chi = AND(chi,NOT(q))
     */

	Formula *f = NULL; //formula of the new node q (if q is generated)

	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
		for (int i = firstLabelId; i <= lastLabelId; ++i){
			//assert(n->second->outEdges[i].size() > 0); //holds if the graph is total
			if (n->second->outEdges[i].size() > 0){
				for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){

					if (n->second->outEdges[i].begin() == s){ //first formula
						f = (*s)->formula->getCopy();
					}

					else{
						Formula *g = (*s)->formula->getCopy();
						Formula *h = new FormulaOR(f, g) ;
						f = h;
					}
				}

				Formula *h = new FormulaNOT(f); //f is the disjunction over all x successors of p

				f = h;
				assert(f);

				if(f->isSatisfiable() == true){
					//if(!formulaFounded(f)){
						//generate new node q with the formula f and add edge with label i to it

						Node *q = getNode(f);

//						Node *q = new Node(f);
//						addedNodes[q->id] = q;
						n->second->addEdge(i,q);

						//connect q with the trap state
//						for (int l = firstLabelId; l <= lastLabelId; ++l){
//							q->addEdge(l,trap);
//						}

					//printf("added: %d -%s-> %d   forumula: %s\n", n->second->id, id2label[i].c_str(),q->id, q->formula->toString().c_str());
					//}
				}
				else {
					delete f;
				}
			}
		}
	}
}

bool Graph::formulaFounded(Formula *f, Formula *g){
	Formula * h1 = new FormulaOR(f,g);
	Formula * h2 = new FormulaOR(new FormulaNOT(f), new FormulaNOT(g));
	Formula * h3 = new FormulaAND(h1,h2);
	//cout << f->toString() << "   " << g->toString() << endl;
	bool erg = h3->isSatisfiable();

	if(erg){
		//cout << "sind ungleich" << endl;
		return false;
	}
	//cout << "sind gleich";
	delete h3;
	return true;
}

Node* Graph::getNode(Formula *f){
	bool founded = false;
	Node *q ;
//	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n){
//		if (formulaFounded(f, n->second->formula)){
//			founded = true;
//			q = n->second;
//			break;
//		}
//	}

	if (!founded){
		q = new Node(f);
		addedNodes[q->id] = q;
		//cout << q->id - start << " ";

		//connect q with the trap state
		for (int l = firstLabelId; l <= lastLabelId; ++l){
			q->addEdge(l,trap);
		}
	}

	return q;
}


//! \brief creates a dot output of the graph
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void Graph::toDot(FILE* out, string title) const {

		fprintf(out, "digraph D {\n");
		fprintf(out, "graph [fontname=\"Helvetica\", label=\"");
		fprintf(out, "%s", title.c_str());
		fprintf(out, "\"];\n");
		fprintf(out, "node [fontname=\"Helvetica\" fontsize=10];\n");
		fprintf(out, "edge [fontname=\"Helvetica\" fontsize=10];\n");


		for (list<int>::const_iterator n = initialNodes.begin(); n!= initialNodes.end(); ++n){
			// an arrow indicating the initial state
			fprintf(out, "  q_%d [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"];\n", *n);
			fprintf(out, "  q_%d -> %d [minlen=\"0.5\"];\n", *n, *n);
		}

		for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
			fprintf(out, "  %d [label=\"%d\\n %s\"]\n", n->second->id, n->second->id, n->second->formula->toString().c_str());

			for (int i = firstLabelId; i <= lastLabelId; ++i){
				for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
					//fprintf(out, "  %d -> %d [label=\"%s\"]\n", n->second->id, (*s)->id, id2label[i].c_str());
					fprintf(out, "  %d -> %d [label=\"%c%s\"]\n", n->second->id, (*s)->id, inout[i] ,id2label[i].c_str());
				}
			}
		}

		for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
			fprintf(out, "  %d [label=\"%d\\n %s\", style=dashed, color=red]\n", n->second->id, n->second->id, n->second->formula->toString().c_str());

//			for (int i = firstLabelId; i <= lastLabelId; ++i){
//				for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
//					fprintf(out, "  %d -> %d [label=\"%c%s\", style=dashed, color=red]\n", n->second->id, (*s)->id, inout[i], id2label[i].c_str());
//				}
//			}

			fprintf(out, "  %d -> %d [style=dashed, color=red]\n", n->second->id, trap->id);

		}

		fprintf(out, "}\n");
}


//! \brief creates a dot output of the graph (result of intersection or union)
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void Graph::toDotPair(FILE* out, string title) const {

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

void Graph::printInterface(ostream& o) const {
	o << "INTERFACE\n";
	o << "  INPUT\n";

	bool first = true;
	for (int i = firstLabelId; i <= lastLabelId; ++i){
		assert(id2label.find(i) != id2label.end());
		assert(inout.find(i) != inout.end());
		if (inout[i] == '?' && first){
		//if (i == firstInputId){
			o << "    " << id2label[i];
			first = false;
			continue;
		}
		if (inout[i] == '?' && !first){
		//else{
			o << ", " << id2label[i];
		}
	}

	first = true;
	o << ";\n  OUTPUT\n";
	for (int i = firstLabelId; i <= lastLabelId; ++i){
		assert(id2label.find(i) != id2label.end());
		assert(inout.find(i) != inout.end());
		if (inout[i] == '!' && first){
		//if (i == firstOutputId){
			o << "    " << id2label[i];
			first = false;
			continue;
		}
		if (inout[i] == '!' && !first){
		//else{
			o << ", " << id2label[i];
		}
	}

	o << ";\n";
}

void Graph::printInitialNodes(ostream& o) const{
	o << "\nINITIALNODES ";
	for (list<int>::const_iterator n = initialNodes.begin(); n!= initialNodes.end(); ++n){
		if (n == initialNodes.begin()){
			o << " " << *n;
		}
		else {
			o << ", " << *n;
		}
	}
}

void Graph::printGlobalFormulaForComplement(ostream& o) const{

	o << ";\n\nGLOBALFORMULA ";
	getGlobalFormulaForComplement(o);
	o << ";\n";

}

void Graph::printGlobalFormula(ostream& o) const{

	o << ";\n\nGLOBALFORMULA ";
	getGlobalFormula(o);
	o << ";\n";

}

void Graph::getGlobalFormulaForComplement(ostream& o) const{
	assert(globalFormula);

	Formula* g = new FormulaNOT(globalFormula);
	o << g->toString();

	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
		assert(nodes.find(n->second->id) == nodes.end()); //elements in addedNodes-map are not yet in nodes-map

		//generate new global formula
		o << " + " << intToString(n->second->id);

	}
}

void Graph::getGlobalFormula(ostream& o) const{
	assert(globalFormula);

	o << globalFormula->toString();

	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
		assert(nodes.find(n->second->id) == nodes.end()); //elements in addedNodes-map are not yet in nodes-map

		//generate new global formula
		o << " * ~" << intToString(n->second->id);
	}
}


void Graph::printNodes(ostream& o) const{
	o << "\nNODES\n";

    //print all nodes
	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n){
		o << "  " << n->first << ": " << n->second->formula->toString() << endl;
		for (int i = firstLabelId; i <= lastLabelId; ++i){
			for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				assert(id2label.find(i) != id2label.end());
				o << "    " << id2label[i] << " -> " << (*s)->id << endl;
			}
		}
		o << endl;
	}

    //print all addednodes
	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n){
		o << "  " << n->first << ": " << n->second->formula->toString() << endl;
		for (int i = firstLabelId; i <= lastLabelId; ++i){
			for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				assert(id2label.find(i) != id2label.end());
				o << "    " << id2label[i] << " -> " << (*s)->id << endl;
			}
		}
		o << endl;
	}
}

void Graph::printNodepairs(ostream& o) const{
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

    //print all addednodes
	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n){
		o << "  " << n->first << ": " << n->second->formula->toString() << endl;
		for (int i = firstLabelId; i <= lastLabelId; ++i){
			for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				assert(id2label.find(i) != id2label.end());
				o << "    " << id2label[i] << " -> " << (*s)->id << endl;
			}
		}
		o << endl;
	}
}

/// prints the Complement
void Graph::printComplement(ostream& o) const{
	printInterface(o);
	printInitialNodes(o);
	printGlobalFormulaForComplement(o);
	printNodes(o);
}

/// prints the Intersection
void Graph::printIntersection(ostream& o) const{
	printInterface(o);
	printInitialNodes(o);
	//printGlobalFormulaForComplement(o);
	printNodepairs(o);
}

/// prints the graph
void Graph::print(ostream& o) const{
	printInterface(o);
	printInitialNodes(o);
	printGlobalFormula(o);
	printNodes(o);
}


int Graph::getSizeOfAddedNodes(){
	return addedNodes.size();
}

//
///// adds a label to the graphs
//void Graph::addLabel(string label, int id) {
//
//	// pair-return for insert
//	pair<map<string, int>::iterator,bool> success;
//
//	success = label2id.insert(pair<string,int>(label, id)); //label2id[label] = id;
//
//	if(success.second == true){
//		id2label[id] = label;
//	}
//
//}


