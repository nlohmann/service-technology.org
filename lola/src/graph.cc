#include "formula.H"
#include "graph.H"
#include "stubborn.H"
#include "dimensions.H"

#ifdef DISTRIBUTE
#include "distribute.h"
#endif

#include <fstream>
#include <cstring>
#include <cstdio>

Decision ** HashTable;
unsigned int  * BitHashTable;
unsigned int LastChoice;
Decision * LastDecision;
unsigned int Scapegoat;
Statevector * LastVector;
unsigned int currentdfsnum = 1;
SearchTrace * Trace;
unsigned int CardFireList;
unsigned int pos;
unsigned int isbounded;
unsigned int State::card = 0;
binDecision ** binHashTable;
unsigned int largest_sat = 0; // largest dfs + 1 of a state satisfying given predicate
	unsigned int bin_p; // (=place); index in MARKINGVECTOR
	unsigned int bin_pb; // next bit of place to be processed;
	unsigned char bin_byte; // byte to be matched against tree vector; constructed from MARKINGVECTOR
	unsigned int bin_t; // index in tree vector
	unsigned char * bin_v; // current tree vector
	unsigned int bin_s; // nr of bits pending in byte from previous iteration
	unsigned int bin_d; // difference position
	unsigned int bin_b; // bit nr at start of byte
	unsigned int bin_dir; // bit nr at start of byte
	binDecision * fromdec, * todec, *vectordec;
#ifdef FULLTARJAN
State * TarStack;
#endif

int Persistents;
#ifdef COVER
unsigned int * Ancestor;
#endif

char rapportstring[] = "search";

  unsigned int NrOfStates;
  unsigned int Edges;
void statistics(unsigned int s, unsigned int e, unsigned int h)
{
	cout << "\n\n>>>>> " << s << " States, " << e << " Edges, " << h << " Hash table entries\n\n";
}

Transition ** firelist()
{
  Transition ** tl;
  Transition * t;
  int i;
  tl = new Transition * [Transitions[0] -> NrEnabled + 1];
  for(i=0,t = Transitions[0]->StartOfEnabledList; t; t = t -> NextEnabled)
    {
#ifdef EXTENDEDCTL
	if(t -> pathrestriction[TemporalIndex])
	{
#endif
      tl[i++] = t;
#ifdef EXTENDEDCTL
	}
#endif
    }
  tl[i] = (Transition *) 0;
  CardFireList = i;
  return tl;
}

void printstate(char const*, unsigned int *);
void printmarking()
{
	unsigned int i;

	for(i=0;i<Places[0]->cnt;i++)
	{
		cout << Places[i]->name << " : " << CurrentMarking[i] << endl;
	}
	cout << "-------" << endl;
}

StatevectorList * TSCCRepresentitives;

#ifndef MODELCHECKING

#ifdef STUBBORN
#include"stubborn.H"
#endif
State * SEARCHPROC();
unsigned int MinBookmark; // MIN number of the first closed marking
                          // in the currently or last recently processed TSCC
void printpath(State *,ofstream *);
void print_path(State * s)
{
	if(pflg)
	{
		ofstream pathstream(pathfile);
		if(!pathstream)
		{
			cerr << "Cannot open path output file: " << pathfile <<
			"\nno output written\n";
		}
		pathstream << "PATH\n";
		printpath(s,&pathstream);
	}
	if(Pflg)
	{
		cout << "PATH\n";
		printpath(s,(ofstream *) 0);
	}
}
void printpath(State *s,ofstream * pathstream)
{
	// print a path from initial state to s
	if(s -> parent)
	{
		printpath(s -> parent,pathstream);
		if(Pflg)
		{
		cout << s -> parent -> firelist[s -> parent -> current] -> name << "\n";
		}
		if(pflg)
		{
		(*pathstream) << s -> parent -> firelist[s -> parent -> current] -> name << "\n";
		}
	}
}

void printincompletestates(State *s,ofstream * graphstream,int level)
{
#ifndef CYCLE
#ifdef TARJAN
	int j;
	// level = 1 --> top level, no firelist, mark '!'
	// level = 0 --> other level, firelist,  mark '*'
	if(!s) return;

	if(gmflg)
	{
	  (*graphstream) << "STATE " << (level ? "! " : "* ") << s ->dfs ;
	  j=0;
	  if(graphformat == 'm')
	  {
		 for(unsigned int i=0;i<Places[0]->cnt;i++)
		 {
			 if(CurrentMarking[i])
				{
				 if(CurrentMarking[i] == VERYLARGE)
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << "oo" ;
				 }
				 else
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << CurrentMarking[i] ;
				 }
				 }
		 }

	  }
		 (*graphstream) << "\n\n";
	  if(!level)
	  {
	  for(unsigned int i=0; i < s -> current;i++)
	  {
		(*graphstream) << s -> firelist[i]->name << " -> " << s -> succ[i]->dfs << "\n";
      }
		(*graphstream) << s -> firelist[s->current]->name << " => " << s -> succ[s->current]->dfs << "\n";
	  for(unsigned int i = s -> current + 1; s ->firelist[i];i++)
	  {
		(*graphstream) << s -> firelist[i]->name << " -> ?\n";
      }
	  }
	  (*graphstream) << "\n";
	}
	if(GMflg)
	{
	  cout << "STATE " << (level ? "! " : "* ") << s ->dfs;
	  j=0;
	  if(graphformat == 'm')
	  {
		 for(unsigned int i=0;i<Places[0]->cnt;i++)
		 {
			 if(CurrentMarking[i])
			 {
				 if(CurrentMarking[i] == VERYLARGE)
				 {

				 cout << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << "oo";
				 }
				 else
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << CurrentMarking[i];
				 }
			 }
		 }

	  }
	  cout << "\n\n";
	  if(!level)
	  {
	  for(unsigned int i=0; i < s -> current;i++)
	  {
		cout << s -> firelist[i]->name << " -> " <<
		s -> succ[i]->dfs << "\n";
      }
		cout << s -> firelist[s->current]->name << " => " << s -> succ[s->current]->dfs << "\n";
	  for(unsigned int i = s -> current + 1; s ->firelist[i];i++)
	  {
		cout << s -> firelist[i]->name << " -> ?\n";
      }
	  }
	  cout << "\n";
	}
if(!s->parent) return;
#ifdef COVER
		  if(s -> NewOmega)
		  {
			  // Replace new omegas by their old values
			  for(unsigned int i=0;s ->NewOmega[i];i++)
			  {
				s ->NewOmega[i]->set_cmarking(s ->NewOmega[i]->lastfinite);
				s ->NewOmega[i]->bounded = true;
			  }
			  delete [] s ->NewOmega;
		  }
#endif
	s -> parent -> firelist[s -> parent -> current] -> backfire();
	printincompletestates(s -> parent,graphstream,0);
#endif
#endif
}

#ifdef COVER
void print_reg_path(State *s, State * startofrepeatingseq,ofstream * pathstream,int level)
{
	if(level)
	{
		if(!pflg && ! Pflg) return;
		if(pflg)
		{
			pathstream = new ofstream(pathfile);
			if(!pathstream)
			{
				cerr << "Cannot open path output file: " << pathfile
				<< "\nno output written\n";
				return;
			}
			(*pathstream) << "PATH EXPRESSION \n";
		}
		else
		{
			cout << "PATH EXPRESSION \n";
		}
	}
	// print a regular expression for path  from initial state to
	// generalised state s in coverability graph
	if(startofrepeatingseq)
	{
		if(s == startofrepeatingseq)
		{
			if(s -> parent)
			{
				int i;
				print_reg_path(s -> parent,s->smaller,pathstream,0);
			if(s->smaller)
			{
				if(Pflg)
				{
				cout << " ";
				}
				else
				{
				(*pathstream) << " ";
				}
			}
			else
			{
				if(Pflg)
				{
				cout << "\n";
				}
				else
				{
				(*pathstream) << "\n";
				}
			}
				if(Pflg)
				{
				cout << s -> parent -> firelist[s -> parent -> current] -> name;
				}
				else
				{
				(*pathstream) << s -> parent -> firelist[s -> parent -> current] -> name;
				}
			}
			if(s->smaller)
			{
				if(Pflg)
				{
				cout << " )";
				}
				else
				{
				(*pathstream) << " )";
				}
			}
			if(Pflg)
			{
			cout << "\n(";
			}
			else
			{
			(*pathstream) << "\n(";
			}
		}
		else
		{
			if(s -> parent)
			{
				int i;
				print_reg_path(s -> parent,startofrepeatingseq,pathstream,0);
				if(Pflg)
				{
				cout << " " << s -> parent -> firelist[s->parent->current] -> name ;
				}
				else
				{
				(*pathstream)  << " " << s -> parent -> firelist[s->parent->current] -> name;
				}
			}
			if(s->smaller)
			{
				if(Pflg)
				{
				cout << " )";
				}
				else
				{
				(*pathstream) << " )";
				}
			}
		}
	}
	else
	{
		if(s -> parent)
		{
			int i;
			print_reg_path(s -> parent,s->smaller,pathstream,0);
			if(s->smaller)
			{
				if(Pflg)
				{
				cout << " ";
				}
				else
				{
				(*pathstream) << " ";
				}
			}
			else
			{
				if(Pflg)
				{
				cout << "\n";
				}
				else
				{
				(*pathstream) << "\n";
				}
			}
			if(Pflg)
			{
			cout << s -> parent -> firelist[s->parent->current] -> name ;
			}
			else
			{
			(*pathstream) << s -> parent -> firelist[s->parent->current] -> name ;
			}

		}
		if(s->smaller)
		{
			if(Pflg)
			{
			cout << " )";
			}
			else
			{
			(*pathstream) << " )";
			}
		}
	}
}
#endif


#define MIN(X,Y) ( (X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ( (X) > (Y) ? (X) : (Y))

State * CurrentState;

#if defined(FAIRPROP) || defined(EVENTUALLYPROP) || defined(STABLEPROP)

