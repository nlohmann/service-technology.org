#include <sstream>

#include "AnicaLib.h"

anica::TriplePointer::TriplePointer(const pnapi::Place* s, const pnapi::Transition* h, const pnapi::Transition* l)
    :
    place(s),
    high(h),
    low(l)
{}

anica::TriplePointer::~TriplePointer()
{
    place = NULL;
    high = NULL;
    low = NULL;
}

anica::Triple::Triple(const std::string& s, const std::string& h, const std::string& l)
    :
    place(s),
    high(h),
    low(l)
{}

anica::Triple::~Triple()
{}

anica::AnicaLib::AnicaLib()
    :
    initialNet(NULL),
    represantiveNames(false),
    lolaPath(""),
    lolaWitnessPath(false),
    lolaVerbose(false),
    propertyToCheck(anica::PROPERTY_PBNIPLUS),
    unassignedTransitionsCount(0),
    highLabeledTransitionsCount(0),
    lowLabeledTransitionsCount(0),
    downLabeledTransitionsCount(0),
    potentialCausalPlacesCount(0),
    potentialConflictPlacesCount(0),
    activeCausalPlacesCount(0),
    activeConflictPlacesCount(0)
{
    newArcs.clear();
    downgradeTransitions.clear();
    unassignedTransitions.clear();
}

anica::AnicaLib::AnicaLib(const pnapi::PetriNet& net)
{
    AnicaLib();
    
    initialNet = new pnapi::PetriNet(net);
    initialize();
}

anica::AnicaLib::~AnicaLib()
{
    delete initialNet;
    initialNet = NULL;
}

void anica::AnicaLib::setProperty(property_e property)
{
    propertyToCheck = property;
}

const int anica::AnicaLib::getProperty() const
{
    return propertyToCheck;
}

void anica::AnicaLib::setRepresantiveNames(bool r)
{
    represantiveNames = r;
}

const bool anica::AnicaLib::getRepresantiveNames() const
{
    return represantiveNames;
}

void anica::AnicaLib::setLolaPath(const std::string& path)
{
    lolaPath = std::string(path);
}

const std::string& anica::AnicaLib::getLolaPath() const
{
    return lolaPath;
}

void anica::AnicaLib::setLolaWitnessPath(bool w)
{
    lolaWitnessPath = w;
}

const bool anica::AnicaLib::getLolaWitnessPath() const
{
    return lolaWitnessPath;
}

void anica::AnicaLib::setLolaVerbose(bool v)
{
    lolaVerbose = v;
}

const bool anica::AnicaLib::getLolaVerbose() const
{
    return lolaVerbose;
}

void anica::AnicaLib::setTransitionAssignment(const pnapi::Transition* t, confidence_e c)
{
    assert(initialNet != NULL);
    // todo: exception
    assert(t != NULL);
    
    if (t->getConfidence() == c) {
        return;
    }
    
    switch (t->getConfidence()) {
        case anica::CONFIDENCE_NONE:
            unassignedTransitionsCount--;
            unassignedTransitions.erase(const_cast<pnapi::Transition*>(t));
	        break;
	    case anica::CONFIDENCE_LOW:
	        lowLabeledTransitionsCount--;
	        break;
	    case anica::CONFIDENCE_HIGH: 
	        highLabeledTransitionsCount--;
	        break;
	    case anica::CONFIDENCE_DOWN:
	        downLabeledTransitionsCount--;
	        downgradeTransitions.erase(const_cast<pnapi::Transition*>(t));
	        break;
	    default:
	        // todo: add exception 
	        assert(false);
	}
	
	switch (c) {
        case anica::CONFIDENCE_NONE:
            unassignedTransitionsCount++;
            unassignedTransitions.insert(const_cast<pnapi::Transition*>(t));
	        break;
	    case anica::CONFIDENCE_LOW:
	        lowLabeledTransitionsCount++;
	        break;
	    case anica::CONFIDENCE_HIGH: 
	        highLabeledTransitionsCount++;
	        break;
	    case anica::CONFIDENCE_DOWN:
	        downLabeledTransitionsCount++;
	        downgradeTransitions.insert(const_cast<pnapi::Transition*>(t));
	}
}

void anica::AnicaLib::assignTransition(const std::string& t, confidence_e c) {
    assert(initialNet != NULL);
    
    const pnapi::Transition* transition = initialNet->findTransition(t);
    // todo: exception
    assert(transition != NULL);
    
    setTransitionAssignment(transition, c);
}

