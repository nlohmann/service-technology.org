#include "symm.H"
#include<limits.h>
#include"graph.H"
#include<stdlib.h>
#define NodeType int
#define PL 0
#define TR 1
#define DomType int
#define DO 0
#define CO 1

using std::ofstream;

#ifdef DISTRIBUTE
#include "distribute.h"
#endif

unsigned int * kanrep, * kanrep1;

unsigned int * compose, * reservecompose;

unsigned int kanhash;

 void check();
class Reaktoreintrag 
{
public:
  Node * node;
  unsigned int count; // counts number of hits through arcs, for constraint splitting
  unsigned int stamp; // round of last hit, to avoid reset after every round
  Reaktoreintrag();
};
  
Reaktoreintrag::Reaktoreintrag()
{
  stamp = 0;
  count = 0;
  Node * node;
}

Reaktoreintrag * Reaktor[2][2];

class ToDo // stores constraints that need to be used for class splitting
{
public:
  unsigned int constraint;
  ToDo * next;
  ToDo(NodeType,unsigned int);
};

ToDo * ToDoList[2];

ToDo::ToDo(NodeType TY,unsigned int co)
{
  next = ToDoList[TY];
  constraint = co;
  ToDoList[TY] = this;
}

class Constraint
{
public:
  unsigned int first;
  unsigned int last;
  ToDo * changed;
  unsigned int parent;
};

unsigned int CardSpecification[2];
Constraint * Specification[2];

inline void reportprogress()
{
	if(!((CardSpecification[PL] + CardSpecification[TR]) % REPORTFREQUENCY))
	{
		cerr << "Depth " << CardSpecification[PL] + CardSpecification[TR]
		<< "\n";
	}
}
  
unsigned int DeadBranches;
unsigned int CardGenerators;

void ArcSort(Arc ** list,unsigned int from, unsigned int to)
{
  // sort lists of arriving and leaving arcs according to multiplicity
  // (descending order) ; use quicksort
  unsigned int less,current,greater;
  Arc * swap;
  
  greater = from;
  current = from + 1;
  less = to;

  while(current <= less)
    {
      if(list[current]->Multiplicity > list[current - 1] -> Multiplicity)
	{
	  swap = list[current];
	  list[current++] = list[greater];
	  list[greater++] = swap;
	}
      else
	{
	  if(list[current]->Multiplicity == list[current - 1] -> Multiplicity)
	    {
	      current++;
	    }
	  else
	    {
	      swap = list[current];
	      list[current] = list[less];
	      list[less--] = swap;
	    }
	}
    }
  if(greater - from > 1)
    {
      ArcSort(list,from,greater - 1);
    }
  if(to - less > 1)
    {
      ArcSort(list,less + 1, to);
    }
}

void init_syms()
{
  int i;
  
  ToDoList[PL] = ToDoList[TR] = (ToDo *) 0;
  CardSpecification[PL] = CardSpecification[TR] = 1;
  Specification[PL] = new Constraint [Places[0] -> cnt];
  Specification[TR] = new Constraint[Transitions[0] -> cnt];
  Specification[PL][0].first = Specification[TR][0].first = Specification[PL][0].parent 
    = Specification[TR][0].parent = 0;
  Specification[PL][0].last = Places[0]-> cnt - 1;
  Specification[TR][0].last = Transitions[0]-> cnt - 1;
  Reaktor[PL][DO] = new Reaktoreintrag [Places[0]->cnt];
  Reaktor[PL][CO] = new Reaktoreintrag [Places[0]->cnt];
  Reaktor[TR][DO] = new Reaktoreintrag [Transitions[0]->cnt];
  Reaktor[TR][CO] = new Reaktoreintrag [Transitions[0]->cnt];
  for(i=0;i<Places[0]->cnt;i++)
    {
      if(Places[i]->NrOfLeaving)  ArcSort(Places[i]->LeavingArcs,0,Places[i]->NrOfLeaving - 1);
      if(Places[i]->NrOfArriving) ArcSort(Places[i]->ArrivingArcs,0,Places[i]->NrOfArriving - 1);
    }
  for(i=0;i<Transitions[0]->cnt;i++)
    {
      if(Transitions[i]->NrOfLeaving) ArcSort(Transitions[i]->LeavingArcs,0,Transitions[i]->NrOfLeaving - 1);
      if(Transitions[i]->NrOfArriving) ArcSort(Transitions[i]->ArrivingArcs,0,Transitions[i]->NrOfArriving - 1);
    }
  DeadBranches = CardGenerators = 0;
}

// the next few functions serve as parameters to a general sort procedure
// on nodes.

unsigned int get_target_place(Node * node)
{
  return ((Place *) node) == CheckPlace ? 1 : 0;
}

unsigned int get_target_transition(Node * node)
{
  return ((Transition *) node) == CheckTransition ? 1 : 0;
}

unsigned int get_target_marking(Node * node)
{
  return ((Place *) node) -> target_marking;
}

unsigned int get_marking(Node * node)
{
  return ((Place *) node) -> initial_marking;
}

unsigned int get_card_arcs_in(Node * node)
{
  return node ->  NrOfArriving;
}

unsigned int get_card_arcs_out(Node * node)
{
  return node -> NrOfLeaving;
}

unsigned int this_arc_nr;
unsigned int this_direction;

unsigned int get_arc_mult(Node * node)
{
  return ((this_direction ? node -> ArrivingArcs : node -> LeavingArcs)[this_arc_nr] -> Multiplicity);
}

void PlaceSort(unsigned int from, unsigned int to, unsigned int attribute(Node *))
{
  unsigned int less, current,greater,sw;
  Place * swap;

  less = from;
  current = from + 1;
  greater = to;

  while(current <= greater)
    {
      if(attribute((Node *) Places[current]) < attribute((Node *) Places[current-1]))
	{
	  swap = Places[current]; sw = CurrentMarking[current];
	  Places[current] = Places[less]; CurrentMarking[current++] = CurrentMarking[less];
	  Places[less] = swap;CurrentMarking[less++] = sw;
	}
      else
	{
	  if(attribute((Node *) Places[current]) == attribute((Node *) Places[current-1]))
	    {
	      current++;
	    }
	  else
	    {
	      swap = Places[current]; sw = CurrentMarking[current];
	      Places[current] = Places[greater]; CurrentMarking[current] = CurrentMarking[greater];
	      Places[greater] = swap;CurrentMarking[greater--] = sw;
	    }
	}
    }
  if(less - from > 1)
    {
      PlaceSort(from,less -1, attribute);
    }
  if(to - greater > 1)
    {
      PlaceSort(greater+1,to,attribute);
    }
}


void TransitionSort(unsigned int from, unsigned int to, unsigned int attribute(Node *))
{
  unsigned int less, current,greater;
  Transition * swap;

  less = from;
  current = from + 1;
  greater = to;

  while(current <= greater)
    {
      if(attribute((Node *) Transitions[current]) < attribute((Node *) Transitions[current-1]))
	{
	  swap = Transitions[current];
	  Transitions[current++] = Transitions[less];
	  Transitions[less++] = swap;
	}
      else
	{
	  if(attribute((Node *) Transitions[current]) == attribute((Node *) Transitions[current-1]))
	    {
	      current++;
	    }
	  else
	    {
	      swap = Transitions[current];
	      Transitions[current] = Transitions[greater];
	      Transitions[greater--] = swap;
	    }
	}
    }
  if(less - from > 1)
    {
      TransitionSort(from,less -1, attribute);
    }
  if(to - greater > 1)
    {
      TransitionSort(greater+1,to,attribute);
    }
}

void SplitPlacesInVorReaktor(unsigned int c,unsigned int attribute(Node *))
{
  unsigned int i,firstc,lastc,oldc,newc;

  PlaceSort(Specification[PL][c].first,Specification[PL][c].last,attribute);
  oldc = c;
  firstc = Specification[PL][c].first;
  lastc = Specification[PL][c].last;
  while(attribute((Node *) Places[firstc]) != attribute((Node *) Places[lastc]))
    {
      for(i=firstc+1;attribute((Node *) Places[firstc]) == attribute((Node *) Places[i]);i++);
      newc = CardSpecification[PL];
      Specification[PL][oldc].last = i - 1;
      Specification[PL][newc].first = i;
      Specification[PL][newc].last = lastc;
      Specification[PL][newc].changed = (ToDo *) 0;
      Specification[PL][newc].parent = 0;
      CardSpecification[PL]++;
	  reportprogress();
#ifdef DISTRIBUTE
	progress();
#endif
      firstc = i;
      oldc = newc;
    }
}

      

