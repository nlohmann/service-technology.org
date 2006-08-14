#ifndef BDDLABEL_H_
#define BDDLABEL_H_

#include "mynew.h" 

class BddLabelTab;

class BddLabel{
public:
  char * name;	//name of Label
  unsigned int nbr;		//number of Label
  BddLabel * next;
  BddLabel(char * c, int n, BddLabelTab * table);
  ~BddLabel();

// Provides user defined operator new. Needed to trace all new
// operations on this class.
#undef new
        NEW_OPERATOR(BddLabel)
#define new NEW_NEW  
  
};


class BddLabelTab{
	public:
	BddLabel ** table;
	BddLabel * lookup(char *);
	void add(BddLabel *);
	BddLabelTab(unsigned int size = 65536);
  	~BddLabelTab();
	unsigned int card;
	unsigned int size;

// Provides user defined operator new. Needed to trace all new
// operations on this class.
#undef new
        NEW_OPERATOR(BddLabelTab)
#define new NEW_NEW	
};

#endif /*BDDLABEL_H_*/