void anica::AnicaLib::assignUnassignedTransitions(confidence_e c) {
    assert(initialNet != NULL);
    
    PNAPI_FOREACH(t, initialNet->getTransitions()) {
        if ((**t).getConfidence() == anica::CONFIDENCE_NONE) {
            setTransitionAssignment(*t, c);
        }
    }
}

bool anica::AnicaLib::isPotentialCausalPlace(const std::string& p) const {
    assert(initialNet != NULL);
    
    const pnapi::Place* place = initialNet->findPlace(p);
    // todo: exception
    assert(place != NULL);
    
    bool hasHighPredecessor = false;
    
    PNAPI_FOREACH(t, place->getPreset()) {
        if (((pnapi::Transition*)(*t))->getConfidence() == anica::CONFIDENCE_HIGH) {
            hasHighPredecessor = true;
            break;
        }
    }
    
    if (hasHighPredecessor) {
        PNAPI_FOREACH(t, place->getPostset()) {
            if (((pnapi::Transition*)(*t))->getConfidence() == anica::CONFIDENCE_LOW) {
                return true;
            }
        }
    }
    
    return false;
}

bool anica::AnicaLib::isPotentialConflictPlace(const std::string& p) const {
    assert(initialNet != NULL);
    
    const pnapi::Place* place = initialNet->findPlace(p);
    // todo: exception
    assert(place != NULL);
    
    bool hasHighSuccessor = false;
    bool hasLowSuccessor = false;
    
    PNAPI_FOREACH(t, place->getPostset()) {
        const int c = ((pnapi::Transition*)(*t))->getConfidence();
        if (c == anica::CONFIDENCE_HIGH) {
            hasHighSuccessor = true;
        }
        if (c == anica::CONFIDENCE_LOW) {
            hasLowSuccessor = true;
        }
        if (hasHighSuccessor && hasLowSuccessor) {
            return true;
        }
    }
    
    return false;
}

