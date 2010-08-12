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
#include "cmdline.h"
#include "verbose.h"

#include "testFormula.h"
//#include "testNode.h"
//#include "testGraph.h"
#include "types.h"
#include "helpers.h"

using namespace std;
map<string, int> label2id;
map<int, string> id2label;
map<int, char> inout;


gengetopt_args_info args_info;


int main() {

	testFormulaClass();
	//testNodeClass();
	//testGraphClass();
	cout << "tests finished" << endl << endl;
	exit(0);
}