void SplitTransitionsInVorReaktor(unsigned int c,unsigned int attribute(Node *))
{
  unsigned int i,firstc,lastc,oldc,newc;

  TransitionSort(Specification[TR][c].first,Specification[TR][c].last,attribute);
  oldc = c;
  firstc = Specification[TR][c].first;
  lastc = Specification[TR][c].last;
  while(attribute((Node *) Transitions[firstc]) != attribute((Node *) Transitions[lastc]))
    {
      for(i=firstc+1;attribute((Node *) Transitions[firstc]) == attribute((Node *) Transitions[i]);i++);
      newc = CardSpecification[TR];
      Specification[TR][oldc].last = i - 1;
      Specification[TR][newc].first = i;
      Specification[TR][newc].last = lastc;
      Specification[TR][newc].changed = (ToDo *) 0;
      Specification[TR][newc].parent = 0;
      CardSpecification[TR]++;
	  reportprogress();
#ifdef DISTRIBUTE
	progress();
#endif
      firstc = i;
      oldc = newc;
    }
}

void InitialConstraint()
{
  unsigned int c,b,i,bmax,cmax;

  // split acc. to nr of arriving arcs
  SplitPlacesInVorReaktor(0,get_card_arcs_in);
  SplitTransitionsInVorReaktor(0,get_card_arcs_in);
  cmax = CardSpecification[PL];
  for(c = 0; c < cmax;c++)
    {
      SplitPlacesInVorReaktor(c,get_card_arcs_out);
    }
  cmax = CardSpecification[TR];
  for(c = 0; c < cmax;c++)
    {
      SplitTransitionsInVorReaktor(c,get_card_arcs_out);
    }
  // split according to mult of b-th arc
  this_direction = DO;
  bmax = Places[Places[0]->cnt -1]->NrOfLeaving;
  for(b=0;b<bmax;b++)
    {
      this_arc_nr = b;
      cmax = CardSpecification[PL];
      for(c=0;c < cmax;c++)
	{
	  if(Places[Specification[PL][c].first]->NrOfLeaving > b)
	    {
	      SplitPlacesInVorReaktor(c,get_arc_mult);
	    }
	}
    }
  this_direction = CO;
  bmax = Places[Places[0]->cnt -1]->NrOfArriving;
  for(b=0;b<bmax;b++)
    {
      this_arc_nr = b;
      cmax = CardSpecification[PL];
      for(c=0;c < cmax;c++)
	{
	  if(Places[Specification[PL][c].first]->NrOfArriving > b)
	    {
	      SplitPlacesInVorReaktor(c,get_arc_mult);
	    }
	}
    }

  this_direction = DO;
  bmax = Transitions[Transitions[0]->cnt -1]->NrOfLeaving;
  for(b=0;b<bmax;b++)
    {
      this_arc_nr = b;
      cmax = CardSpecification[TR];
      for(c=0;c < cmax;c++)
	{
	  if(Transitions[Specification[TR][c].first]->NrOfLeaving > b)
	    {
	      SplitTransitionsInVorReaktor(c,get_arc_mult);
	    }
	}
    }
  this_direction = CO;
  bmax = Transitions[Transitions[0]->cnt -1]->NrOfArriving;
  for(b=0;b<bmax;b++)
    {
      this_arc_nr = b;
      cmax = CardSpecification[TR];
      for(c=0;c < cmax;c++)
	{
	  if(Transitions[Specification[TR][c].first]->NrOfArriving > b)
	    {
	      SplitTransitionsInVorReaktor(c,get_arc_mult);
	    }
	}
    }
  // split acc. to initial marking
  cmax = CardSpecification[PL];
  for(c=0;c<cmax;c++)
    {
      SplitPlacesInVorReaktor(c,get_marking);
    }
  // according to target marking, if present
  cmax = CardSpecification[PL];
  for(c=0;c<cmax;c++)
    {
      SplitPlacesInVorReaktor(c,get_target_marking);
    }
  // let target place be a fixed point, if it exists
  cmax = CardSpecification[PL];
  for(c=0;c<cmax;c++)
    {
      SplitPlacesInVorReaktor(c,get_target_place);
    }
  // let target transition be fixed point, if exists
  cmax = CardSpecification[TR];
  for(c=0;c<cmax;c++)
    {
      SplitTransitionsInVorReaktor(c,get_target_transition);
    }
#ifdef CYCLE
	// cycle detection transitions must be completed w.r.t transition
	// equivalence. Doing it here seems to be a good approximation.
	for(c=0;c<CardSpecification[TR];c++)
	{
		for(i=Specification[TR][c].first;i <= Specification[TR][c].last;i++)
		{
			if(Transitions[i]->cyclic)
			{
				for(b=Specification[TR][c].first;b <= Specification[TR][c].last;b++)
				{
					Transitions[b] -> cyclic = true;
				}
				break;
			}	
		}
	}
#endif
}

void FuelleReaktor()
{
  unsigned int i,c;
  
  for(i=0;i< Places[0]->cnt;i++)
    {
      Places[i]->pos[DO] = Places[i]->pos[CO] = i;
    }
  for(i=0;i<Transitions[i]->cnt;i++)
    {
      Transitions[i]-> pos[DO] = Transitions[i]->pos[CO] = i;
    }
  for(c=0;c < CardSpecification[PL];c++)
    {
      for(i=Specification[PL][c].first;i<= Specification[PL][c].last;i++)
	{
	  Reaktor[PL][DO][i].node = Reaktor[PL][CO][i].node = Places[i];
	}
      Specification[PL][c].changed = new ToDo(PL,c);
    }
  for(c=0;c < CardSpecification[TR];c++)
    {
      for(i=Specification[TR][c].first;i<= Specification[TR][c].last;i++)
	{
	  Reaktor[TR][DO][i].node = Reaktor[TR][CO][i].node = Transitions[i];
	}
      Specification[TR][c].changed = new ToDo(TR,c);
    }
}


SymmStore * Store;
unsigned int  CurrentStore;
unsigned int CardStore;

Partition * part;

void UnifyClasses(unsigned int e1,unsigned int e2)
{
  unsigned int c1,c2,c,e;

  for(c1=e1;!(part[c1].top);c1 = part[c1].nextorcard);
  for(c2=e2;!(part[c2].top);c2 = part[c2].nextorcard);
  if(c1 != c2)
    {
      if(part[c1].nextorcard > part[c2].nextorcard)
	{
	  part[c1].nextorcard += part[c2].nextorcard;
	  part[c2].nextorcard = c1;
	  part[c2].top = false;
	  c = c1;
	}
      else
	{
	  part[c2].nextorcard += part[c1].nextorcard;
	  part[c1].nextorcard = c2;
	  part[c1].top = false;
	  c = c2;
	}
    }
  else
    {
      c =c1;
    }
  while(e1 != c)
    {
      e = part[e1].nextorcard;
      part[e1].nextorcard = c;
      e1 = e;
    }
  while(e2 != c)
    {
      e = part[e2].nextorcard;
      part[e2].nextorcard = c;
      e2 = e;
    }
}

unsigned int Stamp;

void NewStamp()
{
  NodeType n;
  DomType d;
  unsigned int i;
  if(Stamp == UINT_MAX)
    {
      for(n=0;n<2;n++)
	for(d=0;d<2;d++)
	  for(i=0;i< (n? Transitions[0]->cnt : Places[0]->cnt);i++)
	    {
	      Reaktor[n][d][i].stamp =0;
	    }
      Stamp = 1;
    }
  else
    {
      Stamp++;
    }
}

