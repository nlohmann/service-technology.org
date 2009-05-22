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


#include "graph.H"
#include "formula.H"
#include "ltl.H"
#include "stubborn.H"
#include "dimensions.H"
#include "buchi.h"

#include <fstream.h>
#include <cstring>
#include <cstdio>

Decision ** HashTable;
unsigned int LastChoice;
Decision * LastDecision;
unsigned int Scapegoat;
Statevector * LastVector;
unsigned int currentdfsnum = 1;
SearchTrace * Trace;
unsigned int CardFireList;
unsigned int pos;
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
State * TarStack;


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
      tl[i++] = t;
    }
  tl[i] = (Transition *) 0;
  CardFireList = i;
  return tl;
}

void printstate(char *, unsigned int *);
void printmarking()
{
	unsigned int i;
	
	for(i=0;i<Places[0]->cnt;i++)
	{
		cout << Places[i]->name << " : " << CurrentMarking[i] << endl;
	}
	cout << "-------" << endl;
}



State * SEARCHPROC();
void printpath(State *,ofstream *);
void print_path(State * s)
{
	if(pflg)
	{
		ofstream pathstream(pathfile);
		if(!pathstream)
		{
      fprintf(stderr, "lola: cannot open path output file '%s'\n", pathfile);
      fprintf(stderr, "      no output written\n");
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
	int i,j;
	// level = 1 --> top level, no firelist, mark '!'
	// level = 0 --> other level, firelist,  mark '*'
	if(!s) return;

	if(gmflg)
	{
	  (*graphstream) << "STATE " << (level ? "! " : "* ") << s ->dfs ;
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
	  if(!level)
	  {
	  for(i=0; i < s -> current;i++)
	  {
		(*graphstream) << s -> firelist[i]->name << " -> " << s -> succ[i]->dfs << "\n";
      }
		(*graphstream) << s -> firelist[s->current]->name << " => " << s -> succ[s->current]->dfs << "\n";
	  for(i = s -> current + 1; s ->firelist[i];i++)
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
		 for(i=0;i<Places[0]->cnt;i++)
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
	  for(i=0; i < s -> current;i++)
	  {
		cout << s -> firelist[i]->name << " -> " <<
		s -> succ[i]->dfs << "\n";
      }
		cout << s -> firelist[s->current]->name << " => " << s -> succ[s->current]->dfs << "\n";
	  for(i = s -> current + 1; s ->firelist[i];i++)
	  {
		cout << s -> firelist[i]->name << " -> ?\n";
      }
	  }
	  cout << "\n";
	}
if(!s->parent) return;
	s -> parent -> firelist[s -> parent -> current] -> backfire();
	printincompletestates(s -> parent,graphstream,0);
#endif
#endif
}


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
  ofstream * graphstream;
  unsigned int i;
  State * NewState;
  // init initial marking and hash table
  isbounded = 1;
  if(gmflg)
  {
	graphstream = new ofstream(graphfile);
	if(!*graphstream)
	{
    fprintf(stderr, "lola: cannot open graph output file '%s'\n", graphfile);
    fprintf(stderr, "      no output written\n");
		gmflg = false;
	}
  }
#if defined(SYMMETRY) && SYMMINTEGRATION==1
  Trace = new SearchTrace [Places[0]->cnt];
#endif
  for(i = 0; i < HASHSIZE;i++)
    {
      binHashTable[i] = (binDecision *) 0;
    }
#ifdef WITHFORMULA 
	int res;
	if(!F)
	{	
    fprintf(stderr, "lola: specify predicate in analysis task file\n");
		_exit(4);
	}
	F = F -> reduce(&res);
	if(res<2) return res;	
	F = F -> posate();
	F -> tempcard = 0;
	F -> setstatic();
	if(F ->  tempcard) 
	{
    fprintf(stderr, "lola: temporal operators are not allowed in state predicates\n");
		exit(3);
	}
	cout << "\n Formula with\n" << F -> card << " subformula(s).\n";
	F -> parent = (formula *) 0;
#endif
  if(SEARCHPROC()) cerr << "Sollte eigentlich nicht vorkommen";
	NrOfStates = 1;
	Edges = 0;

  CurrentState = INSERTPROC();
        CurrentState -> firelist = FIRELIST();
  CurrentState -> current = 0;
  CurrentState -> parent = (State *) 0;
	F -> initatomic();

  CurrentState -> succ = new State * [CardFireList+1];
  CurrentState -> dfs = CurrentState -> min = 0;
	CurrentState -> phi = F -> value;
	CurrentState -> nexttar = CurrentState -> prevtar = CurrentState;
	TarStack = CurrentState;
  
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
              cerr << "st: " << NrOfStates << "     edg: " << Edges << "\n";
		}
	  CurrentState -> firelist[CurrentState -> current] -> fire();
	  if(NewState = SEARCHPROC())
	    {
		  // State exists! (or, at least, I am not responsible for it (in the moment))
	      CurrentState -> firelist[CurrentState -> current] -> backfire();
#ifdef WITHFORMULA
	update_formula(CurrentState -> firelist[CurrentState -> current]);
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
	update_formula(CurrentState -> firelist[CurrentState -> current]);
	      NewState = INSERTPROC();
	      NewState -> firelist = FIRELIST();
	      NewState -> dfs = NewState -> min = NrOfStates++;
		NewState -> phi = F -> value;
		NewState -> prevtar = TarStack;
		NewState -> nexttar = TarStack -> nexttar;
		TarStack = TarStack -> nexttar = NewState;
		NewState -> nexttar -> prevtar = NewState;
	      NewState -> current = 0;
	      NewState -> parent = CurrentState;
	      NewState -> succ =  new State * [CardFireList+1];
	      CurrentState -> succ[CurrentState -> current] = NewState;
	      CurrentState = NewState;
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
	      if(CurrentState -> parent) CurrentState -> parent -> min = MIN(CurrentState -> min, CurrentState-> parent -> min);

	  CurrentState = CurrentState -> parent;
	  if(CurrentState)
	    {
	      CurrentState -> firelist[CurrentState -> current] -> backfire();
		  update_formula(CurrentState -> firelist[CurrentState -> current]);
	      CurrentState -> current ++;
	    }
	}
	}
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





