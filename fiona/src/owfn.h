// *****************************************************************************\
// * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
// *                                                                           *
// * This file is part of Fiona.                                               *
// *                                                                           *
// * Fiona is free software; you can redistribute it and/or modify it          *
// * under the terms of the GNU General Public License as published by the     *
// * Free Software Foundation; either version 2 of the License, or (at your    *
// * option) any later version.                                                *
// *                                                                           *
// * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
// * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
// * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
// * more details.                                                             *
// *                                                                           *
// * You should have received a copy of the GNU General Public License along   *
// * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
// * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
// *****************************************************************************/

/*!
 * \file    owfn.h
 *
 * \brief   general functions for internal Petri net representation
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef OWFN_H_
#define OWFN_H_

#include "mynew.h"
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <string>
#include "formula.h"
#include "main.h"
#include "vertex.h"

#include "communicationGraph.h"

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
		
		
	public:
		oWFN();
		~oWFN();
		void RemoveGraph();

		char * filename;
		
		owfnTransition ** quasiFirelist();

		owfnPlace ** Places;				//!< array of places
		owfnTransition ** Transitions;		//!< array of transitions
		
		owfnPlace * * inputPlacesArray;
		owfnPlace * * outputPlacesArray;
		
		setOfMessages inputMessages;		//!< activated input messages of current node
		setOfMessages outputMessages;		//!< activated output messages of current node

	    StateSet setOfStatesTemp;			//!< this set contains all states of the newly calculated node
		StateSet visitedStates;				//!< in case of state reduction, remember those state that we have visited so far by calculating the new node

//		unsigned int commDepth;				//!< depth of operating guideline
//											// wird in syntax.yy gesetzt

		void initializeTransitions();		//!< calls the check_enabled function for all transitions

		unsigned int getPlaceCnt();			//!< number of all places
		unsigned int placeInputCnt;			//!< number of input places
		unsigned int placeOutputCnt;		//!< number of output places

//		unsigned int getCommDepth();
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

// **** Definitions for Stubborn set calculations
#ifdef STUBBORN
		owfnTransition * StartOfStubbornList;		// anchor to linked list of ...
		owfnTransition * EndOfStubbornList;                 // ... transitions in stubborn set
		unsigned int NrStubborn;			// # of activated (!) elements in stubborn set
		owfnTransition * TarjanStack;                       // Stubborn Set Calculation involves ...
		owfnTransition * CallStack;                         // ... SCC investigation on a graph of tr.

		owfnTransition ** stubbornfirelistdeadlocks();  	// returns transitions to be fired for
								// reduced state space that preserves deadlocks
		owfnTransition ** stubbornfirelistmessage(owfnPlace *);
								// returns transitions to be fired for
								// reduced state space for message
								// successors
#endif

		void initialize();						// initializes the net

		void addPlace(unsigned int, owfnPlace *);
		void addTransition(unsigned int, owfnTransition *);

//		void addStateToList(vertex *, State *, bool);
		void addSuccStatesToList(vertex *, State *);
		void addSuccStatesToListOrig(vertex *, State *);
		
//		void addStateToList(vertex *, State *);
		

		bool checkMessageBound();
		void computeAnnotationOutput(vertex *, State *);
		void computeAnnotationInput(vertex *, State *, unsigned int *, bool);
		
		State * calculateNextSate();
		
		unsigned int * copyCurrentMarking();
		void copyMarkingToCurrentMarking(unsigned int * copy);
		void calculateReachableStatesOutputEvent(vertex *, bool, owfnPlace *);
		void calculateReachableStatesInputEvent(vertex *, bool);
		void calculateReachableStatesFull(vertex *);
		
		void addInputMessage(unsigned int);
		int addInputMessage(messageMultiSet);			// adds input message to the current marking

		bool removeOutputMessage(unsigned int);
		bool removeOutputMessage(messageMultiSet);
		
		stateType typeOfState();			// returns the type of state (transient, maximal, minimal)
		bool isMaximal();					// returns true if the state is maximal
		bool isMinimal();					// returns true if the state is minimal
		char * printMarking(unsigned int *); // creates the label of the given marking
		char * printCurrentMarkingForDot();  // creates the label of the current marking
//		bool isFinalMarking(unsigned int *);	// is the given marking == final marking of the net?
		bool isFinal();	// does current marking satisfy final condition/final marking of the net?

/* print the net */
		void printmarking();
		void printmarking(unsigned int *);
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