bool analyse_fairness(State * pool, unsigned int level)
{
	// 1. cut the (not necessarily connected) graph in pool into sccs.
	//    All states in search space have tarlevel = level && ! expired.
	//    Before return "false", all states in pool must be "expired".

	State * C,* N; // current, new state in dfs
	State * T; // local tarjan stack;
	State * S; // local scc
	unsigned int card = 1;
	State * O; // parent of root of local scc (must be set after analysis of scc)
	unsigned int oldd, oldm, oldc;

	while(pool)
	{
		// proceed as long as there are states in pool
		// choose element from pool
		C = pool;
		pool = pool -> nexttar;
		if(pool == C) pool = (State *) 0;
		T = C;
		// unlink from pool and init new dfs
		C -> nexttar -> prevtar = C -> prevtar;
		C -> prevtar -> nexttar = C -> nexttar;
		C -> nexttar  = C -> prevtar = C;
		C -> current = 0;
		C -> tarlevel = level + 1;
		C -> parent = (State *) 0;
		C -> ddfs = C -> mmin = 1;
		while(C)
		{
			if(C -> firelist[C -> current])
			{
				N = C -> succ[C->current];
				if(N -> tarlevel == level && ! N -> expired)
				{
					// new state
					N -> mmin = N -> ddfs = ++card;
					N -> current =0 ;
					N -> parent = C;
					N -> tarlevel = level + 1;
					// put state on local tarjan stack,
					// and remove it from pool
					if(N == pool)
					{
						pool = pool -> nexttar;
						if(pool == N)
						{
							pool = (State *) 0;
						}
					}
					N -> nexttar -> prevtar = N -> prevtar;
					N -> prevtar -> nexttar = N -> nexttar;
					if(T)
					{
						N -> nexttar = T -> nexttar;
						T -> nexttar = N;
						N -> prevtar = T;
						N -> nexttar -> prevtar = N;
					}
					else
					{
						N -> nexttar = N -> prevtar = N;
					}
					T = N;
					C = N;
				}
				else
				{
					if(N -> tarlevel == level + 1 && ! N -> expired)
					{
						// existing state in same scc
						C -> mmin = MIN(N -> ddfs, C -> mmin);
						C -> current++;
					}
					else
					{
						// existing state in expired scc or outside
						// current search space
						C -> current++;
					}
				}
			}
			else
			{
				// close state
				// A) check scc
				if(C -> mmin == C -> ddfs)
				{
					// data of C must be preserved for proper backtracking
					// after having finished this scc
					O = C -> parent;
					oldc = C -> current;
					oldd = C -> ddfs;
					oldm = C -> mmin;
					// B) process scc
					// B0) unlink new component from local tarjan stack
					if(C -> ddfs > 1) // proper cut
					{
						C -> prevtar -> nexttar = T -> nexttar;
						T -> nexttar -> prevtar = C -> prevtar;
						T -> nexttar = C;
						S = C -> prevtar;
						C -> prevtar = T;
						T = S;
					}
					S = C;

					// B1) check fairness of new scc
					State * ss;
#ifdef STABLEPROP
					unsigned int cardphi;
					cardphi = 0;
#endif
					unsigned int cardS;
					unsigned int i;
					for(i=0;i<Transitions[0]->cnt;i++)
					{
						Transitions[i]-> faired = Transitions[i]->fairabled = 0;
					}
					for(cardS = 1,ss = S->nexttar;;cardS++,ss = ss ->nexttar)
					{
						for(i=0;ss->firelist[i];i++)
						{
							ss->firelist[i]->fairabled ++;
							if((ss->succ[i]-> tarlevel == level + 1) && !(ss->succ[i]->expired))
							{
								ss->firelist[i]->faired ++;
							}
						}
#ifdef STABLEPROP
						if(ss->phi) cardphi ++;
#endif
						if(ss == S) break;
					}

#ifdef STABLEPROP
					if(cardphi ==  cardS)
					{
						goto aftercheck;
					}
#endif
					for(i=0;i<Transitions[0]->cnt;i++)
					{
						if(Transitions[i]->fairness > 0)
						{
							if((!Transitions[i]->faired) &&
							     Transitions[i]->fairabled == cardS)
							{
								goto aftercheck;
								// no subset can be fair
							}
						}
					}
					for(i=0;i<Transitions[0]->cnt;i++)
					{
						if(Transitions[i]->fairness == 2)
						{
							if(Transitions[i]->fairabled && ! Transitions[i]->faired)
							{
								// 1. remove all transitions
								// from S that enable t[i]
								// At this point, there must
								// be some state remaining in S,
								// otherwise the weak fairness test
								// would have failed.
								while(Transitions[i]->fairabled)
								{
									State * E;
									unsigned int j;
									E = (State *) 0;
									for(j=0;S -> firelist[j];j++)
									{
										if(S -> firelist[j] == Transitions[i])
										{
											E = S;
											break;
										}
									}
									S = S -> nexttar;
									if(E)
									{
										E -> expired = true;
										E -> nexttar -> prevtar = E -> prevtar;
										E -> prevtar -> nexttar = E -> nexttar;
										Transitions[i]->fairabled--;
									}
								}
								if(analyse_fairness(S,level + 1))
								{
									return true;
								}
								goto aftercheck;
							}
						}
						Transitions[i] -> faired = Transitions[i]-> fairabled = 0;
					}
					return true; // arrived here only if all transitions have paased fairness test.

aftercheck:
					for(;!(S -> expired);S = S -> nexttar)
					{
						S -> expired = true;
					}
					C -> parent = O;
					C -> ddfs = oldd;
					C -> mmin = oldm;
					C -> current = oldc;

				}
				// end process scc
				// C) return to previous state
				N = C;
				C = C -> parent;
				if(C)
				{
					C -> mmin = MIN(C -> mmin, N -> mmin);
					C -> current++;
				}
			} // end for all new scc
		} // end for all states
	} // end, for all nodes in pool
	return false;
} // end analyse_fairness
#endif