void CountSort(NodeType n, DomType d, unsigned int from,unsigned int to)
{
  unsigned int less, current, greater;
  Reaktoreintrag swap;

  less = from;
  current = from + 1;
  greater = to;

  if(from == to)
    {
      if(Reaktor[n][d][from].stamp != Stamp)
	{
	  Reaktor[n][d][from].count = 0;
	}
      return;
    }
  while(current <= greater)
    {
      if(Reaktor[n][d][current].stamp != Stamp)
	{
	  Reaktor[n][d][current].count = 0;
	  if(Reaktor[n][d][current-1].stamp != Stamp)
	    {
	      Reaktor[n][d][current++ -1].count = 0;
	    }
	  else
	    {
	      Reaktor[n][d][less].node->pos[d] = current;
	      Reaktor[n][d][current].node->pos[d] = less;
	      swap = Reaktor[n][d][current];
	      Reaktor[n][d][current++] = Reaktor[n][d][less];
	      Reaktor[n][d][less++] = swap;
	    }
	}
      else
	{
	  if(Reaktor[n][d][current-1].stamp != Stamp)
	    {
	      Reaktor[n][d][current-1].count = 0;
	      Reaktor[n][d][greater].node->pos[d] = current;
	      Reaktor[n][d][current].node -> pos[d] = greater;
	      swap = Reaktor[n][d][current];
	      Reaktor[n][d][current] = Reaktor[n][d][greater];
	      Reaktor[n][d][greater--] = swap;
	      
	    }
	  else
	    {
	      if(Reaktor[n][d][current-1].count < Reaktor[n][d][current].count)
		{
		  Reaktor[n][d][greater].node->pos[d] = current;
		  Reaktor[n][d][current].node -> pos[d] = greater;
		  swap = Reaktor[n][d][current];
		  Reaktor[n][d][current] = Reaktor[n][d][greater];
		  Reaktor[n][d][greater--] = swap;
		}
	      else
		{
		  if(Reaktor[n][d][current-1].count == Reaktor[n][d][current].count)
		    {
		      current++;
		    }
		  else
		    {
		      Reaktor[n][d][less].node->pos[d] = current;
		      Reaktor[n][d][current].node->pos[d] = less;
		      swap = Reaktor[n][d][current];
		      Reaktor[n][d][current++] = Reaktor[n][d][less];
		      Reaktor[n][d][less++] = swap;
		    }
		}
	    }
	}
    }
  if(less - from > 1)
    {
      CountSort(n,d,from,less-1);
    }
  if(to - greater > 1)
    {
      CountSort(n,d,greater+1,to);
    }
}

void ImageSort(Node ** vector, unsigned int from,unsigned int to)
{
  unsigned int less, current, greater;
  Node * swap;

  less = from;
  current = from + 1;
  greater = to;

  while(current <= greater)
    {
		if(CurrentMarking[((Place *) vector[current])->index]< CurrentMarking[((Place *) vector[current-1])->index])
		{
			swap = vector[current];
			vector[current++] = vector[less];
			vector[less++] = swap;
		}
		else
		{
			if(CurrentMarking[((Place *) vector[current])->index] == CurrentMarking[((Place *) vector[current -1])->index])
			{
				current++;
			}
			else
			{
				swap = vector[greater];
				vector[greater --] = vector[current];
				vector[current] = swap;
			}
		}
    }
  if(less - from > 1)
    {
      ImageSort(vector,from,less-1);
    }
  if(to - greater > 1)
    {
      ImageSort(vector,greater+1,to);
    }
}

bool Split(NodeType n, unsigned int c)
{
  unsigned int i,firstc,lastc,oldc,newc;

  oldc = c;
  firstc = Specification[n][c].first;
  lastc = Specification[n][c].last;
  while((Reaktor[n][DO][firstc].count != Reaktor[n][DO][lastc].count)
		||
	(Reaktor[n][CO][firstc].count != Reaktor[n][CO][lastc].count))
    {
      for(i = firstc + 1;(Reaktor[n][DO][firstc].count == Reaktor[n][DO][i].count) 
	    && (Reaktor[n][CO][firstc].count == Reaktor[n][CO][i].count);i++);
      if(Reaktor[n][DO][firstc].count != Reaktor[n][CO][firstc].count)
	{
	  DeadBranches++;
	  return false;
	}
      newc = CardSpecification[n]++;
	  reportprogress();
#ifdef DISTRIBUTE
	progress();
#endif
      Specification[n][oldc].last = i-1;
      Specification[n][newc].first = i;
      Specification[n][newc].last = lastc;
      Specification[n][newc].parent = c;
      firstc = i;
      oldc = newc;
      Specification[n][newc].changed = new ToDo(n,newc);
    }
  if(oldc != c)
    {
      Specification[n][c].changed = new ToDo(n,c);
    }
  else
    {
      if(Reaktor[n][DO][firstc].count != Reaktor[n][CO][firstc].count)
	{
	  DeadBranches++;
	  return false;
	}
    }
  return true;
}
	
void PrintSpec()
{
  unsigned int n,c,i;
  
  for(n=0;n < 2;n++)
    {
      cout << (n ? "Transitions\n" : "Places\n") << Stamp;
      for(c=0;c < CardSpecification[n];c++)
	{
	  cout << c << "(" << Specification[n][c].first << "-" << Specification[n][c].last << ": {";
	  for(i=Specification[n][c].first;i <= Specification[n][c].last;i++)
	    {
	      cout << Reaktor[n][DO][i].node->name;
	      cout << "%";
	      cout << Reaktor[n][DO][i].count ;
	      cout << "&" << Reaktor[n][DO][i].stamp ;
	      cout << "," ;
	    }
	  cout << "} --> {";
	  for(i=Specification[n][c].first;i <= Specification[n][c].last;i++)
	    {
	      cout << Reaktor[n][CO][i].node->name;
	      cout << "/";
	      cout << Reaktor[n][CO][i].count; 
	      cout << "&" << Reaktor[n][CO][i].stamp; 
	      cout << "," ;
	    }
	  cout << "}\n";


	}
    }
}
  
  
bool Refine(NodeType n, unsigned int ref)
{
  unsigned int m1,m2,i,j,arcdir,otherarcdir,currentcardarc,dir,othern,c,cmax;
  Reaktoreintrag * r;
  Arc * a,* aa;

  othern = n ? PL : TR;
  for(arcdir = DO; arcdir < 2; arcdir++)
    {
      otherarcdir = 1 - arcdir;
      currentcardarc = arcdir ? Reaktor[n][DO][Specification[n][ref].first].node->NrOfArriving
	: Reaktor[n][DO][Specification[n][ref].first].node->NrOfLeaving;
      for(j=0;j<currentcardarc;)
	{
	  NewStamp();
	  do
	    {
	      for(i= Specification[n][ref].first; i <= Specification[n][ref].last;i++)
		{
		  for(dir=0;dir< 2;dir++)
		    {
		      r = arcdir ?
			& Reaktor[othern][dir][Reaktor[n][dir][i].node->ArrivingArcs[j]->Source->pos[dir]]
			: 
			& Reaktor[othern][dir][Reaktor[n][dir][i].node->LeavingArcs[j]->Destination->pos[dir]];
		      if(r->stamp == Stamp)
			{
			  r->count++;
			}
		      else
			{
			  r -> stamp = Stamp;
			  r -> count = 1;
			}
		    }
		}
	      m1 =  arcdir ?
		Reaktor[n][DO][Specification[n][ref].first].node->ArrivingArcs[j]-> Multiplicity
		:
		Reaktor[n][DO][Specification[n][ref].first].node->LeavingArcs[j]-> Multiplicity;
	      j++;
	      if(j < currentcardarc)
		{
		  m2 =  arcdir ?
		    Reaktor[n][DO][Specification[n][ref].first].node->ArrivingArcs[j]-> Multiplicity
		    :
		    Reaktor[n][DO][Specification[n][ref].first].node->LeavingArcs[j]-> Multiplicity;
		}
	      else
		{
		  m2 = m1 + 1;
		}
	    }
	  while(m1 == m2);
	  cmax = CardSpecification[othern];
	  for(c=0;c<cmax;c++)
	    {
	      CountSort(othern,DO,Specification[othern][c].first,Specification[othern][c].last);
	      CountSort(othern,CO,Specification[othern][c].first,Specification[othern][c].last);
	      if(!Split(othern,c))
		{
		  return false;
		}
	    }
	}
    }
  return true;
}

bool RefineUntilNothingChanges(NodeType n)
{
  ToDo * tmp;
  
  do
    {
      do
	{
	  if(!Refine(n,ToDoList[n]->constraint))
	    {
	      for(n=PL;n< 2; n++)
		{
		  while(ToDoList[n])
		    {
		      Specification[n][ToDoList[n]->constraint].changed = (ToDo *) 0;
		      tmp = ToDoList[n];
		      ToDoList[n] = ToDoList[n]-> next;
		      delete tmp;
		    }
		}
	      return false;
	    }
	  Specification[n][ToDoList[n]->constraint].changed = (ToDo *) 0;
	  tmp = ToDoList[n];
	  ToDoList[n] = ToDoList[n]-> next;
	  delete tmp;
	}
      while(ToDoList[n]);
      n = 1 - n;
    }
  while(ToDoList[n]);
  return true;
}
		
