/*****************************************************************************\
 LoLA -- a Low Level Petri Net Analyzer

 Copyright (C)  1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
                2008, 2009  Karsten Wolf <lola@service-technology.org>

 LoLA is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 LoLA is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with LoLA.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


#include "symboltab.H"
#include "net.H"

SymbolTab * PlaceTable, * TransitionTable;
extern SymbolTab * GlobalTable, * LocalTable;

SymbolTab::SymbolTab(unsigned int s = 65536)
{

    size = s;
    table = new Symbol * [s];
    card = 0;
    for(unsigned int i = 0; i < s;i++)
    {
        table[i] = NULL;
    }
}

SymbolTab::~SymbolTab()
{
    for(unsigned int i = 0;i < size;i++)
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
    unsigned int h = 0;

    for(unsigned int i = 0; i<strlen(s->name); i++)
    {
        h += (int) s->name[i];
        h %= size;
    }
    
    /* 2. eintragen */
    s->next = table[h];
    table[h] = s;
    card++;
}

Symbol::~Symbol() {}

PlSymbol::PlSymbol(char * txt): Symbol(txt,PlaceTable)
{
    place = NULL;
    kind = pl;
}

PlSymbol::PlSymbol(Place * p):Symbol(p->name,PlaceTable)
{
    place = p;
    kind = pl;
}

TrSymbol::TrSymbol(char * c):Symbol(c,TransitionTable)
{
    vars = NULL;;
    guard = NULL;
    transition = NULL;
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