unsigned int depth_first()
{
Persistents = 0;
#ifdef DEPTH_FIRST
  ofstream * graphstream;
  unsigned int i;
  State * NewState;
#ifdef CYCLE
  bool IsCyclic;
  unsigned int silentpath; // nr of consecutive non-stored states
  Transition ** fl;
#endif
  // init initial marking and hash table
  isbounded = 1;
#ifndef CYCLE
  if(gmflg)
  {
	graphstream = new ofstream(graphfile);
	if(!*graphstream)
	{
		cerr << "cannot open graph output file: " << graphfile << "\n";
		cerr << "no output written\n";
		gmflg = false;
	}
  }
#else
  silentpath = 0;
#endif
#ifndef DISTRIBUTE
#if defined(SYMMETRY) && SYMMINTEGRATION==1
  Trace = new SearchTrace [Places[0]->cnt];
#endif
  for(i = 0; i < HASHSIZE;i++)
    {
#ifdef BITHASH
	 BitHashTable[i]  = 0;
#else
      binHashTable[i] = (binDecision *) 0;
#endif
    }
#endif
#ifdef WITHFORMULA
#ifndef TWOPHASE
	int res;
	if(!F)
	{
		cerr << "\nspecify predicate in analysis task file!\n";
		_exit(4);
	}
	F = F -> reduce(&res);
	if(res<2) return res;
	F = F -> posate();
	F -> tempcard = 0;
	F -> setstatic();
	if(F ->  tempcard)
	{
		cerr << "temporal operators are not allowed in state predicates\n";
		exit(3);
	}
	cout << "\n Formula with\n" << F -> card << " subformula(s).\n";
	F -> parent = (formula *) 0;
#endif
#endif
#ifdef DISTRIBUTE
  Reason WhyTerminated;
	NrOfStates = 0;
        Edges = 0;
  while(get_new_vector(CurrentMarking, WhyTerminated))
  {
       // don't worry, this loop ends only after the end of the actual dfs loop

       CurrentState = new State;
	for(i=0;i<Transitions[0]->cnt;i++)
	{
		Transitions[i]->check_enabled();
	}
       CurrentState -> firelist = FIRELIST();
	NrOfStates++;
#else
  if(SEARCHPROC()) cerr << "Sollte eigentlich nicht vorkommen";
	NrOfStates = 1;
	Edges = 0;
#ifdef CYCLE
  // check if state enables cycling transition
  fl = FIRELIST();
#ifdef NONBRANCHINGONLY
  if(fl && fl[0] && (fl[1] || fl[0]->cyclic))
  {
		IsCyclic = true;
  }
  else
  {
		IsCyclic = false;
  }
#else
  if(fl)
  {
	for(i=0,IsCyclic=false;fl[i];i++)
	{
		if(fl[i]->cyclic)
		{
			IsCyclic = true;
			break;
		}
	}
  }
  else
  {
	IsCyclic = false;
  }
#endif
  if(IsCyclic)
  {
	CurrentState = INSERTPROC();
  	CurrentState -> firelist = fl;
  }
  else
  {
	CurrentState = new State();
	CurrentState -> firelist = fl;
	NrOfStates = 0;
  }
#else

  CurrentState = INSERTPROC();
        CurrentState -> firelist = FIRELIST();
#endif
#endif
#ifdef COVER
	Ancestor = new unsigned int [Places[0]->cnt + 1];
#endif
  CurrentState -> current = 0;
  CurrentState -> parent = (State *) 0;
#if defined(DEADLOCK) && !defined(DISTRIBUTE)
	if(!(CurrentState -> firelist) || ! (CurrentState -> firelist[0]))
	{
		// early abortion
		cout << "\ndead state found!\n";
		printstate("",CurrentMarking);
		print_path(CurrentState);
		printincompletestates(CurrentState,graphstream,1);
		statistics(NrOfStates,Edges,NonEmptyHash);
		return 1;
	}
#endif
#ifdef TSCC
	TSCCRepresentitives = (StatevectorList *) 0;
#endif
#ifdef BOUNDEDPLACE
	if(!CheckPlace)
	{
		cerr << "\nspecify place to be checked in analysis task file\n";
		_exit(4);
	}
#endif
#ifdef DEADTRANSITION
	if(!CheckTransition)
	{
		cerr << "\n specify transition to be checked in analysis task file!\n";
		_exit(4);
	}
#ifndef DISTRIBUTE
	if(CheckTransition -> enabled)
	{
		// early abortion
		cout << "\ntransition " << CheckTransition -> name << " is not dead!\n";
		printstate("",CurrentMarking);
		print_path(CurrentState);
		printincompletestates(CurrentState, graphstream,1);//1=toplevel

		statistics(NrOfStates,Edges,NonEmptyHash);
		return 1;
	}
#endif
#endif
#if defined ( STUBBORN ) && defined ( REACHABILITY ) && ! defined(DISTRIBUTE)
  if(!CurrentState -> firelist )
	{
		// early abortion
	      cout << "\nstate found!\n";
		  printstate("",CurrentMarking);
		print_path(CurrentState);
		printincompletestates(CurrentState, graphstream,1);//1=toplevel
		statistics(NrOfStates,Edges,NonEmptyHash);
		return 1;
         }
#endif
#if defined (REACHABILITY ) && ! defined ( STUBBORN ) && ! defined(DISTRIBUTE)
for(i=0;i<Places[0]->cnt;i++)
{
	 if(CurrentMarking[i] != Places[i]->target_marking)
		 break;
}
if(i >= Places[0]->cnt) // target_marking found!
{
		// early abortion
	      cout << "\nstate found!\n";
		printstate("",CurrentMarking);
		print_path(CurrentState);
		printincompletestates(CurrentState, graphstream,1);//1=toplevel
		statistics(NrOfStates,Edges,NonEmptyHash);
	 return 1;
}
#endif
#ifdef COVER
  CurrentState -> NewOmega = (Place **) 0;
#endif
#if defined(FAIRPROP) || defined(STABLEPROP)
	F -> initatomic();
#endif

#ifdef EVENTUALLYPROP
	if(F -> initatomic())
	{
		cout << "\neventually phi holds.\n";
		statistics(NrOfStates,Edges,NonEmptyHash);
	}
#endif
#ifdef STATEPREDICATE
	if(F -> initatomic())
	{
#if defined(LIVEPROP) && ! defined(TWOPHASE)
		largest_sat = 1;
#else
		cout << "\nstate found!\n";
		printstate("",CurrentMarking);
		print_path(CurrentState);
		printincompletestates(CurrentState,graphstream,1);
		statistics(NrOfStates,Edges,NonEmptyHash);
#ifdef DISTRIBUTE
		heureka(resultfixedR,CurrentMarking);
		end_communication();
#endif
		return 1;
#endif
	}
#endif
#ifdef TARJAN
  CurrentState -> succ = new State * [CardFireList+1];
  CurrentState -> dfs = CurrentState -> min = 0;
#ifdef FULLTARJAN
	CurrentState -> phi = F -> value;
	CurrentState -> nexttar = CurrentState -> prevtar = CurrentState;
	TarStack = CurrentState;
#endif
#endif

  // process marking until returning from initial state

  while(CurrentState)
    {
#ifdef EXTENDED
	// ddfsnum must be passed to net.H for tracing lastdisabed and lastfired
	currentdfsnum = CurrentState -> dfs + 1; // 0 reserved for "never disabled"
											 // and "never fired"
#endif
      if(CurrentState -> firelist[CurrentState -> current])
	{
	  // there is a next state that needs to be explored
	  Edges ++;
	      if(!(Edges % REPORTFREQUENCY))
		{
#ifdef DISTRIBUTE
		rapport(rapportstring);
#else
              cerr << "st: " << NrOfStates << "     edg: " << Edges << "\n";
#endif
		}
	  CurrentState -> firelist[CurrentState -> current] -> fire();
#ifdef DISTRIBUTE
	// In the distributed context, we we to check for target states right now,
        // since a call to SEARCHPROC could mean that the state is shipped somewhere
  	// else, waiting there a long time before being touched. Thus, search would
	// proceed much longer than necessary. Outside the distributed context, it is
	// however wise to postpone the check for target states, because
	// 1. The check can be skipped when the state is not new
	// 2. Information gathered during fire list generation can be used for
	// checking the property efficiently.
#ifdef DEADLOCK
	if(!Transitions[0]->NrEnabled) { cout << "heureka" << endl; heureka(resultfixedR,CurrentMarking); end_communication(); return 1;}
#endif
#ifdef DEADTRANSITION
	if(CheckTransition -> enabled) { heureka(resultfixedR,CurrentMarking); end_communication(); return 1;}
#endif
#ifdef REACHABILITY
	for(i=0;i<Places[0]->cnt;i++)
	{
		if(CurrentMarking[i] != Places[i]->target_marking)
		{
			break;
		}
	}
	if(i < Places[0]->cnt) { heureka(resultfixedR,CurrentMarking); end_communication(); return 1;}
#endif
#ifdef WITHFORMULA
#ifndef TWOPHASE
	update_formula(CurrentState -> firelist[CurrentState -> current]);
#endif
#endif
#ifdef STATEPREDICATE
	if(F -> value) { heureka(resultfixedR,CurrentMarking); end_communication(); return 1;}
#endif
#endif
#ifdef COVER
	//In coverability graphs, we need to check for new w
	// 1. Search backwards until last w-Intro for smaller state
	unsigned int NrCovered;
	State * smallerstate;
	Place ** NewOmegas;

	NewOmegas = (Place **)0;
	// for all ancestor states do ...
	for(i=0;i<Places[0]->cnt;i++)
	{
		Ancestor[i]= CurrentMarking[i];
	}
	for(smallerstate = CurrentState; smallerstate; smallerstate =
	    smallerstate -> parent)
	{

		smallerstate -> firelist[CurrentState ->  current] -> traceback();
		NrCovered = 0;
		for(i=0;i<Places[0]->cnt;i++)
		{
				// case 1: smaller state[i] > current state [i]
				// ---> continue with previous state
				if(Ancestor[i] > CurrentMarking[i])
				{
					goto nextstate;
				}
				// case 2: smaller state < current state
				// count w-Intro
				if(Ancestor[i] < CurrentMarking[i])
				{
					NrCovered++;
				}
				// case 3: smaller state = current state --> do nothing
		}
		// if arrived here, it holds smaller <= current
		// covering is proper iff NrCovered > 0
		// If covering is not proper, (smaller state = current state)
		// current marking is not new, ancestors of smaller marking cannot
		// be smaller than current marking, since they would be smaller than
		// this smaller marking --> leave w-Intro procedure
		if(!NrCovered)
		{
			smallerstate = (State *) 0;
			goto endomegaproc;
		}
		// Here, smallerstate IS less than current state.
		isbounded = 0;
		NewOmegas = new Place * [NrCovered+1];
		// for all fragements of state vector do ...
		NrCovered = 0;
		for(i=0;i<Places[0]->cnt;i++)
		{
				if(Ancestor[i] < CurrentMarking[i])
				{
					// Here we have a place that deserves a new Omega
					// 1. set old value in place record
					Places[i] -> lastfinite =
					 CurrentMarking[i];
					Places[i] -> set_cmarking(VERYLARGE);
					Places[i] -> bounded = false;
					NewOmegas[NrCovered++] = Places[i];
				}
		}
		NewOmegas[NrCovered] = (Place*) 0;
		goto endomegaproc;
		if(smallerstate -> smaller) // smallerstate is a omega-introducing state
		{
			break;
		}
nextstate: ;
	}
endomegaproc:
if(!NewOmegas) smallerstate = (State *) 0;
#endif
#ifdef DISTRIBUTE
#if defined(SYMMETRY) && (SYMMINTEGRATION == 3)
	  canonize();
	  if(!search_and_insert(kanrep))
#else
	  if(!search_and_insert(CurrentMarking))
#endif
#else
	  if((NewState = SEARCHPROC()))
#endif
	    {
		  // State exists! (or, at least, I am not responsible for it (in the moment))
#ifdef COVER
		  if(NewOmegas)
		  {
			  // Replace new omegas by their old values
			  for(i=0;NewOmegas[i];i++)
			  {
				NewOmegas[i]->set_cmarking(NewOmegas[i]->lastfinite);
				NewOmegas[i]->bounded = true;
			  }
			  delete [] NewOmegas;
		  }
#endif
	      CurrentState -> firelist[CurrentState -> current] -> backfire();
#ifdef WITHFORMULA
#ifndef TWOPHASE
	update_formula(CurrentState -> firelist[CurrentState -> current]);
#endif
#endif
#ifdef CYCRED
// closing in {0,1} --> on stack ( 0 = has successor on stack and has not been extended by spp2)
// closing = 2 --> no longer on stack
	      if(NewState -> closing <= 1)
	      CurrentState -> closing = 0;
#endif
#if defined(TARJAN) && !defined(DISTRIBUTE)
	      CurrentState -> succ[CurrentState -> current] = NewState;
#ifdef FULLTARJAN
	if(!(NewState -> tarlevel))
#endif
	      CurrentState -> min = MIN(CurrentState -> min, NewState -> min);
#endif
	      (CurrentState -> current) ++;
	    }
	  else
	    {
#if defined(WITHFORMULA) && ! defined(DISTRIBUTE)
#ifndef TWOPHASE
	update_formula(CurrentState -> firelist[CurrentState -> current]);
#endif
#endif
#ifdef CYCLE
	fl = FIRELIST();
#ifdef NONBRANCHINGONLY
  if(fl && fl[0] && (fl[1] || fl[0]->cyclic))
  {
		IsCyclic = true;
  }
  else
  {
		IsCyclic = false;
  }
#else
	IsCyclic = false;
	if(fl)
	{
		for(i=0;fl[i];i++)
		{
			if(fl[i]->cyclic)
			{
				IsCyclic = true;
				silentpath = 0;
				break;
			}
		}
	}
	else
	{
		IsCyclic = false;
	}
	if(silentpath >= MAXUNSAVED)
	{
		silentpath = 0;
		IsCyclic = true;
	}
#endif
	if(IsCyclic)
	{
	      NewState = INSERTPROC();
	      NewState -> firelist = fl;
#ifdef TARJAN
	      NewState -> dfs = NewState -> min = NrOfStates++;
#else
	      NrOfStates ++;
#endif
	}
	else
	{
		NewState = new State();
		silentpath ++;
		NewState -> firelist = fl;
	}
#else
#ifdef DISTRIBUTE
	      NewState = new State();
#else
	      NewState = INSERTPROC();
#endif
#ifdef CYCRED
		NewState -> closing = 1;
#endif
	      NewState -> firelist = FIRELIST();
#ifdef TARJAN
	      NewState -> dfs = NewState -> min = NrOfStates++;
#ifdef FULLTARJAN
		NewState -> phi = F -> value;
#ifdef EVENTUALLYPROP
	if(!F -> value)
	{
#endif
		NewState -> prevtar = TarStack;
		NewState -> nexttar = TarStack -> nexttar;
		TarStack = TarStack -> nexttar = NewState;
		NewState -> nexttar -> prevtar = NewState;
#ifdef EVENTUALLYPROP
	}
#endif
#endif
#else
		NrOfStates ++;
#endif
#endif
	      NewState -> current = 0;
	      NewState -> parent = CurrentState;
#ifdef TARJAN
	      NewState -> succ =  new State * [CardFireList+1];
	      CurrentState -> succ[CurrentState -> current] = NewState;
#endif
#ifdef EVENTUALLYPROP
	// need to backtrack if phi is satisfied
	if(F -> value)
	{
		CurrentState -> firelist[CurrentState -> current] -> backfire();
		update_formula(CurrentState -> firelist[CurrentState -> current]);
		CurrentState -> current++;
		continue;
	}
#endif
#ifndef DISTRIBUTE
#ifdef STATEPREDICATE
	if(F -> value)
	{
#if defined(LIVEPROP) && !defined(TWOPHASE)
		if(largest_sat < NewState -> dfs + 1) largest_sat = NewState -> dfs + 1;
#else
		// early abortion
		cout << "\nstate found!\n";
		printstate("",CurrentMarking);
		print_path(NewState);
		printincompletestates(NewState, graphstream,1);//1=toplevel
		statistics(NrOfStates,Edges,NonEmptyHash);
		return 1;
#endif
	}
#endif
#ifdef COVER
	NewState -> smaller = smallerstate;
	NewState -> NewOmega = NewOmegas;
#endif
#ifdef DEADLOCK
	if(!(NewState -> firelist) || !(NewState -> firelist[0]))
	{
		// early abortion
		cout << "\ndead state found!\n";
		printstate("",CurrentMarking);
		print_path(NewState);
		printincompletestates(NewState, graphstream,1);//1=toplevel
		statistics(NrOfStates,Edges,NonEmptyHash);
		return 1;
	}
#endif
#ifdef DEADTRANSITION
	if(CheckTransition -> enabled)
	{
		// early abortion
		cout << "\ntransition " <<  CheckTransition -> name << " is not dead!\n";

		printstate("",CurrentMarking);
		print_path(NewState);
		printincompletestates(NewState, graphstream,1);//1=toplevel
		statistics(NrOfStates,Edges,NonEmptyHash);
		return 1;
	}
#endif
#if ( defined ( STUBBORN ) && defined ( REACHABILITY ) )
  if(!NewState -> firelist )
	{
		// early abortion
	      cout << "\nstate found!\n";
		  printstate("",CurrentMarking);
		print_path(NewState);
		printincompletestates(NewState, graphstream,1);//1=toplevel
		statistics(NrOfStates,Edges,NonEmptyHash);
		return 1;
         }
#endif
#if ( defined (REACHABILITY ) && ! defined ( STUBBORN ) )
for(i=0;i<Places[0]->cnt;i++)
{
	 if(CurrentMarking[i] != Places[i]->target_marking)
		 break;
}
if(i >= Places[0]->cnt) // target_marking found!
{
		// early abortion
	      cout << "\nstate found!\n";
		  printstate("",CurrentMarking);
		print_path(NewState);
		printincompletestates(NewState, graphstream,1);//1=toplevel
		statistics(NrOfStates,Edges,NonEmptyHash);
	 return 1;
}
#endif
#endif
	      CurrentState = NewState;
#ifdef BOUNDEDNET
		if(!isbounded)
		{
			cout << "net is unbounded!\n";
		  printstate("",CurrentMarking);
			print_reg_path(CurrentState,CurrentState->smaller,(ofstream *) 0,1);
			cout << "\n";
		printincompletestates(CurrentState,graphstream,1);
		statistics(NrOfStates,Edges,NonEmptyHash);
			return 1;
		}
#endif
#ifdef BOUNDEDPLACE
		if(!CheckPlace -> bounded)
		{
			cout << "place " << CheckPlace -> name << " is unbounded!\n";
		  printstate("",CurrentMarking);
			print_reg_path(CurrentState,CurrentState->smaller,(ofstream *) 0,1);
			cout << "\n";
		printincompletestates(CurrentState,graphstream,1);
		statistics(NrOfStates,Edges,NonEmptyHash);
			return 1;
		}
#endif
	    }
	}
      else
	{
	  // close state and return to previous state


#if defined(FAIRPROP) || defined(EVENTUALLYPROP) || defined(STABLEPROP)
		if(CurrentState ->dfs == CurrentState -> min)
		{
			// unlink scc and check it for counterexample sc sets
			if(CurrentState != TarStack -> nexttar) // current != bottom(stack)
			{
				State * newroot;
				newroot = CurrentState -> prevtar;
				newroot -> nexttar = TarStack -> nexttar;
				TarStack -> nexttar -> prevtar = newroot;
				TarStack -> nexttar = CurrentState;
				CurrentState -> prevtar = TarStack;
				TarStack = newroot;
			}
			// remove all phi-states
			State * s, *start;
#ifdef FAIRPROP
			for(s=CurrentState;s -> phi && (s -> nexttar != s);s = s -> nexttar)
			{
				s -> nexttar -> prevtar = s -> prevtar;
				s -> prevtar -> nexttar = s -> nexttar;
			}
			if(!(s -> phi)) // only nonempty sets need to be checked
			{
				start = s;
#else
				start = CurrentState;
#endif
				for(s = start -> nexttar; s != start; s = s -> nexttar)
				{
#ifdef FAIRPROP
					if(s -> phi)
					{
						s -> prevtar -> nexttar = s -> nexttar;
						s -> nexttar -> prevtar = s -> prevtar;
					}
					else
					{
#endif
						s -> tarlevel = 1;
#ifdef FAIRPROP
					}
#endif
				}
				start -> tarlevel = 1;
				// analyze this
				State * oldpar;
				unsigned int oldc;
				oldpar = CurrentState -> parent;
				oldc = CurrentState -> current;
				if(analyse_fairness(start,1))
				{
#ifdef EVENTUALLYPROP
					cout << "eventually phi does not hold" << endl;
#endif
#ifdef FAIRPROP
					cout << "GF phi does not hold" << endl;
#endif
#ifdef STABLEPROP
					cout << "FG phi does not hold" << endl;
#endif
					return 1;
				}
				CurrentState -> parent = oldpar;
				CurrentState -> current = oldc;

#ifdef FAIRPROP
			}
#endif
		}

#endif
#ifdef CYCRED
		if(CurrentState -> closing == 0)
		{
			// cycle closed. Check for ignored transitions or incomplete up-sets
#if defined(STATEPREDICATE) && defined(RELAXED) && !defined(TWOPHASE)
			// implement SPP2 of Kristensen/Valmari (2000)
			Transition ** forgotten;

			forgotten = F -> spp2(CurrentState);
			if(forgotten)
			{
				// fire list must be extended
				unsigned int nf;
				for(nf = 0; forgotten[nf]; nf++);
				Transition ** newFL = new Transition * [nf + CurrentState -> current + 1];
				State ** newSucc = new State * [nf + CurrentState -> current];
				for(i=0;i < CurrentState -> current;i++)
				{
					newFL[i] = CurrentState -> firelist[i];
					newSucc[i] = CurrentState -> succ[i];
				}
				for(i=0;i<nf;i++)
				{
					newFL[CurrentState -> current + i] = forgotten[i];
				}
				newFL[CurrentState -> current + i] = (Transition *) 0;
				// delete [] CurrentState -> firelist;
				// delete [] CurrentState -> succ;
				CurrentState -> firelist = newFL;
				CurrentState -> succ = newSucc;
				CurrentState -> closing = 1;
				continue;
			}
#endif
		}
		CurrentState -> closing = 2;
#endif
#ifndef CYCLE
#ifdef TARJAN
	if(gmflg)
	{
	  (*graphstream) << "STATE " << CurrentState ->dfs << " Prog: " << CurrentState -> progress_value;
	  if(CurrentState -> persistent) (*graphstream) << " persistent ";
	  int j=0;
	  if(graphformat == 'm')
	  {
		 for(i=0;i<Places[0]->cnt;i++)
		 {
			 if(CurrentMarking[i])
			 {
				 if(CurrentMarking[i] == VERYLARGE)
				 {

				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << "oo" ;
				 }
				 else
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << CurrentMarking[i];
				 }
				 }
		 }

	  }
	  (*graphstream) << "\n\n";
	  for(i=0; CurrentState ->firelist[i];i++)
	  {
		(*graphstream) << CurrentState -> firelist[i]->name << " -> " <<
		CurrentState -> succ[i]->dfs << "\n";
      }
	  (*graphstream) << endl;
	}
	if(GMflg)
	{
	  cout << "STATE " << CurrentState ->dfs;
	  int j=0;
	  if(graphformat == 'm')
	  {
		 for(i=0;i<Places[0]->cnt;i++)
		 {
			 if(CurrentMarking[i])
			 {
				 if(CurrentMarking[i] == VERYLARGE)
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << "oo" ;
				 }
				 else
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << CurrentMarking[i];
				 }
			 }
		 }

	  }
	  cout << "\n\n";
	  for(i=0; CurrentState ->firelist[i];i++)
	  {
		cout << CurrentState -> firelist[i]->name << " -> " <<
		CurrentState -> succ[i]->dfs << "\n";
      }
	  cout << "\n";
	}
#endif
#endif
#if defined(EXTENDED) && ! defined(MODELCHECKING) && !defined(CYCRED)
		// if last state in TSCC, retrieve ignored transitions
		if((CurrentState -> dfs == CurrentState -> min)
			&& (CurrentState -> dfs >= MinBookmark))
		{
			// TSCC closed. Check for ignored transitions or incomplete up-sets
#if defined(STATEPREDICATE) && defined(RELAXED) && ! defined(CYCRED) && !defined(TWOPHASE)
			// implement SPP2 of Kristensen/Valmari (2000)
			Transition ** forgotten;

			forgotten = F -> spp2(CurrentState);
#if defined(LIVEPROP)
			if(!forgotten)
			{
				// check for forgotten down transitions (SPP3 of
				// Kristensen/Valmari (2000)
				for(i=0;i<Transitions[0]->cnt;i++)
				{
					if(Transitions[i]->down)
					{
						if(Transitions[i]->lastfired <= CurrentState ->dfs)
						{
							// no occurrence of down transition
							// inside tscc -> need to extend
							stubborninsert(Transitions[i]);
							stubbornclosure();
							// check for unfired transitions in
							// stubborn set

							unsigned int m,n;
							Transition * st;

							for(st = Transitions[0]-> StartOfStubbornList;st;st=st->NextStubborn)
							{
								if(st->enabled)
								{
									for(m=0;CurrentState->firelist[m];m++)
									{
										if(st == CurrentState->firelist[m])
										{
											break;
										}
									}
									if(!CurrentState->firelist[m])
									{
											// found unfired transition in stubborn set
											// around down transition -> can stop
											// searching and extend firing list by
											// unfired transitions in this stubborn set

											forgotten = new Transition * [Transitions[0]->NrStubborn + 1];
											for(n=0; st; st = st -> NextStubborn)
											{
												st -> instubborn = false;
												if(st -> enabled)
												{
													Transitions[0]->NrStubborn--;
													for(m=0;CurrentState -> firelist[m];m++)
													{
														if(CurrentState->firelist[m] == st) break;
													}
													if(CurrentState -> firelist[m])
													{
															forgotten[n++] = st;
													}
												}
											}
											Transitions[0]->StartOfStubbornList = (Transition *) 0;
											goto afterdownsearch;

									}
								}
								Transitions[0]->StartOfStubbornList = st -> NextStubborn;
								st -> instubborn = false;
								if(Transitions[i]->enabled) Transitions[0]-> NrStubborn--;
							}
						}
					}
				}
				forgotten = (Transition **) 0;
			}
afterdownsearch:

#endif
			if(forgotten)
			{
				// fire list must be extended
				unsigned int nf;
				for(nf = 0; forgotten[nf]; nf++);
				Transition ** newFL = new Transition * [nf + CurrentState -> current+1];
				State ** newSucc = new State * [nf + CurrentState -> current];
				for(i=0;i < CurrentState -> current;i++)
				{
					newFL[i] = CurrentState -> firelist[i];
					newSucc[i] = CurrentState -> succ[i];
				}
				for(i=0;i<nf;i++)
				{
					newFL[CurrentState -> current + i] = forgotten[i];
				}
				newFL[CurrentState -> current + i] = (Transition *) 0;
				delete [] CurrentState -> firelist;
				delete [] CurrentState -> succ;
				CurrentState -> firelist = newFL;
				CurrentState -> succ = newSucc;
				continue;
			}
			else
			{
				//TSCC really closed
				MinBookmark = NrOfStates;
#if defined(LIVEPROP) && ! defined(TWOPHASE)
				// check if tscc reached property
				if(largest_sat <=CurrentState -> dfs)
				{
					// tscc did not reach prop -> prop not live
					cout << "\npredicate not live: not satisfiable beyond reported state\n\n";
					printstate("",CurrentMarking);
	statistics(NrOfStates,Edges,NonEmptyHash);
					return 1;
				}
#endif
			}

#else
			Transition * ignored;
			unsigned int CardIgnored;

			CardIgnored = 0;
			Transitions[0]-> StartOfIgnoredList = (Transition *) 0;
			for(ignored = Transitions[0]->StartOfEnabledList;ignored;
				ignored = ignored -> NextEnabled)
			{
				if((ignored -> lastdisabled <= CurrentState -> dfs)
					&& (ignored -> lastfired <= CurrentState -> dfs))
				{
					// transition IS ignored
					CardIgnored ++;
					ignored -> NextIgnored = Transitions[0]->StartOfIgnoredList;
					Transitions[0]->StartOfIgnoredList = ignored;
				}
			}
			if(Transitions[0]->StartOfIgnoredList)
			{
				// there are ignored transitions
				Transition * tt;
				Transition ** newFL = new Transition *[CurrentState -> current + CardIgnored + 1];
				State ** newSucc = new State *[CurrentState->current + CardIgnored];
				int u;
				for(u=0;u<CurrentState->current;u++)
				{
					newFL[u]= CurrentState->firelist[u];
					newSucc[u] = CurrentState -> succ[u];
				}
				for(tt = Transitions[0]->StartOfIgnoredList;
				    tt;
				    tt = tt -> NextIgnored)
				{
					newFL[u++] = tt;
				}
				newFL[u] = (Transition*)0;
				delete [] CurrentState -> firelist;
				delete [] CurrentState -> succ;
				CurrentState->firelist = newFL;
				CurrentState -> succ = newSucc;
				continue;
			}
#ifdef TSCC
			else
			{
				//TSCC really closed: deposit state
				StatevectorList * s;


				MinBookmark = NrOfStates;
				s = new StatevectorList;
				s -> sv = new Statevector(Places[0]->cnt);
				s -> next = TSCCRepresentitives;
				TSCCRepresentitives = s;
				for(i=0;i < Places[0]->cnt;i++)
				{
					s -> sv ->set(i,CurrentMarking[i]);
				}
			}
#endif
#endif
		}
#endif
#ifdef COVER
		  if(CurrentState -> NewOmega)
		  {
			  // Replace new omegas by their old values
			  for(i=0;CurrentState ->NewOmega[i];i++)
			  {
				CurrentState ->NewOmega[i]->set_cmarking(CurrentState ->NewOmega[i]->lastfinite);
				CurrentState ->NewOmega[i]->bounded = true;
			  }
			  delete [] CurrentState ->NewOmega;
		  }
#endif
#ifdef CYCLE
		silentpath = 0;
#else
#ifdef TARJAN
	      if(CurrentState -> parent) CurrentState -> parent -> min = MIN(CurrentState -> min, CurrentState-> parent -> min);
#endif
#endif

#ifdef CYCRED
		CurrentState -> closing = 2;
#endif
	  CurrentState = CurrentState -> parent;
//**	  delete TmpState;
	  if(CurrentState)
	    {
	      CurrentState -> firelist[CurrentState -> current] -> backfire();
#ifdef WITHFORMULA
#ifndef TWOPHASE
		  update_formula(CurrentState -> firelist[CurrentState -> current]);
#endif
#endif
	      CurrentState -> current ++;
	    }
	}
	}
