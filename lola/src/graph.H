#ifndef GRAPH_H
#define GRAPH_H

#include "net.H"
#include "dimensions.H"
#include "symm.H"
#include "formula.H"

// depository and search structures for reachability graph, TARJAN

//#define CYCRED
extern State * CurrentState;

extern unsigned int BitVectorSize;

class Statevector;


// State objects are nodes in the state graph They are linked to
// the marking vector via the contents entry

class State
{
public:
  State();
  ~State();
  int progress_value;
  bool persistent;
  static unsigned int card;
#ifndef MODELCHECKING
  Transition ** firelist; // Transitions to be fired
  unsigned int current; // Nr of last already fired element of firelist
#if defined(TARJAN) || defined(BREADTH_FIRST)
	State ** succ; // successor states in graph
#endif
  State * parent; // state responsible for first generation
#if defined( TARJAN ) 
  unsigned int dfs,min; // TARJAN's values
#endif
#endif
#ifdef MODELCHECKING
	unsigned int nr;
	State ** witness; // Next states in witness path (per subformula)
	Transition ** witnesstransition; // transition realising the witness path
	Transition *** checkfirelist;
	State *** checksucc;
	State ** checkparent;
	unsigned int * checkcurrent;
	bool * value; // value of subformulas
	bool * known; // subformula already evaluated ?
	State ** checknexttar;
	State ** checkprevtar;
	bool * checkexpired;
	unsigned int * checktarlevel;
	unsigned int * checkddfs;
	unsigned int * checkmmin;
	unsigned int * checkdfs, * checkmin;
	void protocol();
#endif
#ifdef COVER
	State * smaller; // points to covered state that is responsible for 
					 // w-Intro, is nil if such a state does not exist
	Place ** NewOmega; // 0-terminated array of places where 
					   // w-Intro happened at this state
#endif
#ifdef CYCRED
unsigned int closing;
#endif
#ifdef FULLTARJAN
	State * nexttar, * prevtar;
	bool phi, expired;
	unsigned int tarlevel;
	unsigned int ddfs, mmin;
	
#endif

};

inline State::State()
{

        persistent = 0;
#ifdef CYCRED
	closing = 0;
#endif
#ifdef MODELCHECKING
	nr = card++;
	witness = new State * [F -> tempcard+5];
	witnesstransition = new Transition * [F -> tempcard+5];
	value = new bool [F -> card+5];
	known = new bool [F -> card+5];	
	for(int i=0;i<F->card;i++)
	{
		known[i] = false;
	}
	checkfirelist = new Transition ** [F-> tempcard+5];
	checksucc = new State ** [F-> tempcard+5];
	checkparent = new State * [F -> tempcard+5];
	checkcurrent = new unsigned int [F -> tempcard+5];
	checkdfs = new unsigned int [F -> tempcard+5];
	checkmin = new unsigned int [F -> tempcard+5];
	checknexttar = new State * [F -> tempcard];
	F -> evaluateatomic(this);
	checkprevtar = new State * [F -> tempcard];
	checkexpired = new bool [ F -> tempcard];
	checkddfs = new unsigned int [ F -> tempcard];
	checkmmin = new unsigned int [F -> tempcard];
	checktarlevel = new unsigned int [ F -> tempcard];
	for(int i=0;i<F->tempcard;i++)
	{
		witness[i] = NULL;
		witnesstransition[i] = NULL;
		checkexpired[i] = false;
		checktarlevel[i] = 0;
   	}
#endif
#ifdef FULLTARJAN
	expired = false;
	tarlevel = 0;
#endif
}

class binDecision
{
public:
	unsigned int bitnr;
	binDecision *  nextold;
	binDecision *  nextnew;
	unsigned char * vector;
#if (defined(SYMMETRY) && SYMMINTEGRATION == 2)
  binDecision * prev;
#endif
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER) 
  State * state;
#endif
#ifdef SWEEP
	int persistent;
#endif
	binDecision(int);
	~binDecision();
};

State * binSearch();

inline State::~State()
{
#ifndef MODELCHECKING
  if(firelist) delete [] firelist;
#if defined(TARJAN) || defined(BREADTH_FIRST)
  if(succ) delete [] succ;
#endif
#endif
#ifdef MODELCHECKING
	if(witness) delete [] witness;
	if(witnesstransition) delete [] witnesstransition;
	if(checkfirelist) delete [] checkfirelist;
	if(checksucc) delete [] checksucc;
	if(checkparent) delete [] checkparent;
	if(checkcurrent) delete [] checkcurrent;
	if(value) delete [] value;
	if(known) delete [] known;
	if(checknexttar) delete [] checknexttar;
	if(checkdfs) delete [] checkdfs;
	if(checkmin) delete [] checkmin;
	if(checkexpired) delete [] checkexpired;
	if(checkprevtar) delete [] checkprevtar;
	if(checktarlevel) delete [] checktarlevel;
	if(checkddfs) delete [] checkddfs;
	if(checkmmin) delete [] checkmmin;
#endif
#ifdef COVER
	if(NewOmega) delete [] NewOmega;
#endif
}


// In Statevector, we store fragments of the marking, dependent
// on the branching structure of the decision tree
class Statevector
{
public:
  // head
#if (defined(SYMMETRY) && SYMMINTEGRATION == 2)
  Statevector * prev;
  unsigned int length;
#endif
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER) 
  State * state;
#endif
  // tail
  unsigned int * vector;

  Statevector(int);
  ~Statevector();

  unsigned int operator [] (unsigned int);
  void set(unsigned int,unsigned int);
};

class StatevectorList
{
	public:
	Statevector * sv;
	StatevectorList * next;
};

inline Statevector::~Statevector()
{
	if(vector) delete [] vector;
}

inline unsigned int Statevector::operator [] (unsigned int i)
{
#ifdef CAPACITY
  return ((vector[i / MPERWORD ] >> (MSIZE * (i % MPERWORD))) & MASK);
#else
  return vector[i];
#endif
}

inline void Statevector::set(unsigned int index,unsigned int value)
{
#ifdef CAPACITY
  vector[index / MPERWORD] &= ~(MASK << (MSIZE * (index % MPERWORD)));
  vector[index / MPERWORD] |= value << (MSIZE * (index % MPERWORD));
#else
    vector[index] = value;
#endif
}

