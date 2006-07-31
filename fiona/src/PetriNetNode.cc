#include "mynew.h"
#include "petriNetNode.h"

inline void Node::NewArriving(Arc & a) {
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

inline void Node::NewLeaving(Arc & a) {
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

Node::~Node() {
  delete [] name;
  
//  for(int i = 0; i < NrOfArriving - 1; i++) {
//  	if (ArrivingArcs[i]) {
//  		delete ArrivingArcs[i];
//  	}
//  	ArrivingArcs[i] = NULL;
//  }
  delete [] ArrivingArcs;
  
//  for(int i = 0; i < NrOfLeaving - 1; i++) {
//  	if (LeavingArcs[i]) {
//	  	delete LeavingArcs[i];
//  	}
//  	LeavingArcs[i] = NULL;
//  }  
  delete [] LeavingArcs;
}

Node::Node(char * n)
{
  name = new char [strlen(n) + 1];
  strcpy(name, n);
  ArrivingArcs = new Arc * [1];
  LeavingArcs = new Arc *  [1];
}

inline ostream& operator << (ostream & str,Node n)
{
  str << n.name;
  return str;
}

Arc::Arc(owfnTransition * t, owfnPlace * p, bool totrans, unsigned int mult)
{
  tr = t;
  pl = p;
  Source = totrans ? (Node *) p : (Node *) t;
  Destination = totrans ? (Node *) t : (Node *) p;
  Multiplicity = mult;
//	cnt++;
}

inline Node * Arc::Get(bool dest)
{
  return dest? Destination : Source;
}

void Arc::operator += (unsigned int incr)
{
  Multiplicity += incr;
}
