/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    owfn.h
 *
 * \brief   general functions for internal Petri net representation
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
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
#include "pnapi.h"
#include "SinglyLinkedList.h"

using namespace std;

class owfnPlace;
class owfnTransition;
class binDecision;
class State;
class PlSymbol;
class Arc;
class AnnotatedGraph;
class AnnotatedGraphNode;
class GraphFormulaAssignment;
#ifdef CHECKCAPACITY
class CapacityException;
#endif


class oWFN {
    // first public typedefs, then private members, then public methods
    public:
        /// Type of the container holding all transitions of this oWFN.
        typedef std::vector<owfnTransition*> Transitions_t;

        /// Type of the container holding all enabled transitions of this oWFN.
        typedef SList<owfnTransition*> EnabledTransitions;

        /// Type of the container holding all quasi enabled transitions of this
        /// oWFN.
        typedef SList<owfnTransition*> QuasiEnabledTransitions;

        /// Type of the containers holding all places of this oWFN.
        typedef std::vector<owfnPlace*> Places_t;
        
        /// Type needed for matching -> remember which state and node we have checked so far
        typedef std::map<State*, std::set<unsigned int> > StateNodesAssoc_t;

    private:
        unsigned int CurrentCardFireList;
        unsigned int CurrentCardQuasiFireList;

        State * TarStack;		  /// stack of states as (T)SCCs are calculated	
        unsigned int MinBookmark; /// lowlink number of the first closed marking
                                  /// in the currently or last recently processed TSCC
        
        /// array of places
        Places_t Places;
        Places_t inputPlaces;
        Places_t outputPlaces;

        /// associates places with their corresponding index in the place set
        std::map<owfnPlace *, Places_t::size_type> PlaceIndices;

        /// associates input places with their corresponding index in the input set
        std::map<owfnPlace *, Places_t::size_type> inputPlaceIndices;

        /// associates output places with their corresponding index in the output set 
        std::map<owfnPlace *, Places_t::size_type> outputPlaceIndices;

        /// the ports
        std::map<std::string, Places_t> ports;

        /// Contains all transitions of this oWFN.
        Transitions_t Transitions;

        /// returns an array of transition pointers containing all enabled transition
        owfnTransition** firelist();

        /// helper for matchesWithOG
        bool matchesWithOGRecursive(AnnotatedGraphNode* currentOGNode,
                                    State* currentState,
                                    string& reasonForFailedMatch,
                                    StateNodesAssoc_t& stateNodesAssoc);

        /// returns the port of a given interface place
        std::string get_port_from_place(const owfnPlace *place) const;
    
        /// detaches an interface place from the oWFN
        void detachInterfacePlace(owfnPlace *p);
    
    public:
        /// basic constructor
        oWFN();
        
        /// basic deconstructor
        ~oWFN();

        string filename;

        /// returns an array of transition pointers containing all quasi enabled transition
        owfnTransition ** quasiFirelist();
        
        /// activated input messages of current node
        setOfMessages inputMessages; 
        
        /// activated output messages of current node
        setOfMessages outputMessages; 

        /// calls the check_enabled function for all transitions
        void checkEnablednessOfAllTransitions();

        /// returns the number of all places of the net
        Places_t::size_type getPlaceCount() const;

        /// Get owfnPlace from index i. Indices start at 0. Get the total number
        /// of owfnPlaces from getPlaceCount().
        owfnPlace* getPlace(Places_t::size_type i) const;

        /// get a pointer to owfnPlace with given name
        owfnPlace* getPlace(std::string) const;

        /// Returns the number of transitions this oWFN has.
        Transitions_t::size_type getTransitionCount() const;

        /// Return transition with index i of this oWFN. Indices start at 0.
        /// Get the total number of transitions by getTransitionCount().
        owfnTransition* getTransition(Transitions_t::size_type i) const;

        /// returns the place hash value
        unsigned int getPlaceHashValue();

        /// returns the number of input places of the net.
        Places_t::size_type getInputPlaceCount() const;

        /// Get an input owfnPlace from index i. Indices start at 0. Get the
        /// total number of input owfnPlaces from getInputPlaceCount().
        owfnPlace* getInputPlace(Places_t::size_type i) const;

        /// returns the number of output places of the net.
        Places_t::size_type getOutputPlaceCount() const;

