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

#include <config.h>
#include <iostream>
#include <cstdlib>
#include <map>

#include "testFormula.h"
#include "testNode.h"
#include "testGraph.h"
#include "types.h"
#include "helpers.h"
#include "Output.h"

using namespace std;
map<string, int> label2id;
map<int, string> id2label;
map<int, char> inout;

int firstLabelId; //all labels including tau
int firstInputId; //input labels
int firstOutputId;//output labels

int lastLabelId;
int lastInputId;
int lastOutputId;

void testClauseToString();

int main() {

	testClauseToString();
	testFormulaClass();
	testNodeClass();

	TestGraphComplement tgc;
	tgc.initGraph();
    tgc.complement();
	Output o("testGraph_complement.eaa", "complement testGraph");
	tgc.print(o);
	cout << "testGraphClass... \t passed" << endl;

	cout << "tests finished" << endl << endl;
	exit(0);
}

void testClauseToString(){
	Clause c;
	c.literal0 = 23;
	c.literal1 = 42;
	c.literal2 = -1;
	assert(clauseToString(c) == "  23 42 -1 ");
}
