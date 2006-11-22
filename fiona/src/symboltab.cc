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
 * \file    symboltab.cc
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
 
#include "mynew.h"
#include "symboltab.h"
#include "petriNetNode.h"
#include "owfnPlace.h"
#include "owfnTransition.h"

SymbolTab * PlaceTable, * TransitionTable;
extern SymbolTab * GlobalTable;

SymbolTab::SymbolTab(unsigned int s = 65536)
{

	size = s;
	table = new Symbol * [s];
	card = 0;
	for(int i = 0; i < s;i++)
	{
		table[i] = (Symbol *) 0;
	}
}

SymbolTab::~SymbolTab()
{
	for(int i = 0;i < size;i++)
	{
		Symbol * temp;
		while(table[i])
		{
			temp = table[i];
			table[i] = table[i] -> next;
			delete temp;
		}	
	}
	delete [] table;
}

	
Symbol * SymbolTab::lookup(char * name)
{
  /* 1. Hashvalue bestimmen */
  unsigned int h,i;
  Symbol * lst;
  h = 0;
  for(i = 0;i<strlen(name);i++)
    {
      h += (int) name[i];
      h %= size;
    }
  /* 2. suchen */
  for(lst = table[h];lst;lst = lst -> next)
    {
      if(!strcmp(lst->name,name))
	 {
	   break;
	 }
    }
  return lst;
}
  
Symbol::Symbol(char * n, SymbolTab * table)
{
	name = strdup(n);
	table->add(this);
}
void SymbolTab::add(Symbol * s)
{
  /* 1. Hashvalue bestimmen */
  unsigned int h,i;
  h = 0;
  for(i = 0;i<strlen(s->name);i++)
    {
      h += (int) s->name[i];
      h %= size;
    }
  /* 2. eintragen */
  s->next = table[h];
  table[h] = s;
  card++;
}

Symbol::~Symbol()
{
    free(name);
}

PlSymbol::PlSymbol(char * txt):Symbol(txt,PlaceTable)
{
	place = (owfnPlace *) 0;
	kind = pl;
}

PlSymbol::PlSymbol(owfnPlace * p):Symbol(p->name,PlaceTable)
{
	place = p;
	kind = pl;
}

TrSymbol::TrSymbol(char * c):Symbol(c,TransitionTable)
{
	transition = (owfnTransition *) 0;
	kind = tr;
}

