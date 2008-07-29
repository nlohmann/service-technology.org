// Nodes, arcs, Petri net containers, and basic access to a net (enabling, firing,...)

#ifndef NET_H
#define NET_H

#include<streambuf>
#include<cstring> ///// LINE ADDED BY NIELS
#include<cstdlib> ///// LINE ADDED BY NIELS
#include<iostream>
#include<limits.h>
#include<unistd.h>
#include "dimensions.H"

using std::ostream;
using std::ofstream; ///// LINE ADDED BY NIELS
using std::cerr;
using std::cout;
using std::endl;

#define true 1
#define false 0

class Arc;
extern unsigned int * CurrentMarking;

class formula;
class overflow {};

class Node 
{
 public:
  unsigned int nr;
  char * name; 
  unsigned short int NrOfArriving;
  unsigned short int NrOfLeaving;
  Arc ** ArrivingArcs;
  Arc ** LeavingArcs;
  Node(char *);
  ostream& operator << (ostream &);
  ~Node();
  void NewArriving(Arc&);
  void NewLeaving(Arc&);
  unsigned int pos[2]; // index in array of all nodes, two values for appearance in two arrays
#ifdef CYCLE
  Node * parent ; // fuer Tiefensuche im Netz
#endif
};

inline void Node::NewArriving(Arc & a)
{
  Arc ** Old = ArrivingArcs;
  NrOfArriving++;
  ArrivingArcs = new Arc* [NrOfArriving];
  for(int i = 0; i < NrOfArriving -1;i++)
  {
    ArrivingArcs[i] = Old[i];
  }
  ArrivingArcs[NrOfArriving -1] = & a;
  delete [] Old;
}

inline void Node::NewLeaving(Arc & a)
{
  Arc ** Old = LeavingArcs;
  NrOfLeaving++;
  LeavingArcs = new Arc* [NrOfLeaving];
  for(int i = 0; i < NrOfLeaving -1;i++)
  {
    LeavingArcs[i] = Old[i];
  }
  LeavingArcs[NrOfLeaving -1] = & a;
  delete [] Old;
}

inline Node::~Node()
{
  delete [] name;
  delete []  ArrivingArcs;
  delete [] LeavingArcs;
}

inline Node::Node(char * n)
{
  name = new char [strlen(n) + 1];
  strcpy(name,n);
  pos[0] = pos[1] = NrOfArriving = NrOfLeaving = 0;
  ArrivingArcs = new Arc* [1];
  LeavingArcs = new Arc*  [1];
}

extern unsigned int currentdfsnum;
// contains dfsnum of currently processed state, used for
//  ignorance managment
#ifdef MODELCHECKING
extern unsigned int formulaindex; // in modelchecking,
//the different subformulas must be treated spearately 
#endif

inline ostream& operator << (ostream & str,Node n)
{
  str << n.name;
  return str;
}
  
class Transition;

class Place: public Node
{
 public:
  static unsigned int cnt; // Number of places for statistics;
  Place(char *);
  ~Place();
  static unsigned int hash_value;
  unsigned int target_marking;
  unsigned int initial_marking;
  unsigned int hash_factor;
  void operator += (unsigned int);  // increment marking of place
  void operator -= (unsigned int);  // decrement marking of place
  bool operator >= (unsigned int);  // test enabledness with respect to place
  void set_marking(unsigned int);   // set initial 1marking of place;
  void set_cmarking(unsigned int);   // set current marking of place;
  void set_hash(unsigned int);      // define a factor for hash value calculation
  // hash(m) = sum(p in P) p.hash_factor*CurrentMarking[p]
  unsigned int index; // index in place array, necessary for symmetries
  unsigned int references; // we remove isolated places 
  static unsigned int NrSignificant;
  bool significant; // set by p-invariants; only marking of sig. places is stored
#ifdef STUBBORN
  unsigned int visible; // number of visible pre-transitions
  Transition ** PreTransitions; // List of pre-transitions (used as "mustbeincluded")
  Transition ** PostTransitions; // List of post-transitions (used as "mustbeincluded")
  void initialize();
#endif
#ifdef COVER
	bool bounded; // true if marking in current state is not omega
	unsigned int lastfinite; // number of tokens in the moment of omega-intro
#endif
#ifdef WITHFORMULA
	unsigned int cardprop; // number of propositions mentioning this pl.
	formula ** propositions;
#endif
  int capacity;   // maximum capacity
  int nrbits;     // nr of bits required for storing its marking (= log capacity)
};

