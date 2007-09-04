/*****************************************************************************
 * Copyright 2005, 2006, 2007 Peter Massuthe, Daniela Weinberg               *
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
#include <map>
#include "formula.h"
#include "main.h"
#include "state.h"
#include "containers.h"
#include "pnapi/pnapi.h"

using namespace std;

class owfnPlace;
class owfnTransition;
class binDecision;
class State;
class PlSymbol;
class TrSymbol;
class Arc;
class Graph;
class GraphNode;
class GraphFormulaAssignment;


class oWFN {
    // first public typedefs, then private members, then public methods
    public:
        //! Type of the container holding all transitions of this oWFN.
        typedef std::vector<owfnTransition*> Transitions_t;

        //! Type of the containers holding all places of this oWFN.
        typedef std::vector<owfnPlace*> Places_t;

    private:
        unsigned int NrOfStates;
        unsigned int arcCnt; //!< number of arcs
        unsigned int CurrentCardFireList;
        unsigned int CurrentCardQuasiFireList;

        //! array of places
        Places_t Places;
        Places_t inputPlaces;
        Places_t outputPlaces;

        //! associates places with their corresponding index in the place set
        std::map<owfnPlace *, Places_t::size_type> PlaceIndices;

        //! associates input places with their corresponding index in the input set
        std::map<owfnPlace *, Places_t::size_type> inputPlaceIndices;

        //! associates output places with their corresponding index in the output set 
        std::map<owfnPlace *, Places_t::size_type> outputPlaceIndices;

        //! the ports
        std::map<std::string, Places_t> ports;

        //! Contains all transitions of this oWFN.
        Transitions_t Transitions;

        owfnTransition** firelist();

        /**
         * helper for matchesWithOG
         *
         * @param currentOGNode current node in the OG, so we know where we are
         * @param currentState current oWFN state
         * @param reasonForFailedMatch In case of a failed match, holds a text
         *     describing why the matching failed.
         *
         * @retval true If this oWFN matches with given OG.
         * @retval false Otherwise.
         */
        bool matchesWithOGRecursive(GraphNode* currentOGNode,
                                    State* currentState,
                                    string& reasonForFailedMatch);

    public:
        oWFN();
        ~oWFN();
        void RemoveGraph();

        string filename;

        owfnTransition ** quasiFirelist();

        binDecision * tempBinDecision; // we want to store some the states calculated temporarily in a new binDecision structure

        setOfMessages inputMessages; //!< activated input messages of current node
        setOfMessages outputMessages; //!< activated output messages of current node

        StateSet setOfStatesTemp; //!< this set contains all states of the newly calculated node
        StateSet visitedStates; //!< in case of state reduction, remember those state that we have visited so far by calculating the new node

        void initializeTransitions(); //!< calls the check_enabled function for all transitions

        //! returns the number of all places of the net
        Places_t::size_type getPlaceCount() const;

        //! Get owfnPlace from index i. Indices start at 0. Get the total number
        //! of owfnPlaces from getPlaceCount().

        owfnPlace* getPlace(Places_t::size_type i) const;

        //! Returns the number of transitions this oWFN has.
        Transitions_t::size_type getTransitionCount() const;

        //! Return transition with index i of this oWFN. Indices start at 0.
        //! Get the total number of transitions by getTransitionCount().
        owfnTransition* getTransition(Transitions_t::size_type i) const;

        unsigned int getPlaceHashValue();

        //! returns the number of input places of the net.
        Places_t::size_type getInputPlaceCount() const;

        //! Get an input owfnPlace from index i. Indices start at 0. Get the
        //! total number of input owfnPlaces from getInputPlaceCount().
        owfnPlace* getInputPlace(Places_t::size_type i) const;

        //! returns the number of output places of the net.
        Places_t::size_type getOutputPlaceCount() const;

        //! Get an output owfnPlace from index i. Indices start at 0. Get the
        //! total number of output owfnPlaces from getOutputPlaceCount().
        owfnPlace* getOutputPlace(Places_t::size_type i) const;

        //! Get the index of Place in the place set
        Places_t::size_type getPlaceIndex(owfnPlace *Place);

        //! Get the index of inputPlace in the input place set
        Places_t::size_type getInputPlaceIndex(owfnPlace *inputPlace);

        //! Get the index of outputPlace in the output place set
        Places_t::size_type getOutputPlaceIndex(owfnPlace *outputPlace);

        unsigned int getCardFireList();

        binDecision ** binHashTable;

        unsigned int * CurrentMarking;
        unsigned int * FinalMarking;
        formula * FinalCondition;
        std::string finalConditionString;
        std::string finalMarkingString;
        State * currentState;

        unsigned int transNrEnabled; //!< number of really enabled transitions
        unsigned int transNrQuasiEnabled; //!< number of quasi enabled transitions
        unsigned int placeHashValue;
        long int BitVectorSize;

        owfnTransition * startOfQuasiEnabledList; //!< start of list of quasi enabled transitions
        owfnTransition * startOfEnabledList; //!< start of list of real enabled transitions

        // **** Definitions for Stubborn set calculations
