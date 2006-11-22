// *****************************************************************************\
// * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf       *
// *                                                                           *
// * This file is part of Fiona.                                               *
// *                                                                           *
// * Fiona is free software; you can redistribute it and/or modify it          *
// * under the terms of the GNU General Public License as published by the     *
// * Free Software Foundation; either version 2 of the License, or (at your    *
// * option) any later version.                                                *
// *                                                                           *
// * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
// * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
// * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
// * more details.                                                             *
// *                                                                           *
// * You should have received a copy of the GNU General Public License along   *
// * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
// * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
// *****************************************************************************/

/*!
 * \file    symboltab.h
 *
 * \brief   functions for Petri net elements
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef SYMBOLTAB_H
#define SYMBOLTAB_H

#include "mynew.h"
#include "petriNetNode.h"
#include "dimensions.h"
#include "owfnPlace.h"
#include "owfnTransition.h"

class Symbol;

class SymbolTab
{
	public:
	Symbol ** table;
	Symbol * lookup(char *);
	void add(Symbol *);
	SymbolTab(unsigned int);
  	~SymbolTab();
	unsigned int card;
	unsigned int size;

    // Provides user defined operator new. Needed to trace all new operations
    // on this class.
#undef new
    NEW_OPERATOR(SymbolTab)
#define new NEW_NEW
};

extern SymbolTab * PlaceTable, * TransitionTable;
	
typedef enum {pl,tr} kinds;
class Symbol
{
public:
  char * name;
  kinds kind;
  Symbol * next;
  Symbol(char *, SymbolTab *);
  ~Symbol();

  // Provides user defined operator new. Needed to trace all new operations
  // on this class.
#undef new
  NEW_OPERATOR(Symbol)
#define new NEW_NEW
};

class PlSymbol: public Symbol
{
public:
	owfnPlace * place;
        PlSymbol(owfnPlace *); // LL- places
        PlSymbol(char *); // HL-places
  
  // Provides user defined operator new. Needed to trace all new operations
  // on this class.
#undef new
    NEW_OPERATOR(PlSymbl);
#define new NEW_NEW
};

class TrSymbol: public Symbol
{
public:
	owfnTransition * transition;
        TrSymbol(char *);

  // Provides user defined operator new. Needed to trace all new operations
  // on this class.
#undef new        
    NEW_OPERATOR(TrSymbol);
#define new NEW_NEW
};

class IdList
{
	public:
	char * name;
	IdList * next;
};

#endif











