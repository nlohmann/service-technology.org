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
#include <cassert>
#include "helpers.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <map>
#include "cmdline.h"

/// the command line parameters
extern gengetopt_args_info args_info;

extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;

extern int lastLabelId;

void addLabel(string label, char io){

	if (label2id.find(label) == label2id.end()){
		++lastLabelId;
		assert(id2label.find(lastLabelId) == id2label.end());
		assert(inout.find(lastLabelId) == inout.end());
		id2label[lastLabelId] = label;
		label2id[label] = lastLabelId;
		inout[lastLabelId] = io;

		//cout << "label: " << label << "  id: " << label2id[label] << endl;
	}
}

vector<int> clauseToIntVector(const Clause &c) {
	vector<int> result;

	if (c.literal0 != emptyLiteral()){
		result.push_back(c.literal0);
	}
	if (c.literal1 != emptyLiteral()){
		result.push_back(c.literal1);
	}
	if (c.literal2 != emptyLiteral()){
		result.push_back(c.literal2);
	}
  
	return result;
}

string clauseToString(Clause c){
	string s = " ";
	if (c.literal0 != emptyLiteral()){
		s = s + " " + intToString(c.literal0);
	}
	if (c.literal1 != emptyLiteral()){
		s = s + " " + intToString(c.literal1);
	}
	if (c.literal2 != emptyLiteral()){
		s = s + " " + intToString(c.literal2);
	}

	s = s + " ";

	return s;  // s = " " + c->literal0 + " " + c->literal1 + " " + c->literal2 + " "
}

string intToString(const int i){

	stringstream ssout;
	ssout <<  i;
       // string s (ssout.str());
	   string s = ssout.str();
	return s;
}

int negateLiteral(int i){
	return (-1 * i);
}

int emptyLiteral(){
	return 0;
}

//x <-> true  is equivalent to x
list<Clause> xEqualsTrue(int x){
	Clause cl;
	cl.literal0 = x;
	cl.literal1 = emptyLiteral();
	cl.literal2 = emptyLiteral();

	list<Clause> clauses;
	clauses.push_back(cl);
	return clauses;
}

// "x <-> false" is equivalent to "~x"
list<Clause> xEqualsFalse(int x){

	Clause cl;
	cl.literal0 = negateLiteral(x);
	cl.literal1 = emptyLiteral();
	cl.literal2 = emptyLiteral();

	list<Clause> clauses;
	clauses.push_back(cl);
	return clauses;
}

// "x <-> i" is equivalent to "(x + ~i) * (~x + i)"
list<Clause> xEqualsI(int x, int i){

	Clause cl1;
	cl1.literal0 = negateLiteral(x);
	cl1.literal1 = i;
	cl1.literal2 = emptyLiteral();

	list<Clause> clauses;
	clauses.push_back(cl1);

	Clause cl2;
	cl2.literal0 = x;
	cl2.literal1 = negateLiteral(i);
	cl2.literal2 = emptyLiteral();
	clauses.push_back(cl2);

	return clauses;
}

//max: largest ID
list<Clause> xEqualsFormula(int x, int& max, const Formula* f){
	assert(f->formulaType != TRUE);
	assert(f->formulaType != FALSE);

	if (f->formulaType == INT || f->formulaType == LIT || f->formulaType == NOT || f->formulaType == FINAL){

		// if f is NUM/LIT/FINAL/NOT then x <-> f is equivalent to "(~x + f) * (x + f)"
		// remark: NOT is only possible before INT or LIT, because moveNegation() was executed before
		list<Clause> tmpClauses = f->toCNF(x,max);
		assert(tmpClauses.size() == 2);
		assert(tmpClauses.begin()->literal0 == -1*x);
		assert(tmpClauses.begin()->literal1 != emptyLiteral());
		assert(tmpClauses.begin()->literal2 == emptyLiteral());

		return xEqualsI(x,tmpClauses.begin()->literal1);
	}
	else{
		assert (f->formulaType == AND || f->formulaType == OR);
		// if f is an AND/OR-Formula then x <-> f is equivalent to "(~x + max) * (x + ~max) * (max <-> f)

		/* (~x + maxId) */
		Clause cl1;
		cl1.literal0 = negateLiteral(x);
		cl1.literal1 = max+1;
		cl1.literal2 = emptyLiteral();

		/* (x + ~maxId) */
		Clause cl2;
		cl2.literal0 = x;
		cl2.literal1 = negateLiteral(max+1);
		cl2.literal2 = emptyLiteral();

		list<Clause> clauses;
		clauses.push_back(cl1);
		clauses.push_back(cl2);

		/* (maxId <-> f) */
		++max;
		list<Clause> tmp = f->toCNF(max, max);

		//append the clauses of (sMax1 <-> f)
		list<Clause>::iterator iter = clauses.end();
		clauses.splice(iter, tmp);
		return clauses;
	}

}


