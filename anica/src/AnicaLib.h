#pragma once

#include <cstring>
#include <cstdlib>

#include <pnapi/pnapi.h>
#include <cuddObj.hh>

#include "exceptions.h"

namespace anica {

typedef enum
{
    CONFIDENCE_NONE = 0,
    CONFIDENCE_LOW = 1,
    CONFIDENCE_HIGH = 2,
    CONFIDENCE_DOWN = 3
} confidence_e;

typedef enum
{
    COLOR_CONFIDENCE_TASK_ALL = 0,
    COLOR_CONFIDENCE_TASK_LOW = 1,
    COLOR_CONFIDENCE_TASK_HIGH = 2,
    COLOR_CONFIDENCE_TASK_DOWN = 3
} colorConfidenceTask_e;

typedef enum
{
    COLOR_PLACE_TASK_ALL = 0,
    COLOR_PLACE_TASK_CAUSAL = 1,
    COLOR_PLACE_TASK_CONFLICT = 2
} colorPlaceTask_e;

typedef enum
{
    PROPERTY_PBNIPLUS = 0,
    PROPERTY_PBNID = 1
} property_e;

// external representation of a triple (as names)
class Triple
{ 
    public:
        Triple(const std::string&, const std::string&, const std::string&);    
        ~Triple();
 
        const std::string place;
        const std::string high;
        const std::string low;
};

class AnicaLib
{
    public:
	    // creates the object with given Petri net
        explicit AnicaLib(const pnapi::PetriNet&);
        
        // destroys the object   
        ~AnicaLib();

        // assigns given transition with given assignment
        void assignTransition(const std::string&, confidence_e);
        // assigns all transitions with given assignment
        void assignUnassignedTransitions(confidence_e);

        // checks whether the given place (by name) is a potential causal place (returns the number of potential causal triples)
        size_t isPotentialCausalPlace(const std::string&) const;
        // checks whether the given place (by name) is a potential conflict place (returns the number of potential conflict triples)
        size_t isPotentialConflictPlace(const std::string&) const;
        // checks whether the given place (by name) is an active causal place (returns the number of active causal triples)
        size_t isActiveCausalPlace(const std::string&);
        // checks whether the given place (by name) is an active conflict place (returns the number of active conflict triples)
        size_t isActiveConflictPlace(const std::string&);
        // checks whether the given triple (by names) is an active causal triple
        bool isActiveCausalTriple(const Triple&);
        // checks whether the given triple (by names) is an active conflict triple
        bool isActiveConflictTriple(const Triple&);
        // checks whether the given the net is secure (returns the number of active places)
        size_t isSecure();
        // returns all potential causal places by name
        std::set<std::string> getAllPotentialCausalPlaces() const;
        // returns all potential conflict places by name
        std::set<std::string> getAllPotentialConflictPlaces() const;
        // returns all potential places by name
        std::set<std::string> getAllPotentialPlaces() const;
        // returns all active causal places by name
        std::set<std::string> getAllActiveCausalPlaces();
        // returns all active conflict places by name
        std::set<std::string> getAllActiveConflictPlaces();
        // returns all active places by name
        std::set<std::string> getAllActivePlaces();
    
        // calculates the complete characterization of all valid assignments
        Cudd* getCharacterization(char**, BDD*, std::map<std::string, int>&);
        // creates a controller problem to characterize all valid assignments
        pnapi::PetriNet* getControllerProblem();
        
        // returns the current net
        pnapi::PetriNet* getCurrentNet() const;
        
        // colors potential places (which can be parameterized)
        pnapi::PetriNet* colorPotentialPlaces(colorPlaceTask_e);
        // colors active places (which can be parameterized)
        pnapi::PetriNet* colorActivePlaces(colorPlaceTask_e);
        // colors confidence levels of transitions (which can be parameterized)
        pnapi::PetriNet* colorConfidence(colorConfidenceTask_e);
        // deletes colors of all places and tranisitons
        void clearColors(pnapi::PetriNet&);
        
        
        // sets the file name of the input net (for output purposes, i.e., witness path)
        void setInputName(const std::string&);
        // sets the property which should be considered in all tests
        void setProperty(property_e);
        // gets the property which is considered in all tests
        const int getProperty() const;
        // sets whether represantive names for new places and transitions should be used
        void setRepresantiveNames(bool);
        // gets whether represantive names for new places and transitions are used
        const bool getRepresantiveNames() const;
        // sets whether tests for a place should be finished after the first active triple of a place
        void setOneTripleOnly(bool);
        // gets whether tests for a place are finished after the first active triple of a place
        const bool getOneTripleOnly() const;
        // sets whether tests for the net should be finished after the first active place
        void setOneActiveOnly(bool);
        // gets whether tests for the net are finished after the first active place
        const bool getOneActiveOnly() const;
        