void ReUnify(unsigned int plpegel, unsigned int trpegel)
{
  unsigned int c;
  NodeType n;
  
  for(c = CardSpecification[PL]-1; c >= plpegel;c--)
    {
      if(Specification[PL][c].first < Specification[PL][Specification[PL][c].parent].first)
	{
	  Specification[PL][Specification[PL][c].parent].first = Specification[PL][c].first;
	}
      if(Specification[PL][c].last > Specification[PL][Specification[PL][c].parent].last)
	{
	  Specification[PL][Specification[PL][c].parent].last = Specification[PL][c].last;
	}
    }
  CardSpecification[PL] = plpegel;
  reportprogress();
#ifdef DISTRIBUTE
	progress();
#endif
  for(c = CardSpecification[TR]-1; c >= trpegel;c--)
    {
      if(Specification[TR][c].first < Specification[TR][Specification[TR][c].parent].first)
	{
	  Specification[TR][Specification[TR][c].parent].first = Specification[TR][c].first;
	}
      if(Specification[TR][c].last > Specification[TR][Specification[TR][c].parent].last)
	{
	  Specification[TR][Specification[TR][c].parent].last = Specification[TR][c].last;
	}
    }
  CardSpecification[TR] = trpegel;
  reportprogress();
#ifdef DISTRIBUTE
	progress();
#endif
}

void StoreSymmetry(unsigned int pos)
{
  unsigned int offset,i,c,*v;
  
  offset = Store[CurrentStore].arg->nr;
  v = Store[CurrentStore].image[pos].vector = new unsigned int [Places[0]->cnt - offset];
  for(c=0;c < CardSpecification[PL];c++)
    {
      if(Reaktor[PL][DO][Specification[PL][c].first].node->nr >= offset)
	{
	  v[Reaktor[PL][DO][Specification[PL][c].first].node->nr - offset] =
	    Reaktor[PL][CO][Specification[PL][c].first].node->nr;
	  UnifyClasses(Reaktor[PL][DO][Specification[PL][c].first].node->nr,Reaktor[PL][CO][Specification[PL][c].first].node->nr);
	}
    }
}

void WriteSymms()
{
	if(Yflg)
	{
		unsigned int etage, raum, x,y;
		for(etage = 0; etage < CardStore; etage++)
		{
			for(raum = 0; raum < Store[etage].card;raum++)
			{
				cout << "GENERATOR # " << etage +1<< "." << raum+1 << "\n";
				for(x = Store[etage].argnr; x < Places[0]->cnt;x++)
				{
					// write cycle of x iff x is smallest el. of its cycle

					// 1. find out whether x is smallest cycle member
					for(y = Store[etage].image[raum].vector[x-Store[etage].argnr]; y > x ; y = Store[etage].image[raum].vector[y-Store[etage].argnr]);
					if((y == x) && (Store[etage].image[raum].vector[x-Store[etage].argnr] != x))
					{
						// print cycle of x
						cout << "(";
						cout << Places[x] -> name;
						for(y = Store[etage].image[raum].vector[x-Store[etage].argnr]; y > x ; y = Store[etage].image[raum].vector[y-Store[etage].argnr])
						{
							cout << " " << Places[y] -> name;
						}
						cout << ")\n";
					}
				}
				cout << "\n";
			}
		}
	}
	if(yflg)
	{
		ofstream symmstream(symmfile);
		if(!symmstream)
		{
			cerr << "Cannot open symmetry output file: " << symmfile
			<< "\nno output written";
			return;
		}
		unsigned int etage, raum, x,y;
		for(etage = 0; etage < CardStore; etage++)
		{
			for(raum = 0; raum < Store[etage].card;raum++)
			{
				symmstream << "GENERATOR # " << etage +1<< "." << raum+1 << "\n";
				for(x = Store[etage].argnr; x < Places[0]->cnt;x++)
				{
					// write cycle of x iff x is smallest el. of its cycle

					// 1. find out whether x is smallest cycle member
					for(y = Store[etage].image[raum].vector[x-Store[etage].argnr]; y > x ; y = Store[etage].image[raum].vector[y-Store[etage].argnr]);
					if((y == x) && (Store[etage].image[raum].vector[x-Store[etage].argnr] != x))
					{
						// print cycle of x
						symmstream << "(";
						symmstream << Places[x] -> name;
						for(y = Store[etage].image[raum].vector[x-Store[etage].argnr]; y > x ; y = Store[etage].image[raum].vector[y-Store[etage].argnr])
						{
							symmstream << " " << Places[y] -> name;
						}
						symmstream << ")\n";
					}
				}
				symmstream << "\n";
			}
		}
	}
}


			
			
void DefineToOther(unsigned int imagepos)
{
  Node ** possibleImages;
  unsigned int cntriv,intriv,i,k,j;
  NodeType type;
  Reaktoreintrag swap;
  DomType dir;
  unsigned int MyCardSpecification[2];

  for(cntriv = 0; Specification[PL][cntriv].first == Specification[PL][cntriv].last;cntriv++);
  possibleImages = new Node * [Specification[PL][cntriv].last - Specification[PL][cntriv].first + 2];
  for(i=Specification[PL][cntriv].first;i<=Specification[PL][cntriv].last;i++)
    {
      possibleImages[i-Specification[PL][cntriv].first] = Reaktor[PL][CO][i].node;
    }
  possibleImages[i-Specification[PL][cntriv].first] = (Node *) 0;
  Specification[PL][CardSpecification[PL]].first = Specification[PL][CardSpecification[PL]].last 
    = Specification[PL][cntriv].first;
  Specification[PL][CardSpecification[PL]].parent = cntriv;
  Specification[PL][CardSpecification[PL]].changed = new ToDo(PL,CardSpecification[PL]);
  CardSpecification[PL]++;
  reportprogress();
#ifdef DISTRIBUTE
	progress();
#endif
  Specification[PL][cntriv].first++;
  Specification[PL][cntriv].changed = new ToDo(PL,cntriv);
  MyCardSpecification[PL] = CardSpecification[PL];
  MyCardSpecification[TR] = CardSpecification[TR];
  for(j=0;possibleImages[j];j++)
    {
      if(RefineUntilNothingChanges(PL))
	{
	  if(CardSpecification[PL]== Places[0]->cnt)
	    {
	      StoreSymmetry(imagepos);
	      break;
	    }
	  else
	    {
	      DefineToOther(imagepos);
	      if(Store[CurrentStore].image[imagepos].vector)
		{
		  break;
		}
	    }
	}
	  if(possibleImages[j+1])
	  {
      ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
      i = possibleImages[j+1]->pos[CO];
      Reaktor[PL][CO][i].node->pos[CO] =
	Specification[PL][CardSpecification[PL]-1].first;
      Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first].node->pos[CO] = i;
      swap = Reaktor[PL][CO][i];
      Reaktor[PL][CO][i] = Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first];
      Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first] = swap;
  Specification[PL][CardSpecification[PL]-1].changed = new ToDo(PL,CardSpecification[PL]);
  Specification[PL][cntriv].changed = new ToDo(PL,cntriv);

	  }
    }
  delete possibleImages;
}

bool found;

void OnlineDefineToOther()
{
  Node ** possibleImages;
  unsigned int cntriv,intriv,i,k,j;
  NodeType type;
  Reaktoreintrag swap;
  DomType dir;
  unsigned int MyCardSpecification[2];

  for(cntriv = 0; Specification[PL][cntriv].first == Specification[PL][cntriv].last;cntriv++);
  possibleImages = new Node * [Specification[PL][cntriv].last - Specification[PL][cntriv].first + 2];
  for(i=Specification[PL][cntriv].first;i<=Specification[PL][cntriv].last;i++)
    {
      possibleImages[i-Specification[PL][cntriv].first] = Reaktor[PL][CO][i].node;
    }
  possibleImages[i-Specification[PL][cntriv].first] = (Node *) 0;
  Specification[PL][CardSpecification[PL]].first = Specification[PL][CardSpecification[PL]].last 
    = Specification[PL][cntriv].first;
  Specification[PL][CardSpecification[PL]].parent = cntriv;
  Specification[PL][CardSpecification[PL]].changed = new ToDo(PL,CardSpecification[PL]);
  CardSpecification[PL]++;
  reportprogress();
#ifdef DISTRIBUTE
	progress();
#endif
  Specification[PL][cntriv].first++;
  Specification[PL][cntriv].changed = new ToDo(PL,cntriv);
  MyCardSpecification[PL] = CardSpecification[PL];
  MyCardSpecification[TR] = CardSpecification[TR];
  for(j=0;possibleImages[j];j++)
    {
      if(RefineUntilNothingChanges(PL))
	{
	  if(CardSpecification[PL]== Places[0]->cnt)
	    {
	      found = true;
	      break;
	    }
	  else
	    {
	      OnlineDefineToOther();
	      if(found)
		{
		  break;
		}
	    }
	}
      ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
	  if(possibleImages[j+1])
	  {
      i = possibleImages[j+1]->pos[CO];
      Reaktor[PL][CO][i].node->pos[CO] =
	Specification[PL][CardSpecification[PL]-1].first;
      Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first].node->pos[CO] = i;
      swap = Reaktor[PL][CO][i];
      Reaktor[PL][CO][i] = Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first];
      Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first] = swap;
  Specification[PL][cntriv].changed = new ToDo(PL,cntriv);
  Specification[PL][CardSpecification[PL]-1].changed = new ToDo(PL,CardSpecification[PL]);
	}
    }
  delete possibleImages;
}

