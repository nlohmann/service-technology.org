#include "mynew.h"
#include "symboltab.h"
#include "petriNetNode.h"
#include "owfnPlace.h"
#include "owfnTransition.h"

SymbolTab * PlaceTable, * TransitionTable, * InterfaceTable;
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
	name = n;
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
{}

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

ISymbol::ISymbol(char * txt):Symbol(txt, InterfaceTable)
{
	place = (owfnPlace *) 0;
	kind = pl;
}

ISymbol::ISymbol(owfnPlace * p):Symbol(p->name, InterfaceTable)
{
	place = p;
	kind = pl;
}

TrSymbol::TrSymbol(char * c):Symbol(c,TransitionTable)
{
	transition = (owfnTransition *) 0;
	kind = tr;
}