// x <-> l*r  is equals to the Conjunction of the three Clauses: (~x,l,r) (x,~l) (x, ~r)
list<Clause> tripleAND(int x, int l, int r){
	assert(x != emptyLiteral() && l != emptyLiteral() && r != emptyLiteral());
	list<Clause> clauses;

	Clause cl1;
	cl1.literal0 = x;
	cl1.literal1 = negateLiteral(l);
	cl1.literal2 = negateLiteral(r);
	clauses.push_back(cl1);

	Clause cl2;
	cl2.literal0 = negateLiteral(x);
	cl2.literal1 = l;
	cl2.literal2 = emptyLiteral();
	clauses.push_back(cl2);

	Clause cl3;
	cl3.literal0 = negateLiteral(x);
	cl3.literal1 = emptyLiteral();
	cl3.literal2 = r;
	clauses.push_back(cl3);

	return clauses;
}

// x <-> l+r  is equals to the Conjunction of the three Clauses: (x,~l,~r) (~x,l) (~x, r)
list<Clause> tripleOR(int x, int l, int r){
	assert(x != emptyLiteral() && l != emptyLiteral() && r != emptyLiteral());
	list<Clause> clauses;

	Clause cl1;
	cl1.literal0 = negateLiteral(x);
	cl1.literal1 = l;
	cl1.literal2 = r;
	clauses.push_back(cl1);

	Clause cl2;
	cl2.literal0 = x;
	cl2.literal1 = negateLiteral(l);
	cl2.literal2 = emptyLiteral();
	clauses.push_back(cl2);

	Clause cl3;
	cl3.literal0 = x;
	cl3.literal1 = emptyLiteral();
	cl3.literal2 = negateLiteral(r);
	clauses.push_back(cl3);

	return clauses;
}

void printClause(const Clause& cl){
	cout << "Clausel: " << cl.literal0 << ", " << cl.literal1 << ", " << cl.literal2 << endl;
}


string assignmentToString(vector<bool>* assignment){
	string s = "";
	if(assignment){
		assert(assignment->size() >= label2id.size());
		for(unsigned int i = 1; i < label2id.size(); ++i){
			if(assignment->at(i) == true){
				s = s + id2label[i] + " ";
				//cout << id2label[i] << " ";
			}
			else{
				assert(assignment->at(i) == false);
				s = s + "-" + id2label[i] + " ";
				//cout << "-" << id2label[i] << " ";
			}
		}
	}
	else {
		s = "no satisfying assignment";
	}
	return s;

}

void printFormulaTree(FormulaTree* root, string filename){

	string call = string(CONFIG_DOT) + " -Tpdf -q -o " + filename;
	FILE *s = popen(call.c_str(), "w");
	assert(s);

	FormulaTreeToDot(s, root);
	assert(!ferror(s));

	pclose(s);
}

void FormulaTreeToDot(FILE* out, FormulaTree* root){

	fprintf(out, "digraph D {\n");
	fprintf(out, "graph [fontname=\"Helvetica\", label=\"");
	fprintf(out, "%s", "FormulaTree");
	fprintf(out, "\"];\n");
	fprintf(out, "node [fontname=\"Helvetica\" fontsize=10];\n");
	fprintf(out, "edge [fontname=\"Helvetica\" fontsize=10];\n");

	if (root->node || root->satAssignment){
		// an arrow indicating the initial state
		fprintf(out, "  q_%d [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"];\n", root->id);
		fprintf(out, "  q_%d -> %d [minlen=\"0.5\"];\n", root->id, root->id);
	}

	dfsTree(out, root);
	fprintf(out, "}\n");
}

void dfsTree(FILE* out, FormulaTree* n){

	if(n->node){
		fprintf(out, "  %d [label=\"%d\\n %d: %s\"]\n", n->id, n->id, n->node->id, n->node->formula->toString().c_str());
	}

	if(n->satAssignment){
		fprintf(out, "  %d [label=\"%d\\n %s\"]\n", n->id, n->id, assignmentToString(n->satAssignment).c_str());
	}

	if(n->yes){
		fprintf(out, "  %d -> %d [label=\"%s\"]\n", n->id, n->yes->id, "yes");
		dfsTree(out, n->yes);
	}

	if(n->no){
		fprintf(out, "  %d -> %d [label=\"%s\"]\n", n->id, n->no->id, "no");
		dfsTree(out, n->no);
	}
}

bool graphIsPrinted(){
	bool printGraph;
	if (args_info.output_given > 0){
		printGraph = false;
		for (unsigned int j = 0; j<args_info.output_given; ++j){
			if (args_info.output_arg[j] != output_arg_none) {
				printGraph = true;
			}
		}
	}
	else {
		printGraph = true; //Graph is printed on stdout
	}
	return printGraph;
}

