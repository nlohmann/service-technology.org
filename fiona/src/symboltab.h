#ifndef SYMBOLTAB_H
#define SYMBOLTAB_H

#include"petriNetNode.h"
#include"dimensions.h"
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
};

class PlSymbol: public Symbol
{
public:
	owfnPlace * place;
        PlSymbol(owfnPlace *); // LL- places
        PlSymbol(char *); // HL-places
};

class TrSymbol: public Symbol
{
public:
	owfnTransition * transition;
        TrSymbol(char *);
};

class IdList
{
	public:
	char * name;
	IdList * next;
};

#endif











