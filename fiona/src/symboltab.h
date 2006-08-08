#ifndef SYMBOLTAB_H
#define SYMBOLTAB_H

#include "mynew.h"
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