#ifdef DISTRIBUTE
	} //finally, we close that while loop around the dfs search
#else
#ifdef BITHASH
	cout << "\nno conclusive result!\n");
#else
#ifdef REACHABILITY
	cout << "\nstate is not reachable!\n";
#endif
#ifdef DEADLOCK
	cout << "\nnet does not have deadlocks!\n";
#endif
#if defined(STATEPREDICATE) && ! defined(LIVEPROP)
	cout << "\n predicate is not satisfiable!\n";
#endif
#ifdef DEADTRANSITION
	cout << "\ntransition " << CheckTransition -> name << " is dead!\n";
#endif
#ifdef BOUNDEDPLACE
	cout << "\nplace " << CheckPlace -> name << " is bounded!\n";
#endif
#if defined(LIVEPROP) && ! defined(TWOPHASE)
	cout << "\npredicate is live!\n";
#endif
#ifdef BOUNDEDNET
	if(isbounded)
	{
		cout << "\nnet is bounded!\n";
	}
	else
	{
		cout << "\nnet is unbounded!\n";
	}
#endif
#endif
#endif
#ifdef STABLEPROP
	cout << "\n FG phi holds" << endl;
#endif
#ifdef FAIRPROP
	cout << "\n GF phi holds" << endl;
#endif
#ifdef EVENTUALLYPROP
	cout << "\n eventually phi holds\n";