unsigned int Attempt;

void OnlineCanonize()
{
  unsigned int * swaprep;
  Node ** possibleImages;
  unsigned int cn, cntriv,intriv,i,k,j;
  NodeType type;
  Reaktoreintrag swap;
  DomType dir;
  unsigned int MyCardSpecification[2];
  unsigned int SourceIndex;
  unsigned int maxmarking, checkmarking;

  // get non-trivial class
  SourceIndex = CardSpecification[PL] - 1;
  for(cn = 0; cn < CardSpecification[PL]; cn ++)
  {
	if(Specification[PL][cn].first == Specification[PL][cn].last) continue;
	for(i=Specification[PL][cn].first;i<=Specification[PL][cn].last;i++)
	{
		if(Reaktor[PL][DO][i].node -> nr < Reaktor[PL][DO][SourceIndex].node -> nr)
		{
			SourceIndex = i;
			cntriv = cn;
		}
	}
  }
  // get all images
  possibleImages = new Node * [Specification[PL][cntriv].last - Specification[PL][cntriv].first + 2];
  for(i=Specification[PL][cntriv].first;i<=Specification[PL][cntriv].last;i++)
    {
      possibleImages[i-Specification[PL][cntriv].first] = Reaktor[PL][CO][i].node;
    }
  possibleImages[i-Specification[PL][cntriv].first] = (Node *) 0;
  ImageSort(possibleImages,0,Specification[PL][cntriv].last - Specification[PL][cntriv].first );
  maxmarking = CurrentMarking[((Place *) possibleImages[Specification[PL][cntriv].last - Specification[PL][cntriv].first])->index];
  Reaktor[PL][DO][SourceIndex].node -> pos[DO] = Specification[PL][cntriv].first;
  Reaktor[PL][DO][Specification[PL][cntriv].first].node -> pos[DO] = SourceIndex;
  swap = Reaktor[PL][DO][SourceIndex];
  Reaktor[PL][DO][SourceIndex] = Reaktor[PL][DO][Specification[PL][cntriv].first];
  Reaktor[PL][DO][Specification[PL][cntriv].first] = swap;


  i = possibleImages[0]->pos[CO];
  Reaktor[PL][CO][i].node -> pos[CO] = Specification[PL][cntriv].first;
  Reaktor[PL][CO][Specification[PL][cntriv].first].node -> pos[CO] = i;
  swap = Reaktor[PL][CO][i];
  Reaktor[PL][CO][i] = Reaktor[PL][CO][Specification[PL][cntriv].first];
  Reaktor[PL][CO][Specification[PL][cntriv].first] = swap;

  // split new class
  Specification[PL][CardSpecification[PL]].first = Specification[PL][CardSpecification[PL]].last 
    = Specification[PL][cntriv].first;
  Specification[PL][CardSpecification[PL]].parent = cntriv;
  Specification[PL][CardSpecification[PL]].changed = new ToDo(PL,CardSpecification[PL]);
  CardSpecification[PL]++;
  reportprogress();
#ifdef DISTRIBUTE
	progress();
#endif
  Specification[PL][cntriv].first++;
  Specification[PL][cntriv].changed = new ToDo(PL,cntriv);
  MyCardSpecification[PL] = CardSpecification[PL];
  MyCardSpecification[TR] = CardSpecification[TR];
  for(j=0;possibleImages[j];j++)
    {
	  checkmarking = CurrentMarking[((Place *) possibleImages[j])->index];
	  if(checkmarking > maxmarking) break;
	  NewStamp();
      if(RefineUntilNothingChanges(PL))
	{
	  if(CardSpecification[PL]== Places[0]->cnt)
	    {
	      Attempt++;found = true;
		  maxmarking = checkmarking;
		  for(i=0;i<Places[0]->cnt;i++)
		  {
			kanrep1[Reaktor[PL][DO][i].node -> nr] = 
			CurrentMarking[((Place *) Reaktor[PL][CO][i].node)->index];
		  }
		  for(i=0;i<Places[0]->cnt;i++)
		  {
			if(kanrep[i] != kanrep1[i]) break;
		  }
		  if(i < Places[0]->cnt && kanrep1[i] < kanrep[i])
		  {
			swaprep = kanrep;
			kanrep = kanrep1;
			kanrep1 = swaprep;
		  }
			
#ifdef APPROXIMATE_CANONIZATION
				if(Attempt >= MAXATTEMPT) break;
#endif
	    }
	  else
	    {
	      OnlineCanonize();
#ifdef APPROXIMATE_CANONIZATION
	      if(found && Attempt >= MAXATTEMPT)
		{
		  break;
		}
#endif
	    }
	}
      ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
	  if(possibleImages[j+1])
	  {
      i = possibleImages[j+1]->pos[CO];
      Reaktor[PL][CO][i].node->pos[CO] =
	Specification[PL][CardSpecification[PL]-1].first;
      Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first].node->pos[CO] = i;
      swap = Reaktor[PL][CO][i];
      Reaktor[PL][CO][i] = Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first];
      Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first] = swap;
  Specification[PL][cntriv].changed = new ToDo(PL,cntriv);
  Specification[PL][CardSpecification[PL]-1].changed = new ToDo(PL,CardSpecification[PL] - 1);
	}
    }
  delete possibleImages;
      ReUnify(MyCardSpecification[PL] - 1,MyCardSpecification[TR]);
}