inline Statevector::Statevector(int l)
{
#if (defined(SYMMETRY) && SYMMINTEGRATION == 2)
length = l;
#endif
#ifdef CAPACITY
  vector = new unsigned int  [l / MPERWORD + 1];
#else
  vector = new unsigned int [l];
#endif
#if defined(SYMMETRY) && SYMMINTEGRATION == 2
#endif
}

class Decision
{
public:
  unsigned int PlaceNr;
  unsigned int size;
  unsigned int capacity;
  Statevector ** vector;
  unsigned int * index;
  Decision ** next;
  unsigned int * value; 

  Decision(unsigned int);
  ~Decision();
  void insert(unsigned int,Statevector *,unsigned int,Decision *);
};

inline Decision::~Decision()
{
	unsigned int i;

	for(i=0;i<size;i++)
	{
		if(next[i]) delete next[i];
		if((!index[i]) && vector[i]) delete vector[i];
	}
	if(index) delete [] index;
	if(vector) delete [] vector;
	if(value) delete [] value;
	if(next) delete [] next;
}

inline Decision::Decision(unsigned int p)
{
  PlaceNr = p;
  size = 2;
  capacity = 2;
  vector = new Statevector * [2];
  next = new Decision * [2];
  value = new unsigned int [2];
  index = new unsigned int [2];
  next[0] = NULL;
  next[1] = NULL;
}

inline void Decision::insert(unsigned int v, Statevector * vec, unsigned int idx,Decision *nxt)
{
  unsigned int i,pos;
  Decision ** newd;
  Statevector ** newsv;
  unsigned int * newi, * newv;
  


  if(size >= capacity)
    {
      // enlarge table
      newi = new unsigned int [2 * capacity];
      newv = new unsigned int [2 * capacity];
      newd = new Decision * [2 * capacity];
      newsv = new Statevector * [2 * capacity];
      pos = 0;
      for(i=size;i>0;i--)
	{
	  if(value[i-1] < v)
	    {
	      pos = i;
	      break;
	    }
	  newv[i] = value[i-1];
	  newi[i] = index[i-1];
	  newd[i] = next[i-1];
	  newsv[i] = vector[i-1];
	}
      newv[pos] = v;
      newi[pos] = idx;
      newd[pos] = nxt;
      newsv[i] = vec;
      for(i=0;i<pos;i++)
	{
	  newv[i] = value[i];
	  newi[i] = index[i];
	  newd[i] = next[i];
	  newsv[i] = vector[i];
	}
      delete [] value;
      delete [] index;
      delete [] next;
      delete [] vector;
      value = newv;
      index = newi;
      next = newd;
      vector = newsv;
      size += 1;
      capacity *= 2;
    }
  else
    {
      pos = 0;
      for(i=size;i>0;i--)
	{
	  if(value[i-1] < v)
	    {
	      pos = i;
	      break;
	    }
	  value[i] = value[i-1];
	  index[i] = index[i-1];
	  next[i] = next[i-1];
	  vector[i] = vector[i-1];
	}
      value[pos] = v;
      index[pos] = idx;
      next[pos] = nxt;
      vector[i] = vec;
      size += 1;
    }
}


extern Decision ** HashTable;
extern unsigned int * BitHashTable;

// The next four variables describe the situation where the last recent search
// has failed. This information is useful for a faster insertion.

extern Decision * LastDecision;
extern unsigned int LastChoice;
extern unsigned int Scapegoat;
extern Statevector * LastVector;

// search_marking exists in two versions. One uses
// CurrentMarking and Places[0]->hash_value, the other
// one uses kanrep and kanhash

#if defined(SYMMETRY) && SYMMINTEGRATION==3
#define MARKINGVECTOR kanrep
#define HASHENTRY kanhash
#else
#define MARKINGVECTOR CurrentMarking
#define HASHENTRY Places[0]->hash_value
#endif

inline State * search_marking()
{
  unsigned int l,r,j;
  unsigned int & i = Scapegoat;
  unsigned int & m = LastChoice;
  Decision * & d = LastDecision;
  Statevector * & v = LastVector;

  // 0. Hashabelle leer -> alles klar
  v = NULL;
  if(!(d = HashTable[HASHENTRY])) return NULL;
  i = 0;
  while(d)
    {
      // 1. Interpolationssuche in Tabelle
      if(MARKINGVECTOR[i] < d -> value[0]) return NULL;
      if(MARKINGVECTOR[i] > d -> value[d->size - 1]) return NULL; 
      if(MARKINGVECTOR[i] == d -> value[0])
	{
	  // found, no interpolation necessary
	  m = 0;
	}
      else
	{
	  if(MARKINGVECTOR[i] == d -> value[d->size - 1])
	    {
	      // found, no interpolation necessary
	      m = d->size - 1; 
	    }
	  else
	    {
	      // Interpolation, value must be somewhere between 1 and size - 2
	      for(l=0,r=d->size - 1;l+1<r;)
		{
		  m = l + (r - l ) * (MARKINGVECTOR[i] - d -> value[l]) / (d->value[r]-d->value[l]);
		  if(m == l) m++;
		  if(m == r) m--; // Progress sichern!
		  if(d->value[m] == MARKINGVECTOR[i])
		    {
		      l = r = m;
		      break;
		    }
		  else
		    {
		      if(d->value[m] < MARKINGVECTOR[i])
			{
			  l= m;
			}
		      else
			{
			  r = m;
			}
		    }
		}
	      if(l != r) // this means: not found!
		{
		  return NULL; 
		}
	    }
	}
      // Interpolationssuche beendet, jetzt im Statevector suchen
      i++;
      v = d -> vector[m];
      for(j=d->index[m]+1;i < (d->next[m] ? d -> next[m] -> PlaceNr : Places[0]-> NrSignificant); i++,j++)
	{
	  if((*(d -> vector[m]))[j] != MARKINGVECTOR[i]) return NULL;
	}
      d = d -> next[m];
    } // while d
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
  return LastVector -> state; 
#else
  return CurrentState; // Hauptsache, was anderes als NULL
#endif
}

void canonize();
inline State * bit_search()
{
#ifdef SYMMETRY	
#if SYMMINTEGRATION == 3
	canonize();
#else
	canonize_on_the_fly();
#endif
	return ( BitHashTable[kanhash/MPERWORD] >> (kanhash % MPERWORD) & 1 ) ? CurrentState : NULL;
#else
	return ( BitHashTable[Places[0]->hash_value/MPERWORD] >> (Places[0]->hash_value % MPERWORD) & 1 ) ? CurrentState : NULL;
#endif
}

