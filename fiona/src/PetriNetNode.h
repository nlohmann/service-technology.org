#ifndef PETRINETNODE_H_
#define PETRINETNODE_H_

#include "mynew.h"
//#include<streambuf.h>
#include<iostream>

using namespace std;

class Arc;

class Node {
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
};

class owfnTransition;
class owfnPlace;

class Arc
{
	public:
		Arc(owfnTransition *, owfnPlace *, bool ,unsigned int);
		
	//	static unsigned int cnt;
		Node * Source;
	  	Node * Destination;
	  	owfnPlace * pl;
	  	owfnTransition * tr;
	  	unsigned int Multiplicity;
	  	Node * Get(bool);
	  	void operator += (unsigned int);

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(Arc)
#define new NEW_NEW
};

#endif /*PETRINETNODE_H_*/
