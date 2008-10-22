#include "symboltab.H"
#include "formula.H"
#include "net.H"

#ifndef BUCHI
#define BUCHI

// 
extern SymbolTab * BuchiTable;


class buchitransition;

class buchistate
{
public:
	bool final;
	char * name;
	buchitransition ** delta;
	buchitransition ** getenabled();
	int nrdelta;
	int code;
	static int nr;
	buchistate(char *);
	buchistate(){}
	buchitransition * transitionlist;  // only used for parsing
	buchistate * next; // only used for parsing
};

class buchitransition
{
public:
	formula * guard;
	buchistate * delta;
	buchitransition * next;  // only used for parsing
};


extern buchistate * initialbuchistate;
extern buchistate ** buchiautomaton;
void print_buchi();

#endif
