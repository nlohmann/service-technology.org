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


/// This file organises the internal representation of a Petri net, i.e.
/// Nodes, arcs, Petri net containers, and basic access to a net (enabling, firing,...)
/// as well as output of a net.
/// This file does also contain the main routine and related topics such as
/// command line options.

#ifndef NET_H
#define NET_H

#include <iostream>
#include <limits.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "dimensions.H"
//#include "userconfig.H" // should fix https://gna.org/bugs/?15223

using std::ostream;
using std::ofstream;
using std::cerr;
using std::cout;
using std::endl;

#define true 1
#define false 0


///// 4 LINES ADDED BY NIELS
extern void statistics(unsigned int s, unsigned int e, unsigned int h);
extern unsigned int NrOfStates;
extern unsigned int Edges;
extern unsigned int NonEmptyHash;


#ifdef MAXIMALSTATES
/// abort LoLA if more than MAXIMALSTATES states are processed
void checkMaximalStates(unsigned int states);
#endif


class Arc; ///< Petri net arcs
extern unsigned int * CurrentMarking; //< The current marking used in state space explorations

class formula; ///< Can hold a state predicate or a temporal logic formula
class overflow {}; ///< a dummy class for handling overflow exceptions

/// Petri net nodes, including places and transitions
class Node
{
 public:
  unsigned int nr;
  char * name;  ///< external (string) representation
  unsigned short int NrOfArriving; ///< number of incoming arcs
  unsigned short int NrOfLeaving; ///< number of outgoing arcs
  Arc ** ArrivingArcs; ///< array with pointers to each incoming arc
  Arc ** LeavingArcs; ///< array with pointers to each outgoing arc
  Node(char *); ///< constructor
  ostream& operator << (ostream &); ///< output procedure
  ~Node(); ///< destructor
  void NewArriving(Arc&); ///< insertion of a new incoming arc during net construction
  void NewLeaving(Arc&); ///< insertion of a new outgoing arc during net construction
  unsigned int pos[2]; ///< index in array of all nodes, two values for appearance in two arrays
#ifdef CYCLE
  Node * parent ; ///< for organizing a list that serves as a stack in a depth-first search through the net
                  ///< idea: cycles in the net are necessary for seeing cycles in the state space
#endif
};

/// insert a new incoming arc
inline void Node::NewArriving(Arc & a)
{
  Arc ** Old = ArrivingArcs; /// remember original array
  NrOfArriving++; /// increment size
  ArrivingArcs = new Arc* [NrOfArriving]; /// enlarge array
  for(int i = 0; i < NrOfArriving -1;i++) ///copy old entries
  {
    ArrivingArcs[i] = Old[i];
  }
  ArrivingArcs[NrOfArriving -1] = & a; /// add new arc
  delete [] Old; /// free old array
}

/// insert a new outgoing arc
inline void Node::NewLeaving(Arc & a)
{
  Arc ** Old = LeavingArcs; /// remember old array
  NrOfLeaving++; /// increment size
  LeavingArcs = new Arc* [NrOfLeaving]; ///enlarge array
  for(int i = 0; i < NrOfLeaving -1;i++) /// copy old entries
  {
    LeavingArcs[i] = Old[i];
  }
  LeavingArcs[NrOfLeaving -1] = & a; /// add new arc
  delete [] Old; /// free old arry
}

/// destructor
inline Node::~Node()
{
	/// release nontrivial ingredients
  delete [] name;
  delete []  ArrivingArcs;
  delete [] LeavingArcs;
}

/// constructor
inline Node::Node(char * n)
{
	/// copy external representation
  name = new char [strlen(n) + 5];
  strcpy(name,n);
   /// initialize arcs
  pos[0] = pos[1] = NrOfArriving = NrOfLeaving = 0;
   /// dummy arrays to start with
  ArrivingArcs = new Arc* [1];
  LeavingArcs = new Arc*  [1];
}

/// contains dfsnum of currently processed transition, used for
/// stubborn.ignorance management which requires depth first search
/// through transitions
extern unsigned int currentdfsnum;



