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
#include <iostream>
#include "testTestCase.h"
#include "TestCase.h"
#include "Node.h"
#include "helpers.h"


using namespace std;

void testTestCaseClass(){
	cout << "testTestCaseClass... ";

	initGlobalVariables();
	addLabel("A",'!');
	addLabel("R",'!');
	addLabel("I",'?');
	addLabel("O",'?');

	TestCase* t1 = new TestCase(4); // t1: root=4
	assert(t1->root->idOg == 4);
	assert(t1->root->idTestOg == 0);
	assert(t1->getId() == 0);

	TestCase* t2 = new TestCase(27); // t2: root=27
	assert(t2->root->idOg == 27);
	assert(t2->root->idTestOg == 1);
	assert(t2->getId() == 1);

	t1->addNewRoot(23,3); //  t1: 23--label3-> 4 bzw. 2--label3->0
	assert(t1->root->idOg == 23);
	assert(t1->root->idTestOg == 2);

	TNode* tn = *(t1->root->outEdges[3].begin());
	assert(tn->idOg == 4);
	assert(tn->idTestOg == 0);

	assert(t1->root->outEdges[3].size() == 1);
	assert(t1->nodes.find(0) != t1->nodes.end());
	assert(t1->nodes.find(2) != t1->nodes.end());
	assert(t1->nodes.find(1) == t1->nodes.end());

	t2->appendPartialTestCase(5,t1);
	delete t1;

	assert(t2->nodes.find(0) != t2->nodes.end());
	assert(t2->nodes.find(2) != t2->nodes.end());
	assert(t2->nodes.find(1) != t2->nodes.end());

	t2->isFullTestCase = true;

	delete t2;

	cout << "\t passed." << endl;
}



