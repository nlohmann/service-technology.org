#include <config.h>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
//#include "cmdline.h"
#include "GraphComplement.h"
#include "FormulaTree.h"
#include "helpers.h"
#include <time.h>

#include "cmdline.h"

/// the command line parameters
extern gengetopt_args_info args_info;

using std::map;
using std::string;
using std::vector;

extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;

extern int firstLabelId; //all labels including tau
extern int lastLabelId;

int start;
int treeNumber;

using namespace std;

///constructor
GraphComplement::GraphComplement() : trap(NULL){
	root = new FormulaTree();
}

///destructor
GraphComplement::~GraphComplement(){

	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
		delete n->second;
	}
}


void GraphComplement::complement(){
	makeTotal();
	if (args_info.complement_arg == 0){
		makeComplete_fast();
	}
	if (args_info.complement_arg == 1){
		makeComplete_stupid();
	}
	if (args_info.complement_arg == 2){
		makeComplete_efficient();
	}
	generateGlobalFormula();
}

void GraphComplement::generateGlobalFormula(){
	//generate global formula

	globalFormulaAsString = "~(" + globalFormulaAsString + ")";
	Formula *h = new FormulaNOT(globalFormula);
	globalFormula = h;

	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
		    assert(nodes.find(n->second->id) == nodes.end()); //elements in addedNodes-map are not yet in nodes-map
		//	nodes[n->second->id] = n->second; //add new nodes to nodes-map

			//append the new nodes to the global formula;
		    globalFormulaAsString = globalFormulaAsString + " + " + intToString(n->second->id);

			const Formula *f = new FormulaNUM(n->second->id);
			Formula *g = new FormulaOR(globalFormula, f) ;
			globalFormula = g;
	}
}

void GraphComplement::makeTotal(){

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



void GraphComplement::makeComplete_fast() {
//	cout << "FAST" << endl;

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
			assert(n->second->outEdges[i].size() > 0); //holds if the graph is total
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
}

void GraphComplement::makeComplete_stupid() {
	//cout << "STUPID" << endl;
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
					//generate new node q with the formula f and add edge with label i to it

					Node *q = getNode_stupid(f);
					n->second->addEdge(i,q);
					//printf("added: %d -%s-> %d   forumula: %s\n", n->second->id, id2label[i].c_str(),q->id, q->formula->toString().c_str());

				}
				else {
					delete f;
				}
			}
		}
	}
}

void GraphComplement::makeComplete_efficient() {
//	cout << "EFFICIENT" << endl;

//	start = 0;

	assert(NULL != trap);
	assert(initialNodes.size()!=0);
	//treeNumber = 0;

	/* complete the initial nodes:
	 * if the formula of a initial node p is not equal true:
	 * (1) insert an new initial node q with successors in the trap state and
	 * (2) replace the global formula chi with chi = AND(chi,NOT(q))
	 */
	list<int> addedInitialNodes;
	for (list<int>::const_iterator n = initialNodes.begin(); n!= initialNodes.end(); ++n){
		Formula *g = nodes[*n]->formula->getCopy();
		Formula *f = new FormulaNOT(g);

//		cout << "makeComplete(): initNode:" << nodes[*n]->id << " " << nodes[*n]->formula->toString() << endl;
		if(f->isSatisfiable()){
			Node *q = getNode(f);
			addedInitialNodes.push_back(q->id);
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
			assert(n->second->outEdges[i].size() > 0); //holds if the graph is total
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
//				cout << "===========================================\n";
//				cout << "makeComplete(): Node:" << n->second->id << "  " << f->toString() << "  for " << id2label[i] << "-successorts " << endl;
				if(f->isSatisfiable()){
					Node *q = getNode(f);
					n->second->addEdge(i,q);
				}
				else {
					delete f;
				}
			}
		}
	}
//	string filename = "FormulaTree" + intToString(treeNumber++) + ".pdf";
//	printFormulaTree(root, filename);
}

Node* GraphComplement::getNode(Formula *f){
	Node *q = searchNode(f, root);
//	cout << "getNode(...): Node" << q->id << "  " << q->formula->toString() << endl;
	return q;
}

