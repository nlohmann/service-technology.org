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
#include <cstdlib>
#include <map>
#include "cmdline.h"
#include "verbose.h"

#include "testFormula.h"
#include "testTestCase.h"
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
	testTestCaseClass();

	cout << "tests finished" << endl << endl;
	exit(0);
}