inline State * bit_insert()
{
#ifdef SYMMETRY
  BitHashTable[kanhash / MPERWORD] |= 1 << (kanhash % MPERWORD);
#else
  BitHashTable[Places[0]->hash_value / MPERWORD] |= 1 << (Places[0]->hash_value % MPERWORD);
#endif
return new State();
}
	

class SearchTrace
{
public:
	unsigned char bn_byte; // byte to be matched against tree vector; constructed from MARKINGVECTOR
	int bn_t; // index in tree vector
	unsigned char * bn_v; // current tree vector
	int bn_s; // nr of bits pending in byte from previous iteration
	int bn_dir; // did we go "old" or "new" in last decision?
	int bn_b; // bit nr at start of byte
	binDecision * frmdec, * tdec;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
        binDecision * vectrdec;
#endif
};

extern SearchTrace * Trace;
extern unsigned int pos;

inline void trace();

//inline State * first_symm_search_marking()
//{
//  unsigned int l,r,j;
//  unsigned int & i = Scapegoat;
//  unsigned int & m = LastChoice;
//  Decision * & d = LastDecision;
//  Statevector * & v = LastVector;
//
//  // 0. Hashabelle leer -> alles klar
//  v = (Statevector *) 0;
//  if(!(d = HashTable[Places[0]->hash_value])) return (State *) 0;
//  i = 0;
//  pos = 0;
//  FirstSymm();
//  while(d)
//    {
//      trace();
//      // 1. Interpolationssuche in Tabelle
//      if(CurrentMarking[i] < d -> value[0]) return (State *) 0;
//      if(CurrentMarking[i] > d -> value[d->size - 1]) return (State *) 0; 
//      if(CurrentMarking[i] == d -> value[0])
//	{
//	  // found, no interpolation necessary
//	  m = 0;
//	}
//      else
//	{
//	  if(CurrentMarking[i] == d -> value[d->size - 1])
//	    {
//	      // found, no interpolation necessary
//	      m = d->size - 1;
//	    }
//	  else
//	    {
//	      // Interpolation, value must be somewhere between 1 and size - 2
//	      for(l=0,r=d->size - 1;l+1<r;)
//		{
//		  m = l + (r - l ) * (CurrentMarking[i] - d -> value[l]) / (d->value[r]-d->value[l]);
//		  if(m == l) m++;
//		  if(m == r) m--; // Progress sichern!
//		  if(d->value[m] == CurrentMarking[i])
//		    {
//		      l = r = m;
//		      break;
//		    }
//		  else
//		    {
//		      if(d->value[m] < CurrentMarking[i])
//			{
//			  l= m;
//			}
//		      else
//			{
//			  r = m;
//			}
//		    }
//		}
//	      if(l != r) // this means: not found!
//		{
//		  return (State *) 0; 
//		}
//	    }
//	}
 //     // Interpolationssuche beendet, jetzt im Statevector suchen
 //     i++;
//      v = d -> vector[m];
//      for(j=d->index[m]+1;i < (d->next[m] ? d -> next[m] -> PlaceNr : Places[0]-> NrSignificant); i++,j++)
//	{
//	  trace();
//	  if((*(d -> vector[m]))[j] != CurrentMarking[i]) return (State *) 0;
//	}
 //     d = d -> next[m];
 //   } // while d
//#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
//  return LastVector -> state; 
//#else
//  return CurrentState; // Hauptsache, was anderes als NULL
//#endif
//}
//
//inline State * next_symm_search_marking()
//{
//  unsigned int l,r,j;
//  unsigned int & i = Scapegoat;
//  unsigned int & m = LastChoice;
//  Decision * & d = LastDecision;
//  Statevector * & v = LastVector;
//
//  d = Trace[pos].ld;
//  i = Trace[pos].sc;
//  m = Trace[pos].lc;
//  v = Trace[pos].lv;
//  if(i > d -> PlaceNr)
//    {
//      for(j=d->index[m]+i-d->PlaceNr;i < (d->next[m] ? d -> next[m]->PlaceNr : Places[0]->NrSignificant);i++,j++)
//	{
//	  trace();
//	  if((*(d->vector[m]))[j] != CurrentMarking[CurrentSymm[i]]) return (State *) 0;
//
//	}
//      d = d -> next[m];
//    }
//  while(d)
//    {
//      // 1. Interpolationssuche in Tabelle
//      if(CurrentMarking[CurrentSymm[i]] < d -> value[0]) return (State *) 0;
//      if(CurrentMarking[CurrentSymm[i]] > d -> value[d->size - 1]) return (State *) 0; 
//      if(CurrentMarking[CurrentSymm[i]] == d -> value[0])
//	{
//	  // found, no interpolation necessary
//	  m = 0;
//	}
//      else
//	{
//	  if(CurrentMarking[CurrentSymm[i]] == d -> value[d->size - 1])
//	    {
//	      // found, no interpolation necessary
//	      m = d->size - 1;
//	    }
//	  else
//	    {
//	      // Interpolation, value must be somewhere between 1 and size - 2
//	      for(l=0,r=d->size - 1;l+1<r;)
//		{
//		  m = l + (r - l ) * (CurrentMarking[CurrentSymm[i]] - d -> value[l]) / (d->value[r]-d->value[l]);
//		  if(m == l) m++;
//		  if(m == r) m--; // Progress sichern!
//		  if(d->value[m] == CurrentMarking[CurrentSymm[i]])
//		    {
//		      l = r = m;
//		      break;
//		    }
//		  else
//		    {
//		      if(d->value[m] < CurrentMarking[CurrentSymm[i]])
//			{
//			  l= m;
//			}
//		      else
//			{
//			  r = m;
//			}
//		    }
//		}
//	      if(l != r) // this means: not found!
//		{
//		  return (State *) 0; 
//		}
//	    }
//	}
//      trace();
//      // Interpolationssuche beendet, jetzt im Statevector suchen
//      i++;
//      v = d -> vector[m];
//      for(j=d->index[m]+1;i < (d->next[m] ? d -> next[m] -> PlaceNr : Places[0]-> NrSignificant); i++,j++)
//	{
//	  if((*(d -> vector[m]))[j] != CurrentMarking[CurrentSymm[i]]) return (State *) 0;
//	  trace();
//	}
//      d = d -> next[m];
//    } // while d
//#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
//  return LastVector -> state; 
//#else
//	return CurrentState; // Hauptsache, was anderes als NULL
//#endif
//}

