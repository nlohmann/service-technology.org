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

#ifndef __TARJAN_H
#define __TARJAN_H

#include <map>
#include <set>
#include <stack>
#include <string>

#include <pnapi/pnapi.h>
#include <config.h>
#include "types.h"
#include "verbose.h"

using std::map;
using std::multimap;
using std::set;
using std::stack;
using std::string;

class Tarjan {

	private:
		bool mSCCCalculated;
		bool mHasNonTrivialSCC;
		int mMaxDFS;
		
		nodes_t mUnprocessedNodes;
		nodes_t mStackContent;
		stack<node_t> mStack;
		node_value_t mNodeDFS;
		node_value_t mNodeLowlink;
		node_value_t mNode2SCC;
		int mCurSCC;		
		multimap<int, node_t> mSCC2Node;	
		pnapi::PetriNet *mNet;
		
		void init();
		void tarjan(node_t);

		inline void push(node_t & Node) {
			this->mStackContent.insert(Node);
			this->mStack.push(Node);
		}
		inline node_t pop() {
			assert(!this->mStack.empty());
			node_t curNode = this->mStack.top();
			this->mStackContent.erase(curNode);
			this->mStack.pop();
			return curNode;
		}

		inline void setDFS(node_t & Node, int DFS) {
			node_value_t::iterator curNode = this->mNodeDFS.find(Node);
			if (curNode != this->mNodeDFS.end()) {
				this->mNodeDFS.erase(curNode);
			}
			this->mNodeDFS.insert( std::make_pair(Node, DFS) );
		}
		inline void setLowlink(node_t & Node, int Lowlink) {
			node_value_t::iterator curNode = this->mNodeLowlink.find(Node);
			if (curNode != this->mNodeLowlink.end()) {
				this->mNodeLowlink.erase(curNode);
			}
			this->mNodeLowlink.insert( std::make_pair(Node, Lowlink) );
		}
		
		inline int getDFS(node_t & Node) {
			node_value_t::iterator curNode = this->mNodeDFS.find(Node);
			assert(curNode != this->mNodeDFS.end());
			return curNode->second;
		}
		inline int getLowlink(node_t & Node) {
			node_value_t::iterator curNode = this->mNodeLowlink.find(Node);
			assert(curNode != this->mNodeLowlink.end());
			return curNode->second;
		}

	public:
		inline Tarjan(pnapi::PetriNet & Petrinet) {
			this->mNet = &Petrinet;
			this->init();
		}
		inline ~Tarjan() {};

		void calculateSCC();

		inline node_value_t getNode2SCC() const {
			assert(this->mSCCCalculated);
			return this->mNode2SCC;
		}
		inline multimap<int, node_t> getSCC2Node() const {
			assert(this->mSCCCalculated);
			return this->mSCC2Node;
		}
		inline int getNodeSCC(const node_t & Node) {
			assert(this->mSCCCalculated);
			assert(this->mNode2SCC.find(Node) != this->mNode2SCC.end());
			return this->mNode2SCC.find(Node)->second;
		}
		inline bool hasNonTrivialSCC() {
			assert(this->mSCCCalculated);
			return this->mHasNonTrivialSCC;
		}
		inline bool isNonTrivialSCC(const int SCC) {
			assert(this->mSCCCalculated);
			assert(this->mSCC2Node.find(SCC) != this->mSCC2Node.end());
			return (this->mSCC2Node.count(SCC) > 1);
		}

		
};

#endif //__TARJAN_H
