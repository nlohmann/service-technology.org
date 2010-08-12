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
#include <cassert>
#include <cstdio>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include "Graph.h"
#include "helpers.h"
#include <time.h>
#include "cmdline.h"
#include <list>
#include <set>

using std::map;
using std::string;
using std::vector;

extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;

extern int firstLabelId; //all labels including tau

/// the command line parameters
extern gengetopt_args_info args_info;

using namespace std;


///constructor
Graph::Graph() : globalFormula(NULL){

}

///destructor
Graph::~Graph(){

	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
		delete n->second;
	}

	delete globalFormula;
}


/// add a node to the graph
void Graph::addInitialNode(int id){
	initialNodes.push_back(id);
}

void Graph::generateTestCases(){
	assert(initialNodes.size()==1);

	int initId = *(initialNodes.begin());
	Node* initialNode = nodes[initId];
	maximalEditDistance = 1;

	toTestOg();

	//TNode::init();

	testCases = dfs(initialNode,0);

	//matching fehlt noch!!!

}

vector<TNode*> Graph::dfs(Node* q, int ed, bool fromShadowNode){
	cout << endl << endl;
	cout << "start dfs for " << q->id << endl;

//	if(fromShadowNode){
//		cout << "fromShadowNode == true" << endl;
//	}
//	else{
//		cout << "fromShadowNode == false" << endl;
//	}
	//if(q->visited){cout << "visited" << endl;}

	map<Node*, vector<TNode*> > allPartialTestCases [label2id.size()]; // = new map<Node*, vector<TNode*> >[label2id.size()];
	map<Node*, vector<TNode*> > partialTestCasesMap; //contains the partial test cases of the successor nodes

	if (q->visited && !fromShadowNode){
		vector<TNode*> partialTestCases;
		cout << "start backtracking" << endl;
		return partialTestCases; //empty list
	}

	if (shadowNodes.find(q->id) == shadowNodes.end()){	//if q is not a shadow node
		q->visited = true;
	}

	list<set<int> > assignments = q->formula->toMinimalDnf();

	bool finish = true;
	int numberOfSuccessors = 0;

	if(shadowNodes.find(q->id) != shadowNodes.end()){  //if q is a shadow node
//		cout << q->id << " is a shadowNode" << endl;
		fromShadowNode = true;
	}
	else {
//		cout << q->id << " is NOT a shadowNode" << endl;
		fromShadowNode = false;
	}

	//for all successor nodes ...
	for (int i = firstLabelId; i < id2label.size(); ++i){
		int label = i;
		cout << "   " << q->id << ": " << id2label[i];
		for (set<Node*>::const_iterator s = q->outEdges[i].begin(); s != q->outEdges[i].end(); ++s){

			Node* successorNode = *s;
			++numberOfSuccessors;
			//cout << "successor id: " << successorNode->id << endl;

			if(shadowNodes.find(successorNode->id) == shadowNodes.end()){  //if successorNode is not a shadow node

				finish = false;
				allPartialTestCases[label][successorNode] = dfs(successorNode, ed, fromShadowNode);
				cout << "\n   " << q->id << ": number of partial test cases for " << id2label[label] << ": " << allPartialTestCases[label][successorNode].size() << endl;
				cout << "   " << q->id << ": continue dfs for " << q->id << endl;
				//partialTestCasesMap[successorNode] = dfs(successorNode, ed);
			}

			else {
				assert(shadowNodes.find(successorNode->id) != shadowNodes.end()); //successorNode is a shadow node

				if (ed < maximalEditDistance){

					assert(ed+1 <= maximalEditDistance);
					finish = false;
					allPartialTestCases[label][successorNode] = dfs(successorNode, ed+1);
					//partialTestCasesMap[successorNode] = dfs(successorNode, ed+1);
				}
				else{
					//since the shadow node of q is not used for the test cases
					//remove the assignment Tau from the list of q's DNF (assignments)

					//cout << "id: " << q->id << "  minimal DNF (before deletion): " << dnfToString(assignments) << endl;
					bool found = false;
					for(list<set<int> >::iterator iter = assignments.begin(); iter != assignments.end(); ++iter){
						set<int> currentSet = *iter;
						if (currentSet.find(label2id[Tau]) != currentSet.end()){ //label tau in the set
							assert(currentSet.size() == 1);
							found = true;
							assignments.erase(iter); //delete current set
							break;
						}
					}
					//cout << "id: " << q->id << "  minimal DNF (after deletion): " << dnfToString(assignments) << endl << endl;
					assert(found);
				}
			}
		}
	}

	/* if there are no successors for the current node q or in the previous for-loop
	 * or the dfs-procedure was not called for any successor of the current node:
	 * start with backtracking
	 */
	if (finish){
		q->visited = false;
		TNode* root = new TNode (q->id);
		vector<TNode*> partialTestCases;
		partialTestCases.push_back(root);
		cout << "   start backtracking: return only one test case containing node " << root->id <<  endl;
		return partialTestCases; //empty list
	}

	vector<TNode*> partialTestCases = combine(allPartialTestCases, assignments, q);
	cout << "number of test cases in " << q->id << ": " << partialTestCases.size() << endl;
	q->visited = false;
	return partialTestCases; //all partial test cases starting in q

}