inline void print_marking()
{
  unsigned int i;
  for(i=0;i< Places[0]->cnt;i++)
    {
      if(MARKINGVECTOR[i])
      cout << Places[i]->name << ": " << MARKINGVECTOR[i] << ", ";
    }
}

//inline State * symm_search_marking()
//{
//  State * s;
//  unsigned int fsc;
//  unsigned int flc;
//  Decision * fld;
//  Statevector * flv;
//  
//  if(!HashTable[Places[0]->hash_value])
//    {
//      LastDecision = (Decision *) 0;
//      return (State *) 0;
//    }
//  if(s = first_symm_search_marking())
//    {
//      return s;
//    }
//  fsc = Scapegoat;
//  flc = LastChoice;
//  fld = LastDecision;
//  flv = LastVector;
//  
//  while((pos = NextSymm(Scapegoat)) < CardStore)
//    {
//      if(s = next_symm_search_marking())
//	{
//	  return s;
//	}
//    }
//  Scapegoat = fsc;
//  LastChoice = flc;
//  LastDecision = fld;
//  LastVector = flv;
//  return (State *) 0;
//}
      
      
  
extern unsigned int NonEmptyHash;
      
inline State * insert_marking()
{
  // Fuegt die aktuelle Markierung in die Suchstruktur ein und erzeugt das Markierungsobjekt
  // Die Suche nach der Markierung muss vorher fehlgeschlagen sein, LastDecision, LastChoice und Scapegoat muessen 
  // unbedingt die Werte
  // vom fehlgeschlagenen Versuch haben.
  
  Statevector * sv;

  unsigned int i;
  // HashTable n    
  if(LastDecision)
    {
      // HashTable not empty
      if(Scapegoat == LastDecision -> PlaceNr)
	{
	  // In diesem Fall muss die Tabelle um einen neuen Eintrag ergaenzt werden und ein neuer
	  // Statevector angelegt
	  sv = new Statevector(Places[0]->NrSignificant - Scapegoat);
	  for(i=Scapegoat;i < Places[0]-> NrSignificant;i++)
	    {
	      sv -> set(i - Scapegoat,MARKINGVECTOR[i]);
	    }
	  LastDecision -> insert(MARKINGVECTOR[Scapegoat],sv,0, NULL);
#if (defined(SYMMETRY) && SYMMINTEGRATION == 2)
	  sv -> prev = LastVector;
#endif
#if defined(COVER) || defined(MODELCHECKING) || defined(TARJAN)
	  sv -> state = new State;
#endif
#if defined(TARJAN) || defined(MODELCHECKING)
	return sv -> state;
#endif
	return new State;
	}
      else
	{
	  // EineTabelle muss neu angelegt werden, ein Eintrag zeigt auf die
	  // bisherige Markierung, ein anderer auf die neue
	  
	  Decision * NewDecision;
	  unsigned int oldindex,newindex;
	  NewDecision = new Decision(Scapegoat);
	  oldindex = MARKINGVECTOR[Scapegoat] < (*LastVector)[Scapegoat - LastDecision  -> PlaceNr + LastDecision -> index[LastChoice]] ? 1 : 0;
	  // bearbeite die Seite der alten Markierung
	  NewDecision -> value[oldindex] = (*LastVector)[Scapegoat - LastDecision -> PlaceNr + LastDecision -> index[LastChoice]];
	  NewDecision -> vector[oldindex] = LastVector;
	  NewDecision -> index[oldindex] = Scapegoat - LastDecision  -> PlaceNr + LastDecision -> index[LastChoice];
	  NewDecision -> next[oldindex] = LastDecision -> next[LastChoice];
	  LastDecision -> next[LastChoice] = NewDecision;
	  // bearbeite die Seite der neuen Markierung
	  newindex = 1 - oldindex;
	  sv = new Statevector(Places[0]->NrSignificant - Scapegoat);
	  for(i=Scapegoat;i < Places[0]-> NrSignificant;i++)
	    {
	      sv ->set(i - Scapegoat,MARKINGVECTOR[i]);
	    }
	  NewDecision -> value[newindex] = MARKINGVECTOR[Scapegoat];
	  NewDecision -> vector[newindex] = sv;
	  NewDecision -> index[newindex] = 0;
	  NewDecision -> next[newindex] = NULL;
#if defined(COVER) || defined(TARJAN) || defined(MODELCHECKING)
	  sv -> state = new State;
#endif
#if (defined(SYMMETRY) && SYMMINTEGRATION == 2)
	  sv -> prev = LastVector;
#endif
#if defined(COVER) || defined(TARJAN) || defined(MODELCHECKING)
	  return sv -> state;
#endif
	return new State;
	}
    }
  else
    {
      // Hashtabelleneintrag leer, also Einertabelle anlegen und komplette Markeirung speichern
      NonEmptyHash++;
      Decision * d;
      d = new Decision(0);
      d -> size = 1;
      d -> value[0] = MARKINGVECTOR[0];
      d -> index[0] = 0;
      d -> next[0] = NULL;
      sv = d -> vector[0] = new Statevector(Places[0] -> NrSignificant);
      for(i=0;i< Places[0]-> NrSignificant;i++)
	{
	  sv ->set(i,MARKINGVECTOR[i]);
	}
#if defined(COVER) || defined(TARJAN) || defined(MODELCHECKING)
      sv -> state = new State;
#endif
#if (defined(SYMMETRY) && SYMMINTEGRATION == 2)
      sv -> prev = NULL;
#endif
      HashTable[HASHENTRY] = d; 
#if defined(COVER) || defined(TARJAN) || defined(MODELCHECKING)
      return sv -> state;
#endif
	return new State;
    }
}

