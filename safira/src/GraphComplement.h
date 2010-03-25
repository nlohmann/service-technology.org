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

#ifndef _GRAPHCOMPLEMENT_H
#define _GRAPHCOMPLEMENT_H

#include <map>
#include "FormulaTree.h"
#include "Graph.h"

using namespace std;

class GraphComplement : public Graph {
	protected:
		Node* trap;
		FormulaTree* root;
		static double getNode_time;

        /// complete the extended annotated graph - fast algorithm, but unreduced result
        void makeComplete_fast();

        /// complete the extended annotated graph - stupid algorithm (simple reduction, slow)
        void makeComplete_stupid();

        /// complete the extended annotated graph - efficient algorithm (intelligent reduction, fast)
        void makeComplete_efficient();

        /// make the extended annotated graph total
        void makeTotal();

        void addTrapState();

        Node* getNode(Formula *f);
        Node* getNode_stupid(Formula *f);
        bool formulaFound(Formula *f, Formula *g);
        Node* searchNode(Formula *f, FormulaTree *n);

        void toDot_Nodes(FILE* out) const;

        int getSizeOfAddedNodes();
        void appendAddedNodes(); //appends the added nodes to the nodes-mapâ

        void generateGlobalFormula();


    public:
        map<int, Node*> addedNodes;

    	///constructor
    	GraphComplement();

    	///destructor
    	~GraphComplement();

    	/// computes the complement for the extended annotated graph
    	void complement();

        /// Graphviz dot output
        void toDot(FILE* out, string title = "") const;

//		/// negates the global Formulaâ
//        void negateGlobalFormula();

        static double getTime();
};

#endif /* GRAPHCOMPLEMENT_H_ */
