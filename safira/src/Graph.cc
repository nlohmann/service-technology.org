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

using std::map;
using std::string;
using std::vector;

extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;

extern int firstLabelId; //all labels including tau
extern int lastLabelId;

using namespace std;


///constructor
Graph::Graph() : globalFormula(NULL), globalFormulaAsString(""){

}

///destructor
Graph::~Graph(){

//	for (map<int, Node*>::const_iterator n = nodes.begin(); n != nodes.end(); ++n) {
//		delete n->second;
//	}

	delete globalFormula;

}


/// add a node to the graph
void Graph::addInitialNode(int id){
	initialNodes.push_back(id);
}


void Graph::printInterface(ostream& o) const {
	o << "\nINTERFACE\n";
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


void Graph::printGlobalFormula(ostream& o) const{

	o << ";\n\nGLOBALFORMULA " << globalFormulaAsString << ";\n";
	//o << ";\n\nGLOBALFORMULA " << globalFormula->toString() << ";\n";

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
}


/// prints the graph
void Graph::print(ostream& o) const{

	o << "EXPLICITE_TAULOOPS\n";

	printInterface(o);
	printInitialNodes(o);
	printGlobalFormula(o);
	printNodes(o);
}


//! \brief creates a dot output of the graph
//! \param out: output file
//! \param title: a string containing the title for the graph to be shown in the image (optional)
void Graph::toDot(FILE* out, string title) const {

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

		for (int i = firstLabelId; i <= lastLabelId; ++i){
			for (list<Node*>::iterator s = n->second->outEdges[i].begin(); s != n->second->outEdges[i].end(); ++s){
				//fprintf(out, "  %d -> %d [label=\"%s\"]\n", n->second->id, (*s)->id, id2label[i].c_str());
				fprintf(out, "  %d -> %d [label=\"%c%s\"]\n", n->second->id, (*s)->id, inout[i] ,id2label[i].c_str());
			}
		}
	}
}