inline void canonize()
{
  unsigned int level, current, current_min,i;
  unsigned int * swap;

  // 2. Markierung nach kanrep austragen

  for(i=0;i<Places[0]->cnt;i++)
    {
      kanrep[i] = CurrentMarking[i];
    }

  // 3. Sortieren auf kanrep
  for(level = 0; level < CardStore;level++)
    {
      current_min = Store[level].card;
      for(current = 0;current < Store[level].card;current++)
	{
	  for(i=Store[level].argnr;i<Places[0]->cnt;i++)
	    {
	      if((current_min < Store[level].card ?
		  kanrep[Store[level].image[current_min].vector[i-Store[level].argnr]] : kanrep[i]) 
		 != kanrep[Store[level].image[current].vector[i-Store[level].argnr]])
		{
		  break;
		}
	    }
	  if(i >= Places[0]->cnt) continue;
	  if((current_min < Store[level].card ?
	      kanrep[Store[level].image[current_min].vector[i-Store[level].argnr]] : kanrep[i]) 
	     > kanrep[Store[level].image[current].vector[i-Store[level].argnr]])
	    {
	      current_min = current;
	    }
	}
      if(current_min < Store[level].card)
	{
	  for(i=0;i<Store[level].argnr;i++)
	    {
	      kanrep1[i]=kanrep[i];
	    }
	  for(i=Store[level].argnr;i < Places[0]->cnt;i++)
	    {
	      kanrep1[i] = kanrep[Store[level].image[current_min].vector[i-Store[level].argnr]];
	    }
	  swap = kanrep;
	  kanrep = kanrep1;
	  kanrep1 = swap;
	}
    }
  // 5. suchen
  kanhash = 0;
  for(i=0;i<Places[0]->cnt;i++)
  {
	 kanhash += Places[i]->hash_factor * kanrep[i];
	  kanhash %= HASHSIZE;
  }
}

inline State * canonical_representitive()
{
  canonize();
  // return search_marking();
  return binSearch();
}
      
void RemoveGraph();      
unsigned int depth_first();
unsigned int breadth_first();

Transition ** firelist();
extern unsigned int CardFireList;
int reversibility();
int liveproperty();
int home();
void statistics(unsigned int, unsigned int, unsigned int);

#ifdef EXTENDED
	extern unsigned int currentdfsnum;
#endif

inline void printdecision(Decision *);
inline void print_depository()
{
	unsigned int i;
	for(i=0;i<HASHSIZE;i++)
	{
		if(HashTable[i]) 
		{
			cout << " contents of hash value "<< i << endl;
			printdecision(HashTable[i]);
		}
	}
	cout << endl << "*******************************************" << endl;
}

inline void printdecision(Decision * d)
{
		
	unsigned int m,i,j,limit;
	for(m = 0; m < d -> size;m++)
        {
		cout << "* P[" << d -> PlaceNr <<"] = " << d -> value[m];
		if(d->next[m]) limit = d -> next[m]->PlaceNr;
		else limit = Places[0]->NrSignificant;
	
		for(i=d->PlaceNr,j=d->index[m]+1;i<limit;i++,j++)
		{
			cout << " " << (*(d -> vector[m]))[j];
		}
		cout << endl;
		if(d->next[m]) printdecision(d->next[m]);
	}
}	
		
inline int logzwo(int m)
{
	int k;

	k = 0;
	while(m)
	{
		k++;
		m = m >> 1;
	}
	return k;
}

inline unsigned int bitstoint(unsigned char * bytepos, int bitpos, int nrbits)
{
	// transform the sequence of nrbits bits in a char array starting at byte bytepos, bit bitpos
 	// into an integer

	unsigned int result;

	result = 0;

	if(nrbits <= 8 - bitpos)
	{
		// all bits in same byte
		result = (*bytepos >> 8 - (bitpos + nrbits)) % (1 << nrbits);
		return result;
	}
	result = (*bytepos % (1 << ((8 - bitpos))));
	nrbits-= 8 - bitpos;
	bytepos++;
	while(nrbits > 8)
	{
		nrbits -= 8;
		result = (result << 8) + * bytepos;
		bytepos++;
	}
	if(nrbits)
	{
		result = (result << nrbits) + ( * bytepos >> (8 - nrbits));
	}
	return result;
}

inline void inttobits(unsigned char * bytepos, int bitpos, int nrbits, unsigned int value)
{
	// store value as a bit sequence of a char array starting at byte bytepos, bit bitpos and consisting of nrbits bits


	if(nrbits <= 8 - bitpos)
	{
		// value fits completely into first byte
		* bytepos |= value << (8 - (bitpos + nrbits));
		return;
	}
	* bytepos |= value >> (nrbits + bitpos - 8);
	nrbits -= 8 - bitpos;
	value = value % (1 << nrbits);
	bytepos++;
	while(nrbits > 8)
	{
		nrbits -= 8;
		*bytepos = value >> nrbits;
		bytepos++;
		value = value % (1 << nrbits) ;
	}
	if(nrbits)
	{
		*bytepos = value << 8 - nrbits;
	}
	return;
}
		
void print_binDec(int);
inline binDecision::binDecision(int b)
{
	unsigned int i;
	bitnr = b;
	nextold = nextnew = NULL;
	vector = new unsigned char [ (BitVectorSize - (bitnr + 2) ) / 8 + 2];
	for(i=0;i<(BitVectorSize - (bitnr + 2) ) / 8 + 1;i++)
	{
		vector[i] = 0;
	}
#ifdef SWEEP
	persistent = 0;
#endif
}

inline binDecision::~binDecision()
{
	delete [] vector;
}

extern binDecision ** binHashTable;
	extern unsigned int bin_p; // (=place); index in MARKINGVECTOR
	extern unsigned int bin_pb; // next bit of place to be processed;
	extern unsigned char bin_byte; // byte to be matched against tree vector; constructed from MARKINGVECTOR
	extern unsigned int bin_t; // index in tree vector
	extern unsigned char * bin_v; // current tree vector
	extern unsigned int bin_s; // nr of bits pending in byte from previous iteration
	extern unsigned int bin_d; // difference position
	extern unsigned int bin_dir; // did we go "old" or "new" in last decision?
	extern unsigned int bin_b; // bit nr at start of byte
	extern binDecision * fromdec, * todec;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
        extern binDecision * vectordec;
#endif
inline void trace()
{
	// bin_p = index, bin_pb = 0!
	Trace[bin_p].bn_byte =  bin_byte; 
	Trace[bin_p].bn_t = bin_t; 
	Trace[bin_p].bn_v = bin_v; 
	Trace[bin_p].bn_s = bin_s; 
	Trace[bin_p].bn_dir = bin_dir; 
	Trace[bin_p].bn_b = bin_b; 
	Trace[bin_p].frmdec = fromdec;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
        Trace[bin_p].vectrdec = vectordec;
#endif
	Trace[bin_p].tdec = todec;
}