void DefineToId(void)
{
      
  unsigned int cntriv,nrmin,intriv,c,i,j,k,MyCardSpecification[2],MyStorePosition,current,composed,val,oldstorenr;
  NodeType type;
  Reaktoreintrag swap;
  DomType dir;
  SymmImage * sigma,* svec;
      
  cntriv = CardSpecification[PL]-1;
  nrmin = UINT_MAX;
  for(c=0;c<CardSpecification[PL];c++)
    {
      if(Specification[PL][c].first != Specification[PL][c].last)
	{
	  for(i=Specification[PL][c].first;i <= Specification[PL][c].last;i++)
	    {
	      if(Reaktor[PL][DO][i].node->nr < nrmin)
		{
		  nrmin = Reaktor[PL][DO][i].node->nr;
		  intriv = i;
		  cntriv = c;
		}
	    }
	}
    }
  CardStore++;
  Store[CardStore-1].image = new SymmImage [Specification[PL][cntriv].last - Specification[PL][cntriv].first + 1];
  oldstorenr = CurrentStore;
  CurrentStore = CardStore - 1;
  Store[CurrentStore].length = Specification[PL][cntriv].last - Specification[PL][cntriv].first;
  for(dir = DO; dir < 2; dir++)
    {
      Reaktor[PL][dir][Specification[PL][cntriv].first].node->pos[dir] = intriv;
      Reaktor[PL][dir][intriv].node->pos[dir] = Specification[PL][cntriv].first;
      swap = Reaktor[PL][dir][Specification[PL][cntriv].first];
      Reaktor[PL][dir][Specification[PL][cntriv].first] = Reaktor[PL][dir][intriv];
      Reaktor[PL][dir][intriv] = swap;
    }
  Specification[PL][CardSpecification[PL]].last 
    = Specification[PL][CardSpecification[PL]].first = Specification[PL][cntriv].first;
  Specification[PL][CardSpecification[PL]].changed = new ToDo(PL,CardSpecification[PL]);  
  Specification[PL][CardSpecification[PL]].parent = cntriv;
  CardSpecification[PL]++;
  reportprogress();
#ifdef DISTRIBUTE
	progress();
#endif
  Store[CurrentStore].arg = Reaktor[PL][DO][Specification[PL][cntriv].first].node;
  Store[CurrentStore].argnr = Reaktor[PL][DO][Specification[PL][cntriv].first].node->nr;
  (Specification[PL][cntriv].first)++;
  Specification[PL][cntriv].changed = new ToDo(PL,cntriv); 
  MyCardSpecification[PL] = CardSpecification[PL];
  MyCardSpecification[TR] = CardSpecification[TR];
  for(j=0;j< Store[CurrentStore].length;j++)
    {
      Store[CurrentStore].image[j].vector = (unsigned int *) 0;
      Store[CurrentStore].image[j].value = Reaktor[PL][CO][j + Specification[PL][cntriv].first].node;
    }
  MyStorePosition = CurrentStore;
  if(!RefineUntilNothingChanges(PL))
    {
      cout << " magic error\n";
    }
  if(CardSpecification[PL] != Places[0]->cnt)
    {
      DefineToId();
    }
  ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
  for(j=0;j < Store[CurrentStore].length;j++)
    {
      sigma = Store[CurrentStore].image + j;
      if(!(sigma->vector))
	{
	  for(i=Specification[PL][cntriv].first;Reaktor[PL][CO][i].node != sigma -> value;i++);
	  Reaktor[PL][CO][i].node->pos[CO] = Specification[PL][CardSpecification[PL]-1].first;
	  Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first].node ->pos[CO] = i;
	  swap = Reaktor[PL][CO][i];
	  Reaktor[PL][CO][i] = Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first];
	  Reaktor[PL][CO][Specification[PL][CardSpecification[PL]-1].first] = swap;
	  Specification[PL][cntriv].changed = new ToDo(PL,cntriv);
	  Specification[PL][MyCardSpecification[PL]-1].changed = new ToDo(PL,MyCardSpecification[PL]-1);
	  if(RefineUntilNothingChanges(PL))
	    {
	      if(CardSpecification[PL] == Places[0]->cnt)
		{
		  StoreSymmetry(j);
		}
	      else
		{
		  DefineToOther(j);
		}
	      if(Store[MyStorePosition].image[j].vector)
		{
		  compose = Store[MyStorePosition].image[j].vector;
		  while(1)
		    {
		      val = compose[Store[MyStorePosition].image[j].value->nr - Store[MyStorePosition].argnr];
		      if(val == Store[MyStorePosition].argnr) break;
		      for(composed = 0;Store[MyStorePosition].image[composed].value->nr != val;composed++);
		      if(!(Store[MyStorePosition].image[composed].vector))
			{
			  Store[MyStorePosition].image[composed].vector = new unsigned int [Places[0]->cnt - 
											   Store[MyStorePosition].argnr +1];
			  for(k=0;k <Places[0]->cnt-Store[MyStorePosition].argnr;k++)
			    {
			      Store[MyStorePosition].image[composed].vector[k] =
				Store[MyStorePosition].image[j].vector[compose[k] - 
								Store[MyStorePosition].argnr];
			    }
			    compose = Store[MyStorePosition].image[composed].vector;
			}
		      else
			{
			  for(k=0;k <Places[0]->cnt-Store[MyStorePosition].argnr;k++)
			    {
			      reservecompose[k] =
				Store[MyStorePosition].image[j].vector[compose[k] - 
								Store[MyStorePosition].argnr];
			    }
			    compose = reservecompose;
			}
		    }
		}
	    }
	  ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
	}
    }
  CurrentStore = oldstorenr;
}

unsigned int * CurrentSymm;

void ComputeSymmetries(void)
{
  unsigned int i,j;
  unsigned long int CardSymm;
  unsigned int h;
  unsigned int plp,trp;

  cout << "\n computing symmetries...\n";
  init_syms();
  reservecompose = new unsigned int [Places[0]->cnt];
  Stamp = 1;
  InitialConstraint();
  FuelleReaktor();
  if(!RefineUntilNothingChanges(PL))
    {
      cout << " Was komisches ist passiert";
    }
  for(i=0;i<Places[0]->cnt;i++)
    {
      Places[i] = (Place *) Reaktor[PL][DO][i].node;
      Places[i]-> nr = Places[i] -> index = i;
      CurrentMarking[i] = Places[i]->initial_marking;
	
    }
  part = new Partition [Places[0]-> cnt];
  for(i=0;i < Places[0]->cnt;i++)
    {
      part[i].nextorcard = 1;
      part[i].top = true;
    }
  Store = new SymmStore [Places[0]->cnt];
  CardStore = 0;
  plp = CardSpecification[PL];
  trp = CardSpecification[TR];
  if(CardSpecification[PL] != Places[0]->cnt) DefineToId();
  ReUnify(plp,trp);
  CardGenerators = 0;
  CardSymm = 1;
  for(i=0;i<CardStore;i++)
    {
      Store[i].card = 0;
      for(j=0;j< Store[i].length;j++)
	{
	  if(Store[i].image[j].vector)
	    {
	      if(j>Store[i].card)
		{
		  Store[i].image[Store[i].card] = Store[i].image[j];
		  
		}
	      Store[i].card++;
	    }
	}
      CardGenerators += Store[i].card;
      CardSymm *= Store[i].card + 1;
      
    }
  for(i=0;i<CardStore;)
    {
      if(Store[i].card)
	{
	  Store[i].reference = new unsigned int[Places[0]->cnt - Store[i].argnr + 1];
	  i++;
	}
      else
	{
	  for(j=i+1;j<CardStore;j++)
	    {
	      Store[j-1] = Store[j];
	    }
	  CardStore--;
	}
    }
  cout << "\n" << CardGenerators << " generators in " << CardStore << " groups for " << CardSymm << " symmetries found.\n";
  cout << DeadBranches << " dead branches entered during calculation.\n";
  
  // Reaktoren auf Markierungsabb.suche vorbereiten, indem Aeq.klassen
  // zu Constraints werden. Countsort wird misbraucht.
  NewStamp();
  for(i=0;i<Places[0]->cnt;i++)
    {
      for(j=i;!part[j].top;j = part[j].nextorcard);
      Reaktor[PL][DO][Places[i]->pos[DO]].stamp = Reaktor[PL][CO][Places[i]->pos[CO]].stamp = Stamp;
      Reaktor[PL][DO][Places[i]->pos[DO]].count = Reaktor[PL][CO][Places[i]->pos[CO]].count = j;
    }
  j = CardSpecification[PL];
  for(i=0;i< j;i++)
    {
      CountSort(PL,DO,Specification[PL][i].first,Specification[PL][i].last);
      CountSort(PL,CO,Specification[PL][i].first,Specification[PL][i].last);
      Split(PL,i);
    }
  if(ToDoList[PL])
     {
       RefineUntilNothingChanges(PL);
     }
  // Hashfaktoren eintragen
#if SYMMINTEGRATION < 3
  for(i=0;i<Places[i]->cnt;i++)
    {
      if(part[i].top) 
	{
	  part[i].nextorcard = rand();
	}
    }
  for(i=0;i<Places[0]->cnt;i++)
    {
      for(j=i;!part[j].top;j = part[j].nextorcard);
      Places[i]->set_hash(part[j].nextorcard);
    }
  CurrentSymm = new unsigned int [Places[0]->cnt];
#else
  kanrep = new unsigned int [Places[0]->cnt];
  kanrep1 = new unsigned int [Places[0]->cnt];
#endif
  WriteSymms();
}
  
