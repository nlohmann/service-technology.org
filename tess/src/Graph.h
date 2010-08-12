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

#ifndef _GRAPH_H
#define _GRAPH_H

#include <iostream>
#include <map>
#include <set>
#include <list>
#include <vector>
#include "Node.h"
#include "Formula.h"
#include "TestCase.h"
#include "types.h"

using namespace std;

class Graph {
	protected:
		void toDot_Header(FILE* out, string title) const;
		void toDot_Nodes(FILE* out) const;

		void printInterface(ostream& o) const;
		void printInitialNodes(ostream& o) const;
		void printGlobalFormula(ostream& o) const;
		void printNodes(ostream& o) const;

		void toTestOg();
		int maximalEditDistance;

		vector<TNode*> testCases; //a vector containing the test cases
		vector<TNode*> dfs(Node* q, int ed, bool fromShadowNode = false);
		vector<TNode*> combine(map<Node*, vector<TNode*> >* allPartialTestCases, list< set<int> > assignments, Node* q);
		vector<TNode*> combineForShadowNode(map<Node*, vector<TNode*> >* allPartialTestCases, int label, Node* q);
		vector<TNode*> combineForNormalNode(map<Node*, vector<TNode*> >* allPartialTestCases, map<int, Node*> affectedNodes, int maxSize, Node* q);

    public:
		list<int> initialNodes;
		map<int, Node*> nodes; // the nodes indexed by an identifier
		map<int, Node*> shadowNodes; // the shadow nodes indexed by an identifier

		Formula *globalFormula;


    	/// constructor
    	Graph();

    	/// destructor
    	~Graph();

    	void FormulasToDnf();

    	void addInitialNode(int id);

    	/// Graphviz dot output
        void toDot(FILE* out, string title = "") const;

        /// Graph output
        void toEaa(ostream& o) const;

        void generateTestCases();

};

#endif
