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

#include "FormulaTree.h"


int FormulaTree::maxId = 0;

FormulaTree::FormulaTree() : satAssignment(NULL), node(NULL), yes(NULL), no(NULL){
	id = maxId++;
}

FormulaTree::FormulaTree(Node * _node): satAssignment(NULL), node(_node), yes(NULL), no(NULL){
	id = maxId++;
}

FormulaTree::~FormulaTree(){
	delete no;
	delete yes;
	delete satAssignment;
}

//FormulaTree::FormulaTree(vector<bool>* _satAssigment, FormulaTree* _yes, FormulaTree* _no):
//		satAssignment(_satAssigment), node(NULL), yes(_yes), no(_no){
//	id = maxId++;
//}