void ComputePartition(void)
{
  unsigned int i,j;
  unsigned long int CardSymm;
  unsigned int h;
  unsigned int c;
  unsigned int plp,trp;

  cout << "\n partitioning nodes wrt symmetries...\n";
  init_syms();
  Stamp = 1;
  InitialConstraint();
  FuelleReaktor();
  if(!RefineUntilNothingChanges(PL))
    {
      cout << " Was komisches ist passiert";
    }
  for(i=0;i<Places[0]->cnt;i++)
    {
      Places[i] = (Place *) Reaktor[PL][DO][i].node;
      Places[i]-> nr = Places[i] -> index = i;
      CurrentMarking[i] = Places[i]->initial_marking;
    }
  // partition places
  part = new Partition [Places[0]-> cnt];
  for(i=0;i < Places[0]->cnt;i++)
    {
      part[i].nextorcard = 1;
      part[i].top = true;
    }
	for(c=0;c< CardSpecification[PL];c++)
	{
		for(i=Specification[PL][c].first;i <= Specification[PL][c].last;i++)
		{
			UnifyClasses(Reaktor[PL][DO][i].node -> nr,Reaktor[PL][DO][Specification[PL][c].first].node->nr);
		}
	}

  cout << "\n" << CardSpecification[PL] << " classes computed.\n";
  
  // Reaktoren auf Markierungsabb.suche vorbereiten, indem Aeq.klassen
  // zu Constraints werden. Countsort wird misbraucht.
  NewStamp();
  for(i=0;i<Places[0]->cnt;i++)
    {
      for(j=i;!part[j].top;j = part[j].nextorcard);
      Reaktor[PL][DO][Places[i]->pos[DO]].stamp = Reaktor[PL][CO][Places[i]->pos[CO]].stamp = Stamp;
      Reaktor[PL][DO][Places[i]->pos[DO]].count = Reaktor[PL][CO][Places[i]->pos[CO]].count = j;
    }
  j = CardSpecification[PL];
  for(i=0;i< j;i++)
    {
      CountSort(PL,DO,Specification[PL][i].first,Specification[PL][i].last);
      CountSort(PL,CO,Specification[PL][i].first,Specification[PL][i].last);
      Split(PL,i);
    }
  if(ToDoList[PL])
     {
       RefineUntilNothingChanges(PL);
     }
#if SYMMINTERGATION < 3
  // Hashfaktoren eintragen
  for(i=0;i<Places[i]->cnt;i++)
    {
      if(part[i].top) 
	{
	  part[i].nextorcard = rand();
	}
    }
  for(i=0;i<Places[0]->cnt;i++)
    {
      for(j=i;!part[j].top;j = part[j].nextorcard);
      Places[i]->set_hash(part[j].nextorcard);
    }
#else
  kanrep = new unsigned int [Places[0]->cnt];
  kanrep1 = new unsigned int [Places[0]->cnt];
#endif
}
  
  
void FirstSymm()

{
  unsigned int i,j;

  for(i=0;i< CardStore;i++)
    {
      Store[i].current = Store[i].card;
      for(j=Store[i].argnr;j < Places[i]->cnt;j++)
	{
	  Store[i].reference[j-Store[i].argnr] = j;
	}
    }
  for(i=0;i< Places[0]->cnt;i++)
    {
      CurrentSymm[i] = i;
    }
}

unsigned int NextSymm(unsigned int scg)
{
  //identify next symm where at least one value between 0 and scg 
  // could have changed 
  // return the smallest value where something has changed
  // return value > #PL --> no more symm.


  int i,j,l,m,r;
  SymmStore * sigma;
  
  // search the largest argument less or equal to scg
  
  if(Store[0].argnr > scg) 
    {
      return Places[0]->cnt + 27;
    }
  l = 0;
  if((Store[r = CardStore-1].argnr) <= scg)
    {
      m = r;
    }
  else
    {
      m = l;
      while((r - l) > 1)
	{
	  m = l + (scg - Store[l].argnr) * (r - l) / (Store[r].argnr - Store[l].argnr);
	  if(Store[m].argnr == scg) break;
	  if(m == l) m++;
	  if(m == r) m--;
	  if(Store[m].argnr == scg) break;
	  if(Store[m].argnr < scg) 
	{
		if(Store[m+1].argnr >scg) break;
	else
	    {
	      l = m;
	    }
	}
	  else
	    {
	      r = m;
	    }
	}
    }
  while(!(Store[m].current))
    {
      if(m == 0) return Places[0]->cnt + 27;
      Store[m].current = Store[m--].card;
    };
  sigma = Store+m;
  sigma ->current--;
  for(i=sigma->argnr;i< Places[0]->cnt;i++)
    {
      CurrentSymm[i] = sigma->reference[sigma->image[sigma->current].vector[i-sigma->argnr]-sigma->argnr];
    }
  for(i = m+1; i < CardStore;i++)
    {
      for(j = Store[i].argnr;j < Places[0]->cnt;j++)
	{
	  Store[i].reference[j - Store[i].argnr] = CurrentSymm[j];
	}
    }
  return m;
}

void AllSyms()
{
unsigned int i;
	FirstSymm();
	i=1;
	while(NextSymm(Places[0]->cnt) <= Places[0]->cnt) i++;
	cout << i << "\n";
}
#include"graph.H"

void check()
{
  unsigned int i,j,k;
  for(i=0;i<Places[0]->cnt;i++)
    {
      if(Places[i] != Reaktor[PL][DO][Places[i]->pos[DO]].node)
	{
	  cout << "aua\n";
	}
      if(Places[i] != Reaktor[PL][CO][Places[i]->pos[CO]].node)
	{
	  cout << "weia\n";
	}
    }
  
  for(i=0;i<Places[0]->cnt;i++)
    {
      if(!Reaktor[PL][DO][i].node) cout << "aah\n";
      if(!Reaktor[PL][CO][i].node) cout << "uuh\n";
    }
  for(i=0;i<Transitions[0]->cnt;i++)
    {
      if(Transitions[i] != Reaktor[TR][DO][Transitions[i]->pos[DO]].node)
	{
	  cout << "aua\n";
	}
      if(Transitions[i] != Reaktor[TR][CO][Transitions[i]->pos[CO]].node)
	{
	  cout << "weia\n";
	}
    }
  
  for(i=0;i<Transitions[0]->cnt;i++)
    {
      if(!Reaktor[TR][DO][i].node) cout << "aah\n";
      if(!Reaktor[TR][CO][i].node) cout << "uuh\n";
    }
  if(CardSpecification[PL] > Places[0]->cnt)
    {
      cout << "sclimm\n";
    }
  if(CardSpecification[TR] > Transitions[0]->cnt)
    {
      cout << "auch sclimm\n";
    }
  for(i=0;i < CardSpecification[PL];i++)
    {
      if(Specification[PL][i].first > Specification[PL][i].last)
	{
	  cout << "boese\n";
	}
    }
  for(i=0;i < CardSpecification[TR];i++)
    {
      if(Specification[TR][i].first > Specification[TR][i].last)
	{
	  cout << "auch boese\n";
	}
    }
  for(i=0;i < Places[0]->cnt;i++)
    {
      k = 0;
      for(j=0;j < CardSpecification[PL];j++)
	{
	  if((Specification[PL][j].first <= i)  && (i <= Specification[PL][j].last))
	    {
	      k++;
	    }
	}
      if(k != 1)
	{
	  cout << "unfassbar\n";
	}
    }
  for(i=0;i < Transitions[0]->cnt;i++)
    {
      k = 0;
      for(j=0;j < CardSpecification[TR];j++)
	{
	  if((Specification[TR][j].first <= i)  && (i <= Specification[TR][j].last))
	    {
	      k++;
	    }
	}
      if(k != 1)
	{
	  cout << "auch unfassbar\n";
	}
    }
}
		
State * symm_search2(Decision * d)
{
#if defined(SYMMETRY) && SYMMINTEGRATION == 2
  unsigned int i,k;
  State * s;
  unsigned int MyCardSpecification[2];
  bool spl;
  ToDo * tmp;
  MyCardSpecification[PL] = CardSpecification[PL];
  MyCardSpecification[TR] = CardSpecification[TR];
  for(i=0;i< d -> size;i++)
    {
      if(d -> next[i])
	{
	  if(s = symm_search2(d->next[i])) 
		{
			return s;
		}
	}
      else
	{
	  Statevector * v;
	  // try to calculate a symmetry from current to this state
	  NewStamp();
	  v = d -> vector[i];
	  for(k=Places[0]->cnt -1;;k--)
	    {
	      Reaktor[PL][DO][k].count = CurrentMarking[((Place *) Reaktor[PL][DO][k].node)->index];
	      Reaktor[PL][DO][k].stamp = Reaktor[PL][CO][k].stamp = Stamp;
	      Reaktor[PL][CO][Places[k]->pos[CO]].count = (*v)[v->length + k - Places[0]->cnt];
	      if(v->length + k == Places[0]->cnt )
		{
		  v = v -> prev;
		}
	      if(k==0)
		{
		  break;
		}
	    }
	  // Jetzt sortieren, splitten, Symmetrie suchen, reunifizieren, zurueckkehren
	  spl = true;
	  for(k=0;k< MyCardSpecification[PL];k++)
	    {
	      CountSort(PL,CO,Specification[PL][k].first,Specification[PL][k].last);
	      if(!Split(PL,k))
		{
		  spl = false;
		  while(ToDoList[PL])
		    {
		      tmp = ToDoList[PL];
		      ToDoList[PL] = ToDoList[PL] -> next;
		      delete tmp;
		    }
		  break;
		}

	    }
	  if(!spl)
	    {
	      ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
	      continue;
	    }
	  if(ToDoList[PL])
	    {
	      if(!RefineUntilNothingChanges(PL))
		{
		  ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
		  continue;
		}
	    }
	  if(CardSpecification[PL] == Places[0]->cnt)
	    {
	      ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
#if defined(TARJAN) || defined(COVER) || defined(MODELCHECKING)
	      return d -> vector[i]->state;
#else
		return CurrentState; // Hauptsache, was anderes als Null
#endif
	    }
	  OnlineDefineToOther();
	  ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
	  if(found)
	{
#if defined(TARJAN) || defined(COVER) || defined(MODELCHECKING)
	      return d -> vector[i]->state;
#else
		return CurrentState; // Hauptsache, was anderes als Null
#endif
	}
	}
    }
  ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
  return (State *) 0;
#endif
}
  