extern Place ** Places;

inline Place::Place(char * name) : Node(name)
{
  cnt += 1;
#ifdef PREDUCTION
  significant = false; // in this case, psolve() sets sign. places to true
#else
  significant = true;
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


inline Place::~Place()
{
  cnt -= 1;
}
  
inline void Place::operator += (unsigned int i)
{
  initial_marking += i;
#ifndef SWEEP
  hash_value += i*hash_factor;
  hash_value %= HASHSIZE;
#endif
}

inline void Place::operator -= (unsigned int i)
{
  initial_marking -= i;
#ifndef SWEEP
  hash_value -= i*hash_factor;
  hash_value %= HASHSIZE;
#endif
}

inline bool Place::operator >= (unsigned int i)
{
  return((initial_marking >= i) ? 1 : 0);
}

inline void Place::set_hash(unsigned int i)
{
  hash_value -= hash_factor * initial_marking;
  hash_factor = i;
  hash_value += hash_factor * initial_marking;
  hash_value %= HASHSIZE;
}
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


class Arc
{
 public:
	static unsigned int cnt;
  Node * Source;
  Node * Destination;
  Place * pl;
  Transition * tr;
  unsigned int Multiplicity;
  Arc(Transition *, Place *, bool ,unsigned int);
  Node * Get(bool);
  void operator += (unsigned int);
};

#ifdef STUBBORN
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
    PreTransitions[NrOfArriving] = (Transition *) 0;
  for(i=0;i < NrOfLeaving;i++)
    {
	PostTransitions[i] = LeavingArcs[i]-> tr;
    }
    PostTransitions[NrOfLeaving] = (Transition *) 0;
}
#endif

inline Arc::Arc(Transition * t,Place * p, bool totrans, unsigned int mult)
{
  tr = t;
  pl = p;
  Source = totrans ? (Node *) p : (Node *) t;
  Destination = totrans ? (Node *) t : (Node *) p;
  Multiplicity = mult;
	cnt++;
}

inline Node * Arc::Get(bool dest)
{
  return dest? Destination : Source;
}

inline void Arc::operator += (unsigned int incr)
{
  Multiplicity += incr;
}