//allPartialTestCases: partial test cases of the successor nodes
vector<TNode*> Graph::combine(map<Node*, vector<TNode*> >* allPartialTestCases, list< set<int> > assignments, Node* q){

	cout << endl << q->id << ": start combine\n";
	cout << "DNF: " << dnfToString(assignments) << endl;
	printAllPartialTestCases(allPartialTestCases);

	vector<TNode*> partialTestCases; //all partial test cases starting in the current node q

	// for all assignments do ...
	for(list<set<int> >::const_iterator iter1 = assignments.begin(); iter1 != assignments.end(); ++iter1){

		set<int> currentAssignment = *iter1;

		if (shadowNodes.find(q->id) != shadowNodes.end()){ //if q is a shadow node
			assert(currentAssignment.size() == 1);
			int label = *(currentAssignment.begin());

			vector<TNode*> result = combineForShadowNode(allPartialTestCases, label, q);
			partialTestCases.insert(partialTestCases.end(), result.begin(), result.end());
		}

		else { //q is not a shadow node

			int max = 1;
			map<int,Node*> affectedNodes;

			/* determine all affected nodes for the current assignment and
			 * the maximal number of test cases of the affected nodes */
			for(set<int>::const_iterator iter2 = currentAssignment.begin(); iter2 != currentAssignment.end(); ++iter2){

				int label = *iter2;

				// there is no edges labled with final
				if (label != label2id[Final]){
					assert(q->outEdges[label].size() == 1);

					Node* affectedNode = *(q->outEdges[label].begin()); //since q is not a shadow node, there is only one outgoing edge with the current label
					affectedNodes[label] = affectedNode;

					int size = allPartialTestCases[label][affectedNode].size();

					if(size > max){
						max = size;
					}
				}
			}

			//combine the test cases of the affected nodes
			vector<TNode*> result = combineForNormalNode(allPartialTestCases, affectedNodes, max, q);
			partialTestCases.insert(partialTestCases.end(), result.begin(), result.end());
		}
	}
	cout << "end combine\n";
	return partialTestCases;
}



vector<TNode*> Graph::combineForShadowNode(map<Node*, vector<TNode*> >* allPartialTestCases, int label, Node* q){

	cout << q->id << ": start combineForShadowNode" << endl;
	assert(label < label2id.size());

	vector<TNode*> partialTestCases;

	//for each x-labeled edge
	cout << "label: " << label;

	for (set<Node*>::iterator s = q->outEdges[label].begin(); s != q->outEdges[label].end(); ++s){

		Node* successorNode = *s;

		assert(allPartialTestCases[label].find(successorNode) != allPartialTestCases[label].end());
		vector<TNode*> tc_list = allPartialTestCases[label][successorNode]; //vector of the partial test cases in the successor node

		TNode* oldRoot;
		TNode* newRoot;

		assert(tc_list.size() != 0);

//		if(tc_list.size() == 0){ //the tc_list is empty if the test case ends in successor
//			oldRoot = new TNode(successorNode->id);
//			newRoot = new TNode(q->id);
//			partialTestCases.push_back(newRoot);
//			newRoot->addEdge(label, oldRoot);
//		}
//
//		else {
			//for each partial test case of the successor node
			for (vector<TNode*>::const_iterator t = tc_list.begin(); t != tc_list.end(); ++t){
				oldRoot = *t;  //TODO:the successor node?
				newRoot = new TNode(q->id);
				partialTestCases.push_back(newRoot);
				newRoot->addEdge(label, oldRoot);
			}
//		}
	}

	cout << "end: combineForShadowNode" << endl;
	return partialTestCases;

}