#endif
	statistics(NrOfStates,Edges,NonEmptyHash);
	return 0;
#endif
}

unsigned int DistributeNow = 0;

unsigned int breadth_first()
{
	// min true = Dieser Knoten hat noch fruchtbare Zweige
	// succ[i] 0 = dieser Zweig ist nicht mehr fruchtbar
#ifdef BREADTH_FIRST

  ofstream * graphstream;
  unsigned int limit;
  unsigned int d;
  unsigned int i;

  Edges = 0;
  if(gmflg)
  {
	  graphstream = new ofstream(graphfile);
		if(!graphstream)
		{
			cerr << "Cannot open graph output file: " << graphfile
			<< "\nno output written\n";
			gmflg = false;
		}
  }


  State * CurrentState, * NewState, * initial;
  // init initial marking and hash table
  isbounded = 1;
#ifdef SYMMETRY
  Trace = new SearchTrace [CardStore];
#endif
#ifndef DISTRIBUTE
  for(i = 0; i < HASHSIZE;i++)
    {
#ifdef BITHASH
	BitHashTable[i] = 0;
#else
      binHashTable[i] = (binDecision *) 0;
#endif
    }
#endif
  NrOfStates = d = limit = 1;
#ifdef DISTRIBUTE
#if defined(SYMMETRY) && SYMMINTEGRATION == 3
  canonize();
  search_and_insert(kanrep);
#else
  search_and_insert(CurrentMarking);
#endif
  initial = CurrentState = new State();
#else
  CurrentState = SEARCHPROC(); // inits search data structure
  initial = CurrentState = INSERTPROC();
#endif
  CurrentState -> dfs = 0;
  CurrentState -> current = 0;
  CurrentState -> min = true;
for(i=0;i<Transitions[0]->cnt;i++) Transitions[i]->check_enabled();
  CurrentState -> firelist = FIRELIST();
#ifdef COVER
  CurrentState -> NewOmega = (Place **) 0;
  Ancestor = new unsigned int [Places[0]->cnt + 1];
#endif
	int j;
	if(gmflg)
	{
	  (*graphstream) << "STATE " << CurrentState ->dfs << "; DEPTH 0";
	  j=0;
	  if(graphformat == 'm')
	  {
		 for(i=0;i<Places[0]->cnt;i++)
		 {
			 if(CurrentMarking[i])
			 {
				 if(CurrentMarking[i] == VERYLARGE)
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << "oo" ;
				 }
				 else
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << CurrentMarking[i];
				 }
			 }
		 }

	  }
	  (*graphstream) << "\n\n";
	  for(i=0;CurrentState -> firelist[i];i++)
	  {
		(*graphstream) << CurrentState -> firelist[i]->name << "\n";
	  }
	  (*graphstream) << "\n";
	}
	if(GMflg)
	{
	  cout << "STATE " << CurrentState ->dfs << "; DEPTH 0";
	  j=0;
	  if(graphformat == 'm')
	  {
		 for(i=0;i<Places[0]->cnt;i++)
		 {
			 if(CurrentMarking[i])
			 {
				 if(CurrentMarking[i] == VERYLARGE)
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << "oo" ;
				 }
				 else
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << CurrentMarking[i];
				 }
			 }
		 }

	  }
	  cout << "\n\n";
	  for(i=0;CurrentState -> firelist[i];i++)
	  {
		cout << CurrentState -> firelist[i]->name << "\n";
	  }
	  cout << "\n";
	}
