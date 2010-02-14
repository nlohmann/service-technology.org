#ifndef _TESTFORMULA_H
#define _TESTFORMULA_H

#include "helpers.h"

void initFormulaClass();
void testFormulaClass();
bool checkClause(const Clause& cl, const int l0, const int l1, const int l2);
//void printClause(const Clause& cl);

void testTRUE();
void testFALSE();
void testFINAL();
void testLIT();
void testNUM();
void testNOT();
void testAND();
void testOR();
void testAND_OR_NOT();
void testSat();



#endif