#ifdef STUBBORN
        owfnTransition * StartOfStubbornList; // anchor to linked list of ...
        owfnTransition * EndOfStubbornList; // ... transitions in stubborn set
        unsigned int NrStubborn; // # of activated (!) elements in stubborn set
        owfnTransition * TarjanStack; // Stubborn Set Calculation involves ...
        owfnTransition * CallStack; // ... SCC investigation on a graph of tr.

        owfnTransition ** stubbornfirelistdeadlocks(); // returns transitions to be fired for

        owfnTransition ** stubbornfirelistmessage(owfnPlace *);
        owfnTransition ** stubbornfirelistmessage(messageMultiSet);
        // returns transitions to be fired for
        // reduced state space for message (or messages)
        // successors
#endif

        void initialize(); // initializes the net

        /**
         * Adds an owfnTransition to this oWFN.
         * @retval true Method succeeded.
         * @retval false Method failed. The given transition was not added,
         *     because the oWFN already contains an owfnTransition with the
         *     same name.
         */
        bool addTransition(owfnTransition* transition);

        void addPlace(owfnPlace*);

        void addSuccStatesToList(GraphNode*, State*);
        void addSuccStatesToListStubborn(StateSet&,
                                         owfnPlace*,
                                         State*,
                                         GraphNode*);
        void addSuccStatesToListStubborn(StateSet&,
                                         messageMultiSet,
                                         State*,
                                         GraphNode*);

        // void addSuccStatesToListOrig(GraphNode *, State *);		

        bool checkMessageBound();

        /**
         * Adds recursively the State s and all its successor states to
         * setOfStatesTemp.
         */
        void addRecursivelySuccStatesToSetOfTempStates(State* s);

        State* calculateNextSate();

        unsigned int* copyCurrentMarking();
        void copyMarkingToCurrentMarking(unsigned int* copy);
        void calculateReachableStatesOutputEvent(GraphNode*);
        void calculateReachableStatesInputEvent(GraphNode*);
        void calculateReachableStates(StateSet&, owfnPlace*, GraphNode*);
        void calculateReachableStates(StateSet&, messageMultiSet, GraphNode*);

        void calculateReachableStatesFull(GraphNode*);

        void addInputMessage(unsigned int);
        void addInputMessage(messageMultiSet); // adds input message to the current marking

        bool removeOutputMessage(unsigned int);
        bool removeOutputMessage(messageMultiSet);

        stateType typeOfState(); // returns the type of state (transient, maximal, minimal)

        /**
         * Returns the label of the given marking, that means the label
         * consists of the names of the places of the net that have tokens
         * (is a multiset => occurance of name == number of tokens used in
         * dotFile creation (CommunicationGraph::printGraphToDot).
         *
         * @param marking The marking to be printed out.
         */
        std::string getMarkingAsString(unsigned int* marking) const;

        /**
         * Returns the label of the CurrentMarking. See
         * getMarkingAsString() for more information.
         */
        std::string getCurrentMarkingAsString() const;

        //		bool isFinalMarking(unsigned int *);	// is the given marking == final marking of the net?
        bool isFinal() const; // does current marking satisfy final condition/final marking of the net?


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
        bool matchesWithOG(const Graph* og, string& reasonForFailedMatch);

        /**
         * Creates an assignment for the given state of the oWFN used in the
         * matching algorithm. In this assignment the labels of transitions
         * leaving the given state are used as propositions and all transitions
         * that are enabled are assigned to true. Furthermore the proposition
         * 'final' is assigned to true iff the given state is a final state.
         * All other propositions are implicetly taken to be false. This
         * implicit behaviour should be (and is) implemented by the class
         * GraphFormulaAssignment.
         *
         * @param currentState The state of this oWFN an assignment should be
         *     created for.
         * @returns The above described assignment for the given state.
         */
        GraphFormulaAssignment makeAssignmentForOGMatchingForState(
                                       const State* currentState) const;

        /// adds a place to a port
        void add_place_to_port(owfnPlace *place, std::string port);

        /// counts the ports
        unsigned int getPortCount() const;

        /// returns the port of the interface place identified by a label
        std::string getPortForLabel(std::string label) const;

        /// returns the port identified by its name
        std::set<std::string> getPort(std::string name) const;

        /// returns a pointer to this petrinet in the PNapi net format
        PNapi::PetriNet* returnPNapiNet();

        // Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
        NEW_OPERATOR(oWFN)
#define new NEW_NEW
};

#endif /*OWFN_H_*/