//combine the test cases of the affected nodes
vector<TNode*> Graph::combineForNormalNode(map<Node*, vector<TNode*> >* allPartialTestCases, map<int, Node*> affectedNodes, int maxSize, Node* q){

	cout << "start: combineForNormalNode" << endl;

	vector<TNode*> partialTestCases;

	for(int i = 0; i < maxSize; ++i){

		TNode *newRoot = new TNode(q->id);
		partialTestCases.push_back(newRoot);

		for(map<int, Node*>::const_iterator iter = affectedNodes.begin(); iter != affectedNodes.end(); ++iter){
			Node* affectedNode = iter->second;
			int label = iter->first;
			assert(label < label2id.size() && (label >= firstLabelId || label == label2id[Final]));

			TNode* oldRoot;
			vector<TNode*> affectedTestCases;

			if (label != label2id[Final]){
				affectedTestCases = allPartialTestCases[label][affectedNode];
				assert(affectedTestCases.size() != 0);
			}

			/* if label == label2id[Final] then affectedTestCases is the empty vector */
			if (label == label2id[Final]){
				assert(affectedTestCases.size() == 0);
			}

//			if (affectedTestCases.size() == 0){
//				oldRoot = new TNode(affectedNode->id);
//			}
//
//			else {
				if (i < affectedTestCases.size()){
					oldRoot = affectedTestCases[i];
				}
				else {
					oldRoot = affectedTestCases[1];
//			}
			}

			newRoot->addEdge(label, oldRoot);
		}
	}

	//cout << "end: combineForNormalNode" << endl;
	return partialTestCases;
}


void Graph::FormulasToDnf(){

	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n){
		n->second->formula->toMinimalDnf();
	}

}


void Graph::toTestOg(){
	bool insert_selected = false;
	bool delete_selected = false;
	bool rename_selected = false;
	bool all_selected = false;

	if (args_info.editOperator_given == 0){
		all_selected = true;  //default: all
	}
	else {
		for (unsigned int j = 0; j<args_info.editOperator_given; ++j){
				switch(args_info.editOperator_arg[j]) {

				case (editOperator_arg_insert): {insert_selected = true; break;}
				case (editOperator_arg_delete): {delete_selected = true; break;}
				case (editOperator_arg_rename): {rename_selected = true; break;}
				case (editOperator_arg_all):    {all_selected = true; break;}
				}
		}
	}

//	if (insert_selected){ cout << "insert is activated" << endl;}
//	if (delete_selected){ cout << "delete is activated" << endl;}
//	if (rename_selected){ cout << "rename is activated" << endl;}
//	if (all_selected){ cout << "all is activated" << endl;}


	for (map<int, Node*>::iterator n = nodes.begin(); n != nodes.end(); ++n){
		Node* currentNode = n->second;


		Node* shadowNode = new Node(NULL, true);
		assert(shadowNode->formula == NULL);
//		addEdge(label2id[Tau], shadowNode); // q -tau-> q'
//		shadowNodes[shadowNode->id] = shadowNode;

		if (insert_selected || all_selected){
			currentNode->applyInsertion(shadowNode);
		}

		if (delete_selected || all_selected){
			currentNode->applyDeletion(shadowNode);
		}

		if (rename_selected || all_selected){
			currentNode->applyRenaming(shadowNode);
		}


		bool first = true;
		for (int i = firstLabelId; i < id2label.size(); ++i){

			if(shadowNode->outEdges[i].size()>0){

				if(first){
					assert(shadowNode->formula == NULL);
					shadowNode->formula = new FormulaLit(i);
					first = false;
				}
				else{
					Formula *f = new FormulaOR(shadowNode->formula, new FormulaLit(i));
					shadowNode->formula = f;
				}
			}
		}

		if (shadowNode->formula == NULL){
			// since no outgoing edges are created the shadow node is not needed
			delete shadowNode;


		}
		else{ //connect currentNode with its shadow node
			currentNode->addEdge(label2id[Tau], shadowNode); // q -tau-> q'
			shadowNodes[shadowNode->id] = shadowNode;
			Formula *f = new FormulaOR(currentNode->formula, new FormulaLit(label2id[Tau]));
			currentNode->formula = f;
		}
	}
}