        /// Get an output owfnPlace from index i. Indices start at 0. Get the
        /// total number of output owfnPlaces from getOutputPlaceCount().
        owfnPlace* getOutputPlace(Places_t::size_type i) const;

        /// Get the index of Place in the place set
        Places_t::size_type getPlaceIndex(owfnPlace *Place);

        /// Get the index of inputPlace in the input place set
        Places_t::size_type getInputPlaceIndex(owfnPlace *inputPlace);

        /// Get the index of outputPlace in the output place set
        Places_t::size_type getOutputPlaceIndex(owfnPlace *outputPlace);

        /// DESCRIPTION
        unsigned int getCardFireList();

        binDecision** binHashTable;

        unsigned int* CurrentMarking;
        list<unsigned int*> FinalMarkingList;
        formula* FinalCondition;
        std::string finalConditionString;
        std::string finalMarkingString;
        State* currentState;

        unsigned int placeHashValue;
        long int BitVectorSize;

        /// List of quasi enabled transitions.
        QuasiEnabledTransitions quasiEnabledTransitions;

        /// Real enabled transitions.
        EnabledTransitions enabledTransitions;

        // **** Definitions for Stubborn set calculations
#ifdef STUBBORN
        /// anchor to linked list of ...
        owfnTransition * StartOfStubbornList; 
        
        /// ... transitions in stubborn set
        owfnTransition * EndOfStubbornList; 
        
        /// # of activated (!) elements in stubborn set
        unsigned int NrStubborn; 
        
        /// Stubborn Set Calculation involves ...
        owfnTransition * TarjanStack; 
        
        /// ... SCC investigation on a graph of tr.
        owfnTransition * CallStack; 

        /// returns transitions to be fired for
        owfnTransition ** stubbornfirelistdeadlocks(); 

        /// returns transitions to be fired for
        /// reduced state space for message (or messages)
        /// successors
        owfnTransition ** stubbornfirelistmessage(owfnPlace *);
        owfnTransition ** stubbornfirelistmessage(messageMultiSet);
        owfnTransition ** stubbornFirelistAllMessages();
        
        owfnTransition ** stubbornfirelistmarking(unsigned int *);
#endif

        void initialize(); // initializes the net

        /// Adds an owfnTransition to this oWFN.
        bool addTransition(owfnTransition* transition);

        /// Adds an owfnPlace to this oWFN.
        void addPlace(owfnPlace*);

        /// decodes state, checks for message bound violation and adds successors
        void addSuccStatesToNode(AnnotatedGraphNode*, State*);

        /// decodes state, checks for message bound violation and adds successors
        void addSuccStatesToNodeStubborn(AnnotatedGraphNode*,
                                         StateSet&,
                                         unsigned int*,
                                         State*);

        /// helper function for addSuccStatesToNodeStubborn
        void addSuccStatesToNodeStubborn(AnnotatedGraphNode*,
                                         StateSet&,
                                         StateSet&,
                                         unsigned int*,
                                         State*);

        
        /// decodes state, figures out if state activates output event, 
        ///		checks for message bound violation and adds successors recursively
        void addSuccStatesToListStubborn(StateSet&,
        								 StateSet &,
                                         owfnPlace*,
                                         State*,
                                         AnnotatedGraphNode*);

        /// helper function
        /// decodes state, figures out if state activates output event, 
        ///		checks for message bound violation and adds successors recursively
        void addSuccStatesToListStubborn(StateSet&,
        								 StateSet&,
        								 StateSet&,
                                         owfnPlace*,
                                         State*,
                                         AnnotatedGraphNode*);

        
        /// decodes state, figures out if state activates output event, 
        ///		checks for message bound violation and adds successors recursively
        void addSuccStatesToListStubborn(StateSet&,
        								 StateSet&,
        								 StateSet&,
                                         messageMultiSet,
                                         State*,
                                         AnnotatedGraphNode*);

        /// helper function
        /// decodes state, figures out if state activates output event, 
        ///		checks for message bound violation and adds successors recursively
        void addSuccStatesToListStubborn(StateSet&,
        								 StateSet&,
                                         messageMultiSet,
                                         State*,
                                         AnnotatedGraphNode*);

        
        /// checks whether the message bound is violated
        bool violatesMessageBound();

        /// adds recursively the state s and all its successor states to the given state set
        void addSuccStatesToGivenSetOfStatesStubbornDeadlock(AnnotatedGraphNode*,
                                                             StateSet&,
                                                             StateSet&,
                                                             State* s);

