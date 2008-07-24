#include<stdlib.h>
#include"dimensions.H"
#include"net.H"
#include"formula.H"
#include"graph.H"
#include"stubborn.H"
#include"check.H"

#ifdef FINDPATH
//unsigned int numberenabled;
//unsigned int * enabledstatistic;

Transition ** Reserve; // Transiitons rejected in first selection scheme
unsigned int Rescounter; // next free entry in Reserve
	unsigned int * hashcounter;
	unsigned int * globalhashcounter;

#define stubbinsert(X)	{\
        if(!((X)->instubborn == StubbornStamp))\
		{\
				if((X)->enabled)\
				{\
					if((drand48() <= (1.0 / (1.0 + globalhashcounter[(Places[0]->hash_value+(X)->hash_change) % HASHSIZE] ))))\
					{\
						return(X);\
					}\
					else\
					{\
							Reserve[Rescounter++] = (X);\
					}\
				}\
	(X)->instubborn = StubbornStamp;\
	(X)->NextStubborn = (Transition *) 0;\
	if((X) -> StartOfStubbornList)\
        {\
                (X) -> EndOfStubbornList -> NextStubborn = (X);\
                (X) -> EndOfStubbornList = (X);\
        }\
        else\
        {\
                (X) -> StartOfStubbornList = (X) -> EndOfStubbornList = (X);\
        }\
		}\
}
void printstate(char *, unsigned int *);
unsigned int StubbornStamp;


#ifdef STUBBORN
Transition * closure()
{
	Transition * current;
	int i;

	for(current = current -> StartOfStubbornList;current; current = current -> NextStubborn)
	{
		for(i=0;current -> mustbeincluded[i];i++)
		{
			stubbinsert(current->mustbeincluded[i]);
		}
	}
	return (Transition *) 0;
}
	
Transition * insert_down(formula *);
		
Transition * insert_up(formula *f)
{

	unsigned int i;
	Transition * tt;

	switch(f -> type)
	{
	case neq:   for(i=0;i<((atomicformula *) f) ->p->NrOfArriving;i++)
		   {	
			stubbinsert(((atomicformula *) f) ->p->ArrivingArcs[i]->tr);
		   }
		   for(i=0;i<((atomicformula *) f) ->p->NrOfLeaving;i++)
		   {
			stubbinsert(((atomicformula *) f) ->p->LeavingArcs[i]->tr);
		   }
		   break;
	case eq:   if(CurrentMarking[((atomicformula *) f) ->p->index] < ((atomicformula *) f)  -> k)
		   {
		   for(i=0;i<((atomicformula *) f) ->p->NrOfArriving;i++)
		   {	
			stubbinsert(((atomicformula *) f) ->p->ArrivingArcs[i]->tr);
		   }
		   }
		   else
		   {
		   for(i=0;i<((atomicformula *) f) ->p->NrOfLeaving;i++)
		   {
			stubbinsert(((atomicformula *) f) ->p->LeavingArcs[i]->tr);
		   }
		   }
		   break;
	case leq:
	case lt:   for(i=0;i<((atomicformula *) f) ->p->NrOfLeaving;i++)
                   {
                        stubbinsert(((atomicformula *) f) ->p->LeavingArcs[i]->tr);
                   }   
		   break;
	case geq:
	case gt:   for(i=0;i<((atomicformula *) f) ->p->NrOfArriving;i++)
                   {
                        stubbinsert(((atomicformula *) f) ->p->ArrivingArcs[i]->tr);
                   }   
                   break;  
	case conj: return(insert_up(((booleanformula *) f)->sub[0])); 
	case disj: for(i=0;i < ((booleanformula *) f)->cardsub;i++)
		   {
			if(tt = insert_up(((booleanformula *) f)->sub[i])) return tt;
		   }
		   return(Transition*) 0;
	case neg:  return(insert_down(((unarybooleanformula *) f) ->sub));
		   break;
	default:   cout << "feature not implemented\n";
	}
	return (Transition *) 0;
}

