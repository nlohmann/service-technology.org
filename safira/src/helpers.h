#ifndef HELPERS_H_
#define HELPERS_H_

#include <vector>
#include <string>
#include <list>
#include "types.h"
#include "Formula.h"
#include "FormulaTree.h"

using namespace std;

void addLabel(string label, char io);

string intToString(int i);
string clauseToString(Clause c);
std::vector<int> clauseToIntVector(const Clause &c);
int negateLiteral(int i);
int emptyLiteral();
list<Clause> tripleAND(int x, int l, int r);
list<Clause> tripleOR(int x, int l, int r);

list<Clause> xEqualsFormula(int x, int& max, const Formula* f);
list<Clause> xEqualsTrue(int x);
list<Clause> xEqualsFalse(int x);
list<Clause> xEqualsI(int x, int i);

void printClause(const Clause& cl);
string assignmentToString(std::vector<bool>* satAssignment);
void printFormulaTree(FormulaTree* root, string filename);
void FormulaTreeToDot(FILE* out, FormulaTree* root);
void dfsTree(FILE* out, FormulaTree* root);

#endif
