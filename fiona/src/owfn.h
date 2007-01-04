/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg                     *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

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
class OGFromFile;
class OGFromFileFormulaAssignment;

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

		binDecision * tempBinDecision;   // we want to store some the states calculated temporarily in a new binDecision structure
		
		setOfMessages inputMessages;		//!< activated input messages of current node
		setOfMessages outputMessages;		//!< activated output messages of current node

	    StateSet setOfStatesTemp;			//!< this set contains all states of the newly calculated node
		StateSet visitedStates;				//!< in case of state reduction, remember those state that we have visited so far by calculating the new node

//		unsigned int commDepth;				//!< depth of operating guideline
//											// wird in syntax.yy gesetzt

		void initializeTransitions();		//!< calls the check_enabled function for all transitions

		unsigned int getPlaceCnt() const;			//!< number of all places
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
		owfnTransition ** stubbornfirelistmessage(messageMultiSet);
								// returns transitions to be fired for
								// reduced state space for message (or messages)
								// successors
#endif

		void initialize();						// initializes the net

		void addPlace(unsigned int, owfnPlace *);
		void addTransition(unsigned int, owfnTransition *);

//		void addStateToList(vertex *, State *, bool);
		void addSuccStatesToList(vertex *, State *);
		
		void addSuccStatesToListStubborn(StateSet &, owfnPlace * , State *, vertex *);
		void addSuccStatesToListStubborn(StateSet &, messageMultiSet, State *, vertex *);
		
		void addSuccStatesToListOrig(vertex *, State *);
		
//		void addStateToList(vertex *, State *);
		

		bool checkMessageBound();
		void computeAnnotationOutput(vertex *, State *);
		void computeAnnotationInput(vertex *, State *, unsigned int *, bool);
		
		State * calculateNextSate();
		
		unsigned int * copyCurrentMarking();
		void copyMarkingToCurrentMarking(unsigned int * copy);
		void calculateReachableStatesOutputEvent(vertex *);
		void calculateReachableStatesInputEvent(vertex *, bool);
		void calculateReachableStates(StateSet&, owfnPlace *, vertex *);
		void calculateReachableStates(StateSet&, messageMultiSet, vertex *); 
		
		void calculateReachableStatesFull(vertex *);
		
		void addInputMessage(unsigned int);
		int addInputMessage(messageMultiSet);			// adds input message to the current marking

		bool removeOutputMessage(unsigned int);
		bool removeOutputMessage(messageMultiSet);
		
		stateType typeOfState();			// returns the type of state (transient, maximal, minimal)
		bool isMaximal();					// returns true if the state is maximal
		bool isMinimal();					// returns true if the state is minimal
		/**
		 * Returns the label of the given marking, that means the label
		 * consists of the names of the places of the net that have tokens
		 * (is a multiset => occurance of name == number of tokens used in
		 * dotFile creation (communicationGraph::printGraphToDot).
		 *
		 * @param marking The marking to be printed out.
		 */
		std::string getMarkingAsString(unsigned int *) const;

		/**
		 * Returns the label of the CurrentMarking. See
		 * getMarkingAsString() for more information.
		 */
		std::string getCurrentMarkingAsString() const;

//		bool isFinalMarking(unsigned int *);	// is the given marking == final marking of the net?
		bool isFinal() const;	// does current marking satisfy final condition/final marking of the net?

/* print the net */

		/** Prints the CurrentMarking. */
		void printCurrentMarking() const;

		/**
		 * Prints the given marking.
		 *
		 * @param marking Marking to be printed.
		 */
		void printMarking(unsigned int * marking) const;

		void print_binDec(int);
		void print_binDec(binDecision *, int);
		
		void removeisolated();

		void addArc(Arc *);
		
		void deletePlace(owfnPlace *);
	    void deleteTransition(owfnTransition *);
		
		std::string createLabel(messageMultiSet) const;

		/**
		 * Matches this oWFN with the given operating guideline (OG).
		 *
		 * @param og Operating guideline against this oWFN should be matched.
		 * @param reasonForFailedMatch In case of a failed match, holds a text
		 *     describing why the matching failed.
		 *
		 * @retval true If this oWFN matches with given OG.
		 * @retval false Otherwise.
		 */
		bool matchesWithOG(
			const OGFromFile& og,
			std::string& reasonForFailedMatch
		);

		/**
		 * Creates an assignment for the given state of the oWFN used in the
		 * matching algorithm. In this assignment the labels of transitions
		 * leaving the given state are used as propositions and all transitions
		 * that are enabled are assigned to true. Furthermore the proposition
		 * 'final' is assigned to true iff the given state is a final state.
		 * All other propositions are implicetly taken to be false. This
		 * implicit behaviour should be (and is) implemented by the class
		 * OGFromFileFormulaAssignment.
		 *
		 * @param currentState The state of this oWFN an assignment should be
		 *     created for.
		 * @returns The above described assignment for the given state.
		 */
		OGFromFileFormulaAssignment makeAssignmentForOGMatchingForState(
			const State* currentState
		) const;

        // Provides user defined operator new. Needed to trace all new
        // operations on this class.
#undef new
        NEW_OPERATOR(oWFN)
#define new NEW_NEW
};

#endif /*OWFN_H_*/
