/*****************************************************************************\
 			 _____                  _             
			|  __ \                (_)            
			| |  | | ___  _ __ ___  _ _ __   ___  
			| |  | |/ _ \| '_ ` _ \| | '_ \ / _ \ 
			| |__| | (_) | | | | | | | | | | (_) |
			|_____/ \___/|_| |_| |_|_|_| |_|\___/ 		                          
			DecOMposition of busINess wOrkflows (into services)
			http://service-technology.org/domino

 Copyright (c) 2010 Andreas Lehmann

 Domino is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Domino is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Domino.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#include "Tarjan.h"

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
	assert(!this->mSCCCalculated);
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