// TODO: shift to check.H ?
#ifdef MODELCHECKING
extern unsigned int formulaindex; ///< in modelchecking,
///< the different subformulas must be treated spearately
#endif

/// output operator
inline ostream& operator << (ostream & str,Node n)
{
  str << n.name;
  return str;
}

class Transition;

/// A Petri net place
class Place: public Node
{
 public:
  static unsigned int cnt; ///< overall number of places;
  Place(char *); ///< constructor
  ~Place(); ///< destructor
  static unsigned int hash_value; ///< hash value of CurrentMarking
  // TODO: conditional compilation?
  unsigned int target_marking; ///< marking that is checked for reachability
  unsigned int initial_marking; ///< the initial marking (as defined in input file)
  unsigned int hash_factor; ///< weight of place in hash value calculation (= weighted sum of markings)
  void operator += (unsigned int);  ///<increment marking of place
  void operator -= (unsigned int);  ///< decrement marking of place
  bool operator >= (unsigned int);  ///< test enabledness with respect to place
  void set_marking(unsigned int);   ///< set initial marking of place;
  void set_cmarking(unsigned int);   ///< set current marking of place;
  void set_hash(unsigned int);      ///< define a factor for hash value calculation
  ///< hash(m) = sum(p in P) p.hash_factor*CurrentMarking[p]
  unsigned int index; ///< index in place array, necessary for symmetries
  unsigned int references; ///< #objects referring to this place - if 0 => remove place as isolated
  static unsigned int NrSignificant; ///< nr of significant places (insignificant =
  ///< linear dependent on signifiucant places
  bool significant; ///< set when PREDUCTION is switched on ; only marking of sig. places is stored
#ifdef STUBBORN
  unsigned int visible; ///< number of visible pre-transitions; used in stubborn set algorithms
  Transition ** PreTransitions; ///< List of pre-transitions (used as "mustbeincluded" in stubborn set algorithms)
  Transition ** PostTransitions; ///< List of post-transitions (used as "mustbeincluded" in stubborn set algorithms)
  void initialize(); ///< should be called upon parsing a net
#endif
#ifdef COVER
	bool bounded; ///< true if marking in current state is not omega (coverability feature)
	unsigned int lastfinite; ///< number of tokens in the moment of omega-intro (coverability feature)
#endif
#ifdef WITHFORMULA
	unsigned int cardprop; ///< number of propositions mentioning this place when a formula is involved
	formula ** propositions; ///< arry of all propositions mentioning this place. Used whebn formula values are updated
#endif
  unsigned int capacity;   ///< maximum capacity used for determining nr of bits in Decision tree
  unsigned int nrbits;     ///< nr of bits required for storing its marking (= log capacity)
};

extern Place ** Places; ///< array containing all places of the net
///< in this array, significant places are sorted to the beginning

/// constructor
inline Place::Place(char * name) : Node(name)
{
  cnt += 1;
#ifdef PREDUCTION
  significant = false; // in this case, psolve() sets sign. places subsequently
#else
  significant = true; // without PREDUCTION, all places are significant
#endif
  if(!(cnt % REPORTFREQUENCY))
  {
	cerr << "\n" << cnt << "places parsed\n";
  }
  references = initial_marking = target_marking = hash_factor = 0;
#ifdef COVER
	bounded = true;
#endif
#ifdef STUBBORN
	visible = 0; // real setting in sortscapegoats()
#endif
#ifdef WITHFORMULA
	cardprop = 0;
#endif
}

///desctructor
// TODO: free arrays that are present depending on STUBBORN, MODELCHECKING etc.?
inline Place::~Place()
{
  cnt -= 1;
}

/// add i to initial marking of place, adjust hash value
inline void Place::operator += (unsigned int i)
{
  initial_marking += i;
#ifndef SWEEP
  hash_value += i*hash_factor;
  hash_value %= HASHSIZE;
#endif
}

/// sub i from initial marking of place, adjust hash value
inline void Place::operator -= (unsigned int i)
{
  initial_marking -= i;
#ifndef SWEEP
  hash_value -= i*hash_factor;
  hash_value %= HASHSIZE;
#endif
}

