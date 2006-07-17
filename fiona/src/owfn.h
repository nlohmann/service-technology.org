/*!
 * 	\class oWFN
    \brief container for the elements needed to store an oWFN

	\author Daniela Weinberg
	\par History
		- 2005-12-09 creation

 */ 

#ifndef OWFN_H_
#define OWFN_H_

#include "mynew.h"
#include<fstream>
#include<iosfwd>
#include<iostream>
#include <string>
//#include "stateList.h"
#include "formula.h"
#include "main.h"

#include "reachGraph.h"

using namespace std;

class owfnPlace;
class owfnTransition;
class binDecision;
class State;
class PlSymbol;
class TrSymbol;
class Arc;

class oWFN  {
	private:
		unsigned int NrOfStates;
		unsigned int placeCnt;				//!< number of places
		unsigned int arcCnt;				//!< number of arcs
		unsigned int transCnt;				//!< number of transitions
		unsigned int CardFireList;
		unsigned int CardQuasiFireList;
		
		owfnTransition ** firelist();
		owfnTransition ** quasiFirelist();
		
	public:
		oWFN();
		~oWFN();
		void RemoveGraph();

		owfnPlace ** Places;				//!< array of places
		owfnTransition ** Transitions;		//!< array of transitions
		
		int * inputPlacesArray;
		int * outputPlacesArray;

		unsigned int commDepth;				//!< depth of operating guideline
											// wird in readnet.y gesetzt
		void initializeTransitions();		//!< calls the check_enabled function for all transitions

		unsigned int getPlaceCnt();			//!< number of all places
		unsigned int placeInputCnt;			//!< number of input places
		unsigned int placeOutputCnt;		//!< number of output places

		unsigned int getCommDepth();
		unsigned int getTransitionCnt();
		unsigned int getPlaceHashValue();

		unsigned int getInputPlaceCnt();
		unsigned int getOutputPlaceCnt();
		
		unsigned int getCardFireList();
		
		binDecision ** binHashTable;
		
		unsigned int * CurrentMarking;
		unsigned int * FinalMarking;
		formula * FinalCondition;
		State * currentState;
		
		unsigned int transNrEnabled;			//!< number of really enabled transitions
		unsigned int transNrQuasiEnabled;		//!< number of quasi enabled transitions
		unsigned int placeHashValue;	
		long int BitVectorSize;

		owfnTransition * startOfQuasiEnabledList;	//!< start of list of quasi enabled transitions
		owfnTransition * startOfEnabledList;		//!< start of list of real enabled transitions
		
		void initialize();						// initializes the net

		void addPlace(unsigned int, owfnPlace *);
		void addTransition(unsigned int, owfnTransition *);

		void addStateToList(vertex *, State *, bool);
		void addSuccStatesToList(vertex *, State *);
		
		void addStateToList(vertex *, State *);
		
		State * calculateNextSate();
		
	//	int setCurrentMarkingFromState(reachGraphState *);// extracts the marking of the given state
		unsigned int * copyCurrentMarking();
		void copyMarkingToCurrentMarking(unsigned int * copy);
		void calculateReachableStates(vertex *, bool);
		void calculateReachableStatesFull(vertex *, bool);
		
		int addInputMessage(unsigned int);
		int addInputMessage(messageMultiSet);			// adds input message to the current marking

		int removeOutputMessage(unsigned int);
		int removeOutputMessage(messageMultiSet);
		
		stateType typeOfState();			// returns the type of state (transient, maximal, minimal)
		bool isMaximal();					// returns true if the state is maximal
		bool isMinimal();					// returns true if the state is minimal
		char * printMarking(unsigned int *); // creates the label of the given marking
		char * printCurrentMarkingForDot();  // creates the label of the current marking
		bool isFinalMarking(unsigned int *);	// is the given marking == final marking of the net?
		bool isFinal();	// does current marking satisfy final condition/final marking of the net?

/* print the net */
		void printmarking();
		void printmarking(unsigned int *);
		void printstate(char *, unsigned int *);
		void print_binDec(int);
		void print_binDec(binDecision *, int);
		
		void removeisolated();

		void addArc(Arc *);
		
		void deletePlace(owfnPlace *);
	    void deleteTransition(owfnTransition *);
		
		char * createLabel(messageMultiSet);	

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(oWFN)
#define new NEW_NEW
};

#endif /*OWFN_H_*/