State * symm_search_marking2()
{
  int i;
  
  
  State * s;
  Decision * ld;
  Statevector * lv;
  unsigned int lc,li;
  
  if(s = search_marking())
    {
      return s;
    }
  ld = LastDecision;
  lv = LastVector;
  li = Scapegoat;
  lc = LastChoice;
  if(HashTable[Places[0]->hash_value])
    {
      int i;
  
      NewStamp();
      for(i=0;i<Places[0]->cnt;i++)
	{
	  Reaktor[PL][DO][i].count = CurrentMarking[((Place *) Reaktor[PL][DO][i].node)->index];
	  Reaktor[PL][DO][i].stamp = Stamp;
	}
      for(i=0;i < CardSpecification[PL];i++)
	{
	  CountSort(PL,DO,Specification[PL][i].first,Specification[PL][i].last);
	}
      LastDecision = ld;
      LastVector = lv;
      Scapegoat = li;
      LastChoice = lc;
      return symm_search2(HashTable[Places[0]->hash_value]);
    }
  LastDecision = ld;
  LastVector = lv;
  Scapegoat = li;
  LastChoice = lc;
  return (State *) 0;
}

		
State * bin_symm_search2(binDecision * d)
{
#if defined(SYMMETRY) && SYMMINTEGRATION == 2
  unsigned int pm, p,pb,t,k,byte,s;
  unsigned  char * v;
  State * st;
  unsigned int MyCardSpecification[2];
  bool spl;
  ToDo * tmp;
  MyCardSpecification[PL] = CardSpecification[PL];
  MyCardSpecification[TR] = CardSpecification[TR];
  if(d -> nextold)
  {
    if(st = bin_symm_search2(d->nextold)) 
  	{
  		return st;
  	}
  }
  if(d -> nextnew)
  {
    if(st = bin_symm_search2(d->nextnew)) 
  	{
  		return st;
  	}
  }
	  NewStamp();
#if defined(TARJAN) || defined(COVER) || defined(MODELCHECKING)
	      st = d -> state;
#endif
	
	  // try to calculate a symmetry from current to this state
            p = Places[0] -> cnt - 1;
	    v = d -> vector;
	    t = (BitVectorSize - (d -> bitnr+1)) / 8;
	    s = (BitVectorSize - (d -> bitnr+1)) % 8;
            pb = Places[p] -> nrbits;
	    byte = v[t] >> (8 - s);
	    pm = 0;

	    while(1)
	    {
              
		// compute marking pm of place p corr. to decision entry d
		if(pb > s)
		{
			// place has more bits left than remaining in current byte
			if(s)
			{
				// at least, there _are_ bits left in current byte
				pm += byte << (Places[p]->nrbits - pb);
				pb -= s;
				s = 0;
			}
			else
			{
				// current byte empty
				if(t)
				{
					// there is another byte in current vector
					t--;
					s = 8;
					byte = v[t];
				}
				else
				{
					// proceed to previous vector
					int oldbitnr = d -> bitnr;
					d = d -> prev;
					v = d -> vector;
					t = (oldbitnr - (d -> bitnr)) / 8;
					s = (oldbitnr - (d -> bitnr)) % 8;
					/// pay attn to implicit bit!
					if(s==0)
					{
						s = 8;
						t--;
					}
					byte = v[t] >> (8-s) ;
					byte = byte ^ 1;
				}
			}
		}
		else
		{
			// all bits left for this place are in current byte
			pm += (byte % (1 << pb)) << (Places[p]->nrbits - pb);
			byte = byte >> pb;
			s -= pb;
		
		      Reaktor[PL][DO][Places[p]->pos[DO]].count = MARKINGVECTOR[p];
		      Reaktor[PL][DO][p].stamp =  Reaktor[PL][CO][p].stamp = Stamp;
		      Reaktor[PL][CO][Places[p]->pos[CO]].count = pm;
		      if(p==0)
			{
			  break;
			}
			p--;
			pb = Places[p]->nrbits;
			pm = 0;
		}
	    }
	  // Jetzt sortieren, splitten, Symmetrie suchen, reunifizieren, zurueckkehren
	  spl = true;
	  for(k=0;k< MyCardSpecification[PL];k++)
	    {
//	      CountSort(PL,DO,Specification[PL][k].first,Specification[PL][k].last);
	      CountSort(PL,CO,Specification[PL][k].first,Specification[PL][k].last);
		}
	  for(k=0;k< CardSpecification[PL];k++)
	    {
	      if(!Split(PL,k))
		{
		  spl = false;
		  while(ToDoList[PL])
		    {
		      tmp = ToDoList[PL];
		      ToDoList[PL] = ToDoList[PL] -> next;
		      delete tmp;
		    }
		  break;
		}

	    }
	  if(!spl)
	    {
	      ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
	      return (State *) 0;
	    }
	  if(ToDoList[PL])
	    {
	      if(!RefineUntilNothingChanges(PL))
		{
		  ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
		  return (State *) 0;
		}
	    }
	  if(CardSpecification[PL] == Places[0]->cnt)
	    {
	      ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
#if defined(TARJAN) || defined(COVER) || defined(MODELCHECKING)
	      return st;
#else
		return CurrentState; // Hauptsache, was anderes als Null
#endif
	    }
	  OnlineDefineToOther();
	  ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
	  if(found)
	{
#if defined(TARJAN) || defined(COVER) || defined(MODELCHECKING)
	      return st;
#else
		return CurrentState; // Hauptsache, was anderes als Null
#endif
	}
  ReUnify(MyCardSpecification[PL],MyCardSpecification[TR]);
  return (State *) 0;
#endif
}
  
State * bin_symm_search_marking2()
{
  int i;
  
  
  State * s;
  //Decision * ld;
  //Statevector * lv;
  //unsigned int lc,li;
  
  if(s = binSearch())
    {
      return s;
    }
  //ld = LastDecision;
  //lv = LastVector;
  //li = Scapegoat;
  //lc = LastChoice;
  if(binHashTable[Places[0]->hash_value])
    {
      int i;
  
      NewStamp();
      for(i=0;i<Places[0]->cnt;i++)
	{
	  Reaktor[PL][DO][i].count = CurrentMarking[((Place *) Reaktor[PL][DO][i].node)->index];
	  Reaktor[PL][DO][i].stamp = Stamp;
	}
      for(i=0;i < CardSpecification[PL];i++)
	{
	  CountSort(PL,DO,Specification[PL][i].first,Specification[PL][i].last);
	}
  //    LastDecision = ld;
   //   LastVector = lv;
    //  Scapegoat = li;
     // LastChoice = lc;
      return bin_symm_search2(binHashTable[Places[0]->hash_value]);
    }
  //LastDecision = ld;
  //LastVector = lv;
  //Scapegoat = li;
  //LastChoice = lc;
  return (State *) 0;
}

void canonize_on_the_fly()
{
	unsigned int i;
	// return canonical_representitive of current marking as int vector.
	// use initialized Reaktor (i.e. rely on RefineUntilNothingChanges on initial
	// partition). 
	Attempt = 0;
	found = false;
	for(i=0;i<Places[0]->cnt;i++)
	{
		kanrep[i] = CurrentMarking[i];
	}
	if(CardSpecification[PL] >= Places[0] -> cnt) 
	{
		kanhash = Places[0]->hash_value;
		return;
	}
		
	OnlineCanonize();
	kanhash = 0;
	for(i=0;i<Places[0]->cnt;i++)
	{
		kanhash += Places[i] -> hash_factor * kanrep[i];
        kanhash %= HASHSIZE;
	}
}

State * canonical_representitive_on_the_fly()
{
	canonize_on_the_fly();
	return binSearch();
}