// TODO: Do we ever use this?
/// compare initial marking with i
inline bool Place::operator >= (unsigned int i)
{
  return((initial_marking >= i) ? 1 : 0);
}

/// define a weight for a place that is used for computing a hash value
inline void Place::set_hash(unsigned int i)
{
  hash_value -= hash_factor * initial_marking;
  hash_factor = i;
  hash_value += hash_factor * initial_marking;
  hash_value %= HASHSIZE;
}

/// set the initial marking  of a place to i; adjust hash values
inline void Place::set_marking(unsigned int i)
{
#ifndef SWEEP
  hash_value -= hash_factor * initial_marking;
#endif
  initial_marking = i;
#ifndef SWEEP
 hash_value += hash_factor * initial_marking;
  hash_value %= HASHSIZE;
#endif
}

/// set CurrentMarking of a place, adjust hash value
inline void Place::set_cmarking(unsigned int i)
{
#ifndef SWEEP
  hash_value -= hash_factor * CurrentMarking[index];
#endif
  CurrentMarking[index] = i;
#ifndef SWEEP
 hash_value += hash_factor *CurrentMarking[index];
  hash_value %= HASHSIZE;
#endif
}

/// Petri net arc
class Arc
{
 public:
	static unsigned int cnt; ///< number of arcs
  Node * Source; ///< the source node of arc (can be place or transition)
  Node * Destination; ///< the sink node of arc (can be place or transition)
  Place * pl; ///< the place connected to this arc (can be source or sink)
  Transition * tr; ///< the transition connected to this arc (can be source of sink)
  unsigned int Multiplicity; ///< multiplicity attached to this arc
  Arc(Transition *, Place *, bool ,unsigned int); ///< constructor
  Node * Get(bool); ///< false --> get source  true --> get destination
  void operator += (unsigned int);  ///< add i to multiplicity of arc
};

#ifdef STUBBORN

/// create those arrays for the place which are required for stubborn set calculations
inline void Place::initialize()
{
	int i;

  // Create list of pre-transitions. If this place is the scapegoat for a disabled transition in
  // stubborn set then these transitions must be in the stubborn set, too.
  PreTransitions = new Transition * [NrOfArriving+1];
  PostTransitions = new Transition * [NrOfLeaving+1];
  for(i=0;i < NrOfArriving;i++)
    {
	PreTransitions[i] = ArrivingArcs[i]-> tr;
    }
    PreTransitions[NrOfArriving] = NULL;
  for(i=0;i < NrOfLeaving;i++)
    {
	PostTransitions[i] = LeavingArcs[i]-> tr;
    }
    PostTransitions[NrOfLeaving] = NULL;
}
#endif

/// constructor
inline Arc::Arc(Transition * t,Place * p, bool totrans, unsigned int mult)
{
  tr = t;
  pl = p;
  Source = totrans ? (Node *) p : (Node *) t;
  Destination = totrans ? (Node *) t : (Node *) p;
  Multiplicity = mult;
	cnt++;
}

/// get source (0) or destination (1) of node
inline Node * Arc::Get(bool dest)
{
  return dest? Destination : Source;
}

/// add i to multiplicity of node (used in parser, otherwise deprecated)
inline void Arc::operator += (unsigned int incr)
{
  Multiplicity += incr;
}