bool anica::AnicaLib::isActiveCausalPlace(const std::string& p) {
    assert(initialNet != NULL);
    
    const pnapi::Place* place = initialNet->findPlace(p);
    // todo: exception
    assert(place != NULL);
    
    pnapi::PetriNet* taskNet;
    TriplePointer* resultTriple;
    
    PNAPI_FOREACH(h, place->getPreset()) {
        if (((pnapi::Transition*)(*h))->getConfidence() == anica::CONFIDENCE_HIGH) {
            PNAPI_FOREACH(l, place->getPostset()) {
                if (((pnapi::Transition*)(*l))->getConfidence() == anica::CONFIDENCE_LOW) {
                    if (isActiveCausalTriple(place, (pnapi::Transition*)*h, (pnapi::Transition*)*l)) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

bool anica::AnicaLib::isActiveConflictPlace(const std::string& p) {
    assert(initialNet != NULL);
    
    const pnapi::Place* place = initialNet->findPlace(p);
    // todo: exception
    assert(place != NULL);
    
    pnapi::PetriNet* taskNet;
    TriplePointer* resultTriple;
    
    PNAPI_FOREACH(h, place->getPostset()) {
        if (((pnapi::Transition*)(*h))->getConfidence() == anica::CONFIDENCE_HIGH) {
            // todo: smarter implementieren...;-)
            PNAPI_FOREACH(l, place->getPostset()) {
                if (((pnapi::Transition*)(*l))->getConfidence() == anica::CONFIDENCE_LOW) {
                    if (isActiveConflictTriple(place, (pnapi::Transition*)*h, (pnapi::Transition*)*l)) {
                        return true;
                    }
                }
            }
        }
    }
    
    return false;
}

bool anica::AnicaLib::isActiveCausalTriple(const pnapi::Place* s, const pnapi::Transition* h, const pnapi::Transition* l)
{
    assert(s != NULL);
    assert(h != NULL);
    assert(l != NULL);
    
    pnapi::PetriNet* taskNet = new pnapi::PetriNet(*initialNet);
    newArcs.clear();
    TriplePointer* resultTriple = addCausalPattern(*taskNet, taskNet->findPlace(s->getName()), taskNet->findTransition(h->getName()), taskNet->findTransition(l->getName()), true);
    
    const int ret = callLoLA(*taskNet, resultTriple->place);
    
    delete taskNet;
    delete resultTriple;

    switch(ret) {
        case 0:
            return true;
        case 1:
            return false;
        default:
            assert(false);
    }
}

bool anica::AnicaLib::isActiveConflictTriple(const pnapi::Place* s, const pnapi::Transition* h, const pnapi::Transition* l)
{
    assert(s != NULL);
    assert(h != NULL);
    assert(l != NULL);
    
    pnapi::PetriNet* taskNet = new pnapi::PetriNet(*initialNet);
    newArcs.clear();
    TriplePointer* resultTriple = addConflictPattern(*taskNet, taskNet->findPlace(s->getName()), taskNet->findTransition(h->getName()), taskNet->findTransition(l->getName()), true);
    
    const int ret = callLoLA(*taskNet, resultTriple->place);
    
    delete taskNet;
    delete resultTriple;

    switch(ret) {
        case 0:
            return true;
        case 1:
            return false;
        default:
            assert(false);
    }
}

void anica::AnicaLib::initialize()
{
    assert(initialNet != NULL);
    
    // initial with same values as empty constructor
    propertyToCheck = anica::PROPERTY_PBNIPLUS;
    represantiveNames = false;
    lolaPath = "";
    lolaWitnessPath = false;
    lolaVerbose = false;
    newArcs.clear();
    downgradeTransitions.clear();
    unassignedTransitions.clear();
    potentialCausalPlacesCount = 0;
    potentialConflictPlacesCount = 0;
    activeCausalPlacesCount = 0;
    activeConflictPlacesCount = 0;
    
    PNAPI_FOREACH(t, initialNet->getTransitions()) {
        switch ((**t).getConfidence()) {
		    case anica::CONFIDENCE_NONE:
                unassignedTransitionsCount++;
                unassignedTransitions.insert(*t);
		        break;
		    case anica::CONFIDENCE_LOW:
		        lowLabeledTransitionsCount++;
		        break;
		    case anica::CONFIDENCE_HIGH: 
		        highLabeledTransitionsCount++;
		        break;
		    case anica::CONFIDENCE_DOWN:
		        downLabeledTransitionsCount++;
		        downgradeTransitions.insert(*t);
		        break;
		    default:
		        // todo: add exception 
		        assert(false);
		} 
    }
}

int anica::AnicaLib::callLoLA(const pnapi::PetriNet& net, const pnapi::Place* goal) const {
    assert(lolaPath != "");
	
	std::stringstream ss;
	ss << pnapi::io::lola << net;
	ss << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";

	// select LoLA binary and build LoLA command 
    #if defined(__MINGW32__) 
    // // MinGW does not understand pathnames with "/", so we use the basename 
    const std::string command_line = "\"" + lolaPath + (lolaWitnessPath ? " -p " : "") + (lolaVerbose ? "" : " 2> nul"); 
    #else 
    const std::string command_line = lolaPath + (lolaWitnessPath ? " -p " : "") + (lolaVerbose ? "" : " 2> nul"); 
    #endif 

	FILE *fp = popen(command_line.c_str(), "w");
	if (fp == NULL) {
	    // todo: add exception 
        assert(false);
	}
	fprintf(fp, "%s", ss.str().c_str());

	return (pclose(fp) / 256);
}

anica::TriplePointer* anica::AnicaLib::addCausalPattern(pnapi::PetriNet& extendedNet, pnapi::Place* s, pnapi::Transition* h, pnapi::Transition* l, bool insertArcs)
{   
    pnapi::Transition* lC = &extendedNet.createTransition();
    pnapi::Transition* hC = &extendedNet.createTransition();
    pnapi::Place* fired = &extendedNet.createPlace();
    pnapi::Place* goal = &extendedNet.createPlace();

    const std::string triple = "(" + s->getName() + ", " + h->getName() + ", " + l->getName() + ")";

    if (represantiveNames) {
        lC->setName("lC_" + triple);
        hC->setName("hC_" + triple);
        fired->setName("fired_" + triple);
        goal->setName("goal_" + triple);
    }

    TriplePointer* ret = new TriplePointer(goal, hC, lC);

    PNAPI_FOREACH(prePlace, h->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, hC));
    }
    PNAPI_FOREACH(postPlace, h->getPostset()) {
        newArcs.insert(std::make_pair(hC, *postPlace));
    }

    PNAPI_FOREACH(prePlace, l->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, lC));
    }

    PNAPI_FOREACH(preTransition, s->getPreset()) {
        if (((**preTransition).getName() != l->getName()) && ( (**preTransition).getName() != h->getName() )) {
            pnapi::Place* a = &extendedNet.createPlace("", 1);
            if (represantiveNames) {
                a->setName("p_" + (**preTransition).getName() + "_" + triple);
            }
            newArcs.insert(std::make_pair(*preTransition, a));
            newArcs.insert(std::make_pair(a, *preTransition));
            newArcs.insert(std::make_pair(a, hC));
        }
    }

    if (propertyToCheck == anica::PROPERTY_PBNID) {
        //handle downgrading transitions
        PNAPI_FOREACH(downTransition, downgradeTransitions) {
            pnapi::Place* a = &extendedNet.createPlace("", 1);
            pnapi::Transition* d = (pnapi::Transition*)extendedNet.findTransition((**downTransition).getName());
            if (represantiveNames) {
                a->setName("d_" + d->getName() + "_" + triple);
            }
            newArcs.insert(std::make_pair(d, a));
            newArcs.insert(std::make_pair(a, d));
            newArcs.insert(std::make_pair(a, hC));
        }
    }

    newArcs.insert(std::make_pair(hC, fired));
    newArcs.insert(std::make_pair(fired, lC));
    newArcs.insert(std::make_pair(lC, goal));

    if (insertArcs) {
        PNAPI_FOREACH(a, newArcs) {
            extendedNet.createArc(*a->first, *a->second);
        }
        newArcs.clear();
    }

    return ret;
}

anica::TriplePointer* anica::AnicaLib::addConflictPattern(pnapi::PetriNet& extendedNet, pnapi::Place* s, pnapi::Transition* h, pnapi::Transition* l, bool insertArcs)
{   
    pnapi::Transition* lC = &extendedNet.createTransition();
    pnapi::Transition* hC = &extendedNet.createTransition();
    pnapi::Place* fired = &extendedNet.createPlace();
    pnapi::Place* enabled = &extendedNet.createPlace("", 1);
    pnapi::Place* goal = &extendedNet.createPlace();

    const std::string triple = "(" + s->getName() + ", " + h->getName() + ", " + l->getName() + ")";

    if (represantiveNames) {
        lC->setName("lC_" + triple);
        hC->setName("hC_" + triple);
        fired->setName("fired_" + triple);
        enabled->setName("enabled_" +triple);
        goal->setName("goal_" + triple);
    }

    TriplePointer* ret = new TriplePointer(goal, hC, lC);

    PNAPI_FOREACH(prePlace, h->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, hC));
        newArcs.insert(std::make_pair(hC, *prePlace));
    }

    PNAPI_FOREACH(prePlace, l->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, lC));
    }

    PNAPI_FOREACH(preTransition, s->getPreset()) {
        if (((**preTransition).getName() != l->getName()) && ( (**preTransition).getName() != h->getName() )) {
            pnapi::Place* a = &extendedNet.createPlace("", 1);
            if (represantiveNames) {
                a->setName("p_" + (**preTransition).getName() + "_" + triple);
            }
            newArcs.insert(std::make_pair(*preTransition, a));
            newArcs.insert(std::make_pair(a, *preTransition));
            newArcs.insert(std::make_pair(a, hC));
        }
    }

    if (propertyToCheck == anica::PROPERTY_PBNID) {
        //handle downgrading transitions
        PNAPI_FOREACH(downTransition, downgradeTransitions) {
            pnapi::Place* a = &extendedNet.createPlace("", 1);
            pnapi::Transition* d = (pnapi::Transition*)extendedNet.findTransition((**downTransition).getName());
            if (represantiveNames) {
                a->setName("d_" + d->getName() + "_" + triple);
            }
            newArcs.insert(std::make_pair(d, a));
            newArcs.insert(std::make_pair(a, d));
            newArcs.insert(std::make_pair(a, hC));
        }
    }

    newArcs.insert(std::make_pair(hC, fired));
    newArcs.insert(std::make_pair(fired, lC));
    newArcs.insert(std::make_pair(lC, goal)); 
    newArcs.insert(std::make_pair(enabled, h));
    newArcs.insert(std::make_pair(h, enabled));
    newArcs.insert(std::make_pair(enabled, hC));

    if (insertArcs) {
        PNAPI_FOREACH(a, newArcs) {
            extendedNet.createArc(*a->first, *a->second);
        }
        newArcs.clear();
    }

    return ret;
}
