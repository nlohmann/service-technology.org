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

#ifndef _TESTCASE_H
#define _TESTCASE_H

#include <iostream>
#include <map>
#include <set>
#include "Node.h"
#include "Formula.h"
#include "types.h"

using namespace std;

class TestCase {
	protected:
		int id;
		static int maxId;		/// maximal id of all TestCase objects

//		void toDot_Header(FILE* out, string title) const;
//		void toDot_Nodes(FILE* out) const;
//
//		void printInitialNodes(ostream& o) const;
//		void printNodes(ostream& o) const;

//		int maximalEditDistance;

    public:
		bool isFullTestCase;
		TNode* root;
		map<int, TNode*> nodes; // the nodes indexed by an identifier

    	/// constructor
    	TestCase(int rootId);

    	/// destructor
    	TestCase(const TestCase&);

    	/// destructor
    	~TestCase();

    	int getId();
    	void addNewRoot(int id, int label);
    	void appendPartialTestCase(int label, TestCase* partialTestCase);

    	/// Graphviz dot output
        void toDot(FILE* out, string title = "") const;
		void toDot_Header(FILE* out, string title = "") const;
		void toDot_Nodes(FILE* out) const;

        /// Graph output
		void printNodes(ostream& o) const;
//        void toEaa(ostream& o) const;


};

#endif
