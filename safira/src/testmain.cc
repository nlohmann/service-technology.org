#include <config.h>
#include <iostream>
#include <cstdlib>
#include <map>

#include "testFormula.h"
#include "testNode.h"
#include "testGraph.h"

using namespace std;
map<string, unsigned int> label2id;
map<unsigned int, string> id2label;

unsigned int firstLabelId; //all labels including tau
unsigned int firstInputId; //input labels
unsigned int firstOutputId;//output labels

unsigned int lastLabelId;
unsigned int lastInputId;
unsigned int lastOutputId;

int main() {

	testFormulaClass();
	testNodeClass();
	testGraphClass();
	cout << "test finished" << endl << endl;
	exit(0);
}