State * symm_first_binSearch();
State * symm_next_binSearch();

inline State * bin_symm_search_marking()
{
  State * st;
	unsigned char byte; 
	unsigned int t,p,pb,d; 
	unsigned char * v; 
	int s; 
	int dir; 
	int b; 
	binDecision * from, * to;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
        binDecision * vector;
#endif
  
	cout << endl << "SEARCH" << endl;
	print_marking();
	cout << endl;
	print_binDec(0);
	cout << endl;
  if(!binHashTable[Places[0]->hash_value])
    {
      return NULL;
    }
  if((st = symm_first_binSearch()))
    {
      return st;
    }
	// store state of search, for subsequent insert
	byte = bin_byte; 
	t = bin_t; 
	v = bin_v; 
	s = bin_s; 
	d = bin_d;
	p = bin_p;
	pb = bin_pb;
	dir = bin_dir; 
	b = bin_b; 
	from = fromdec;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
        vector = vectordec;
#endif
	to = todec;
  
  while((bin_p = NextSymm(Scapegoat)) < Places[0]->cnt)
    {
      if((st = symm_next_binSearch()))
	{
	  return st;
	}
    }
	// restore state of search, for subsequent insert
	bin_pb = pb;
	bin_p = p;
	bin_d = d;
	bin_byte = byte; 
	bin_t = t; 
	bin_v = v; 
	bin_s = s; 
	bin_dir = dir; 
	bin_b = b; 
	fromdec = from;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
        vectordec = vector;
#endif
	todec = to;
  return NULL;
}
      
inline State * binSearch(binDecision * Bucket)
{
	bin_dir = 0;
	// search the state given in MARKINGVECTOR in the binHashTable
	if(!( fromdec = Bucket)) 
	{
		return NULL;
	}
	
	bin_p = 0; bin_pb = 0; bin_s = 0; bin_t = 0; bin_b = 0;
	bin_byte = 0;
	bin_v = fromdec -> vector;
	todec = fromdec -> nextold;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
	vectordec = fromdec;
#endif

	while(1)
	{

		// - fill byte starting at bit s
		while(bin_s < 8 && bin_p < Places[0] -> NrSignificant)
		{
			if(8 - bin_s < Places[bin_p]->nrbits - bin_pb)
			{
				inttobits(&bin_byte, bin_s, 8 - bin_s, (MARKINGVECTOR[bin_p] % (1 << (Places[bin_p] -> nrbits - bin_pb)))>> (Places[bin_p]->nrbits + bin_s - (8 + bin_pb)));
				bin_pb += 8 - bin_s;
				bin_s = 8;
				break;
				
			}
			else
			{
				inttobits(&bin_byte, bin_s, Places[bin_p] -> nrbits - bin_pb, MARKINGVECTOR[bin_p] % (1 << (Places[bin_p] -> nrbits - bin_pb)));
				bin_s += Places[bin_p] -> nrbits - bin_pb;
				bin_p++;
				bin_pb = 0;
			}
		}
		// - compare byte with bin_v[bin_t] and find first difference
		bin_d = 8 - logzwo( bin_v[bin_t] ^ bin_byte);
		// - conclude
		// case 0: length of vector reached without difference = state found
		if(bin_b + bin_d >= BitVectorSize)
		{
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
			return vectordec -> state;
#else
			return CurrentState; // not relevant, must be something != 0
#endif
		}
checkagain:     if(todec)
		{
			if(bin_b + bin_d < todec -> bitnr)
			{
		// case 1: no difference or difference before "to" branch = state not found
				if(bin_d < 8)
				{
					return NULL;
				}
				else
				{
					bin_s = 0;
					bin_t ++;
					bin_b += 8;
					bin_byte = 0;
					continue;
				}
			}
			if(bin_b + bin_d == todec -> bitnr)
			{
		// case 2: no difference or difference equals "to" branch = continue with nextnew
				if(bin_d<8)
				{
					bin_byte = bin_byte << (bin_d+1);
					bin_s = 7 - bin_d;
					bin_v = todec -> vector;
					bin_t = 0;
					bin_b = todec -> bitnr + 1;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
					vectordec = 
#endif
						fromdec = todec;
					todec = fromdec -> nextnew;
					bin_dir = 1;
					continue;
				}
				else
				{
					bin_s = 0;
					bin_t ++;
					bin_b += 8;
					bin_byte = 0;
					continue;
				}
			}
		// case 3: difference beyond "to" branch = continue with nextold
			fromdec = todec;
			todec = fromdec -> nextold;
			bin_dir = 0;
			goto checkagain;
		}
		else
		{
		// case 4: no difference or state not found
			if(bin_d < 8)
			{
				return NULL;
			}
			else
			{
				bin_s = 0;
				bin_t++;
				bin_b += 8;
				bin_byte = 0;
				continue;
			}
			
		}
	}
}

inline State * binInsert(binDecision ** Bucket)
{
	// relies on a previous bin_search with all values bin_* set properly!

	binDecision * newd;
	int vby, vbi;

	if(!*Bucket) 
	{
		// empty hash bucket --> create first entry

	        NonEmptyHash++;
		*Bucket = new binDecision(-1);
		// 3. remaining places
		vby = vbi = 0;
		for(bin_p=0; bin_p < Places[0]-> NrSignificant;bin_p ++)
		{
			inttobits((*Bucket)->vector + vby, vbi, Places[bin_p] -> nrbits, MARKINGVECTOR[bin_p]);
			vby += (vbi + Places[bin_p] -> nrbits) / 8;
			vbi =  (vbi + Places[bin_p] -> nrbits) % 8;
		}
#if (defined(SYMMETRY) && SYMMINTEGRATION == 2)
		(*Bucket)->prev = NULL;
#endif

#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER) 
		(*Bucket)->state = new State;
		return (*Bucket)->state;
#endif
		return new State;
	}
	newd = new binDecision(bin_b + bin_d);
	if(bin_dir)
	{
		newd -> nextold = fromdec -> nextnew;
		fromdec -> nextnew = newd;
	}
	else
	{
		newd -> nextold = fromdec -> nextold;
		fromdec -> nextold = newd;
	}
#if (defined(SYMMETRY) && SYMMINTEGRATION == 2)
        newd-> prev = vectordec;
#endif
	// fill vector
	// 1. remaining   bin_byte
	if(bin_d < 7)
	{
		newd->vector[0] = bin_byte << (bin_d+1);
		vby = 0;
		vbi = 7 - bin_d;
	}
	else
	{
		vby = 0;
		vbi = 0;
	}
		
	// 2. remaining bit of current place
	if(bin_pb)
	{
		inttobits(newd->vector, vbi, Places[bin_p] -> nrbits - bin_pb, MARKINGVECTOR[bin_p] % (1 << (Places[bin_p] -> nrbits - bin_pb)));
		vby += (vbi + Places[bin_p] -> nrbits - bin_pb) / 8;
		vbi =  (vbi + Places[bin_p] -> nrbits - bin_pb) % 8;
		bin_p++;
	}

	// 3. remaining places
	for(; bin_p < Places[0]-> NrSignificant;bin_p ++)
	{
		inttobits(newd->vector + vby, vbi, Places[bin_p] -> nrbits, MARKINGVECTOR[bin_p]);
		vby += (vbi + Places[bin_p] -> nrbits) / 8;
		vbi =  (vbi + Places[bin_p] -> nrbits) % 8;
	}

#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER) 
        newd-> state = new State;
	return newd-> state;
#endif
	return new State;
	
}

