#include "buchi.H"

int buchistate::nr = 0;

SymbolTab * BuchiTable;
buchistate * initialbuchistate;
buchistate ** buchiautomaton;

buchitransition ** buchistate::getenabled()
{
	int i,j;
	buchitransition ** enabled;

	enabled = new buchitransition * [nrdelta+1];
	for(i=j=0;i<nrdelta;i++)
	{
		if(delta[i]->guard->value)
		{
			enabled[j++] = delta[i];
		}
	}
	enabled[j] = (buchitransition *) 0;
	return enabled;
}

buchistate::buchistate(char * n)
{
	name = n;
	code = nr++;
	final = 0;
	nrdelta = 0;
	delta = (buchitransition **) 0;
	transitionlist = (buchitransition *) 0;
	next = (buchistate *) 0;
}

void init_buchi()
{

	int i;
	// 0. translate list of states into array of states
	buchiautomaton = new buchistate * [buchistate::nr + 1];
	buchistate * b;
	for(i=0,b=initialbuchistate;b;b = b -> next, i++)
	{
		buchiautomaton[i] = b;
	}
	buchiautomaton[i] = (buchistate *) 0;
	
	// 1. translate list of transitions into array of transitions

	
	// 2. process guard formulas
}

void print_buchi()
{
	int i,j;

	cout << "Initial state: " << initialbuchistate -> name << endl;
	for(i=0;i<buchistate::nr;i++)
	{
		cout << "State " << buchiautomaton[i]->name << buchiautomaton[i]-> code;
		if(buchiautomaton[i]->final) cout << " (final)";
		cout << endl << "transitions:" << endl;
		for(j=0;j<buchiautomaton[i]->nrdelta;j++)
		{
			cout << "to " << buchiautomaton[i]->delta[j]->delta -> name << endl;
		}
	}
}
		
