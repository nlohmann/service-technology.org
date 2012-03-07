#pragma once

#include <cstring>
#include <cstdlib>

#include <pnapi/pnapi.h>
#include <cuddObj.hh>

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


class TriplePointer
{ 
    public:
        TriplePointer(const pnapi::Place*, const pnapi::Transition*, const pnapi::Transition*);    
        ~TriplePointer();
 
        const pnapi::Place* place;
        const pnapi::Transition* high;
        const pnapi::Transition* low;
};

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
	    // creates just the object
	    AnicaLib();
	    // creates the object with given Petri net
        explicit AnicaLib(const pnapi::PetriNet&);
        // creates the object with given Petri net and assigns unassigned transitions with given value
        AnicaLib(const pnapi::PetriNet&, confidence_e);
        
        // destroys the object   
        ~AnicaLib();

        // assigns given transition with given assignment
        void assignTransition(const std::string&, confidence_e);
        // assigns all transitions with given assignment
        void assignUnassignedTransitions(confidence_e);

        size_t isPotentialCausalPlace(const std::string&) const;
        size_t isPotentialConflictPlace(const std::string&) const;
        size_t isActiveCausalPlace(const std::string&);
        size_t isActiveConflictPlace(const std::string&);
        
        bool isActiveCausalTriple(const Triple&);
        bool isActiveConflictTriple(const Triple&);
        
        size_t isSecure();
    
        Cudd* getCharacterization(char**, BDD*, std::map<std::string, int>&);
        pnapi::PetriNet* getControllerProblem();
        
        pnapi::PetriNet* getCurrentNet() const;
        
        void colorPotentialPlaces(colorPlaceTask_e);
        void colorActivePlaces(colorPlaceTask_e);
        void colorConfidence(colorConfidenceTask_e);
        void clearColors();
        
        const Triple* addCausalPattern(pnapi::PetriNet&, const Triple&);
        const Triple* addConflictPattern(pnapi::PetriNet&, const Triple&);
        
        void setProperty(property_e);
        const int getProperty() const;
        
        void setRepresantiveNames(bool);
        const bool getRepresantiveNames() const;
        void setOneTripleOnly(bool);
        const bool getOneTripleOnly() const;
        void setOneActiveOnly(bool);
        const bool getOneActiveOnly() const;
        
        const size_t getUnassignedTransitionsCount() const;
        const size_t getHighLabeledTransitionsCount() const;
        const size_t getLowLabeledTransitionsCount() const;
        const size_t getDownLabeledTransitionsCount() const;
        
        void setLolaPath(const std::string&);
        const std::string& getLolaPath() const;
        void setLolaWitnessPath(bool);
        const bool getLolaWitnessPath() const;
        void setLolaVerbose(bool);
        const bool getLolaVerbose() const;
        
        void setKeepTempFiles(bool);
        const bool getKeepTempFiles() const;
        
    private:
        int callLoLA(const pnapi::PetriNet&, const pnapi::Place*) const;
        
        void initialize();
    
        void setTransitionAssignment(pnapi::Transition*, confidence_e);
    
        TriplePointer* addCausalPattern(pnapi::PetriNet&, const TriplePointer*, bool);
        TriplePointer* addConflictPattern(pnapi::PetriNet&, const TriplePointer*, bool);
    
        size_t isPotentialCausalPlace(const pnapi::Place*) const;
        size_t isPotentialConflictPlace(const pnapi::Place*) const;
        size_t isActiveCausalPlace(pnapi::PetriNet&, const pnapi::Place*);
        size_t isActiveConflictPlace(pnapi::PetriNet&, const pnapi::Place*);
        
        bool isActiveCausalTriple(pnapi::PetriNet&, const TriplePointer*);
        bool isActiveConflictTriple(pnapi::PetriNet&, const TriplePointer*);
        
        void colorPotentialPlaces(pnapi::PetriNet&, colorPlaceTask_e);
        void colorActivePlaces(pnapi::PetriNet&, colorPlaceTask_e);
        void colorConfidence(pnapi::PetriNet&, colorConfidenceTask_e);
        void clearColors(pnapi::PetriNet&);
        
        pnapi::PetriNet* initialNet;
 
        property_e propertyToCheck;
 
        bool represantiveNames;
        bool oneTripleOnly;
        bool oneActiveOnly;
 
        std::string lolaPath;
        bool lolaWitnessPath;
        bool lolaVerbose;
 
        bool keepTempFiles;
 
        std::set< std::pair<pnapi::Node*, pnapi::Node*> > newArcs;
 
        std::set<pnapi::Transition*> downgradeTransitions;
        std::set<pnapi::Transition*> unassignedTransitions;
        
        size_t highLabeledTransitionsCount;
        size_t lowLabeledTransitionsCount;
        size_t potentialCausalPlacesCount;
        size_t potentialConflictPlacesCount;
        size_t activeCausalPlacesCount;
        size_t activeConflictPlacesCount;
};

} // namespace anica
