#include <config.h>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
//#include "cmdline.h"
#include "GraphComplement.h"
#include "helpers.h"
#include <time.h>

/// the command line parameters
//extern gengetopt_args_info args_info;

using std::map;
using std::string;
using std::vector;

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
GraphComplement::GraphComplement() : trap(NULL){

}

///destructor
GraphComplement::~GraphComplement(){

//	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
//		delete n->second;
//	}

	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
		delete n->second;
	}

//	delete globalFormula;

}


void GraphComplement::complement(){
	makeTotal();
	makeComplete();
}

void GraphComplement::makeTotal(){
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



void GraphComplement::makeComplete() {

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

bool GraphComplement::formulaFounded(Formula *f, Formula *g){
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

Node* GraphComplement::getNode(Formula *f){
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
void GraphComplement::toDot(FILE* out, string title) const {

	toDot_Header(out, title);
	toDot_Nodes(out);
	toDot_addedNodes(out);
	fprintf(out, "}\n");
}

//! \brief creates the added nodes for the dot output
//! \param out: output file
void GraphComplement::toDot_addedNodes(FILE* out) const {

		for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
			fprintf(out, "  %d [label=\"%d\\n %s\", style=dashed, color=red]\n", n->second->id, n->second->id, n->second->formula->toString().c_str());

//			for (int i = firstLabelId; i <= lastLabelId; ++i){
//				for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
//					fprintf(out, "  %d -> %d [label=\"%c%s\", style=dashed, color=red]\n", n->second->id, (*s)->id, inout[i], id2label[i].c_str());
//				}
//			}

			fprintf(out, "  %d -> %d [style=dashed, color=red]\n", n->second->id, trap->id);

		}
}


void GraphComplement::printGlobalFormulaForComplement(ostream& o) const{

	o << ";\n\nGLOBALFORMULA ";
	getGlobalFormulaForComplement(o);
	o << ";\n";

}


void GraphComplement::getGlobalFormulaForComplement(ostream& o) const{
	assert(globalFormula);

	//Formula* g = new FormulaNOT(globalFormula);
	o << "~(" << globalFormula->toString() << ")";

	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
		assert(nodes.find(n->second->id) == nodes.end()); //elements in addedNodes-map are not yet in nodes-map

		//generate new global formula
		o << " + " << intToString(n->second->id);

	}

}


void GraphComplement::printAddedNodes(ostream& o) const{

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
void GraphComplement::print(ostream& o) const{
	printInterface(o);
	printInitialNodes(o);
	printGlobalFormulaForComplement(o);
	printNodes(o);
	printAddedNodes(o);
}


int GraphComplement::getSizeOfAddedNodes(){
	return addedNodes.size();
}
