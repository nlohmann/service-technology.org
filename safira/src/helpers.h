#ifndef HELPERS_H_
#define HELPERS_H_

#include <vector>
#include <string>
#include <list>
#include "types.h"
#include "Formula.h"

using namespace std;

void addLabel(string label, int id);

string intToString(int i);
string clauseToString(Clause c);
std::vector<int> clauseToIntVector(const Clause &c);
int negateLiteral(int i);
int emptyLiteral();
list<Clause> tripleAND(int x, int l, int r);
list<Clause> tripleOR(int x, int l, int r);

list<Clause> xEqualsFormula(int x, int max, const Formula* f);
list<Clause> xEqualsTrue(int x = 0);
list<Clause> xEqualsFalse(int x);

#endif