inline void binDelete(binDecision ** Bucket)
{

	// deletes the top level state in HashTable[h]

	// procedure:
	// - top level state has nextold, but no nextnew (no branch at -1)
	// - descend to end of nextold list (that state has largest common prefix with top level state)
	// - copy remainder of that state into top level state vector
	// - append nextnew of that state to end of nextold list (its nextold is 0)
	// - free that state's vector and decision record

	binDecision ** oldlist;

	if((*Bucket) -> nextold)
	{
		unsigned int vbyte, obyte;
		unsigned int upper, lower; 
		unsigned char * v, * o;
		v = (*Bucket) -> vector;
		
		for(oldlist = &((*Bucket) -> nextold); (*oldlist) -> nextold ; oldlist = & (  (*oldlist) -> nextold));
#ifdef SWEEP
		(*Bucket) -> persistent = (*oldlist) -> persistent;
#endif
		vbyte = (*oldlist) -> bitnr / 8;
		upper = (*oldlist) -> bitnr % 8;
		o = (*oldlist) -> vector;
		// copy state vector
		// 1. reverse branch bit
		v[vbyte] = v[vbyte] ^ (1 << (7 - upper++));
		
		if(upper == 8) { upper = 0; vbyte++;}
		// 2. copy remaining vector
		if(upper)
		{
			
			lower = 8 - upper;
			obyte = 0;
			unsigned char vuppermask = ( (1 << upper) - 1) << lower ;
			unsigned char vlowermask = (1 << lower) - 1;
			unsigned char ouppermask = ( (1 << lower) - 1) << upper ;
			unsigned char olowermask = (1 << upper) - 1;
			do
			{
				/* 1. upper part of o --> lower part of v */
				v[vbyte] = (v[vbyte]  & vuppermask) | ((o[obyte] & ouppermask) >> upper);
				vbyte++;
				if( 8 * vbyte >= BitVectorSize) break;
				/* 2. lower part of o --> upper part of v */
				v[vbyte] = (v[vbyte]  & vlowermask) | ((o[obyte++] & olowermask) << lower);
				if( (*oldlist)->bitnr + 8 * obyte >= BitVectorSize) break;
			}
			while(1);
		}
		else
		{
			for(obyte=0; (*oldlist)->bitnr + 8 * obyte < BitVectorSize;obyte++)
			{
				v[vbyte++] = o[obyte];
			}
		}
		// 3. delete vector and set pointer in decision records
		delete [] (*oldlist) -> vector;
		(*oldlist) = (*oldlist) -> nextnew;
	}
	else
	{
		// top level state is last state in list
		delete [] (*Bucket) -> vector;
		*Bucket = NULL;
	}
}


inline void binDeleteAll(binDecision * d)
{
	if(!d) return;
	binDeleteAll(d -> nextold);
	binDeleteAll(d -> nextnew);
	//delete [] d -> vector;
	delete d;
}
	
inline void binDeleteAll(int h)
{
	// delete all states in hash bucket h
	if(!binHashTable[h]) return;
	binDeleteAll(binHashTable[h] -> nextold);
	delete [] binHashTable[h] -> vector;
	binHashTable[h] = NULL;
}

inline State * symm_first_binSearch()
{
	bin_dir = 0;
	// search the state given in MARKINGVECTOR in the binHashTable
	fromdec = binHashTable[HASHENTRY]; 
	bin_p = 0; bin_pb = 0; bin_s = 0; bin_t = 0; bin_b = 0;
	bin_byte = 0;
	bin_v = fromdec -> vector;
	todec = fromdec -> nextold;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
	vectordec = fromdec;
#endif

	FirstSymm();
	while(1)
	{

		// - fill byte starting at bit s
		do
		{
			if(bin_pb == 0) trace();
			if(8 - bin_s < Places[bin_p]->nrbits - bin_pb)
			{
				inttobits(&bin_byte, bin_s, 8 - bin_s, (MARKINGVECTOR[bin_p] % (1 << (Places[bin_p] -> nrbits - bin_pb)))>> (Places[bin_p]->nrbits + bin_s - (8 + bin_pb)));
				bin_pb += 8 - bin_s;
				bin_s = 8;
				break;
				
			}
			else
			{
				inttobits(&bin_byte, bin_s, Places[bin_p] -> nrbits - bin_pb, MARKINGVECTOR[bin_p] % (1 << (Places[bin_p] -> nrbits - bin_pb)));
				bin_s += Places[bin_p] -> nrbits - bin_pb;
				bin_p++;
				bin_pb = 0;
			}
		}
		while(bin_s < 8 && bin_p < Places[0] -> NrSignificant);
		// - compare byte with bin_v[bin_t] and find first difference
		bin_d = 8 - logzwo( bin_v[bin_t] ^ bin_byte);
		// - conclude
		// case 0: length of vector reached without difference = state found
		if(bin_b + bin_d >= BitVectorSize)
		{
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
			return vectordec -> state;
#else
			return CurrentState; // not relevant, must be something != 0
#endif
		}
checkagain:     if(todec)
		{
			if(bin_b + bin_d < todec -> bitnr)
			{
		// case 1: no difference or difference before "to" branch = state not found
				if(bin_d < 8)
				{
					// compute scapegoat;
					unsigned int pb,s;
					s = bin_s;	
					Scapegoat = bin_p;
					pb = bin_pb;
					while(1)
					{	
						s -= pb;
						if(s <= bin_d) return NULL;
						Scapegoat--;
						pb = Places[Scapegoat] -> nrbits;
					}
				}
				else
				{
					bin_s = 0;
					bin_t ++;
					bin_b += 8;
					bin_byte = 0;
					continue;
				}
			}
			if(bin_b + bin_d == todec -> bitnr)
			{
		// case 2: no difference or difference equals "to" branch = continue with nextnew
				if(bin_d<8)
				{
					bin_byte = bin_byte << (bin_d+1);
					bin_s = 7 - bin_d;
					bin_v = todec -> vector;
					bin_t = 0;
					bin_b = todec -> bitnr + 1;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
					vectordec = 
#endif
						fromdec = todec;
					todec = fromdec -> nextnew;
					bin_dir = 1;
					continue;
				}
				else
				{
					bin_s = 0;
					bin_t ++;
					bin_b += 8;
					bin_byte = 0;
					continue;
				}
			}
		// case 3: difference beyond "to" branch = continue with nextold
			fromdec = todec;
			todec = fromdec -> nextold;
			bin_dir = 0;
			goto checkagain;
		}
		else
		{
		// case 4: no difference or state not found
			if(bin_d < 8)
			{
				// compute scapegoat
				unsigned int pb,s;
				s = bin_s;	
				Scapegoat = bin_p;
				pb = bin_pb;
				while(1)
				{	
					s -= pb;
					if(s <= bin_d) return NULL;
					Scapegoat--;
					pb = Places[Scapegoat] -> nrbits;
				}
			}
			else
			{
				bin_s = 0;
				bin_t++;
				bin_b += 8;
				bin_byte = 0;
				continue;
			}
			
		}
	}
}