/// A Petri net transition
class Transition:public Node
{
 public:
  static unsigned int cnt; ///< statistics; overall number of transitions
  static Transition * StartOfEnabledList; ///< anchor to list of enabled transitions
  Transition(char *); ///< constructor
  ~Transition(); ///< destructor
  unsigned int fairness; ///< fairness constraint defined for this transition: 0=no, 1=weak, 2=strong
  static unsigned int NrEnabled; ///< Number of enabled tr. at CurrentMarking
  bool enabled; ///< is this transition enabled in CurrentMarking?
  Transition * NextEnabled; ///< enabled transitions are kept in double linked list --> insertion, deletion in O(1)
  Transition * PrevEnabled; ///< enabled transitions are kept in double linked list --> insertion, deletion in O(1)
  // in the sequel, lists are represented as 0-terminated arrays
  unsigned int * PrePlaces; ///< Places to be checked for enabledness after this fired
  unsigned int * Pre; ///< Multiplicity to be checked for enabledness after this fired
  unsigned int * IncrPlaces; ///< Places that are incremented by this transition firing
  unsigned int * Incr; ///< Amount of increment when this transition fires
  unsigned int * DecrPlaces; ///< Places that are decremented by this transition firing
  unsigned int * Decr; ///< amount of decrement when this transition fires
  Transition ** ImproveEnabling; ///< list of transitions where enabledness
  ///< must be checked again after firing this transition
  Transition ** ImproveDisabling; ///< list of transitions where disabledness
  ///< must be checked again after firing this transition
  void initialize(); ///< Set arrays, list, enabled etc. for this transition
  void fire(); ///< replace current marking by successor marking, force
  ///< enabling test where necessary
  void backfire(); ///< fire transition backwards to replace original state,
  ///< force enabling tests where necessary, used in backtracking
  inline void check_enabled(); ///< test if tr is enabled. If necessary, rearrange Enabled list
	int hash_change; ///< Delta of hash value: must be added to current hash value when t fires;
	void set_hashchange(); ///< set the hash value Delta for this transition
#ifdef STUBBORN
  static Transition * StartOfStubbornList; ///< Anchor to start of stubborn set
  static Transition * EndOfStubbornList; ///< Anchor to end of stubborn set
  static unsigned int NrStubborn; ///< Nr. of enabled (!) transitions in stubborn set
  Transition * NextStubborn; ///< stubborn set is organized as a single linked list
  unsigned int instubborn; ///< This transition in stubborn set?
  Place * scapegoat; ///< insufficiently marked place for disabled transition
  Transition ** mustbeincluded; ///< If this transition is in a stubborn set, these ones must be, too
  Transition ** conflicting; ///< Transitions that can disable this transition, used as mustbeincluded
  unsigned int dfs; ///< dfs number for depth first search through mustbeincluded graph
  unsigned int min; ///< lowlink used for finding SCC in mustbeincluded graph
  unsigned int stamp; ///< used for determining whether dfs values of this section stem from this search
  unsigned int mbiindex; ///< index in mustbeincluded graph: represents successor in depth first search through mustbeincluded graph;
  Transition * nextontarjanstack; ///< represents Tarjan stack for scc detection in mustbeincluded graph
  Transition * nextoncallstack; ///< represents call stack for scc detection in mustbeincluded graph
  static Transition * TarjanStack; ///< Top of Tarjan stack in mustbeincluded graph search
  static Transition * CallStack; ///< Top of call stack in mustbeincluded graph search
  bool visible; ///< can this transition alter any atomic proposition in used formula?
#endif
#ifdef MODELCHECKING
	unsigned int * lstdisabled; ///<array containing, for each recursive search, largest dfsnum where this transition has been disabled
	 ///< used for resolving fairness and ignorance issues
	unsigned int * lstfired; ///< array containing, for each recursive search, largest dfsnum where this transition has fired.
	 ///< used for resolving fairness and ignorance issues ///
#else
	bool down; ///< transition is in the global down set of a state predicate (see Kristensen/Valmari approach)
	unsigned int lastdisabled; ///< dfsnum of last state where
							  ///< some fired transition disables this one
	 ///< used for resolving fairness and ignorance issues
	unsigned int lastfired; ///< dfsnum of last state where this tr. was fired
	 ///< used for resolving fairness and ignorance issues
	Transition ** add_up; ///< addditional transitions to be included when
						  ///< this transition is in down set
	 ///< used for resolving fairness and ignorance issues in the Kristensen/Valmari approach
#endif
  static Transition * StartOfIgnoredList; ///< Anchor to Ignored list
  Transition * NextIgnored;               ///< list of potentially ignored transitions
#ifdef WITHFORMULA
	bool * pathrestriction;  ///< for which subformulas, this transition is in the cope of corresponding path quantifier?
	///< used for extended CTL treatment
#endif
#if defined(CYCLE) || defined(STRUCT)
	bool cyclic ; ///< can firing potentially close cycle in reach-graph?
#endif
#ifdef COVER
	void traceback();///< fire transition backwards, but in Ancestor vector instead of CurrentMarking
	    ///< used for walking backwards in coverability graph generation
#endif
#if defined(FAIRPROP) || defined(EVENTUALLYPROP) || defined(STABLEPROP) || defined(MODELCHECKING)
	unsigned int fairabled; ///< (#enabled [strongfair trans], #disabled [weak])
	unsigned int faired; ///< #succs inside scc [strongfair trans])
#endif
#ifdef SWEEP
	long int progress_value; ///< Delta of this transition to progress values of markings
#endif
};

