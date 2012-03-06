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
    highLabeledTransitionsCount(0),
    lowLabeledTransitionsCount(0),
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

anica::AnicaLib::AnicaLib(const pnapi::PetriNet& net, confidence_e c)
{
    AnicaLib();
    
    initialNet = new pnapi::PetriNet(net);
    initialize();
    
    assignUnassignedTransitions(c);
}

anica::AnicaLib::~AnicaLib()
{
    delete initialNet;
    initialNet = NULL;
}

const size_t anica::AnicaLib::getUnassignedTransitionsCount() const {
    assert(initialNet != NULL);
    
    return unassignedTransitions.size();
}

const size_t anica::AnicaLib::getHighLabeledTransitionsCount() const {
    assert(initialNet != NULL);
    
    return highLabeledTransitionsCount;
}

const size_t anica::AnicaLib::getLowLabeledTransitionsCount() const {
    assert(initialNet != NULL);
    
    return lowLabeledTransitionsCount;
}

const size_t anica::AnicaLib::getDownLabeledTransitionsCount() const {
    assert(initialNet != NULL);
    
    return downgradeTransitions.size();
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

void anica::AnicaLib::setTransitionAssignment(pnapi::Transition* t, confidence_e c)
{
    assert(initialNet != NULL);
    // todo: exception
    assert(t != NULL);
    
    if (t->getConfidence() == c) {
        return;
    }
    
    switch (t->getConfidence()) {
        case anica::CONFIDENCE_NONE:
            unassignedTransitions.erase(const_cast<pnapi::Transition*>(t));
	        break;
	    case anica::CONFIDENCE_LOW:
	        lowLabeledTransitionsCount--;
	        break;
	    case anica::CONFIDENCE_HIGH: 
	        highLabeledTransitionsCount--;
	        break;
	    case anica::CONFIDENCE_DOWN:
	        downgradeTransitions.erase(const_cast<pnapi::Transition*>(t));
	        break;
	    default:
	        // todo: add exception 
	        assert(false);
	}
	
	t->setConfidence(c);
	switch (c) {
        case anica::CONFIDENCE_NONE:
            unassignedTransitions.insert(const_cast<pnapi::Transition*>(t));
	        break;
	    case anica::CONFIDENCE_LOW:
	        lowLabeledTransitionsCount++;
	        break;
	    case anica::CONFIDENCE_HIGH: 
	        highLabeledTransitionsCount++;
	        break;
	    case anica::CONFIDENCE_DOWN:
	        downgradeTransitions.insert(const_cast<pnapi::Transition*>(t));
	}
}

void anica::AnicaLib::assignTransition(const std::string& t, confidence_e c) {
    assert(initialNet != NULL);
    
    pnapi::Transition* transition = initialNet->findTransition(t);
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
        const int presetConfidence = ((pnapi::Transition*)(*h))->getConfidence();
        assert(presetConfidence != anica::CONFIDENCE_NONE); 
        if (presetConfidence == anica::CONFIDENCE_HIGH) {
            PNAPI_FOREACH(l, place->getPostset()) {
                const int postsetConfidence = ((pnapi::Transition*)(*l))->getConfidence();
                assert(postsetConfidence != anica::CONFIDENCE_NONE); 
                if (postsetConfidence == anica::CONFIDENCE_LOW) {
                    TriplePointer* tripel = new TriplePointer(place, (pnapi::Transition*)*h, (pnapi::Transition*)*l); 
                    if (isActiveCausalTriple(tripel)) {
                        delete tripel;
                        return true;
                    }
                    delete tripel;
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
    
    const std::set<pnapi::Node*> postset = place->getPostset();
    
    PNAPI_FOREACH(h, postset) {
        const int postsetConfidence = ((pnapi::Transition*)(*h))->getConfidence();
        assert(postsetConfidence != anica::CONFIDENCE_NONE); 
        if (postsetConfidence == anica::CONFIDENCE_HIGH) {
            PNAPI_FOREACH(l, postset) {
                if (((pnapi::Transition*)(*l))->getConfidence() == anica::CONFIDENCE_LOW) {
                    TriplePointer* tripel = new TriplePointer(place, (pnapi::Transition*)*h, (pnapi::Transition*)*l); 
                    if (isActiveConflictTriple(tripel)) {
                        delete tripel;
                        return true;
                    }
                    delete tripel;
                }
            }
        }
    }
    
    return false;
}

bool anica::AnicaLib::isActiveCausalTriple(const anica::TriplePointer* tripel)
{
    assert(tripel->place != NULL);
    assert(tripel->high != NULL);
    assert(tripel->low != NULL);
    
    pnapi::PetriNet* taskNet = new pnapi::PetriNet(*initialNet);
    newArcs.clear();
    TriplePointer* taskTriple = new TriplePointer(taskNet->findPlace(tripel->place->getName()), taskNet->findTransition(tripel->high->getName()), taskNet->findTransition(tripel->low->getName()));
    TriplePointer* resultTriple = addCausalPattern(*taskNet, taskTriple, true);
    
    const int ret = callLoLA(*taskNet, resultTriple->place);
    
    delete taskTriple;
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

bool anica::AnicaLib::isActiveConflictTriple(const anica::TriplePointer* tripel)
{
    assert(tripel->place != NULL);
    assert(tripel->high != NULL);
    assert(tripel->low != NULL);
    
    pnapi::PetriNet* taskNet = new pnapi::PetriNet(*initialNet);
    newArcs.clear();
    TriplePointer* taskTriple = new TriplePointer(taskNet->findPlace(tripel->place->getName()), taskNet->findTransition(tripel->high->getName()), taskNet->findTransition(tripel->low->getName()));
    TriplePointer* resultTriple = addConflictPattern(*taskNet, taskTriple, true);
    
    const int ret = callLoLA(*taskNet, resultTriple->place);
    
    delete taskTriple;
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
    lowLabeledTransitionsCount = 0;
    highLabeledTransitionsCount = 0;
    potentialCausalPlacesCount = 0;
    potentialConflictPlacesCount = 0;
    activeCausalPlacesCount = 0;
    activeConflictPlacesCount = 0;
    
    PNAPI_FOREACH(t, initialNet->getTransitions()) {
        switch ((**t).getConfidence()) {
		    case anica::CONFIDENCE_NONE:
                unassignedTransitions.insert(*t);
		        break;
		    case anica::CONFIDENCE_LOW:
		        lowLabeledTransitionsCount++;
		        break;
		    case anica::CONFIDENCE_HIGH: 
		        highLabeledTransitionsCount++;
		        break;
		    case anica::CONFIDENCE_DOWN:
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

anica::TriplePointer* anica::AnicaLib::addCausalPattern(pnapi::PetriNet& extendedNet, const anica::TriplePointer* tripel, bool insertArcs)
{   
    pnapi::Transition* lC = &extendedNet.createTransition();
    pnapi::Transition* hC = &extendedNet.createTransition();
    pnapi::Place* fired = &extendedNet.createPlace();
    pnapi::Place* goal = &extendedNet.createPlace();

    const std::string tripleName = "(" + tripel->place->getName() + ", " + tripel->high->getName() + ", " + tripel->low->getName() + ")";

    if (represantiveNames) {
        lC->setName("lC_" + tripleName);
        hC->setName("hC_" + tripleName);
        fired->setName("fired_" + tripleName);
        goal->setName("goal_" + tripleName);
    }

    TriplePointer* ret = new TriplePointer(goal, hC, lC);

    PNAPI_FOREACH(prePlace, tripel->high->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, hC));
    }
    PNAPI_FOREACH(postPlace, tripel->high->getPostset()) {
        newArcs.insert(std::make_pair(hC, *postPlace));
    }

    PNAPI_FOREACH(prePlace, tripel->low->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, lC));
    }

    PNAPI_FOREACH(preTransition, tripel->place->getPreset()) {
        if (((**preTransition).getName() != tripel->low->getName()) && ( (**preTransition).getName() != tripel->high->getName() )) {
            pnapi::Place* a = &extendedNet.createPlace("", 1);
            if (represantiveNames) {
                a->setName("p_" + (**preTransition).getName() + "_" + tripleName);
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
                a->setName("d_" + d->getName() + "_" + tripleName);
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

anica::TriplePointer* anica::AnicaLib::addConflictPattern(pnapi::PetriNet& extendedNet, const anica::TriplePointer* tripel, bool insertArcs)
{   
    pnapi::Transition* lC = &extendedNet.createTransition();
    pnapi::Transition* hC = &extendedNet.createTransition();
    pnapi::Place* fired = &extendedNet.createPlace();
    pnapi::Place* enabled = &extendedNet.createPlace("", 1);
    pnapi::Place* goal = &extendedNet.createPlace();

    const std::string tripleName = "(" + tripel->place->getName() + ", " + tripel->high->getName() + ", " + tripel->low->getName() + ")";

    if (represantiveNames) {
        lC->setName("lC_" + tripleName);
        hC->setName("hC_" + tripleName);
        fired->setName("fired_" + tripleName);
        enabled->setName("enabled_" +tripleName);
        goal->setName("goal_" + tripleName);
    }

    TriplePointer* ret = new TriplePointer(goal, hC, lC);

    PNAPI_FOREACH(prePlace, tripel->high->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, hC));
        newArcs.insert(std::make_pair(hC, *prePlace));
    }

    PNAPI_FOREACH(prePlace, tripel->low->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, lC));
    }

    PNAPI_FOREACH(preTransition, tripel->place->getPreset()) {
        if (((**preTransition).getName() != tripel->low->getName()) && ( (**preTransition).getName() != tripel->high->getName() )) {
            pnapi::Place* a = &extendedNet.createPlace("", 1);
            if (represantiveNames) {
                a->setName("p_" + (**preTransition).getName() + "_" + tripleName);
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
                a->setName("d_" + d->getName() + "_" + tripleName);
            }
            newArcs.insert(std::make_pair(d, a));
            newArcs.insert(std::make_pair(a, d));
            newArcs.insert(std::make_pair(a, hC));
        }
    }

    newArcs.insert(std::make_pair(hC, fired));
    newArcs.insert(std::make_pair(fired, lC));
    newArcs.insert(std::make_pair(lC, goal)); 
    newArcs.insert(std::make_pair(enabled, const_cast<pnapi::Transition*>(tripel->high)));
    newArcs.insert(std::make_pair(const_cast<pnapi::Transition*>(tripel->high), enabled));
    newArcs.insert(std::make_pair(enabled, hC));

    if (insertArcs) {
        PNAPI_FOREACH(a, newArcs) {
            extendedNet.createArc(*a->first, *a->second);
        }
        newArcs.clear();
    }

    return ret;
}

bool anica::AnicaLib::isSecure()
{
    assert(initialNet != NULL);
    assert(lolaPath != "");
    
    assert(unassignedTransitions.size() == 0);
    if (propertyToCheck != anica::PROPERTY_PBNID && downgradeTransitions.size() > 0) {
        assert(false);
    }
    
    const size_t transitionsCount = initialNet->getTransitions().size();
    if (highLabeledTransitionsCount == transitionsCount) {
        return true;
    }
    if (lowLabeledTransitionsCount == transitionsCount) {
        return true;
    }
       
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::string placeName = (**p).getName();
        if (isActiveCausalPlace(placeName)) {
            return false;
        }
        if (isActiveConflictPlace(placeName)) {
            return false;
        }
    }
    return true;
}

bool anica::AnicaLib::isActiveCausalTriple(const anica::Triple& triple)
{
    assert(initialNet != NULL);
    assert(triple.place != "");
    assert(triple.high != "");
    assert(triple.low != "");
    
    const anica::TriplePointer* taskTriple = new anica::TriplePointer(initialNet->findPlace(triple.place), initialNet->findTransition(triple.high), initialNet->findTransition(triple.low));
 
    const bool ret = isActiveCausalTriple(taskTriple);   
    delete taskTriple;
    
    return ret;
}

bool anica::AnicaLib::isActiveConflictTriple(const anica::Triple& triple)
{
    assert(initialNet != NULL);
    assert(triple.place != "");
    assert(triple.high != "");
    assert(triple.low != "");
    
    const anica::TriplePointer* taskTriple = new anica::TriplePointer(initialNet->findPlace(triple.place), initialNet->findTransition(triple.high), initialNet->findTransition(triple.low));
 
    const bool ret = isActiveConflictTriple(taskTriple);   
    delete taskTriple;
    
    return ret;
}

const anica::Triple* anica::AnicaLib::addCausalPattern(pnapi::PetriNet& net, const Triple& triple)
{
    assert(initialNet != NULL);
    assert(triple.place != "");
    assert(triple.high != "");
    assert(triple.low != "");
    
    newArcs.clear();
    const anica::TriplePointer* taskTriple = new anica::TriplePointer(net.findPlace(triple.place), net.findTransition(triple.high), net.findTransition(triple.low));
    const anica::TriplePointer* resultTriple = addCausalPattern(net, taskTriple, true);
    
    delete taskTriple;
    const anica::Triple* returnTriple = new anica::Triple(resultTriple->place->getName(), resultTriple->high->getName(), resultTriple->low->getName());
    delete resultTriple;
    
    return returnTriple;
}

const anica::Triple* anica::AnicaLib::addConflictPattern(pnapi::PetriNet& net, const Triple& triple)
{
    assert(initialNet != NULL);
    assert(triple.place != "");
    assert(triple.high != "");
    assert(triple.low != "");
    
    newArcs.clear();
    const anica::TriplePointer* taskTriple = new anica::TriplePointer(net.findPlace(triple.place), net.findTransition(triple.high), net.findTransition(triple.low));
    const anica::TriplePointer* resultTriple = addConflictPattern(net, taskTriple, true);
    
    delete taskTriple;
    const anica::Triple* returnTriple = new anica::Triple(resultTriple->place->getName(), resultTriple->high->getName(), resultTriple->low->getName());
    delete resultTriple;
    
    return returnTriple;
}

Cudd* anica::AnicaLib::getCharacterization(char** cuddVariableNames, BDD* cuddOutput, std::map<std::string, BDD>& cuddVariables)
{
    assert(initialNet != NULL);
    assert(downgradeTransitions.size() == 0);
    
    Cudd* cuddManager = new Cudd();
    //std::map<std::string, BDD> cuddVariables;
	cuddVariables.clear();
	
    cuddManager->AutodynEnable(CUDD_REORDER_GROUP_SIFT_CONV);
    *cuddOutput = cuddManager->bddOne();
    size_t i = 0;
    PNAPI_FOREACH(t, initialNet->getTransitions()) {
        cuddVariables[(**t).getName()] = cuddManager->bddVar();
        cuddVariableNames[i++] = strdup((**t).getName().c_str());
        // encode given assignments
        switch ((**t).getConfidence()) {
            case anica::CONFIDENCE_HIGH:
                *cuddOutput *= cuddVariables[(**t).getName()];
                break;
            case anica::CONFIDENCE_LOW:
                *cuddOutput *= !cuddVariables[(**t).getName()];
                break;
            case anica::CONFIDENCE_NONE:
                break;
            default:
                assert(false);
        }
    }
    
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::set<pnapi::Node*> postset = (*p)->getPostset();
        // check potential causal triples
        PNAPI_FOREACH(highTransition, (*p)->getPreset()) {
            const int highConfidence = ((pnapi::Transition*)(*highTransition))->getConfidence();
            assert(highConfidence != anica::CONFIDENCE_DOWN);
            if (highConfidence == anica::CONFIDENCE_HIGH || highConfidence == anica::CONFIDENCE_NONE) {
                // current preTransition may be part of a causal triple
                PNAPI_FOREACH(lowTransition, postset) {
                    const int lowConfidence = ((pnapi::Transition*)(*lowTransition))->getConfidence();
                    assert(lowConfidence != anica::CONFIDENCE_DOWN);
                    if (lowConfidence == anica::CONFIDENCE_LOW || lowConfidence == anica::CONFIDENCE_NONE) {
                        // potential causal triple
                        const anica::TriplePointer* taskTriple = new anica::TriplePointer((pnapi::Place*)*p, (pnapi::Transition*)*highTransition, (pnapi::Transition*)*lowTransition);
                        if (isActiveCausalTriple(taskTriple)) {
                            *cuddOutput *= !(cuddVariables[(**highTransition).getName()] * !cuddVariables[(**lowTransition).getName()]);  
                        }
                        delete taskTriple;
                    }
                }
            }
        } 
        // check potential conflict triples
        PNAPI_FOREACH(highTransition, postset) {
            const int highConfidence = ((pnapi::Transition*)(*highTransition))->getConfidence();
            assert(highConfidence != anica::CONFIDENCE_DOWN);
            if (highConfidence == anica::CONFIDENCE_HIGH || highConfidence == anica::CONFIDENCE_NONE) {
                // current highTransition may be part of a causal triple
                PNAPI_FOREACH(lowTransition, postset) {
                    if (lowTransition != highTransition) {
                        // low and high transition must be different ones
                        const int lowConfidence = ((pnapi::Transition*)(*lowTransition))->getConfidence();
                        // potential conflict triple
                        const anica::TriplePointer* taskTriple = new anica::TriplePointer((pnapi::Place*)*p, (pnapi::Transition*)*highTransition, (pnapi::Transition*)*lowTransition);
                        if (isActiveConflictTriple(taskTriple)) {
                            *cuddOutput *= !(cuddVariables[(**highTransition).getName()] * !cuddVariables[(**lowTransition).getName()]);  
                        }
                        delete taskTriple;
                    }
                }
            }
        }
    }   
    
    return cuddManager;   	   
}