inline State * symm_next_binSearch()
{
	// bin_p set by calling procedure!
	bin_pb = 0;
	bin_byte = Trace[bin_p].bn_byte; 
	bin_t = Trace[bin_p].bn_t; 
	bin_v = Trace[bin_p].bn_v; 
	bin_s = Trace[bin_p].bn_s; 
	bin_dir = Trace[bin_p].bn_dir; 
	bin_b = Trace[bin_p].bn_b; 
	fromdec = Trace[bin_p].frmdec;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
        vectordec = Trace[bin_p].vectrdec;
#endif
	todec = Trace[bin_p].tdec;
	// search the state given in MARKINGVECTOR in the binHashTable

	while(1)
	{

		// - fill byte starting at bit s
		do
		{
			trace();
			if(8 - bin_s < Places[bin_p]->nrbits - bin_pb)
			{
				inttobits(&bin_byte, bin_s, 8 - bin_s, (MARKINGVECTOR[CurrentSymm[bin_p]] % (1 << (Places[bin_p] -> nrbits - bin_pb)))>> (Places[bin_p]->nrbits + bin_s - (8 + bin_pb)));
				bin_pb += 8 - bin_s;
				bin_s = 8;
				break;
				
			}
			else
			{
				inttobits(&bin_byte, bin_s, Places[bin_p] -> nrbits - bin_pb, MARKINGVECTOR[CurrentSymm[bin_p]] % (1 << (Places[bin_p] -> nrbits - bin_pb)));
				bin_s += Places[bin_p] -> nrbits - bin_pb;
				bin_p++;
				bin_pb = 0;
			}
		}
		while(bin_s < 8 && bin_p < Places[0] -> NrSignificant);
		// - compare byte with bin_v[bin_t] and find first difference
		bin_d = 8 - logzwo( bin_v[bin_t] ^ bin_byte);
		// - conclude
		// case 0: length of vector reached without difference = state found
		if(bin_b + bin_d >= BitVectorSize)
		{
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
			return vectordec -> state;
#else
			return CurrentState; // not relevant, must be something != 0
#endif
		}
checkagain:     if(todec)
		{
			if(bin_b + bin_d < todec -> bitnr)
			{
		// case 1: no difference or difference before "to" branch = state not found
				if(bin_d < 8)
				{
					// compute scapegoat
					unsigned int pb,s;
					s = bin_s;	
					Scapegoat = bin_p;
					pb = bin_pb;
					while(1)
					{	
						s -= pb;
						if(s <= bin_d) return NULL;
						Scapegoat--;
						pb = Places[Scapegoat] -> nrbits;
					}
				}
				else
				{
					bin_s = 0;
					bin_t ++;
					bin_b += 8;
					bin_byte = 0;
					continue;
				}
			}
			if(bin_b + bin_d == todec -> bitnr)
			{
		// case 2: no difference or difference equals "to" branch = continue with nextnew
				if(bin_d<8)
				{
					bin_byte = bin_byte << (bin_d+1);
					bin_s = 7 - bin_d;
					bin_v = todec -> vector;
					bin_t = 0;
					bin_b = todec -> bitnr + 1;
#if defined(TARJAN) || defined(MODELCHECKING) || defined(COVER)
					vectordec = 
#endif
						fromdec = todec;
					todec = fromdec -> nextnew;
					bin_dir = 1;
					continue;
				}
				else
				{
					bin_s = 0;
					bin_t ++;
					bin_b += 8;
					bin_byte = 0;
					continue;
				}
			}
		// case 3: difference beyond "to" branch = continue with nextold
			fromdec = todec;
			todec = fromdec -> nextold;
			bin_dir = 0;
			goto checkagain;
		}
		else
		{
		// case 4: no difference or state not found
			if(bin_d < 8)
			{
				// compute scapegoat
				unsigned int pb,s;
				s = bin_s;	
				Scapegoat = bin_p;
				pb = bin_pb;
				while(1)
				{	
					s -= pb;
					if(s <= bin_d) return NULL;
					Scapegoat--;
					pb = Places[Scapegoat] -> nrbits;
				}
			}
			else
			{
				bin_s = 0;
				bin_t++;
				bin_b += 8;
				bin_byte = 0;
				continue;
			}
			
		}
	}
}

inline State * binSearch()
{
	return binSearch(binHashTable[HASHENTRY]);
}

inline State * binInsert()
{
	return binInsert(binHashTable + (HASHENTRY));

}
void print_path(State *);
	
#endif
