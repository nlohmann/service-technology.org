//#include <cstdio>
#include "config.h"
#include <cassert>
#include "Node.h"
#include "map"
#include <iostream>

int Node::maxId = 0;
extern int currentIdPos;

extern map<string, int> label2id;
extern map<int, string> id2label;

/// constructor
Node::Node(Formula *_formula, int _id) : id(_id), formula(_formula) {
	if (maxId < id){
		maxId = id;
	}
	assert(id <= maxId);
	outEdges = new list<Node*>[label2id.size()];

}

Node::Node(Formula *_formula) : formula(_formula){
	assert(formula);
	id = ++maxId;
	outEdges = new list<Node*>[label2id.size()];
}


/// destructor
Node::~Node() {
	delete formula;
	delete[] outEdges;
}

/// add an outoing edge to the node
void Node::addEdge(int label, Node *target) {
	assert(target);
	//assert(label <= label2id.size()-1);
	assert(id2label.find(label) != id2label.end());
	outEdges[label].push_back(target);
}