void Graph::printInterface(ostream& o) const {
	o << "\nINTERFACE\n";
	o << "  INPUT\n";

	bool first = true;
	for (unsigned int i = firstLabelId; i < id2label.size(); ++i){
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
	for (unsigned int i = firstLabelId; i < id2label.size(); ++i){
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


void Graph::printGlobalFormula(ostream& o) const{

	//o << ";\n\nGLOBALFORMULA " << globalFormulaAsString << ";\n";
	o << ";\n\nGLOBALFORMULA " << globalFormula->toString() << ";\n";

}


void Graph::printNodes(ostream& o) const{
	o << "\nNODES\n";

    //print all nodes
	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n){
		o << "  " << n->first << ": " << n->second->formula->toString() << endl;
		for (unsigned int i = firstLabelId; i < id2label.size(); ++i){
			for (set<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				assert(id2label.find(i) != id2label.end());
				o << "    " << id2label[i] << " -> " << (*s)->id << endl;
			}
		}
		o << endl;
	}

    //print all shadow nodes
	for (map<int, Node*>::const_iterator n = shadowNodes.begin(); n != shadowNodes.end(); ++n){
		o << "  " << n->first << ": " << n->second->formula->toString() << endl;
		for (unsigned int i = firstLabelId; i < id2label.size(); ++i){
			for (set<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				assert(id2label.find(i) != id2label.end());
				o << "    " << id2label[i] << " -> " << (*s)->id << endl;
			}
		}
		o << endl;
	}
}


/// prints the graph
void Graph::toEaa(ostream& o) const{

	printInterface(o);
	printInitialNodes(o);
	printGlobalFormula(o);
	printNodes(o);
}

//! \brief creates dot files for given test cases
//! \param out: prefix of the output files
//! \param title: a string containing the title for the graph to be shown in the image (optional)
//! \param testCases: vector of test cases
//void Graph::ToDot_TestCases(FILE* out, string title, vector<TNode*> testCases) const {
//
//	toDot_Header(out, title);
//	toDot_TNodes(out);
//	fprintf(out, "}\n");
//}

//! \brief creates a dot output of the graph
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void Graph::toDot(FILE* out, string title ) const {

	toDot_Header(out, title);
	toDot_Nodes(out);
	fprintf(out, "}\n");
}

//! \brief creates the header for the dot output
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void Graph::toDot_Header(FILE* out, string title) const {

	fprintf(out, "digraph D {\n");
	fprintf(out, "graph [fontname=\"Helvetica\", label=\"");
	fprintf(out, "%s", title.c_str());
	fprintf(out, "\"];\n");
	fprintf(out, "node [fontname=\"Helvetica\" fontsize=10];\n");
	fprintf(out, "edge [fontname=\"Helvetica\" fontsize=10];\n");

}

//! \brief creates the nodes of a tests case for the dot output
//! \param out: output file
//void Graph::toDot_TNodes(FILE* out, TNode root) const{
//
//}

//! \brief creates the nodes for the dot output
//! \param out: output file
void Graph::toDot_Nodes(FILE* out) const{
	for (list<int>::const_iterator n = initialNodes.begin(); n!= initialNodes.end(); ++n){
		// an arrow indicating the initial state
		fprintf(out, "  q_%d [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"];\n", *n);
		fprintf(out, "  q_%d -> %d [minlen=\"0.5\"];\n", *n, *n);
	}

	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
		fprintf(out, "  %d [label=\"%d\\n %s\"]\n", n->second->id, n->second->id, n->second->formula->toString().c_str());

		for (unsigned int i = firstLabelId; i < id2label.size(); ++i){
			for (set<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				//fprintf(out, "  %d -> %d [label=\"%s\"]\n", n->second->id, (*s)->id, id2label[i].c_str());
				fprintf(out, "  %d -> %d [label=\"%c%s\"]\n", n->second->id, (*s)->id, inout[i] ,id2label[i].c_str());
			}
		}
	}

	for (map<int, Node*>::const_iterator n = shadowNodes.begin(); n != shadowNodes.end(); ++n) {
		fprintf(out, "  %d [label=\"%d\\n %s\"]\n", n->second->id, n->second->id, n->second->formula->toString().c_str());

		for (unsigned int i = firstLabelId; i < id2label.size(); ++i){
			for (set<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				//fprintf(out, "  %d -> %d [label=\"%s\"]\n", n->second->id, (*s)->id, id2label[i].c_str());
				fprintf(out, "  %d -> %d [label=\"%c%s\"]\n", n->second->id, (*s)->id, inout[i] ,id2label[i].c_str());
			}
		}
	}
}