#ifdef STUBBORN
  if(!CurrentState -> firelist )
	{
		// early abortion
#ifdef REACHABILITY
	      cout << "\nstate found!\n";
#endif
#ifdef DEADTRANSITION
	      cout << "\ntransition " << CheckTransition -> name << " is not dead!\n";
#endif
		  printstate("",CurrentMarking);
		  print_path(CurrentState);
	statistics(NrOfStates,Edges,NonEmptyHash);
	return 1;
         }
#endif
#if defined(DEADTRANSITION) && !defined(STUBBORN)
	if(CheckTransition->enabled)
	{
	      cout << "\ntransition " << CheckTransition -> name << " is not dead!\n";
		  printstate("",CurrentMarking);
		  print_path(CurrentState);
	statistics(NrOfStates,Edges,NonEmptyHash);
	return 1;
	}
#endif
#ifdef BOUNDEDPLACE
	if(!CheckPlace)
	{
		cerr << "\nspecify place to be checked in analysis task file\n";
		_exit(4);
	}
#endif
#ifdef DEADLOCK
	if(!CurrentState -> firelist || ! (CurrentState -> firelist[0]))
	{
		 // early abortion
		 cout << "\ndead state found!\n";
		 printstate("",CurrentMarking);
		 print_path(CurrentState);
		 statistics(NrOfStates,Edges,NonEmptyHash);
		 return 1;
	}
#endif
#ifdef STATEPREDICATE
	int res;
	if(!F)
	{
		cerr << "\nspecify predicate in analysis task file!\n";
		_exit(4);
	}
	F = F -> reduce(&res);
	if(res<2) return res;
	F = F -> posate();
	F -> tempcard = 0;
	F -> setstatic();
	if(F ->  tempcard)
	{
		cerr << "temporal operators are not allowed in state predicates\n";
		exit(3);
	}
	cout << "\n Formula with\n" << F -> card << " subformula.\n";
	F -> parent = (formula *) 0;
	if(F -> initatomic())
	{
		cout << "\nstate found!\n";
		printstate("",CurrentMarking);
		print_path(CurrentState);
		printincompletestates(CurrentState,graphstream,1);
		statistics(NrOfStates,Edges,NonEmptyHash);
		return 1;
	}
#endif

