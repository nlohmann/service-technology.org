/*****************************************************************************\
 Tess -- Selecting Test Cases for Services

 Copyright (c) 2010 Kathrin Kaschner

 Tess is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tess is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Tess.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include <config.h>
#include "Node.h"
#include "map"
#include <iostream>

int Node::maxId = 0;
extern int firstLabelId;

extern int firstInputId;
extern int lastInputId;
extern int firstOutputId;
extern int lastOutputId;

extern map<string, int> label2id;
extern map<int, string> id2label;

/// constructor
Node::Node(Formula *_formula, int _id, bool _isShadowNode) :
		formula(_formula), id(_id), isShadowNode(_isShadowNode) {
	if (maxId < id){
		maxId = id;
	}
	assert(id <= maxId);
	outEdges = new set<Node*>[label2id.size()];
	visited = false;

}

Node::Node(Formula *_formula, bool _isShadowNode) : formula(_formula), isShadowNode(_isShadowNode){
	assert(formula || (isShadowNode == true && formula == NULL));
	id = ++maxId;
	outEdges = new set<Node*>[label2id.size()];
	visited = false;
}


/// destructor
Node::~Node() {
	delete formula;
	delete[] outEdges;
}

/// add an outpoing edge to the node
void Node::addEdge(int label, Node *target) {
	assert(target);
	assert(label < label2id.size());
	assert(firstLabelId <= label);
	assert(id2label.find(label) != id2label.end());

	outEdges[label].insert(target);
}

/// reset the maxId
//void Node::init(){
//	maxId = -1;
//}

/// implements the edit operator insertion
void Node::applyInsertion(Node* shadowNode){
	assert(shadowNode->isShadowNode == true);
	for (int i = firstOutputId; i <= lastOutputId; ++i){
		shadowNode->addEdge(i, this);
	}
}

/// implements the edit operator removal
void Node::applyDeletion(Node* shadowNode){
	assert(shadowNode->isShadowNode == true);

	for (int i = firstInputId; i < id2label.size(); ++i){
		for (set<Node*>::iterator s = outEdges[i].begin(); s != outEdges[i].end(); ++s){
			shadowNode->addEdge(label2id[Tau], *s);
		}
	}
}

/// implements the edit operator renaming
void Node::applyRenaming(Node* shadowNode){
	assert(shadowNode->isShadowNode == true);

	for (int i = firstLabelId; i < id2label.size(); ++i){

		for (set<Node*>::iterator s = outEdges[i].begin(); s != outEdges[i].end(); ++s){

			Node* currentSuccessorNode = *s;
			if (!currentSuccessorNode->isShadowNode){

				for (int j = firstOutputId; j <= lastOutputId; ++j){
					if (i != j && outEdges[j].find(currentSuccessorNode) == outEdges[j].end()){

						shadowNode->addEdge(j, currentSuccessorNode);
					}
				}
			}
		}
	}
}

/// implements the edit operator "change future"
//void Node::applyChangeFuture(Node* shadowNode){
//	assert(shadowNode->isShadowNode == true);
//	assert(false);
//}

int TNode::maxId = 0;

/// constructor
TNode::TNode(int _idOg) : idOg(_idOg) {
	idTestOg = maxId++;
	outEdges = new set<TNode*>[label2id.size()];
	final = false; //as default a node is not a final node
}

/// destructor
TNode::~TNode() {
	delete[] outEdges;
}

/// add an outpoing edge to the node
void TNode::addEdge(int label, TNode *target) {
	assert(target);
	assert(label < label2id.size());
	assert(firstLabelId <= label);
	assert(id2label.find(label) != id2label.end());

	outEdges[label].insert(target);
}


void TNode::setAsFinalNode(){
	final = true;
}

bool TNode::isFinalNode(){
	return final;
}
// reset the maxId
//void TestNode::init(){
//	maxId = 0;
//}

int TNode::getMaxId(){
	return maxId;
}