Transition * insert_down(formula *f)
{

	unsigned int i;
	Transition * tt;

	switch(f -> type)
	{
	case neq: if(CurrentMarking[((atomicformula *) f) ->p->index] < ((atomicformula *) f) ->k)
		   {
		   for(i=0;i<((atomicformula *) f) ->p->NrOfArriving;i++)
		   {	
			stubbinsert(((atomicformula *) f) ->p->ArrivingArcs[i]->tr);
		   }
		   }
		   else
		   {
		   for(i=0;i<((atomicformula *) f) ->p->NrOfLeaving;i++)
		   {
			stubbinsert(((atomicformula *) f) ->p->LeavingArcs[i]->tr);
		   }
		   }
		   break;
	case eq:   for(i=0;i<((atomicformula *) f) ->p->NrOfArriving;i++)
		   {	
			stubbinsert(((atomicformula *) f) ->p->ArrivingArcs[i]->tr);
		   }
		   for(i=0;i<((atomicformula *) f) ->p->NrOfLeaving;i++)
		   {
			stubbinsert(((atomicformula *) f) ->p->LeavingArcs[i]->tr);
		   }
		   break;
	case geq:
	case gt:   for(i=0;i<((atomicformula *) f) ->p->NrOfLeaving;i++)
                   {
                        stubbinsert(((atomicformula *) f) ->p->LeavingArcs[i]->tr);
                   }   
		   break;
	case leq:
	case lt:   for(i=0;i<((atomicformula *) f) ->p->NrOfArriving;i++)
                   {
                        stubbinsert(((atomicformula *) f) ->p->ArrivingArcs[i]->tr);
                   }   
                   break;  
	case disj: return(insert_down(((booleanformula *) f) ->sub[(rand() %(((booleanformula *) f) ->cardsub - ((booleanformula *) f) ->firstvalid)) + ((booleanformula *) f) ->firstvalid]));
	case conj: for(i=((booleanformula *) f)->firstvalid;i<((booleanformula *) f)->cardsub;i++)
		   {
			if(tt = insert_down(((booleanformula *) f)->sub[i])) return tt;
		   }
		   return(Transition *) 0;
	case neg:  return(insert_up(((unarybooleanformula *) f) ->sub));
		   break;
        default:   cout << "feature not implemented\n";
	}
	return(Transition*) 0;
}
#endif

#ifndef STUBBORN
Transition * GetFullTransition()
{
	Transition ** fl;
	Transition * t;

	fl = firelist();
	if(fl && CardFireList)
	{
	t= fl[rand() % CardFireList];
	}
	else
	{
	t= (Transition *) 0;
	}
	delete [] fl;
	return(t);
}
#endif

#ifdef STUBBORN
Transition * GetStubbornTransition()
{
 	Transition * t;

	Transitions[0]->StartOfStubbornList = (Transition *) 0;
	StubbornStamp++;
	Rescounter = 0;
	if(t = insert_up(F)) return t;
	if(t = closure()) return t;
	if(Rescounter)
	{
		return Reserve[rand() % Rescounter];
	}
	else
	{
		return(Transition *) 0;
	}
}
#endif

	unsigned int j;
	unsigned int attempt;
	unsigned int nonemptyhash;
	unsigned int globalnonemptyhash;
	unsigned int x;
extern	unsigned int NrOfStates;
	unsigned int depth;
	Transition ** path;
	unsigned int k; // current path element;

	
unsigned int * strichliste;