Node* GraphComplement::getNode_stupid(Formula *f){
	bool found = false;
	Node *q ;
	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n){
		if (formulaFound(f, n->second->formula)){
			found = true;
			q = n->second;
			break;
		}
	}

	if (!found){
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

bool GraphComplement::formulaFound(Formula *f, Formula *g){
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
	return true;
}

Node* GraphComplement::searchNode(Formula *f, FormulaTree *n){
    if(n->satAssignment != NULL){
//    	cout << "searchNode(...): compare with assignment " << assignmentToString(n->satAssignment) << endl;
    	if(f->isSatisfiable(n->satAssignment)){
//    		cout << "searchNode(...): go yes" << endl;
    		return searchNode(f, n->yes);
    	}
    	else{
//    		cout << "searchNode(...): go no" << endl;
    		return searchNode(f, n->no);
    	}
    }

    else {
    	if(n->node != NULL){
			assert(!n->yes);
			assert(!n->no);

    		//is f equals to n->node->formula?
    		Formula * h1 = new FormulaOR(f,n->node->formula);
    		Formula * h2 = new FormulaOR(new FormulaNOT(f), new FormulaNOT(n->node->formula));
    		Formula * h3 = new FormulaAND(h1,h2);
//			cout << "searchNode(...): compare " << f->toString() << " and " << n->node->formula->toString() << endl;
    		vector<bool> * assignment = h3->getSatisfyingAssignment();
//    		cout << "searchNode(...): satisfying assignment: " << assignmentToString(assignment) << endl;
    		if(assignment){ //there is a satisfying assignment for h3 -> the two formulas are not equal
//    			cout << "searchNode(...): Node doesn't exist yet!" << endl;
    			Node *q;
    			n->satAssignment = assignment;

    			if(n->node->formula->isSatisfiable(assignment)){
    				assert(!f->isSatisfiable(assignment));
    				n->yes = new FormulaTree(n->node);
    				n->node = NULL;

    				q = new Node(f);
    				addedNodes[q->id] = q;
//    				cout << start++ << " ";
    				//connect q with the trap state
    				for (int l = firstLabelId; l <= lastLabelId; ++l){
    					q->addEdge(l,trap);
    				}
    				n->no = new FormulaTree(q);
    			}

    			else{
    				assert(f->isSatisfiable(assignment));
    				assert(!n->node->formula->isSatisfiable(assignment));
    				n->no = new FormulaTree(n->node);
    				n->node = NULL;

    				q = new Node(f);
    				addedNodes[q->id] = q;
//    				cout << start++ << " ";
    				//connect q with the trap state
    				for (int l = firstLabelId; l <= lastLabelId; ++l){
    					q->addEdge(l,trap);
    				}
    				n->yes = new FormulaTree(q);
    			}
    			return q;

    		}
    		else{
//    			cout << "searchNode(...): Node already exists!" << endl;
    			return n->node;
    		}
    	}
    }

	//init
    assert(!n->node);
    assert(!n->satAssignment);
	n->node = new Node(f);
	addedNodes[n->node->id] = n->node;
	for (int l = firstLabelId; l <= lastLabelId; ++l){
		n->node->addEdge(l,trap);
	}
	return n->node;
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


//void GraphComplement::printGlobalFormulaForComplement(ostream& o) const{
//
//	o << ";\n\nGLOBALFORMULA ";
//	getGlobalFormulaForComplement(o);
//	o << ";\n";
//
//}


//void GraphComplement::getGlobalFormulaForComplement(ostream& o) const{
//	assert(globalFormula);
//
//	//Formula* g = new FormulaNOT(globalFormula);
//	o << "~(" << globalFormula->toString() << ")";
//
//	for (map<int, Node*>::const_iterator n = addedNodes.begin(); n != addedNodes.end(); ++n) {
//		assert(nodes.find(n->second->id) == nodes.end()); //elements in addedNodes-map are not yet in nodes-map
//
//		//generate new global formula
//		o << " + " << intToString(n->second->id);
//
//	}
//
//}


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
	printGlobalFormula(o);
	printNodes(o);
	printAddedNodes(o);
}


int GraphComplement::getSizeOfAddedNodes(){
	return addedNodes.size();
}
