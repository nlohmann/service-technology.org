/*!
 * 	\class owfnPlace
    \brief class for the places of an owfn net

	\author Daniela Weinberg
	\par History
		- 2006-01-16 creation

 */ 

#ifndef OWFNPLACE_H_
#define OWFNPLACE_H_

#include "petriNetNode.h"
#include "formula.h"

#ifdef LOG_NEW
#include "mynew.h"
#endif

class oWFN;
class formula;

enum placeType {INPUT, OUTPUT, INTERNAL, INITIAL, FINAL};	//< type of place

class owfnPlace : public Node {
	private :
		oWFN * net;			// pointer to underlying petri net (needed for hash value)
		
 	public:
	  	owfnPlace(char *, placeType, oWFN *);
	  	~owfnPlace();

		placeType type;		// type of place (input, output, internal, initial, final)  
		
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
	  	int capacity;   // maximum capacity
	  	int nrbits;     // nr of bits required for storing its marking (= log capacity)
		int startbit;   // first bit representing this place in bit vector
	//	placeType getType();
		unsigned int cardprop; // number of propositions in final condition that mention this place
		formula ** proposition; // array of propositions in final condition that mention this place
					// used for quick re-evaluation of condition
};

#endif /*OWFNPLACE_H_*/