/// constructor
inline Transition::Transition(char * name):Node(name)
{
  cnt += 1;
  if(!(cnt % REPORTFREQUENCY))
  {
	cerr << "\n" << cnt << "transitions parsed\n";
  }
  enabled = false;  // real setting in first enabledness check
#ifdef STUBBORN
  stamp = 0; // not seen in any search
  NextStubborn = NULL;
  instubborn = false; // real set in init of stubbornm set
  visible = false; // real set upon parsing the formula
#endif
#ifdef EXTENDED
#ifndef MODELCHECKING
  lastfired = lastdisabled = 0;
  down = false;
  add_up = (Transition ** ) 0;
#endif
#endif
#ifdef CYCLE
	cyclic = false ;
#endif
#if defined(FAIRPROP) || defined(EVENTUALLYPROP) || defined(STABLEPROP)
	faired = fairabled = 0;
#endif
}

/// calculate impact of this transition to hash value of a marking
inline void Transition::set_hashchange()
{
	unsigned int i;

	hash_change = 0;
	for(i=0;IncrPlaces[i]<Places[0]->cnt;i++)
	{
		hash_change += Incr[i] * Places[IncrPlaces[i]]->hash_factor;
	}
	for(i=0;DecrPlaces[i]<Places[0]->cnt;i++)
	{
		hash_change -= Decr[i] * Places[DecrPlaces[i]]->hash_factor;
	}
	hash_change %= HASHSIZE;
}

/// destructor
inline Transition::~Transition()
{
  cnt -= 1;
}