        // returns the number of unassigned transitions in the current net
        const size_t getUnassignedTransitionsCount() const;
        // returns the number of high labeled transitions in the current net
        const size_t getHighLabeledTransitionsCount() const;
        // returns the number of low labeled transitions in the current net
        const size_t getLowLabeledTransitionsCount() const;
        // returns the number of downgrade labeled transitions in the current net
        const size_t getDownLabeledTransitionsCount() const;
        
        // sets the path to a suitable lola binary
        void setLolaPath(const std::string&);
        // gets the path to a suitable lola binary
        const std::string& getLolaPath() const;
        // sets whether lola should compute a witness path
        void setLolaWitnessPath(bool);
        // gets whether lola should compute a witness path
        const bool getLolaWitnessPath() const;
        // sets whether lola should work verbose
        void setLolaVerbose(bool);
        // gets whether lola should work verbose
        const bool getLolaVerbose() const;
        
    private:
    
        // internal representation of a triple (as pointers)
        class TriplePointer
        { 
            public:
                TriplePointer(const pnapi::Place*, const pnapi::Transition*, const pnapi::Transition*);    
                ~TriplePointer();
         
                const pnapi::Place* place;
                const pnapi::Transition* high;
                const pnapi::Transition* low;
        };
    
        // calls lola with the task to check the activation of a given place in a given net (returns exit code of lola)
        int callLoLA(const pnapi::PetriNet&, const pnapi::Place*) const;
        int callLoLA(const pnapi::PetriNet&, const pnapi::Place*, const std::string&) const;
    
        // assigns a given confidence to a given transition
        void setTransitionAssignment(pnapi::Transition*, confidence_e);
    
        // extends the given net to check whether the given triple is an acive causal triple 
        TriplePointer* addCausalPattern(pnapi::PetriNet&, const TriplePointer*, bool);
        // extends the given net to check whether the given triple is an acive conflict triple 
        TriplePointer* addConflictPattern(pnapi::PetriNet&, const TriplePointer*, bool);
    
        // checks whether the given place (by pointer) is a potential causal place (returns the number of potential causal triples)
        size_t isPotentialCausalPlace(const pnapi::Place*) const;
        // checks whether the given place (by pointer) is a potential conflict place (returns the number of potential conflict triples)
        size_t isPotentialConflictPlace(const pnapi::Place*) const;
        // checks whether the given place (by pointer) is an active causal place (returns the number of active causal triples)
        size_t isActiveCausalPlace(const pnapi::PetriNet&, const pnapi::Place*);
        // checks whether the given place (by pointer) is an active conflict place (returns the number of active conflict triples)
        size_t isActiveConflictPlace(const pnapi::PetriNet&, const pnapi::Place*);
        // checks whether the given triple (by pointers) is an active causal triple
        bool isActiveCausalTriple(const pnapi::PetriNet&, const TriplePointer*);
        // checks whether the given triple (by pointers) is an active conflict triple
        bool isActiveConflictTriple(const pnapi::PetriNet&, const TriplePointer*);
        
        // colors potential places (which can be parameterized)
        void colorPotentialPlaces(pnapi::PetriNet&, colorPlaceTask_e);
        // colors active places (which can be parameterized, reachability checks are performed)
        void colorActivePlaces(pnapi::PetriNet&, colorPlaceTask_e);
        // colors confidence levels of transitions (which can be parameterized)
        void colorConfidence(pnapi::PetriNet&, colorConfidenceTask_e);
        
        // M E M B E R S
        // saves current net
        const pnapi::PetriNet* initialNet;
        // saves input name of current net
        std::string inputName;
        // stores property to check
        property_e propertyToCheck;
        // indicates whether represantive names should be used
        bool represantiveNames;
        // indicates whether to stop place check after first active triple
        bool oneTripleOnly;
        // indicates whether to stop net check after first active place
        bool oneActiveOnly;
        // path to suitable lola binary
        std::string lolaPath;
        // indicates whether lola should compute a witness path
        bool lolaWitnessPath;
        // indicates whether lola should work verbose
        bool lolaVerbose;
        // saves new arcs created by activation tests (especially in case of characterization)
        std::set< std::pair<pnapi::Node*, pnapi::Node*> > newArcs;
        // saves all downgrade transitions
        std::set<pnapi::Transition*> downgradeTransitions;
        // saves all unassigned transitions
        std::set<pnapi::Transition*> unassignedTransitions;
        // number of high labeled transitions
        size_t highLabeledTransitionsCount;
        // number of low labeled transitions
        size_t lowLabeledTransitionsCount;
};

} // namespace anica