class Transition:public Node
{
 public:
  static unsigned int cnt; // statistics;
  static Transition * StartOfEnabledList;
  Transition(char *);
  ~Transition();
  unsigned int fairness;
  static unsigned int NrEnabled; // Number of enabled tr. at curr. state
  bool enabled;
  Transition * NextEnabled;
  Transition * PrevEnabled; // double linking in list of enabled transitions
  // in the sequel, lists are NIL-terminated
  unsigned int * PrePlaces; // Places to be checked for enabledness
  unsigned int * Pre; // Multiplicity to be checked
  unsigned int * IncrPlaces; // Places that are incremented by transition
  unsigned int * Incr; // Amount of increment
  unsigned int * DecrPlaces; // Places that are decremented by transition
  unsigned int * Decr; // amount of decrement
  Transition ** ImproveEnabling; // list of transitions where enabledness
  //must be checked again after firing this transition
  Transition ** ImproveDisabling; // list of transitions where disabledness
  // must be checked again after firing this transition
  void initialize(); // Set above arrays, list, enabled...
  void fire(); // replace current marking by successor marking, force
  // enabling test where necessary
  void backfire(); // fire transition backwards to replace original state,
  // force enabling tests where necessary
  inline void check_enabled(); // test if tr is enabled. If necessary, rearrange list
	int hash_change; // change of hash value by firing t;
	void set_hashchange();
#ifdef STUBBORN
  static Transition * StartOfStubbornList;
  static Transition * EndOfStubbornList;
  static unsigned int NrStubborn; // Nr. of enabled (!) transitions in stubborn set
  Transition * NextStubborn;
  unsigned int instubborn;
  Place * scapegoat; // unsufficiently marked place for disabled transition
  Transition ** mustbeincluded; // If this transition is in a stubborn set, these ones must be, too
  Transition ** conflicting; // Transitions that can disable this transition
  unsigned int dfs; 
  unsigned int min; 
  unsigned int stamp;
  unsigned int mbiindex; // index in must be included list;
  Transition * nextontarjanstack; // for tscc based stubborn set method
  Transition * nextoncallstack; // for tscc based stubborn set method
  static Transition * TarjanStack;
  static Transition * CallStack;
  bool visible;
#endif 
#ifdef MODELCHECKING
	unsigned int * lstdisabled;
	unsigned int * lstfired;
#else
	bool down; // transition is in the down set of a state predicate
	unsigned int lastdisabled; // dfsnum of last state where
							  // some fired transition disables this one
	unsigned int lastfired; // dfsnum of last state where this tr. was fired
	Transition ** add_up; // addditional transitions to be included when
						  // this transition is in down set
#endif
  static Transition * StartOfIgnoredList; // which transitions could be
  Transition * NextIgnored;               //potentially ignored
#ifdef WITHFORMULA
	bool * pathrestriction;
#endif
#if defined(CYCLE) || defined(STRUCT)
	bool cyclic ; // can firing potentially close cycle in reach-graph?
#endif
#ifdef COVER
	void traceback();
#endif
#if defined(FAIRPROP) || defined(EVENTUALLYPROP) || defined(STABLEPROP) || defined(MODELCHECKING)
	unsigned int fairabled; //(#enabled [strongfair trans], #disabled [weak])
	unsigned int faired; // #succs in scc [strongfair trans])
#endif
#ifdef SWEEP
	long int progress_value;
#endif
};

inline Transition::Transition(char * name):Node(name)
{
  cnt += 1;
  if(!(cnt % REPORTFREQUENCY))
  {
	cerr << "\n" << cnt << "transitions parsed\n";
  }
  enabled = false;
#ifdef STUBBORN
  stamp = 0;
  NextStubborn = (Transition *) 0;
  instubborn = false; 
  visible = false;
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


inline Transition::~Transition()
{
  cnt -= 1;
}

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
  StartOfEnabledList = (Transition *) 0;
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
  ImproveEnabling[i] = (Transition *) 0;
  // Create list of transitions where enabledness can change
  // if this transition fires. For collecting these transitions, we
  // abuse the Enabled linked list
  StartOfEnabledList = (Transition *) 0;
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
  ImproveDisabling[i] = (Transition *) 0;
#ifdef STUBBORN
  // Create list of conflicting transitions. If this  transition is enabled nd member of a
  // stubborn set then these transitions must be in the stubborn set, too.
  StartOfEnabledList = (Transition *) 0;
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
  conflicting[i] = (Transition *) 0;
  mustbeincluded = conflicting;
#endif
  NrEnabled = 0;
  set_hashchange();
}


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
		cerr << "capacity of place " << Places[*p]->name << " exceeded!" << endl;
		_exit(4);
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
      PrevEnabled = (Transition *) 0;
      enabled = true;
      NrEnabled++;
#ifdef STUBBORN
      mustbeincluded = conflicting;
#endif
    }
}

extern Place * CheckPlace;
extern Transition * CheckTransition;
extern Transition ** Transitions;
extern unsigned int Edges;
extern Transition * LastAttractor; // Last transition in list of
// static attractor set. Attractor sets form always a prefix in the 
// list of stubborn transitions. Static attractor transitions are never
// removed from that list.

// file names for input and output

extern char * netfile;
extern char * analysefile;
extern char * graphfile;
extern char * pathfile;
extern char * statefile;
extern char * symmfile;
extern char * netbasename;

extern bool Aflg, Sflg, Yflg, Pflg,GMflg, aflg, sflg, yflg,pflg,gmflg;
extern char graphformat;
extern long int BitVectorSize;


#endif



