#if ( defined (REACHABILITY ) && ! defined ( STUBBORN ) )
for(i=0;i<Places[0]->cnt;i++)
{
	 if(CurrentMarking[i] != Places[i]->target_marking)
		 break;
}
if(i >= Places[0]->cnt) // target_marking found!
{
		// early abortion
	      cout << "\nstate found!\n";
		  printstate("",CurrentMarking);
		print_path(CurrentState);
	statistics(NrOfStates,Edges,NonEmptyHash);
	 return 1;
}
#endif
  CurrentState -> parent = (State *) 0;
  CurrentState -> succ = new State * [CardFireList];

  // process marking until returning from initial state

  while(CurrentState)
    {
      if(CurrentState -> firelist[CurrentState -> current])
	{
          if(d == limit)
	  {
		// working layer
	        Edges ++;
	      if(!(Edges % REPORTFREQUENCY))
	{
#ifdef DISTRIBUTE
		rapport(rapportstring);
#else
              cerr << "st: " << NrOfStates << "     edg: " << Edges << "\n";
#endif
	}
	        CurrentState -> firelist[CurrentState -> current] -> fire();
#ifdef COVER
	//In coverability graphs, we need to check for new w

	// 1. Search backwards until last w-Intro for smaller state
	unsigned int NrCovered;
	State * smallerstate;
	Place ** NewOmegas;

	for(i=0;i<Places[0]->cnt;i++)
	{
		Ancestor[i] = CurrentMarking[i];
	}
	NewOmegas = (Place **)0;
	// for all ancestor states do ...
	for(smallerstate = CurrentState; smallerstate; smallerstate =
	    smallerstate -> parent)
	{

		CurrentState -> firelist[CurrentState -> current] -> traceback();
		NrCovered = 0;
		for(i=0;i<Places[0]->cnt;i++)
		{
				// case 1: smaller state[i] > current state [i]
				// ---> continue with previous state
				if(Ancestor[i] > CurrentMarking[i])
				{
					goto nextstate;
				}
				// case 2: smaller state < current state
				// count w-Intro
				if(Ancestor[i] < CurrentMarking[i])
				{
					NrCovered++;
				}
				// case 3: smaller state = current state --> do nothing
		}
		// if arrived here, it holds smaller <= current
		// covering is proper iff NrCovered > 0
		// If covering is not proper, (smaller state = current state)
		// current marking is not new, ancestors of smaller marking cannot
		// be smaller than current marking, since they would be smaller than
		// this smaller marking --> leave w-Intro procedure
		if(!NrCovered)
		{
			smallerstate = (State *) 0;
			goto endomegaproc;
		}
		// Here, smallerstate IS less than current state.
		isbounded = 0;
		NewOmegas = new Place * [NrCovered+1];
		// for all fragements of state vector do ...
		NrCovered = 0;
		for(i=0;i<Places[0]->cnt;i++)
		{
				if(Ancestor[i] < CurrentMarking[i])
				{
					// Here we have a place that deserves a new Omega
					// 1. set old value in place record
					Places[i] -> lastfinite =
					 CurrentMarking[i];
					Places[i] -> set_cmarking(VERYLARGE);
					Places[i] -> bounded = false;
					NewOmegas[NrCovered++] = Places[i];
				}
		}
		NewOmegas[NrCovered] = (Place*) 0;
		goto endomegaproc;


	nextstate:
		if(smallerstate -> smaller) // smallerstate is a omega-introducing state
		{
			break;
		}
	}
endomegaproc:
if(!NewOmegas) smallerstate = (State *) 0;
#endif
#ifdef WITHFORMULA
#ifndef TWOPHASE
			update_formula(CurrentState -> firelist[CurrentState -> current]);
#endif
#endif
#ifdef DISTRIBUTE
#if defined(SYMMETRY) && SYMMINTEGRATION == 3
		canonize();
		if(!search_and_insert(kanrep,DistributeNow))
#else
		if(!search_and_insert(CurrentMarking,DistributeNow))
#endif
#else
	        if(NewState = SEARCHPROC())
#endif
	        {
#ifdef COVER
		  if(NewOmegas)
		  {
			  // Replace new omegas by their old values
			  for(i=0;NewOmegas[i];i++)
			  {
				NewOmegas[i]->set_cmarking(NewOmegas[i]->lastfinite);
				NewOmegas[i]->bounded = true;
			  }
			  delete [] NewOmegas;
		  }
#endif
	            CurrentState -> firelist[CurrentState -> current] -> backfire();
#ifdef WITHFORMULA
#ifndef TWOPHASE
			update_formula(CurrentState -> firelist[CurrentState -> current]);
#endif
#endif
	            CurrentState -> succ[(CurrentState -> current)++] = (State *) 0;
	        }
	        else
	        {
#ifdef DISTRIBUTE
		    NewState = new State();
#else
	            NewState = INSERTPROC();
#endif
		    CurrentState -> min = true;
                NewState -> dfs =  NrOfStates ++;
#ifdef DISTRIBUTE
		    if(NrOfStates >= 500) DistributeNow = 1;
#endif
	            NewState -> current = 0;
	            NewState -> firelist = FIRELIST();
	            NewState -> parent = CurrentState;
	            NewState -> succ =  new State * [CardFireList];
	if(gmflg)
	{
	  (*graphstream) << "STATE " << NewState ->dfs << " FROM " <<
	  CurrentState -> dfs << " BY " <<
	  CurrentState -> firelist[CurrentState -> current] -> name
	  << "; DEPTH " << limit;
	  j=0;
	  if(graphformat == 'm')
	  {
		 for(i=0;i<Places[0]->cnt;i++)
		 {
			 if(CurrentMarking[i])
			 {
				 if(CurrentMarking[i] == VERYLARGE)
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << "oo" ;
				 }
				 else
				 {
				 (*graphstream) << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << CurrentMarking[i] ;
				 }
			  }
		 }

	  }
	  (*graphstream) << "\n\n";
	  if(NewState -> firelist)
	  {
	  for(i=0;NewState -> firelist[i];i++)
	  {
		(*graphstream) << NewState -> firelist[i]->name << "\n";
	  }
	  }
	  (*graphstream) << "\n";
	}
	if(GMflg)
	{
	  cout << "STATE " << NewState ->dfs << " FROM " <<
	  CurrentState -> dfs << " BY " <<
	  CurrentState -> firelist[CurrentState -> current] -> name
	  << "; DEPTH " << limit;
	  j=0;
	  if(graphformat == 'm')
	  {
		 for(i=0;i<Places[0]->cnt;i++)
		 {
			 if(CurrentMarking[i])
			 {
				 if(CurrentMarking[i] == VERYLARGE)
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << "oo" ;
				 }
				 else
				 {
				 cout << (j++ ? ",\n" : "\n") << Places[i]->name << " : " << CurrentMarking[i] ;
				 }
			 }
		 }

	  }
	  cout << "\n\n";
	  if(NewState -> firelist)
	  {
	  for(i=0;NewState -> firelist[i];i++)
	  {
		cout << NewState -> firelist[i]->name << "\n";
	  }
	  }
	  cout << "\n";
	}
	            CurrentState -> succ[CurrentState -> current] = NewState;
#ifdef STUBBORN
                    if(!NewState -> firelist )
	            {
		        // early abortion
#ifdef REACHABILITY
	                cout << "\nstate found!\n";
#endif
#ifdef STATEPREDICATE
	                cout << "state found!\n";
#endif
					printstate("",CurrentMarking);
		        print_path(NewState);
	statistics(NrOfStates,Edges,NonEmptyHash);
		        return 1;
                    }
#endif
#ifdef BOUNDEDPLACE
		if(!CheckPlace -> bounded)
		{
			cout << "place " << CheckPlace -> name << " is unbounded!\n";
					printstate("",CurrentMarking);
			NewState -> smaller = smallerstate;
			print_reg_path(NewState,smallerstate,(ofstream *) 0,1);
			cout << "\n";
		statistics(NrOfStates,Edges,NonEmptyHash);
			return 1;
		}
#endif
#ifdef BOUNDEDNET
		if(!isbounded)
		{
			cout << "net is unbounded!\n";
					printstate("",CurrentMarking);
			NewState -> smaller = smallerstate;
			print_reg_path(NewState,smallerstate,(ofstream *) 0,1);
			cout << "\n";
		statistics(NrOfStates,Edges,NonEmptyHash);
			return 1;
		}
#endif
#ifdef DEADLOCK
                if(!NewState -> firelist || !(NewState -> firelist[0]))
	            {
		        // early abortion
	                cout << "\ndead state found!\n";
					printstate("",CurrentMarking);
		        print_path(NewState);
	statistics(NrOfStates,Edges,NonEmptyHash);
		        return 1;
                    }
#endif
#if defined(DEADTRANSITION)
	if(CheckTransition->enabled)
	{
	      cout << "\ntransition " << CheckTransition -> name << " is not dead!\n";
		  printstate("",CurrentMarking);
		  print_path(CurrentState);
	statistics(NrOfStates,Edges,NonEmptyHash);
	return 1;
	}
#endif
#ifdef STATEPREDICATE
		if(! NewState -> firelist || F->value)
		{
			// early abortion
	                cout << "\nstate found!\n";
					printstate("",CurrentMarking);
		        print_path(NewState);
	statistics(NrOfStates,Edges,NonEmptyHash);
		        return 1;
                    }
#endif
#if ( defined (REACHABILITY ) && ! defined ( STUBBORN ) )
for(i=0;i<Places[0]->cnt;i++)
{
	 if(CurrentMarking[i]!= Places[i]->target_marking)
		 break;
}
if(i >= Places[0]->cnt) // target_marking found!
{
		// early abortion
	      cout << "\nstate found!\n";
		  printstate("",CurrentMarking);
		print_path(NewState);
	statistics(NrOfStates,Edges,NonEmptyHash);
	 return 1;
}
#endif
#ifdef COVER
	NewState -> smaller = smallerstate;
	NewState -> NewOmega = NewOmegas;
		  if(NewOmegas)
		  {
			  // Replace new omegas by their old values
			  for(i=0;NewOmegas[i];i++)
			  {
				NewOmegas[i]->set_cmarking(NewOmegas[i]->lastfinite);
				NewOmegas[i]->bounded = true;
			  }
		  }
#endif
	            CurrentState->firelist[CurrentState -> current]->backfire();

#ifdef WITHFORMULA
#ifndef TWOPHASE
			update_formula(CurrentState -> firelist[CurrentState -> current]);
#endif
#endif
		    CurrentState->current++;
	    }
	}
	else
        {
		// proceed forward to working layer
		if(CurrentState -> succ[CurrentState -> current])
		{
			CurrentState -> firelist[CurrentState -> current]->fire();
#ifdef WITHFORMULA
#ifndef TWOPHASE
			update_formula(CurrentState -> firelist[CurrentState -> current]);
#endif
#endif
			CurrentState = CurrentState->succ[CurrentState->current];
#ifdef COVER
			// set omegas according to stored list
			if(CurrentState -> NewOmega)
			{
				for(i=0;CurrentState -> NewOmega[i];i++)
				{
					CurrentState -> NewOmega[i] -> lastfinite =
					CurrentMarking[CurrentState -> NewOmega[i] -> index];
					CurrentState -> NewOmega[i] -> bounded = false;
					CurrentState -> NewOmega[i] -> set_cmarking(VERYLARGE);
				}
			}
#endif
			CurrentState -> min = false;
			CurrentState -> current = 0;
			d++;
		}
		else
		{
			(CurrentState -> current)++;
		}
	}
      }
      else
	{

	  // close state and return to previous state
	  if(CurrentState -> min)
	  {
		  CurrentState = CurrentState -> parent;
                  if(CurrentState) CurrentState -> min = true;
	  }
	  else
	  {
		  CurrentState = CurrentState -> parent;
	          if(CurrentState) CurrentState -> succ[CurrentState -> current] = (State *) 0;
	  }

	  d--;
	  if(CurrentState)
	    {
#ifdef COVER
		  if(CurrentState -> NewOmega)
		  {
			  // Replace new omegas by their old values
			  for(i=0;CurrentState -> NewOmega[i];i++)
			  {
				CurrentState -> NewOmega[i]->set_cmarking(CurrentState -> NewOmega[i]->lastfinite);
				CurrentState -> NewOmega[i]->bounded = true;
			  }
		  }
#endif
	      CurrentState -> firelist[CurrentState -> current] -> backfire();
#ifdef WITHFORMULA
#ifndef TWOPHASE
			update_formula(CurrentState -> firelist[CurrentState -> current]);
#endif
#endif
	      CurrentState -> current ++;
	    }
	  else
	    {
		if(initial->min)
		{
		limit++;
		d = 1;
		CurrentState = initial;
		CurrentState -> current = 0;
		initial-> min = false;
		}
            }
	}

	}

#ifdef DISTRIBUTE
	master_done();
	if(TargetState)
	{
		printstate("",TargetState);
		return 1;
	}
#endif
#ifdef REACHABILITY
	cout << "\n state is not reachable!\n";
#endif
#ifdef DEADLOCK
	cout << "\nnet does not have deadlocks!\n";
#endif
#if defined(STATEPREDICATE) && ! defined(LIVEPROP)
	cout << "\n predicate is not satisfiable!\n";
#endif
#ifdef DEADTRANSITION
	cout << "\ntransition " << CheckTransition -> name << " is dead!\n";
#endif
#ifdef BOUNDEDPLACE
	cout << "\nplace " << CheckPlace -> name << " is bounded!\n";
#endif
#ifdef BOUNDEDNET
	if(isbounded)
	{
		cout << "\nnet is bounded!\n";
	}
	else
	{
		cout << "\nnet is unbounded!\n";
	}
#endif
#endif
	statistics(NrOfStates,Edges,NonEmptyHash);
	return 0;


}

void RemoveGraph()
{
	int i;

#ifndef BITHASH
	for(i=0;i<HASHSIZE;i++)
	{
		if(binHashTable[i]) delete binHashTable[i];
		binHashTable[i] = (binDecision *) 0;
	}
#endif
}

#ifdef STUBBORN
bool mutual_reach()
{
#ifdef TARJAN
  unsigned int i;
  State * NewState;
  // init initial marking and hash table
#ifdef SYMMETRY
  Trace = new SearchTrace [CardStore];
#endif
  RemoveGraph();
  NrOfStates = 1;
  NonEmptyHash = 0;
  if(binSearch())
  {
	cerr << " Wat soll dat denn?";
  }
  CurrentState = binInsert();
  CurrentState -> current = 0;
  Edges = 0;
  CurrentState -> firelist = stubbornfirelistreach();

  if(!CurrentState -> firelist )
	{
		// early abortion
  statistics(NrOfStates,Edges,NonEmptyHash);
		return true;
    }
  CurrentState -> parent = (State *) 0;
  CurrentState -> succ = new State * [CardFireList];

  // process marking until returning from initial state

  while(CurrentState)
    {
      if(CurrentState -> firelist[CurrentState -> current])
	{
	  // there is a next state that needs to be explored
	  Edges ++;
	      if(!(Edges % REPORTFREQUENCY))
              cerr << "st: " << NrOfStates << "     edg: " << Edges << "\n";
	  CurrentState -> firelist[CurrentState -> current] -> fire();
	  if(NewState = SEARCHPROC())
	    {
		  // State exists!
	      CurrentState -> firelist[CurrentState -> current] -> backfire();
	      CurrentState -> succ[CurrentState -> current] = NewState;
	      (CurrentState -> current) ++;
	    }
	  else
	    {
	      NewState = INSERTPROC();
	      NewState -> current = 0;
	      NewState -> firelist = stubbornfirelistreach();
NrOfStates ++ ;
	      NewState -> parent = CurrentState;
	      NewState -> succ =  new State * [CardFireList];
	      CurrentState -> succ[CurrentState -> current] = NewState;
  if(!NewState -> firelist )
	{
		// early abortion
  statistics(NrOfStates,Edges,NonEmptyHash);
		return true;
    }
	      CurrentState = NewState;
	    }
	}
      else
	{
	  // close state and return to previous state
	  CurrentState = CurrentState -> parent;
	  if(CurrentState)
	    {
	      CurrentState -> firelist[CurrentState -> current] -> backfire();
	      CurrentState -> current ++;
	    }
	}
    }
#endif
  statistics(NrOfStates,Edges,NonEmptyHash);
  return false;

}

