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

#ifndef _TESTFORMULA_H
#define _TESTFORMULA_H

#include "helpers.h"

void initFormulaClass();
void testFormulaClass();

void testPutIdIntoList();
void testMinimizeDnf();
void testTRUE();
void testFINAL();
void testLIT();
void testNUM();
void testAND();
void testOR();
void testAND_OR();

#endif
