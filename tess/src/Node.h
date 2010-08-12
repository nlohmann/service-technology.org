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

#ifndef _NODE_H
#define _NODE_H

#include <list>
#include "types.h"
#include "Formula.h"

using namespace std;


class Node {

    public:
        Formula *formula;  		/// a formula assigned to this node
        int id; 		        /// an identifier as given by the .og file
        set<Node*> *outEdges; 	/// outgoing edges
        bool visited;

    private:
    	static int maxId;		/// maximal id of all node objects
    	bool isShadowNode;



    public:
        /// constructor
        Node(Formula* _f, int _id, bool _isShadowNode = false);
        Node(Formula* _f, bool _isShadowNode = false);

        /// destructor
        ~Node();

        /// add an outoing edge to the node
        void addEdge(int label, Node *target);

        /// adds the shadow node for the current node
        Node* addShadowNode();

        /// implements the edit operator insertion
        void applyInsertion(Node* shadowNode);

        /// implements the edit operator removal
        void applyDeletion(Node* shadowNode);

        /// implements the edit operator renaming
        void applyRenaming(Node* shadowNode);

        /// implements the edit operator "change future"
        void applyChangeFuture(Node* shadowNode);

        /// reset the maxId
        static void init();
};

class TNode {

    public:
        int id; 		        /// an identifier as given by the .og file
        set<TNode*> *outEdges; 	/// outgoing edges

    private:
    //	static int maxId;		/// maximal id of all node objects


    public:
        /// constructor
        TNode(int _id);

        /// destructor
        ~TNode();

        /// add an outoing edge to the node
        void addEdge(int label, TNode *target);

        /// reset the maxId
//        static void init();
};


#endif
