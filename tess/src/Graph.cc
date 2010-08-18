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
#include <sstream>
#include <cstdio>
#include <cstring>
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
#include "Output.h"

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
Graph::Graph() {

}

///destructor
Graph::~Graph(){

	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
		delete n->second;
	}
}


/// add an initial node to the graph
void Graph::addInitialNode(int id){
	initialNodes.push_back(id);
}


void Graph::generateTestCases(){
	assert(initialNodes.size()==1);

	int initId = *(initialNodes.begin());
	Node* initialNode = nodes[initId];
	maximalEditDistance = 1;

	toTestOg();

	assert(TNode::getMaxId() == 0);

	testCases = dfs(initialNode,0);

	Output stdout("-", "");
	toEaa_TestCases(stdout);

	//matching fehlt noch!!!

}


vector<TestCase*> Graph::dfs(Node* q, int ed, bool fromShadowNode){
	cout << endl << endl;
	cout << "start dfs for " << q->id << endl;

	map<Node*, vector<TestCase*> > allPartialTestCases [label2id.size()]; // for each label l there is a map which contains the partial test cases for the successor nodes reaching by l
	map<Node*, vector<TestCase*> > partialTestCasesMap; //contains the partial test cases of the successor nodes

	// if the original operating guideline contains cycles
	if (q->visited && !fromShadowNode){
		assert(false);
		//TODO: es muss sichergestellt werden, dass im Testfall der Kreis auch existiert (ist zur Zeit nicht der Fall!)
		TestCase* tc = new TestCase (q->id);
		vector<TestCase*> partialTestCases;
		partialTestCases.push_back(tc);
		cout << "\n   visited: start backtracking - return only one test case containing node " << tc->root->idTestOg << "(" << tc->root->idOg << ")" <<  endl;

		return partialTestCases;
	}

	if (shadowNodes.find(q->id) == shadowNodes.end()){	//if q is not a shadow node
		q->visited = true;
	}

	list<set<int> > assignments = q->formula->toMinimalDnf();

	bool finish = true;

	if(shadowNodes.find(q->id) != shadowNodes.end()){  //if q is a shadow node
		fromShadowNode = true;
	}
	else {
		fromShadowNode = false;
	}

	//start dfs for all successor nodes of q
	for (int label = firstLabelId; label < id2label.size(); ++label){

		cout << "   " << q->id << ": " << id2label[label];

		for (set<Node*>::const_iterator s = q->outEdges[label].begin(); s != q->outEdges[label].end(); ++s){

			Node* successorNode = *s;

			if(shadowNodes.find(successorNode->id) == shadowNodes.end()){  //if successorNode is not a shadow node

				finish = false;
				allPartialTestCases[label][successorNode] = dfs(successorNode, ed, fromShadowNode);

				cout << "\n   " << q->id << ": number of partial test cases for " << id2label[label] << ": " << allPartialTestCases[label][successorNode].size() << endl;
				//printTestCases(allPartialTestCases[label][successorNode]);
				cout << "   " << q->id << ": continue dfs for " << q->id << endl;
			}

			else {
				assert(shadowNodes.find(successorNode->id) != shadowNodes.end()); //successorNode is a shadow node

				if (ed < maximalEditDistance){

					assert(ed+1 <= maximalEditDistance);
					finish = false;
					allPartialTestCases[label][successorNode] = dfs(successorNode, ed+1);

				}
				else{
					//since the shadow node of q is not used for the test cases
					//remove the assignment Tau from q's DNF (assignments)

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
		TestCase* tc = new TestCase(q->id);
		vector<TestCase*> partialTestCases;
		partialTestCases.push_back(tc);


		cout << "\n   finish: start backtracking - return only one test case containing node " << tc->root->idTestOg << "(" << tc->root->idOg << ")" <<  endl;
		assert(q->id == tc->root->idOg);
		assert(finalInDnf(assignments));
		return partialTestCases;
	}

	vector<TestCase*> partialTestCases = combine(allPartialTestCases, assignments, q);

	cout << "number of test cases in " << q->id << ": " << partialTestCases.size() << endl;
//	Output stdout("-", "");
//	toEaa_TestCases(stdout,partialTestCases);

	q->visited = false;
	return partialTestCases; //all partial test cases starting in q

}

//allPartialTestCases: partial test cases of the successor nodes
vector<TestCase*> Graph::combine(map<Node*, vector<TestCase*> >* allPartialTestCases, list< set<int> > assignments, Node* q){

	cout << endl << q->id << ": start combine\n";
	cout << "DNF: " << dnfToString(assignments) << endl;

	//printAllPartialTestCases(allPartialTestCases);
	vector<TestCase*> partialTestCases; //all partial test cases starting in the current node q

	// for all assignments do ...
	for(list<set<int> >::const_iterator iter1 = assignments.begin(); iter1 != assignments.end(); ++iter1){

		set<int> currentAssignment = *iter1;

		if (shadowNodes.find(q->id) != shadowNodes.end()){ //if q is a shadow node
			assert(currentAssignment.size() == 1);
			int label = *(currentAssignment.begin());

			vector<TestCase*> result = combineForShadowNode(allPartialTestCases, label, q);
			partialTestCases.insert(partialTestCases.end(), result.begin(), result.end());
		}

		else { //q is not a shadow node

			int max = 1;	//maximal number of partial test cases of the affected nodes
			map<int,Node*> affectedNodes;

			/* determine all affected nodes for the current assignment and
			 * the maximal number of test cases of the affected nodes */
			for(set<int>::const_iterator iter2 = currentAssignment.begin(); iter2 != currentAssignment.end(); ++iter2){

				int label = *iter2;

				// there is no edges labeled with final
				if (label != label2id[Final]){
					assert(q->outEdges[label].size() == 1);

					Node* affectedNode = *(q->outEdges[label].begin()); //since q is not a shadow node, there is only one outgoing edge with the current label
					affectedNodes[label] = affectedNode;

					int size = allPartialTestCases[label][affectedNode].size(); //number of partial test cases in the affected node

					if(size > max){
						max = size;
					}
				}
			}

			//combine the test cases of the affected nodes
			vector<TestCase*> result = combineForNormalNode(allPartialTestCases, affectedNodes, max, q);
			partialTestCases.insert(partialTestCases.end(), result.begin(), result.end());
		}
	}
	cout << "end combine\n";
	return partialTestCases;
}



vector<TestCase*> Graph::combineForShadowNode(map<Node*, vector<TestCase*> >* allPartialTestCases, int label, Node* q){

	cout << "   " << q->id << ": start combineForShadowNode" << endl;
	assert(label < label2id.size());
	assert(id2label[label] != Final);

	vector<TestCase*> partialTestCases;

	//for each x-labeled edge
	cout << "      label: " << id2label[label] << endl;

	for (set<Node*>::iterator s = q->outEdges[label].begin(); s != q->outEdges[label].end(); ++s){

		Node* successorNode = *s;

		assert(allPartialTestCases[label].find(successorNode) != allPartialTestCases[label].end());
		vector<TestCase*> tc_list = allPartialTestCases[label][successorNode]; //vector of the partial test cases in the successor node

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
			for (vector<TestCase*>::const_iterator t = tc_list.begin(); t != tc_list.end(); ++t){
				TestCase* currentTestCase = *t;
				currentTestCase->addNewRoot(q->id,label);
				partialTestCases.push_back(currentTestCase);
//				oldRoot = *t;  //TODO:the successor node?
//				newRoot = new TNode(q->id);
//				partialTestCases.push_back(newRoot);
//				newRoot->addEdge(label, oldRoot);
			}
//		}
	}

	cout << "      end combineForShadowNode" << endl;
	return partialTestCases;

}

//combine the test cases of the affected nodes
vector<TestCase*> Graph::combineForNormalNode(map<Node*, vector<TestCase*> >* allPartialTestCases, map<int, Node*> affectedNodes, int maxSize, Node* q){

	cout << "   " << q->id <<  ": start combineForNormalNode" << endl;

	vector<TestCase*> partialTestCases;

	for(int i = 0; i < maxSize; ++i){

//		TNode *newRoot = new TNode(q->id);
//		partialTestCases.push_back(newRoot);

		TestCase* currentTestCase = new TestCase(q->id);
		partialTestCases.push_back(currentTestCase);

		for(map<int, Node*>::const_iterator iter = affectedNodes.begin(); iter != affectedNodes.end(); ++iter){
			Node* affectedNode = iter->second;
			int label = iter->first;
			assert(label < label2id.size() && (label >= firstLabelId || label == label2id[Final]));

			TestCase* testCase;
			vector<TestCase*> affectedTestCases;

			if (label != label2id[Final]){
				affectedTestCases = allPartialTestCases[label][affectedNode];
				assert(affectedTestCases.size() != 0);
			}

			/* if label == label2id[Final] then affectedTestCases is the empty vector */
			if (label == label2id[Final]){
				assert(affectedTestCases.size() == 0);
			}

			if (i < affectedTestCases.size()){
				testCase = affectedTestCases[i];
			}
			else {
				testCase = affectedTestCases[1];
			}

			currentTestCase->appendPartialTestCase(label,testCase);
		}
	}

	cout << "      end combineForNormalNode" << endl;

//	Output stdout("-", "");
//	toEaa_TestCases(stdout, partialTestCases);

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

//print the interface of the TestOG
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

//print the initial nodes of the TestOG
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
	o << ";\n";
}

//print the nodes of the testOG
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


//! \brief prints the graph in the eaa format
void Graph::toEaa(ostream& o) const{

	printInterface(o);
	printInitialNodes(o);
	printNodes(o);
}


//! \brief prints the test cases of the graph in a text format
//! \param o:
void Graph::toEaa_TestCases(ostream& o) const{
	toEaa_TestCases(o, testCases);
}


//! \brief prints the given test cases in a text format
//! \param o:
//! \param testCases: vector of test cases to be printed
void Graph::toEaa_TestCases(ostream& o, vector<TestCase*> testCases) const{
	//Output stdout("-", "");

	for(vector<TestCase*>::const_iterator i = testCases.begin(); i != testCases.end(); ++i){
		TestCase* currentTestCase = *i;
		currentTestCase->printNodes(o);
	}
	cout << endl;
}

//! \brief creates a dot output of the graph
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void Graph::toDot(FILE* out, string title ) const {
	assert(out);

	toDot_Header(out, title);
	toDot_Nodes(out);
	fprintf(out, "}\n");

	assert(!ferror(out));
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


//! \brief creates dot files for given test cases
//! \param out: output file
//! \param testCases: vector of test cases
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void Graph::testCasesToDot(FILE* out, string title) const{

	assert(out);

	if(testCases.size() <= 999){

		TestCase* tc = *(testCases.begin());
		tc->toDot_Header(out, title);

		for(vector<TestCase*>::const_iterator i = testCases.begin(); i != testCases.end(); ++i){
			TestCase* currentTestCase = *i;
			currentTestCase->toDot_Nodes(out);
		}

		fprintf(out, "}\n");
	}
	else {
		fprintf(out, "too many test cases");
	}

	assert(!ferror(out));
}
