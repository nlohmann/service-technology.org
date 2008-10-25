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