#endif


#ifdef STUBBORN
#ifdef WITHFORMULA
bool target_reach()
{
  unsigned int i;
  unsigned int NrOfStates;
  State * NewState;
  // init initial marking and hash table
#ifdef SYMMETRY
  Trace = new SearchTrace [CardStore];
#endif
  RemoveGraph();
  NrOfStates = 1;
  NonEmptyHash = 0;
  if(binSearch())
  {
	cerr << " Wat soll dat denn?";
  }
  CurrentState = binInsert();
  CurrentState -> current = 0;
#if defined(RELAXED) && ! defined(STRUCT)
  CurrentState -> dfs = CurrentState -> min = 0;
  MinBookmark = 1;
#endif
  Edges = 0;
  if(F -> initatomic())
  {
	return true;
  }
#ifdef RELAXED
  CurrentState -> firelist = relaxedstubbornset();
#else
  CurrentState -> firelist = stubbornfirelistpredicate();
#endif

  if(!CurrentState -> firelist )
	{
		// early abortion
  statistics(NrOfStates,Edges,NonEmptyHash);
		return true;
    }
  CurrentState -> parent = (State *) 0;
#ifdef TARJAN
  CurrentState -> succ = new State * [CardFireList];
#endif

  // process marking until returning from initial state

  while(CurrentState)
    {
      if(CurrentState -> firelist[CurrentState -> current])
	{
	  // there is a next state that needs to be explored
	  Edges ++;
	      if(!(Edges % REPORTFREQUENCY))
              cerr << "st: " << NrOfStates << "     edg: " << Edges << "\n";
	  CurrentState -> firelist[CurrentState -> current] -> fire();
	  if(NewState = SEARCHPROC())
	    {
		  // State exists!
	      CurrentState -> firelist[CurrentState -> current] -> backfire();
#ifdef TARJAN
	      CurrentState -> succ[CurrentState -> current] = NewState;
	      CurrentState -> min = MIN(CurrentState->min,NewState->min);
#endif
	      (CurrentState -> current) ++;
	    }
	  else
	    {
	      NewState = INSERTPROC();
	update_formula(CurrentState -> firelist[CurrentState -> current]);
	      NewState -> current = 0;
#ifdef RELAXED
		NewState -> firelist = relaxedstubbornset();
#else
	      NewState -> firelist = stubbornfirelistpredicate();
#endif
	      NewState -> parent = CurrentState;
#ifdef TARJAN
	      NewState -> succ =  new State * [CardFireList];
	      CurrentState -> succ[CurrentState -> current] = NewState;
	      NewState -> dfs = NewState -> min = NrOfStates;
#endif
		NrOfStates ++;
  if(!NewState -> firelist )
	{
		// early abortion
  statistics(NrOfStates,Edges,NonEmptyHash);
		return true;
    }
	      CurrentState = NewState;
	    }
	}
      else
	{
	  // close state and return to previous state
#if defined(RELAXED) && !defined(STRUCT)
			Transition ** forgotten;

			forgotten = F -> spp2(CurrentState);
			if(forgotten && forgotten[0])
			{
				// fire list must be extended
				unsigned int nf;
				for(nf = 0; forgotten[nf]; nf++);
				Transition ** newFL = new Transition * [nf + CurrentState -> current+1];
				State ** newSucc = new State * [nf + CurrentState -> current];
				for(i=0;i < CurrentState -> current;i++)
				{
					newFL[i] = CurrentState -> firelist[i];
					newSucc[i] = CurrentState -> succ[i];
				}
				for(i=0;i<nf;i++)
				{
					newFL[CurrentState -> current + i] = forgotten[i];
				}
				newFL[CurrentState -> current + i] = (Transition *) 0;
				delete [] CurrentState -> firelist;
				delete [] CurrentState -> succ;
				CurrentState -> firelist = newFL;
				CurrentState -> succ = newSucc;
				continue;
			}
			else
			{
				//TSCC really closed
				MinBookmark = NrOfStates;
				CurrentState = CurrentState -> parent;
			}
#else
	  CurrentState = CurrentState -> parent;
#endif
	  if(CurrentState)
	    {
	      CurrentState -> firelist[CurrentState -> current] -> backfire();
	update_formula(CurrentState -> firelist[CurrentState -> current]);
	      CurrentState -> current ++;
	    }
	}
    }
  statistics(NrOfStates,Edges,NonEmptyHash);
  return false;
}

#endif
#endif
#ifdef REVERSIBILITY
int reversibility()
{
	unsigned int i;
	for(i=0;i<Places[0]->cnt;i++)
	{
		Places[i]->initial_marking = CurrentMarking[i];
	}
	// Compute representitives of all TSCC
	depth_first();
	//Check whether initial marking is reachable
	for(;TSCCRepresentitives;TSCCRepresentitives = TSCCRepresentitives->next)
	{
		// 1. initialize start and target markings
		for(i=0;i < Places[0]->cnt;i++)
		{
			Places[i]->target_marking = Places[i]->initial_marking;
			Places[i]->set_cmarking((*TSCCRepresentitives->sv)[i]);
		}
		for(i=0;i<Transitions[0]->cnt;i++)
		{
			Transitions[i]->check_enabled();
		}
		if(!mutual_reach())
		{
			cout << "\nnot reversible: no return to m0 from reported state\n\n";
			printstate("",CurrentMarking);
			return 1;
		}
	}
	cout << "\n net is reversible!\n";
	return(0);
}
#endif

#if defined(LIVEPROP) && defined(TWOPHASE)
int liveproperty()
{
	unsigned int i;

	int res;
	if(!F)
	{
		cerr << "\nspecify predicate in analysis task file!\n";
		_exit(4);
	}
	F = F -> reduce(&res);
	if(res<2) return res;
	F = F -> posate();
	F -> tempcard = 0;
	F -> setstatic();
	if(F ->  tempcard)
	{
		cerr << "temporal operators are not allowed in state predicates\n";
		exit(3);
	}
	cout << "\n Formula with\n" << F -> card << " subformula.\n";
	F -> parent = (formula *) 0;

	for(i=0;i<Places[0]->cnt;i++)
	{
		Places[i]->initial_marking = CurrentMarking[i];
	}
	// Compute representitives of all TSCC
	depth_first();
	//Check whether target state is reachable
	for(;TSCCRepresentitives;TSCCRepresentitives = TSCCRepresentitives->next)
	{
		// 1. initialize start and target markings
		for(i=0;i < Places[0]->cnt;i++)
		{
			Places[i]->target_marking = Places[i]->initial_marking;
			Places[i]->set_cmarking((*(TSCCRepresentitives->sv))[i]);
		}
		for(i=0;i<Transitions[0]->cnt;i++)
		{
			Transitions[i]->check_enabled();
		}
		if(!target_reach())
		{
			cout << "\npredicate not live: not satisfiable beyond reported state\n\n";
			printstate("",CurrentMarking);
			return 1;
		}
	}
	cout << "\n predicate is live!\n";
	return(0);
}
#endif

#if defined(HOME) && defined(TWOPHASE)

int home()
{
unsigned int i;
	StatevectorList * New, * Old, * Candidate;


	// Compute representitives of all TSCC
	depth_first();
	Candidate = TSCCRepresentitives;
	if(!Candidate)
	{
		cout << "serious internal error, maybe memory overflow?\n";
		_exit( 2);
	}
	New = Candidate -> next;
	Old = (StatevectorList *) 0;

	// First loop creates candidate for home property
	while(New)
	{
		// 1. initialize start and target markings
		for(i=0;i < Places[0]->cnt;i++)
		{
			Places[i]->target_marking = Candidate->sv->vector[i];
			Places[i]->set_cmarking(New->sv->vector[i]);
		}
		for(i=0;i<Transitions[0]->cnt;i++)
		{
			Transitions[i]->check_enabled();
		}
		if(!mutual_reach())
		{
			Candidate -> next = Old;
			Old = Candidate;
			Candidate = New;
		}
		New = New -> next;
	}
	while(Old)
	{
		// 1. initialize start and target markings
		for(i=0;i < Places[0]->cnt;i++)
		{
			Places[i]->target_marking = Candidate->sv->vector[i];
			Places[i]->set_cmarking(Old->sv->vector[i]);
		}
		for(i=0;i<Transitions[0]->cnt;i++)
		{
			Transitions[i]->check_enabled();
		}
		if(!mutual_reach())
		{
			cout << "\nnet does not have home markings!\n\n";
			return 1;
		}
		Old = Old -> next;
	}
	cout << "\n\n home marking found (reported state)\n\n";
	for(i=0;i<Places[0]->cnt;i++)
	{
		Places[i]->set_cmarking(Candidate->sv->vector[i]);
	}
	printstate("",CurrentMarking);
	return(0);
}
#else

int home(){return 0;}

#endif

void print_binDec(binDecision * d, int indent);
void print_binDec(int h)
{
	unsigned int i;
	for(i=0;i< Places[0] -> NrSignificant;i++)
	{
		cout << Places[i] -> name << ": " << Places[i] -> nrbits;
	}
	cout << endl;
	print_binDec(binHashTable[h],0);
}


void print_binDec(binDecision * d, int indent)
{
	// print bin decision table at hash entry h

	for(int i=0;i<indent;i++) cout << ".";

	if(!d)
	{
		cout << " nil " << endl;
		return;
	}

	cout << "b " << d -> bitnr << " v ";
	for(unsigned int i=0; i< (BitVectorSize - (d -> bitnr + 1)) ; i+=8)
	{
		cout << (unsigned int) (d -> vector)[i/8] << " " ;
	}

	for(unsigned int i=0;i<BitVectorSize - (d -> bitnr+1);i++)
	{
		cout << (((d->vector)[i/8] >> (7-i%8))%2);
	}
	cout << endl;
	print_binDec(d -> nextold,indent+1);
	print_binDec(d -> nextnew,indent+1);
}

#endif
