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

using std::map;
using std::string;
using std::vector;

int TestCase::maxId = 0;

extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;

extern int firstLabelId; //all labels including tau

/// the command line parameters
extern gengetopt_args_info args_info;

using namespace std;


///constructor
TestCase::TestCase(int rootId) {
	id = maxId++;
	isFullTestCase = false;

	root = new TNode(rootId);
	nodes[root->idTestOg] = root;
}

///copy constructor
TestCase::TestCase(const TestCase& tc){
	id = tc.id;
	isFullTestCase = tc.isFullTestCase;
	root = tc.root;
	nodes.insert(tc.nodes.begin(), tc.nodes.end());
}

///destructor
TestCase::~TestCase(){
	if (isFullTestCase){
		for (map<int, TNode*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
			delete n->second;
		}
	}
}

int TestCase::getId(){
	return id;
}

///id: id of the correspondig node in the OG
///label: label to the old root node
void TestCase::addNewRoot(int idOg, int label){
	assert(label < label2id.size());
	assert(firstLabelId <= label);
	assert(id2label.find(label) != id2label.end());

	TNode* newRoot = new TNode(idOg);
	newRoot->addEdge(label, root);
	nodes[newRoot->idTestOg] = newRoot;
	root = newRoot;
}

void TestCase::appendPartialTestCase(int label, TestCase* partialTestCase){
	assert(label < label2id.size());
	assert(firstLabelId <= label);
	assert(id2label.find(label) != id2label.end());

	root->addEdge(label, partialTestCase->root);
	nodes.insert(partialTestCase->nodes.begin(), partialTestCase->nodes.end());
}



void TestCase::printNodes(ostream& o) const{
	o << "TEST CASE " << id << endl;
	//o << "-----------------" << endl;

    //print all nodes
	for (map<int, TNode*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n){

		assert (n->first == n->second->idTestOg);
		o << "  " << n->first << ": " << n->second->idOg << endl;

		for (unsigned int i = firstLabelId; i < id2label.size(); ++i){
			for (set<TNode*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				assert(id2label.find(i) != id2label.end());
				o << "    " << id2label[i] << " -> " << (*s)->idTestOg << endl;
			}
		}
	}

	o << "====================" << endl;

}


/// prints the graph
//void TestCase::toEaa(ostream& o) const{
//
//	printInterface(o);
//	printInitialNodes(o);
//	printGlobalFormula(o);
//	printNodes(o);
//}


//! \brief creates a dot output of the graph
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void TestCase::toDot(FILE* out, string title) const {

	toDot_Header(out, title);
	toDot_Nodes(out);
	fprintf(out, "}\n");
}

//! \brief creates the header for the dot output
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void TestCase::toDot_Header(FILE* out, string title) const {

	fprintf(out, "digraph D {\n");
	fprintf(out, "graph [fontname=\"Helvetica\", label=\"");
	fprintf(out, "%s", title.c_str());
	fprintf(out, "\"];\n");
	fprintf(out, "node [fontname=\"Helvetica\" fontsize=10];\n");
	fprintf(out, "edge [fontname=\"Helvetica\" fontsize=10];\n");

}

//! \brief creates the nodes for the dot output
//! \param out: output file
void TestCase::toDot_Nodes(FILE* out) const{

	fprintf(out, "  q_%d [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"];\n", (root->idTestOg)+1000*id);
	fprintf(out, "  q_%d -> %d [minlen=\"0.5\"];\n", (root->idTestOg)+1000*id, (root->idTestOg)+1000*id);

	for (map<int, TNode*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
		fprintf(out, "  %d [label=\"%d (%d)\"]\n", (n->second->idTestOg)+1000*id, (n->second->idOg), (n->second->idTestOg)+1000*id);

		for (unsigned int i = firstLabelId; i < id2label.size(); ++i){
			for (set<TNode*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				fprintf(out, "  %d -> %d [label=\"%c%s\"]\n", (n->second->idTestOg)+1000*id, ((*s)->idTestOg)+1000*id, inout[i] ,id2label[i].c_str());
			}
		}
	}

//	fprintf(out, "  q_%d [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"];\n", (root->id)+100*id);
//	fprintf(out, "  q_%d -> %d [minlen=\"0.5\"];\n", (root->id)+100*id, (root->id)+100*id);
//
//	for (map<int, TNode*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
//		fprintf(out, "  %d [label=\"%d\"]\n", (n->second->id)+100*id, (n->second->id)+100*id);
//
//		for (unsigned int i = firstLabelId; i < id2label.size(); ++i){
//			for (set<TNode*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
//				fprintf(out, "  %d -> %d [label=\"%c%s\"]\n", (n->second->id)+100*id, ((*s)->id)+100*id, inout[i] ,id2label[i].c_str());
//			}
//		}
//	}

}
