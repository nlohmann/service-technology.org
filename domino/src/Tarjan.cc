#include "Tarjan.h"

Tarjan::~Tarjan() {}

Tarjan::Tarjan(pnapi::PetriNet & Petrinet) { 
	this->mNet = &Petrinet;
	
	this->init();
}

void Tarjan::init() {
	//status("Tarjan::init() called...");
	
	this->mMaxDFS = 0;
	this->mSCCCalculated = false;
	this->mHasNonTrivialSCC = false;
	
	PNAPI_FOREACH(n, this->mNet->getNodes()) {
		//status("..%s insert", (*n)->getName().c_str());
		this->mUnprocessedNodes.insert((*n)->getName());
	}
	
	//status("Tarjan::init() finished");
}

void Tarjan::calculateSCC() {
	if (this->mSCCCalculated) {
		abort(2, "Tarjan::calculateSCC(): already called");
	}
	
	//status("Tarjan::calculateSCC() called...");
	while (this->mUnprocessedNodes.size() != 0) {
		tarjan((*this->mUnprocessedNodes.begin()));
	}
	this->mSCCCalculated = true;
	//status("Tarjan::calculateSCC() finished");
}

void Tarjan::tarjan(node_t Node) {
	//status("Tarjan::tarjan(%s) called...", Node.c_str());
	
	node_t adjNode;
	node_t stackNode;
	set<pnapi::Node *> postset;

	this->setDFS(Node, this->mMaxDFS);
	this->setLowlink(Node, this->mMaxDFS);
	this->mMaxDFS++;
	this->push(Node);
	this->mUnprocessedNodes.erase(Node);

	postset = this->mNet->findNode(Node)->getPostset();
	PNAPI_FOREACH(n, postset) {
		adjNode = (*n)->getName();
		if (this->mUnprocessedNodes.find(adjNode) != this->mUnprocessedNodes.end()) {
			this->tarjan(adjNode);
			this->setLowlink(Node, std::min(this->getLowlink(Node), this->getLowlink(adjNode)));
		}
		else if (this->mStackContent.find(adjNode) != this->mStackContent.end()) {
			this->setLowlink(Node, std::min(this->getLowlink(Node), this->getDFS(adjNode)));
		}
	}

	if (this->getLowlink(Node) == this->getDFS(Node)) {
		//status("..SZK found", Node.c_str());
		do {
			stackNode = this->pop();
			//status("....%s", stackNode.c_str());
			this->mNode2SCC.insert( std::make_pair(stackNode, this->getLowlink(stackNode)) );
			this->mSCC2Node.insert( std::make_pair(this->getLowlink(stackNode), stackNode) );
			if (this->mSCC2Node.count(this->getLowlink(stackNode)) > 1) {
				this->mHasNonTrivialSCC = true;
			}
		} while (Node != stackNode); 
	}
	//status("Tarjan::tarjan(%s) finished", Node.c_str());
}

bool Tarjan::hasNonTrivialSCC() {
	if (!this->mSCCCalculated) {
		abort(2, "Tarjan::hasNonTrivialSCC(): Tarjan::calculateSCC() is necessary");
	}
	return this->mHasNonTrivialSCC;
}

bool Tarjan::isNonTrivialSCC(int SCC) {
	if (!this->mSCCCalculated) {
		abort(2, "Tarjan::isNonTrivialSCC(%d): Tarjan::calculateSCC() is necessary", SCC);
	}

	if (this->mSCC2Node.find(SCC) == this->mSCC2Node.end()) {
		abort(2, "Tarjan::isNonTrivialSCC(%d) failed", SCC);
	}
	
	return (this->mSCC2Node.count(SCC) > 1);
}

int Tarjan::getNodeSCC(const node_t & Node) {
	if (!this->mSCCCalculated) {
		abort(2, "Tarjan::getNodeSCC(%s): Tarjan::calculateSCC() is necessary", Node.c_str());
	}

	if (this->mNode2SCC.find(Node) == this->mNode2SCC.end()) {
		abort(2, "Tarjan::getNodeSCC(%s) failed", Node.c_str());
	}
	
	return this->mNode2SCC.find(Node)->second;
}

node_value_t Tarjan::getNode2SCC() const {
	if (!this->mSCCCalculated) {
		abort(2, "Tarjan::getNode2SCC(): Tarjan::calculateSCC() is necessary");
	}
	return this->mNode2SCC;
}

multimap<int, node_t> Tarjan::getSCC2Node() const {
	if (!this->mSCCCalculated) {
		abort(2, "Tarjan::getNode2SCC(): Tarjan::calculateSCC() is necessary");
	}
	return this->mSCC2Node;
}

void Tarjan::push(node_t & Node) {
	this->mStackContent.insert(Node);
	this->mStack.push(Node);
}

node_t Tarjan::pop() {
	if (this->mStack.empty()) {
		abort(2, "pop(): stack empty");
	}
	node_t curNode;

	curNode = this->mStack.top();
	this->mStackContent.erase(curNode);
	this->mStack.pop();

	return curNode;
}

void Tarjan::setDFS(node_t & Node, int DFS) {
	node_value_t::iterator curNode;

	curNode = this->mNodeDFS.find(Node);
	if (curNode != this->mNodeDFS.end()) {
		//set again
		this->mNodeDFS.erase(curNode);
	}
	
	this->mNodeDFS.insert( std::make_pair(Node, DFS) );
}

void Tarjan::setLowlink(node_t & Node, int Lowlink) {
	node_value_t::iterator curNode;

	curNode = this->mNodeLowlink.find(Node);
	if (curNode != this->mNodeLowlink.end()) {
		//set again
		this->mNodeLowlink.erase(curNode);
	}
	
	this->mNodeLowlink.insert( std::make_pair(Node, Lowlink) );
}

int Tarjan::getDFS(node_t & Node) {
	node_value_t::iterator curNode;

	curNode = this->mNodeDFS.find(Node);
	if (curNode == this->mNodeDFS.end()) {
		abort(2, "getDFS(%s) failed", Node.c_str());
	}

	return curNode->second;
}

int Tarjan::getLowlink(node_t & Node) {
	node_value_t::iterator curNode;

	curNode = this->mNodeLowlink.find(Node);
	if (curNode == this->mNodeLowlink.end()) {
		abort(2, "getLowlink(%s) failed", Node.c_str());
	}

	return curNode->second;
}
