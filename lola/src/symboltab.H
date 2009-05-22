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


#ifndef SYMBOLTAB_H
#define SYMBOLTAB_H

#include "net.H"
#include "dimensions.H"
#include "unfold.H"
#include "buchi.H"

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
};

extern SymbolTab * PlaceTable, * TransitionTable;
	
typedef enum {va,en,rc,fc,so,pl,tr,fp,st} kinds;
class Symbol
{
public:
  char * name;
  kinds kind;
  Symbol * next;
  Symbol(char *, SymbolTab *);
  ~Symbol();
};

class PlSymbol: public Symbol
{
public:
	Place * place;
	UType * sort;
        PlSymbol(Place *); // LL- places
        PlSymbol(char *); // HL-places
};

class TrSymbol: public Symbol
{
public:
	Transition * transition;
	SymbolTab * vars;
	UExpression * guard;
        TrSymbol(char *);
};

class VaSymbol: public Symbol
{
public:
	UVar * var;
        VaSymbol(char *, UVar *);
};

class EnSymbol: public Symbol
{
public:
	int ord;
	UType * type;
        EnSymbol(char *);
};

class FcSymbol: public Symbol
{
public:
	UFunction * function;
        FcSymbol(char *, UFunction *);
};

class RcSymbol: public Symbol
{
public:
	UType * type;
	int index;
    RcSymbol(char *,UType *);
};

class SoSymbol: public Symbol
{
public:
	UType * type;
        SoSymbol(char *, UType *);
};

class StSymbol: public Symbol
{
public:
	buchistate * state;
        StSymbol(char *);
};


class IdList
{
	public:
	char * name;
	IdList * next;
};

#endif
