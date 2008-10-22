#include "symboltab.H"
#include "net.H"

SymbolTab * PlaceTable, * TransitionTable;
extern SymbolTab * GlobalTable, * LocalTable;

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
	place = (Place *) 0;
	kind = pl;
}

PlSymbol::PlSymbol(Place * p):Symbol(p->name,PlaceTable)
{
	place = p;
	kind = pl;
}

TrSymbol::TrSymbol(char * c):Symbol(c,TransitionTable)
{
	vars = (SymbolTab * ) 0;
	guard = (UExpression *) 0;
	transition = (Transition *) 0;
	kind = tr;
}

SoSymbol::SoSymbol(char * name, UType * ty): Symbol(name,GlobalTable)
{
	kind = so;
	type = ty;
}

StSymbol::StSymbol(char * name): Symbol(name,BuchiTable)
{
	kind = st;
	state = new buchistate(name);
}

RcSymbol::RcSymbol(char * name, UType * ty): Symbol(name,GlobalTable)
{
	type = ty;
	kind = rc;
}
	
VaSymbol::VaSymbol(char * name, UVar * t): Symbol(name,LocalTable)
{
	var = t;
	kind = va;
}
	
FcSymbol::FcSymbol(char * name, UFunction * f): Symbol(name,GlobalTable)
{
	function = f;
	kind = fc;
}
	
EnSymbol::EnSymbol(char * name): Symbol(name,GlobalTable)
{
	kind = en;
}
	
