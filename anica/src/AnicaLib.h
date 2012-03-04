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

        bool isPotentialCausalPlace(const std::string&) const;
        bool isPotentialConflictPlace(const std::string&) const;
        bool isActiveCausalPlace(const std::string&);
        bool isActiveConflictPlace(const std::string&);
        bool isActiveCausalTriple(const Triple*);
        bool isActiveConflictTriple(const Triple*);
        
        bool isSecure();
        
        const Triple* addCausalPattern(pnapi::PetriNet&, const Triple*);
        const Triple* addConflictPattern(pnapi::PetriNet&, const Triple*);
        
        void setProperty(property_e);
        const int getProperty() const;
        
        void setRepresantiveNames(bool);
        const bool getRepresantiveNames() const;
        
        void setLolaPath(const std::string&);
        const std::string& getLolaPath() const;
        void setLolaWitnessPath(bool);
        const bool getLolaWitnessPath() const;
        void setLolaVerbose(bool);
        const bool getLolaVerbose() const;
        
    private:
        int callLoLA(const pnapi::PetriNet& net, const pnapi::Place* goal) const;
        
        void initialize();
    
        void setTransitionAssignment(const pnapi::Transition*, confidence_e);
    
        TriplePointer* addCausalPattern(pnapi::PetriNet&, const TriplePointer*, bool);
        TriplePointer* addConflictPattern(pnapi::PetriNet&, const TriplePointer*, bool);
    
        bool isActiveCausalTriple(const TriplePointer*);
        bool isActiveConflictTriple(const TriplePointer*);
        
        pnapi::PetriNet* initialNet;
 
        property_e propertyToCheck;
 
        bool represantiveNames;
 
        std::string lolaPath;
        bool lolaWitnessPath;
        bool lolaVerbose;
 
        std::set< std::pair<pnapi::Node*, pnapi::Node*> > newArcs;
 
        std::set<pnapi::Transition*> downgradeTransitions;
        std::set<pnapi::Transition*> unassignedTransitions;
        
        size_t unassignedTransitionsCount;
        size_t highLabeledTransitionsCount;
        size_t lowLabeledTransitionsCount;
        size_t downLabeledTransitionsCount;
        size_t potentialCausalPlacesCount;
        size_t potentialConflictPlacesCount;
        size_t activeCausalPlacesCount;
        size_t activeConflictPlacesCount;
};

} // namespace anica
