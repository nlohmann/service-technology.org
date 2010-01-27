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

extern map<string, int> label2id;
extern map<int, string> id2label;

extern int firstLabelId; //all labels including tau
extern int firstInputId; //input labels
extern int firstOutputId;//output labels

extern int lastLabelId;
extern int lastInputId;
extern int lastOutputId;

using namespace std;
///constructor
Graph::Graph() : trap(NULL), globalFormula(NULL){

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

	// dieser fall kann nicht (mehr) eintreten, das makeComplete immer nach makeTotal aufgerufen wird
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
	}

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
					//generate new node q with the formula f and add edge with label i to it
					Node *q = new Node(f);
					addedNodes[q->id] = q;
					n->second->addEdge(i,q);

					//connect q with the trap state
					for (int l = firstLabelId; l <= lastLabelId; ++l){
						q->addEdge(l,trap);
					}

					//printf("added: %d -%s-> %d   forumula: %s\n", n->second->id, id2label[i].c_str(),q->id, q->formula->toString().c_str());
				}
				else {
					delete f;
				}

			}
		}
	}

	//generate global formula
//	Formula *gf = new FormulaNOT(globalFormula);
//	globalFormula = gf;
//	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
//		    assert(nodes.find(n->second->id) == nodes.end()); //elements in addedNodes-map are not yet in nodes-map
//		//	nodes[n->second->id] = n->second; //add new nodes to nodes-map
//
//			//generate new global formula;
//			const Formula *g = new FormulaNUM(n->second->id);
//			//const Formula *g = new FormulaNOT(f);
//			Formula *h = new FormulaOR(globalFormula, g) ;
//			globalFormula = h;
//	}
}


//! \brief creates a dot output of the graph
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void Graph::toDot(FILE* out, string title) const {
//void Graph::toDot(string filename, string title) const {


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
					fprintf(out, "  %d -> %d [label=\"%s\"]\n", n->second->id, (*s)->id, id2label[i].c_str());
				}
			}
		}

		for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
			fprintf(out, "  %d [label=\"%d\\n %s\", style=dashed, color=red]\n", n->second->id, n->second->id, n->second->formula->toString().c_str());

//			for (int i = firstLabelId; i <= lastLabelId; ++i){
//				for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
//					fprintf(out, "  %d -> %d [label=\"%s\", style=dashed, color=red]\n", n->second->id, (*s)->id, id2label[i].c_str());
//				}
//			}

			fprintf(out, "  %d -> %d [style=dashed, color=red]\n", n->second->id, trap->id);

		}

		fprintf(out, "}\n");
		//fclose (out);


		// prepare dot command line for printing
		//string cmd = "dot -Tpng \"" + dotFile + "\" -o \""+ pngFile + "\"";

}

void Graph::printInterface(ostream& o) const {
	o << "INTERFACE\n";
	o << "  INPUT\n";

	for (int i = firstInputId; i <= lastInputId; ++i){
		assert(id2label.find(i) != id2label.end());
		if (i == firstInputId){
			o << "    " << id2label[i];
		}
		else{
			o << ", " << id2label[i];
		}
	}

	o << ";\n  OUTPUT\n";
	for (int i = firstOutputId; i <= lastOutputId; ++i){
		assert(id2label.find(i) != id2label.end());
		if (i == firstOutputId){
			o << "    " << id2label[i];
		}
		else{
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

	o << ";\n\nGLOBALFORMULA " << getGlobalFormulaForComplement() << ";\n";

}

void Graph::printGlobalFormula(ostream& o) const{

	o << ";\n\nGLOBALFORMULA " << getGlobalFormula() << ";\n";

}

string Graph::getGlobalFormulaForComplement() const{
	assert(globalFormula);

	Formula* g = new FormulaNOT(globalFormula);
	string s = g->toString();

	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
		assert(nodes.find(n->second->id) == nodes.end()); //elements in addedNodes-map are not yet in nodes-map

		//generate new global formula;
		s = s + " + " + intToString(n->second->id);

	}

	return s;
}

string Graph::getGlobalFormula() const{
	assert(globalFormula);
	string s = globalFormula->toString();


	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
		assert(nodes.find(n->second->id) == nodes.end()); //elements in addedNodes-map are not yet in nodes-map

		//generate new global formula;
		s = s + " * ~" + intToString(n->second->id);

	}

	return s;
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

void Graph::printComplement(ostream& o) const{

	printInterface(o);
	printInitialNodes(o);
	printGlobalFormulaForComplement(o);
	printNodes(o);
}

/// print the graph
void Graph::print(ostream& o) const{

	printInterface(o);
	printInitialNodes(o);
	printGlobalFormula(o);
	printNodes(o);


//	o << "INTERFACE\n";
//	o << "  INPUT\n";
//
//	for (int i = firstInputId; i <= lastInputId; ++i){
//		assert(id2label.find(i) != id2label.end());
//		if (i == firstInputId){
//			o << "    " << id2label[i];
//		}
//		else{
//			o << ", " << id2label[i];
//		}
//	}
//
//	o << ";\n  OUTPUT\n";
//	for (int i = firstOutputId; i <= lastOutputId; ++i){
//		assert(id2label.find(i) != id2label.end());
//		if (i == firstOutputId){
//			o << "    " << id2label[i];
//		}
//		else{
//			o << ", " << id2label[i];
//		}
//	}
//
//	o << ";\n\nINITIALNODES ";
//	for (list<int>::const_iterator n = initialNodes.begin(); n!= initialNodes.end(); ++n){
//		if (n == initialNodes.begin()){
//			o << " " << *n;
//		}
//		else {
//			o << ", " << *n;
//		}
//	}
//
//	//o << ";\n\nGLOBALFORMULA " << globalFormula->toString();
//
//	o << ";\n\nGLOBALFORMULA " << getGlobalFormulaForComplement();
//    //o << "\n" << getGlobalFormulaForComplement() << endl;
//
//	o << ";\n\nNODES\n";
//
//    //print all nodes
//	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n){
//		o << "  " << n->first << ": " << n->second->formula->toString() << endl;
//		for (int i = firstLabelId; i <= lastLabelId; ++i){
//			for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
//				assert(id2label.find(i) != id2label.end());
//				o << "    " << id2label[i] << " -> " << (*s)->id << endl;
//			}
//		}
//		o << endl;
//	}
//
//    //print all addednodes
//	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n){
//		o << "  " << n->first << ": " << n->second->formula->toString() << endl;
//		for (int i = firstLabelId; i <= lastLabelId; ++i){
//			for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
//				assert(id2label.find(i) != id2label.end());
//				o << "    " << id2label[i] << " -> " << (*s)->id << endl;
//			}
//		}
//		o << endl;
//	}
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