/// init transition specific arrays
inline void Transition::initialize()
{
  unsigned int i,j,k;

  // Create list of Pre-Places for enabling test
  PrePlaces = new unsigned int  [NrOfArriving + 1];
  Pre = new unsigned int [NrOfArriving + 1];
  for(i = 0; i < NrOfArriving;i++)
    {
      PrePlaces[i] = ArrivingArcs[i]->pl->index;
      Pre[i] = ArrivingArcs[i]->Multiplicity;
    }
  PrePlaces[NrOfArriving] = UINT_MAX;
  // Create list of places where transition increments marking
  IncrPlaces = new unsigned int [NrOfLeaving + 1];
  Incr = new unsigned int [NrOfLeaving + 1];

  k=0;
  for(i = 0; i < NrOfLeaving;i++)
    {
      //Is Place a loop place?
      for(j=0;j<NrOfArriving;j++)
	{
	  if((LeavingArcs[i]->Destination) == (ArrivingArcs[j]->Source))
	    {
	      break;
	    }
	}
      if(j<NrOfArriving)
	{
	  //yes, loop place
	  if(LeavingArcs[i]->Multiplicity > ArrivingArcs[j]->Multiplicity)
	    {
	      // indeed, transition increments place
	      IncrPlaces[k] = LeavingArcs[i]->pl->index;
	      Incr[k] = LeavingArcs[i]->Multiplicity - ArrivingArcs[j]->Multiplicity;
	      k++;
	    }
	}
      else
	{
	  // no loop place
	  IncrPlaces[k] = LeavingArcs[i]->pl->index;
	  Incr[k] = LeavingArcs[i]->Multiplicity;
	  k++;
	}
    }
  IncrPlaces[k] = UINT_MAX;
  Incr[k] = 0;
  // Create list of places where transition decrements marking
  DecrPlaces = new unsigned int [NrOfArriving + 1];
  Decr = new unsigned int [NrOfArriving + 1];
  k=0;
  for(i = 0; i < NrOfArriving;i++)
    {
      //Is Place a loop place?
      for(j=0;j<NrOfLeaving;j++)
	{
	  if((ArrivingArcs[i]->Source) == (LeavingArcs[j]->Destination))
	    {
	      break;
	    }
	}
      if(j<NrOfLeaving)
	{
	  //yes, loop place
	  if(ArrivingArcs[i]->Multiplicity > LeavingArcs[j]->Multiplicity)
	    {
	      // indeed, transition decrements place
	      DecrPlaces[k] = ArrivingArcs[i]->pl->index;
	      Decr[k] = ArrivingArcs[i]->Multiplicity - LeavingArcs[j]->Multiplicity;
	      k++;
	    }
	}
      else
	{
	  // no loop place
	  DecrPlaces[k] = ArrivingArcs[i]->pl->index;
	  Decr[k] = ArrivingArcs[i]->Multiplicity;
	  k++;
	}
    }
  DecrPlaces[k] = UINT_MAX;
  Decr[k] = 0;
  // Create list of transitions where enabledness can change
  // if this transition fires. For collecting these transitions, we
  // abuse the Enabled linked list
  StartOfEnabledList = NULL;
  NrEnabled = 0;
  for(i=0;IncrPlaces[i] < Places[0]->cnt;i++)
    {
      for(j=0;j<Places[IncrPlaces[i]]->NrOfLeaving;j++)
	{
	  if(!(Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->enabled)
	    {
	      // not yet in list
	      (Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->NextEnabled = StartOfEnabledList;
	      StartOfEnabledList = (Places[IncrPlaces[i]]->LeavingArcs)[j]->tr;
	      NrEnabled++;
	      (Places[IncrPlaces[i]]->LeavingArcs)[j]->tr->enabled = true;
	    }
	}
    }
  ImproveEnabling = new Transition * [NrEnabled + 1];
  for(i=0;StartOfEnabledList;StartOfEnabledList = StartOfEnabledList -> NextEnabled,i++)
    {
      StartOfEnabledList->enabled = false;
      ImproveEnabling[i]=StartOfEnabledList;
    }
  ImproveEnabling[i] = NULL;
  // Create list of transitions where enabledness can change
  // if this transition fires. For collecting these transitions, we
  // abuse the Enabled linked list
  StartOfEnabledList = NULL;
  NrEnabled = 0;
  for(i=0;DecrPlaces[i] < Places[0]->cnt;i++)
    {
      for(j=0;j<Places[DecrPlaces[i]]->NrOfLeaving;j++)
	{
	  if(!(Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->enabled)
	    {
	      // not yet in list
	      (Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->NextEnabled = StartOfEnabledList;
	      StartOfEnabledList = (Places[DecrPlaces[i]]->LeavingArcs)[j]->tr;
	      NrEnabled++;
	      (Places[DecrPlaces[i]]->LeavingArcs)[j]->tr->enabled = true;
	    }
	}
    }
  ImproveDisabling = new Transition * [NrEnabled + 1];
  for(i=0;StartOfEnabledList;StartOfEnabledList = StartOfEnabledList -> NextEnabled,i++)
    {
      StartOfEnabledList->enabled = false;
      ImproveDisabling[i]=StartOfEnabledList;
    }
  ImproveDisabling[i] = NULL;
#ifdef STUBBORN
  // Create list of conflicting transitions. If this  transition is enabled nd member of a
  // stubborn set then these transitions must be in the stubborn set, too.
  StartOfEnabledList = NULL;
  NrEnabled = 0;
  for(i=0;PrePlaces[i] < Places[0]->cnt;i++)
    {
      for(j=0;j<Places[PrePlaces[i]]->NrOfLeaving;j++)
	{
	 if(!((Places[PrePlaces[i]]->LeavingArcs)[j]->tr == this))
	 {
	  if(!(Places[PrePlaces[i]]->LeavingArcs)[j]->tr->enabled)
	    {
	      // not yet in list
	      (Places[PrePlaces[i]]->LeavingArcs)[j]->tr->NextEnabled = StartOfEnabledList;
	      StartOfEnabledList = (Places[PrePlaces[i]]->LeavingArcs)[j]->tr;
	      NrEnabled++;
	      (Places[PrePlaces[i]]->LeavingArcs)[j]->tr->enabled = true;
	    }
	 }
	}
    }
  conflicting = new Transition * [NrEnabled + 1];
  for(i=0;StartOfEnabledList;StartOfEnabledList = StartOfEnabledList -> NextEnabled,i++)
    {
      StartOfEnabledList->enabled = false;
      conflicting[i]=StartOfEnabledList;
    }
  conflicting[i] = NULL;
  mustbeincluded = conflicting;
#endif
  NrEnabled = 0;
  set_hashchange();
}

/// fire this transition on CurrentMarking, re-evaluate enabledness
inline void Transition::fire()
{
  unsigned int * p;
  Transition ** t;
  unsigned int * i;

#ifdef EXTENDED
#ifdef MODELCHECKING
	lstfired[formulaindex] = currentdfsnum;
#else
	lastfired = currentdfsnum;
#endif
#endif
  for(p = IncrPlaces,i = Incr; *p < UINT_MAX; p++,i++)
    {
#ifdef COVER
if(Places[(*p)]-> bounded)
{
#endif
      CurrentMarking[* p] += * i;
#ifdef CHECKCAPACITY
	if(CurrentMarking[*p] > Places[*p]->capacity)
	{
    statistics(NrOfStates,Edges,NonEmptyHash);
    fprintf(stderr, "lola: capacity of place '%s' exceeded: CAPACITY=%d, but m(%s)=%d\n",
      Places[*p]->name, Places[*p]->capacity, Places[*p]->name, CurrentMarking[*p]);
		exit(4);
	}
#endif
#ifdef COVER
#ifndef SWEEP
      Places[0]->hash_value += (*i) * Places[*p]->hash_factor;
      Places[0]->hash_value %= HASHSIZE;
#endif
}
#endif
    }
  for(p = DecrPlaces,i = Decr; * p < UINT_MAX; p++,i++)
    {
#ifdef COVER
if(Places[(*p)]-> bounded)
{
#endif
      CurrentMarking[* p] -= * i;
#ifdef COVER
#ifndef SWEEP
      Places[0]->hash_value -= (*i) * Places[*p]->hash_factor;
      Places[0]->hash_value %= HASHSIZE;
#endif
}
#endif
    }


#ifndef COVER
#ifndef SWEEP
  Places[0]->hash_value += hash_change;
  Places[0]->hash_value %= HASHSIZE;
#endif
#endif
  for(t = ImproveEnabling;*t;t++)
    {
      if(!((*t) -> enabled))
	{
	  (*t)->check_enabled();
	}
    }
  for(t = ImproveDisabling;*t;t++)
    {
      if((*t)->enabled)
	{
	  (*t)->check_enabled();
	}
    }
}

/// undo effect of firing transition (used for backtracking)
inline void Transition::backfire()
{
  unsigned int * p;
  Transition ** t;
  unsigned int * i;

  for(p = IncrPlaces,i = Incr; *p < UINT_MAX; p++,i++)
    {
#ifdef COVER
	  if(Places[(*p)]->bounded)
{
#endif
      CurrentMarking[* p] -= * i;
#ifdef COVER
#ifndef SWEEP
      Places[0]->hash_value -= (*i) * Places[*p]->hash_factor;
      Places[0]->hash_value %= HASHSIZE;
#endif
}
#endif
    }
  for(p = DecrPlaces,i = Decr; * p < UINT_MAX; p++,i++)
    {
#ifdef COVER
	  if(Places[(*p)]->bounded)
{
#endif
      CurrentMarking[* p] += * i;
#ifdef COVER
#ifndef SWEEP
	Places[0]->hash_value += (*i) * Places[*p]->hash_factor;
      Places[0]->hash_value %= HASHSIZE;
#endif
}
#endif
    }
#ifndef COVER
#ifndef SWEEP
  Places[0]->hash_value -= hash_change;
  Places[0]->hash_value %= HASHSIZE;
#endif
#endif
  for(t = ImproveEnabling;*t;t++)
    {
      if((*t) -> enabled)
	{
	  (*t)->check_enabled();
	}
    }
  for(t = ImproveDisabling;*t;t++)
    {
      if(!((*t)->enabled))
	{
	  (*t)->check_enabled();
	}
    }
}

#ifdef COVER
extern unsigned int * Ancestor;

/// proceed to predeccessor marking in Ancestor vector
inline void Transition::traceback()
{
  unsigned int * p;
  Transition ** t;
  unsigned int * i;

  for(p = IncrPlaces,i = Incr; *p < UINT_MAX; p++,i++)
    {
	  if(Places[(*p)]->bounded)
{
      Ancestor[* p] -= * i;
}
    }
  for(p = DecrPlaces,i = Decr; * p < UINT_MAX; p++,i++)
    {
	  if(Places[(*p)]->bounded)
{
      Ancestor[* p] += * i;
}
    }
}
#endif

/// check activation of this transition
inline void Transition::check_enabled()
{
  unsigned int * p;
  unsigned int *i;

  for(p = PrePlaces , i = Pre ; *p < UINT_MAX; p++ , i++)
    {
      if(CurrentMarking[*p] < *i)
	{
	  if(enabled)
	    {
#ifdef EXTENDED
#ifdef MODELCHECKING
		  lstdisabled[formulaindex] = currentdfsnum;
#else
		  lastdisabled = currentdfsnum;
#endif
#endif
	      // exclude transition from list of enabled transitions
	      if(NextEnabled)
		{
		  NextEnabled -> PrevEnabled = PrevEnabled;
		}
	      if(PrevEnabled)
		{
		  PrevEnabled -> NextEnabled = NextEnabled;
		}
	      else
		{
		  StartOfEnabledList = NextEnabled;
		}
	      enabled = false;
	      NrEnabled--;
	    }
#ifdef STUBBORN
	      mustbeincluded = Places[(*p)]->PreTransitions;
              scapegoat = Places[*p];
#endif
	  return;
  	}
    }
  if(!enabled)
    {
      // include transition into list of enabled transitions
      NextEnabled = StartOfEnabledList;
      if(StartOfEnabledList)
	{
		NextEnabled -> PrevEnabled = this;
	}
      StartOfEnabledList = this;
      PrevEnabled = NULL;
      enabled = true;
      NrEnabled++;
#ifdef STUBBORN
      mustbeincluded = conflicting;
#endif
    }
}

extern Place * CheckPlace; ///< place that selected verification problem refers to
extern Transition * CheckTransition; ///< transition that selected verification problem refers to
extern Transition ** Transitions; ///< array containing all transitions in the net
extern unsigned int Edges; ///< number of transition occurrences during state space exploration
extern Transition * LastAttractor; ///< Last transition in list of
///< static attractor set. Attractor sets form always a prefix in the
///< list of stubborn transitions. Static attractor transitions are never
///< removed from that list.

/// file names for input and output

extern char * netfile;  ///< name of file containing LoLA net input
extern char * analysefile; ///< name of file containing parameters of verification problem input
extern char * graphfile; ///< name of file for reachability graph output
extern char * pathfile; ///< name of file for witness/counterexample path output
extern char * statefile; ///< name of file for witness/counterexample state output
extern char * symmfile; ///< name of file for output of generating set of all symmetries
extern char * netbasename; ///< base of net file name (used for generic output file names)
extern FILE * resultfile; ///< the result file

extern bool Aflg, Sflg, Yflg, Pflg,GMflg, aflg, sflg, yflg,pflg,gmflg; ///< flags for parsing command line
extern char graphformat; ///< command line option (verbatim) for controlling reachability graph output format
extern unsigned int BitVectorSize; ///< nr of bits for storing a single marking

void readnet(); ///< parse a net(+verification task parameters)

#endif



