        /// helper function to add recursively the state s and all its successor states to the given state set
        void addSuccStatesToGivenSetOfStatesStubbornDeadlock(AnnotatedGraphNode*,
                                                             StateSet&,
                                                             State* s);

        /// calculates and returns the next state
        State* calculateNextSate();

        /// returns a copy of the current marking
        unsigned int* copyCurrentMarking();

        /// returns a copy of the given marking
        unsigned int* copyGivenMarking(unsigned int*);
        
        /// replaces the current marking with a given one
        void copyMarkingToCurrentMarking(unsigned int* copy);
        
        /// checks if the current marking activates at least one of the receiving events of the given 
        /// message multi set
        bool currentMarkingActivatesNecessaryReceivingEvent(messageMultiSet);
        
        /// calculates the reduced set of states of the new AnnotatedGraphNode in case of an output event
//        void calculateReducedSetOfReachableStatesOutputEvent(StateSet&, binDecision**, AnnotatedGraphNode*);

        void storeStateInNode(State*, AnnotatedGraphNode*);
        
        /// calculates the reduced set of states of the new AnnotatedGraphNode in case of an input event
        void calculateReachableStatesStubbornDeadlocks(StateSet&, AnnotatedGraphNode*);
        
        /// calculates the reduced set of states reachable from the current marking and stores them 
        /// given state set
        void calculateReducedSetOfReachableStates(StateSet&,
                                                  StateSet&,
                                                  binDecision**,
                                                  owfnPlace*,
                                                  AnnotatedGraphNode*);

        void calculateReducedSetOfReachableStates(StateSet&,
                                                  StateSet&,
                                                  binDecision**,
                                                  messageMultiSet,
                                                  AnnotatedGraphNode*);
        
        bool currentMarkingActivatesReceivingEvent(unsigned int* );
        
        void calculateStubbornTransitions(State*);
        void calculateReducedSetOfReachableStatesStoreInNode(StateSet&,
                                                             AnnotatedGraphNode*);

        /// NO REDUCTION! calculate all reachable states from the current marking
        /// and store them in the node n (== AnnotatedGraphNode of CommunicationGraph)
        void calculateReachableStatesFull(AnnotatedGraphNode*);

        /// adds input message to the current marking
        void addInputMessage(unsigned int);
        
        /// adds input messages to the current marking
        void addInputMessage(messageMultiSet);

        /// removes an output message from the current marking
        bool removeOutputMessage(unsigned int);

        /// removes output messages from the current marking
        bool removeOutputMessage(messageMultiSet);

        /// returns the type of state (transient, maximal, minimal)
        stateType typeOfState(); 

        /// returns the given marking as a string
        std::string getMarkingAsString(unsigned int* marking) const;

        /// returns the current marking as a string
        std::string getCurrentMarkingAsString() const;

        //		bool isFinalMarking(unsigned int *);	
        
        
        /// is the current marking minimal?
        bool isMinimal();
        
        /// is the given marking == final marking of the net?
        bool isFinal() const; // does current marking satisfy final condition/final marking of the net?

        /// prints the current marking
        void printCurrentMarking() const;

        /// prints the given marking
        void printMarking(unsigned int * marking) const;

        /// DESCRIPTION
        void print_binDec(int);

        /// DESCRIPTION
        void print_binDec(binDecision *, int);

        /// removes all isolated places
        void removeisolated();

        /// creates a label from a message multiset
        std::string createLabel(messageMultiSet) const;

        /// checks whether this owfn matches a given og and returns a reason if not so
        bool matchesWithOG(const AnnotatedGraph* og, string& reasonForFailedMatch);

        /// creates an assignment for the OG from a current state
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
    
        /// returns a pointer to a normalized copy of this petrinet in the oWFN format
        oWFN* returnNormalOWFN();

        /// restricts the oWFN to a given port (i.e., removes all other ports)
        void restrictToPort(const std::string &portName);
        
        // Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
        NEW_OPERATOR(oWFN)
#define new NEW_NEW
};

#ifdef CHECKCAPACITY
class CapacityException
{
    public:
        /// name of place with capacity violation
        std::string place;

        /// Constructor
        CapacityException(std::string name) : place(name) {}

        // Provides user defined operator new. Needed to trace all new operations on this class.
#undef new
        NEW_OPERATOR(CapacityException)
#define new NEW_NEW

};
#endif


#endif /*OWFN_H_*/
