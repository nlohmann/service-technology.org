/*!
 * 	\class petriNetTransition
    \brief class for the transitions of an owfn net

	\author Daniela Weinberg
	\par History
		- 2006-01-16 creation

 */ 

#ifndef OWFNTRANSITION_H_
#define OWFNTRANSITION_H_

#include "mynew.h"
#include "petriNetNode.h"
#include <set>

class oWFN;
class owfnPlace;

class owfnTransition : public Node {
	public:
		owfnTransition(char *);
		~owfnTransition();
	
		bool quasiEnabled;
		bool enabled;
		
		std::set<unsigned int> messageSet;
		
		unsigned int quasiEnabledNr;	//!< number of internal pre-places marked with appropriate tokens
		unsigned int enabledNr;			//!< number of input pre-places marked with appropriate tokens
		
		owfnTransition * NextEnabled;
		owfnTransition * PrevEnabled; 	// double linking in list of enabled transitions
  										// in the sequel, lists are NIL-terminated
		owfnTransition * NextQuasiEnabled;
		owfnTransition * PrevQuasiEnabled; 	// double linking in list of quasi enabled transitions
  											// in the sequel, lists are NIL-terminated								

		owfnPlace* * PrePlaces; // Places to be checked for enabledness
		unsigned int * Pre; // Multiplicity to be checked
		owfnPlace* * IncrPlaces; // Places that are incremented by transition
		unsigned int * Incr; // Amount of increment
		owfnPlace* * DecrPlaces; // Places that are decremented by transition
		unsigned int * Decr; // amount of decrement
		
		owfnTransition ** ImproveEnabling; 	// list of transitions where enabledness
  											//must be checked again after firing this transition
  		owfnTransition ** ImproveDisabling; // list of transitions where disabledness
  											// must be checked again after firing this transition
  		void initialize(oWFN *); // Set above arrays, list, enabled...
  		void fire(oWFN *); // replace current marking by successor marking, force
  								// enabling test where necessary
  		void backfire(oWFN * PN); // fire transition backwards to replace original state,
  										// force enabling tests where necessary
  										
  		void excludeTransitionFromEnabledList(oWFN * );
  		void excludeTransitionFromQuasiEnabledList(oWFN * );
  		
  		void check_enabled(oWFN *); // test if tr is enabled. If necessary, rearrange list
		int hash_change; // change of hash value by firing t;
		void set_hashchange(oWFN *);
		unsigned int lastdisabled; 	// dfsnum of last state where
							  		// some fired transition disables this one
		unsigned int lastfired; 	// dfsnum of last state where this tr. was fired

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(owfnTransition)
#define new NEW_NEW
};

#endif /*OWFNTRANSITION_H_*/