void find_path()
{
	
//Transition ** otherpath;
//Transition ** tmppath;
//unsigned int sss;
//strichliste = new unsigned int [12];
//enabledstatistic = new unsigned int [45];

	int res;
	unsigned int OriginalNrEnabled;
	Reserve = new Transition * [Transitions[0]->cnt];
	if(!F)
	{
		cerr << "\nspecify predicate in analysis task file!\n";
	}
	F = F -> reduce(& res);
	if(res == 0) return;
	if(res == 1) _exit(0);
	F = F -> posate();
	F -> tempcard = 0;
	F -> setstatic();
	if(F -> tempcard > 0)
	{
		cerr << "do not use temporal operators in this mode.\n" ;
		_exit(3);
	}
	cout << "\nFormula with\n" << F -> card << " subformulas.\n";
	attempt = 0;
	StubbornStamp = 0;
	// 1. initial state speichern
	for(x=0;x<Places[0] -> cnt;x++)
	{
		Places[x]->initial_marking = CurrentMarking[x];
	}
	// store change of hash values by transition occurrences
	for(x=0;x<Transitions[0]->cnt;x++)
	{	
		Transitions[x] -> set_hashchange();
	}
	// 2. new hash table
	hashcounter = new unsigned int [HASHSIZE+1];
	globalhashcounter = new unsigned int [HASHSIZE+1];
	// 3. new path
	path = new (Transition *) [MAXPATH+1];
//otherpath = new (Transition *) [MAXPATH+1];

	// 4. optimize formula
	// 4.1. create, for all places, list of propositions mentioning place
	// Pass 1 count;
	F->parent = (formula *) 0;
		for(x=0;x<HASHSIZE;x++)
		{
			globalhashcounter[x]=0;
		}
		globalnonemptyhash = 1;
	OriginalNrEnabled = Transitions[0]->NrEnabled;
	while(1)
	{
//for(sss=0;sss<11;sss++)
//{
//strichliste[sss] = 0;
//}
//for(sss=0;sss<45;sss++)
//{
//	enabledstatistic[sss] = 0;
//}
		// init new search attempt
		attempt++;
		cerr << "Attempt # " << attempt << "\n";
		for(x=0;x<Places[0]->cnt;x++)
		{
			Places[x]->set_marking(Places[x]->initial_marking);
		}
		for(x=0;x<Transitions[0]->cnt;x++)
		{
			Transitions[x]->enabled = true;
			if(x<Transitions[0]->cnt -1) Transitions[x]->NextEnabled = Transitions[x+1];
			if(x) Transitions[x]->PrevEnabled = Transitions[x-1];
#ifdef STUBBORN
			Transitions[x]->mustbeincluded =
				Transitions[x]->conflicting;
#endif
		}
		Transitions[0]->StartOfEnabledList = Transitions[0];
		Transitions[Transitions[0]->cnt -1]->NextEnabled = (Transition*) 0;
		Transitions[0]->PrevEnabled = (Transition *) 0;
		for(x=0;x<Transitions[0]->cnt;x++)
		{
			Transitions[x]->check_enabled();
#ifdef STUBBORN
			Transitions[x]->instubborn = 0;
#endif
		}
		Transitions[0]->NrEnabled = OriginalNrEnabled;
		for(x=0;x<HASHSIZE;x++)
		{
			hashcounter[x]=0;
		}
		nonemptyhash = NrOfStates = 1;
		hashcounter[Places[0]->hash_value] ++;
		globalhashcounter[Places[0]->hash_value] ++;
		if(F -> initatomic())
		{	
			cout << "\n\nInitial state satisfies formula\n\n";
			if(pflg)
			{
	ofstream pathstream(pathfile);
	if(!pathstream)
	{
		cerr << "Cannot open path output file: " << pathfile <<
		"\nno output written";
		pflg = false;
	}
				pathstream << "PATH\n";
			}
			if(Pflg)
			{
				cout <<  "PATH\n";
			}
			return;
		}
		// SEARCH!!
		for(k=0;k<MAXPATH;k++)
		{
			Transition * t;

			if(!((NrOfStates) % REPORTFREQUENCY))
			{
				cerr << "Depth " << NrOfStates << "\n";
			}
			if(!(t=
#ifdef STUBBORN
GetStubbornTransition()
#else
GetFullTransition()
#endif
))
			{
				cerr << "DEADLOCK at depth " << k << "\n";
				break; // Deadlock!
			}
			t->fire();
			path[k] = t;
			update_formula(t);
			if(F->value)
			{
				cout << "\n\n State found!\n\n";
				if(pflg)
				{
	ofstream pathstream(pathfile);
	if(!pathstream)
	{
		cerr << "Cannot open path output file: " << pathfile <<
		"\nno output written";
		pflg = false;
	}
				pathstream << "PATH\n";
				for(j=0;j<=k;j++)
				{
					pathstream << path[j]->name << "\n";
				}
				}
				if(Pflg)
				{
				cout << "PATH\n";
				for(j=0;j<=k;j++)
				{
					cout << path[j]->name << "\n";
				}
				}
				printstate("",CurrentMarking);
				return;
			}
			NrOfStates++;
			if(!hashcounter[Places[0]->hash_value]) nonemptyhash++;
			if(!globalhashcounter[Places[0]->hash_value]) globalnonemptyhash++;
			hashcounter[Places[0]->hash_value]++;
			globalhashcounter[Places[0]->hash_value]++;
		}
		cerr << "hash entries (this attempt): " << nonemptyhash << " (all attempts): " << globalnonemptyhash << "\n";
	}
}

#endif
