#include <sstream>

#include "AnicaLib.h"

namespace anica {

AnicaLib::TriplePointer::TriplePointer(const pnapi::Place* s, const pnapi::Transition* h, const pnapi::Transition* l)
    :
    place(s),
    high(h),
    low(l)
{}

AnicaLib::TriplePointer::~TriplePointer()
{
    place = NULL;
    high = NULL;
    low = NULL;
}

Triple::Triple(const std::string& s, const std::string& h, const std::string& l)
    :
    place(s),
    high(h),
    low(l)
{}

Triple::~Triple()
{}

AnicaLib::AnicaLib(const pnapi::PetriNet& net)
    :
    initialNet(new pnapi::PetriNet(net)),
    represantiveNames(false),
    lolaPath(""),
    lolaWitnessPath(false),
    lolaVerbose(false),
    oneTripleOnly(true),
    oneActiveOnly(true),
    propertyToCheck(PROPERTY_PBNIPLUS),
    highLabeledTransitionsCount(0),
    lowLabeledTransitionsCount(0),
    inputName("_undefined_")
{
    newArcs.clear();
    downgradeTransitions.clear();
    unassignedTransitions.clear();
    
    PNAPI_FOREACH(t, initialNet->getTransitions()) {
        switch ((**t).getConfidence()) {
		    case CONFIDENCE_NONE:
                unassignedTransitions.insert(*t);
		        break;
		    case CONFIDENCE_LOW:
		        lowLabeledTransitionsCount++;
		        break;
		    case CONFIDENCE_HIGH: 
		        highLabeledTransitionsCount++;
		        break;
		    case CONFIDENCE_DOWN:
		        downgradeTransitions.insert(*t);
		        break;
		    default:
		        assert(false);
		} 
    }
}

AnicaLib::~AnicaLib()
{
    delete initialNet;
    initialNet = NULL;
}

const size_t AnicaLib::getUnassignedTransitionsCount() const {
    return unassignedTransitions.size();
}

const size_t AnicaLib::getHighLabeledTransitionsCount() const {
    return highLabeledTransitionsCount;
}

const size_t AnicaLib::getLowLabeledTransitionsCount() const {
    return lowLabeledTransitionsCount;
}

const size_t AnicaLib::getDownLabeledTransitionsCount() const {
    return downgradeTransitions.size();
}

void AnicaLib::setProperty(property_e property)
{
    propertyToCheck = property;
}

void AnicaLib::setInputName(const std::string& name)
{
    inputName = name;
}

const int AnicaLib::getProperty() const
{
    return propertyToCheck;
}

void AnicaLib::setRepresantiveNames(bool r)
{
    represantiveNames = r;
}

const bool AnicaLib::getRepresantiveNames() const
{
    return represantiveNames;
}

void AnicaLib::setOneTripleOnly(bool r)
{
    oneTripleOnly = r;
}

const bool AnicaLib::getOneTripleOnly() const
{
    return oneTripleOnly;
}

void AnicaLib::setOneActiveOnly(bool r)
{
    oneActiveOnly = r;
}

const bool AnicaLib::getOneActiveOnly() const
{
    return oneActiveOnly;
}

void AnicaLib::setLolaPath(const std::string& path)
{
    lolaPath = std::string(path);
}

const std::string& AnicaLib::getLolaPath() const
{
    return lolaPath;
}

void AnicaLib::setLolaWitnessPath(bool w)
{
    lolaWitnessPath = w;
}

const bool AnicaLib::getLolaWitnessPath() const
{
    return lolaWitnessPath;
}

void AnicaLib::setLolaVerbose(bool v)
{
    lolaVerbose = v;
}

const bool AnicaLib::getLolaVerbose() const
{
    return lolaVerbose;
}

void AnicaLib::setTransitionAssignment(pnapi::Transition* t, confidence_e c)
{
    assert(t != NULL);
    
    if (t->getConfidence() == c) {
        return;
    }
    
    switch (t->getConfidence()) {
        case CONFIDENCE_NONE:
            unassignedTransitions.erase(const_cast<pnapi::Transition*>(t));
	        break;
	    case CONFIDENCE_LOW:
	        lowLabeledTransitionsCount--;
	        break;
	    case CONFIDENCE_HIGH: 
	        highLabeledTransitionsCount--;
	        break;
	    case CONFIDENCE_DOWN:
	        downgradeTransitions.erase(const_cast<pnapi::Transition*>(t));
	        break;
	    default:
	        assert(false);
	}
	
	t->setConfidence(c);
	switch (c) {
        case CONFIDENCE_NONE:
            unassignedTransitions.insert(const_cast<pnapi::Transition*>(t));
	        break;
	    case CONFIDENCE_LOW:
	        lowLabeledTransitionsCount++;
	        break;
	    case CONFIDENCE_HIGH: 
	        highLabeledTransitionsCount++;
	        break;
	    case CONFIDENCE_DOWN:
	        downgradeTransitions.insert(const_cast<pnapi::Transition*>(t));
	}
}

void AnicaLib::assignTransition(const std::string& t, confidence_e c) {
    pnapi::Transition* transition = initialNet->findTransition(t);
    if (UNLIKELY(transition == NULL)) {
        throw exceptions::InputError(exceptions::InputError::IE_UNKNOWN_TRANSITION, t);
    }
    
    setTransitionAssignment(transition, c);
}

void AnicaLib::assignUnassignedTransitions(confidence_e c) {
    PNAPI_FOREACH(t, initialNet->getTransitions()) {
        if ((**t).getConfidence() == CONFIDENCE_NONE) {
            setTransitionAssignment(*t, c);
        }
    }
}

size_t AnicaLib::isPotentialCausalPlace(const pnapi::Place* place) const {
    assert(place != NULL);
    
    bool hasHighPredecessor = false;
    size_t ret = 0;
    
    if (oneTripleOnly) {
        // fast check
        PNAPI_FOREACH(t, place->getPreset()) {
            if (((pnapi::Transition*)(*t))->getConfidence() == CONFIDENCE_HIGH) {
                hasHighPredecessor = true;
                break;
            }
        }
        
        if (hasHighPredecessor) {
            PNAPI_FOREACH(t, place->getPostset()) {
                if (((pnapi::Transition*)(*t))->getConfidence() == CONFIDENCE_LOW) {
                    ret++;
                    break;
                }
            }
        }
        
    }
    else {
        // count all potential triples
        PNAPI_FOREACH(t, place->getPreset()) {
            if (((pnapi::Transition*)(*t))->getConfidence() == CONFIDENCE_HIGH) {
                PNAPI_FOREACH(t, place->getPostset()) {
                    if (((pnapi::Transition*)(*t))->getConfidence() == CONFIDENCE_LOW) {
                        ret++;
                    }
                }
            }
        }
    }
    
    return ret;
}

size_t AnicaLib::isPotentialConflictPlace(const pnapi::Place* place) const {
    assert(place != NULL);
    
    bool hasHighSuccessor = false;
    bool hasLowSuccessor = false;
    
    size_t ret = 0;
    
    if (oneTripleOnly) {
        PNAPI_FOREACH(t, place->getPostset()) {
            const int c = ((pnapi::Transition*)(*t))->getConfidence();
            if (c == CONFIDENCE_HIGH) {
                hasHighSuccessor = true;
            }
            if (c == CONFIDENCE_LOW) {
                hasLowSuccessor = true;
            }
            if (hasHighSuccessor && hasLowSuccessor) {
                ret++;
                break;
            }
        }
    }
    else {
        const std::set<pnapi::Node*> postset = place->getPostset();
        PNAPI_FOREACH(h, postset) {
            if (((pnapi::Transition*)(*h))->getConfidence() == CONFIDENCE_HIGH) {
                PNAPI_FOREACH(l, postset) {
                    if (((pnapi::Transition*)(*l))->getConfidence() == CONFIDENCE_LOW) {
                        ret++;
                    }
                }
            }
        }
    }
    
    return ret;
}

size_t AnicaLib::isActiveCausalPlace(const pnapi::PetriNet& net, const pnapi::Place* place) {
    assert(place != NULL);
    
    TriplePointer* resultTriple;
    size_t ret = 0;
    
    PNAPI_FOREACH(h, place->getPreset()) {
        const int presetConfidence = ((pnapi::Transition*)(*h))->getConfidence();
        assert(presetConfidence != CONFIDENCE_NONE); 
        if (presetConfidence == CONFIDENCE_HIGH) {
            PNAPI_FOREACH(l, place->getPostset()) {
                const int postsetConfidence = ((pnapi::Transition*)(*l))->getConfidence();
                assert(postsetConfidence != CONFIDENCE_NONE); 
                if (postsetConfidence == CONFIDENCE_LOW) {
                    TriplePointer* triple = new TriplePointer(place, (pnapi::Transition*)*h, (pnapi::Transition*)*l); 
                    if (isActiveCausalTriple(net, triple)) {
                        ret++;
                    }
                    delete triple;
                    if ((!oneTripleOnly) && (ret > 0)) {
                        return ret;
                    }
                }
            }
        }
    }
    
    return ret;
}

size_t AnicaLib::isActiveConflictPlace(const pnapi::PetriNet& net, const pnapi::Place* place) {
    assert(place != NULL);
    
    TriplePointer* resultTriple;
    size_t ret = 0;
    
    const std::set<pnapi::Node*> postset = place->getPostset();
    
    PNAPI_FOREACH(h, postset) {
        const int postsetConfidence = ((pnapi::Transition*)(*h))->getConfidence();
        assert(postsetConfidence != CONFIDENCE_NONE); 
        if (postsetConfidence == CONFIDENCE_HIGH) {
            PNAPI_FOREACH(l, postset) {
                if (((pnapi::Transition*)(*l))->getConfidence() == CONFIDENCE_LOW) {
                    TriplePointer* triple = new TriplePointer(place, (pnapi::Transition*)*h, (pnapi::Transition*)*l); 
                    if (isActiveConflictTriple(net, triple)) {
                        ret++;
                    }
                    delete triple;
                    if ((!oneTripleOnly) && (ret > 0)) {
                        return ret;
                    }
                }
            }
        }
    }
    
    return ret;
}

size_t AnicaLib::isPotentialCausalPlace(const std::string& p) const {
    const pnapi::Place* place = initialNet->findPlace(p);
    if (UNLIKELY(place == NULL)) {
        throw exceptions::InputError(exceptions::InputError::IE_UNKNOWN_PLACE, p);
    }
    
    return isPotentialCausalPlace(place);
}

size_t AnicaLib::isPotentialConflictPlace(const std::string& p) const {
    const pnapi::Place* place = initialNet->findPlace(p);
    if (UNLIKELY(place == NULL)) {
        throw exceptions::InputError(exceptions::InputError::IE_UNKNOWN_PLACE, p);
    }
    
    return isPotentialConflictPlace(place);
}

size_t AnicaLib::isActiveCausalPlace(const std::string& p) {
    const pnapi::Place* place = initialNet->findPlace(p);
    if (UNLIKELY(place == NULL)) {
        throw exceptions::InputError(exceptions::InputError::IE_UNKNOWN_PLACE, p);
    }
    
    return isActiveCausalPlace(*initialNet, place);
}

size_t AnicaLib::isActiveConflictPlace(const std::string& p) {
    const pnapi::Place* place = initialNet->findPlace(p);
    if (UNLIKELY(place == NULL)) {
        throw exceptions::InputError(exceptions::InputError::IE_UNKNOWN_PLACE, p);
    }
    
    return isActiveConflictPlace(*initialNet, place);
}

std::set<std::string> AnicaLib::getAllPotentialCausalPlaces() const {
    std::set<std::string> returnSet;
    
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::string placeName = (**p).getName();
        if (isPotentialCausalPlace(placeName)) {
            returnSet.insert(placeName);
        }
    }
    
    return std::set<std::string>(returnSet);
}

std::set<std::string> AnicaLib::getAllPotentialConflictPlaces() const {
    std::set<std::string> returnSet;
    
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::string placeName = (**p).getName();
        if (isPotentialConflictPlace(placeName)) {
            returnSet.insert(placeName);
        }
    }
    
    return std::set<std::string>(returnSet);
}

std::set<std::string> AnicaLib::getAllPotentialPlaces() const {
    std::set<std::string> returnSet;
    
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::string placeName = (**p).getName();
        if (isPotentialCausalPlace(placeName) || isPotentialConflictPlace(placeName)) {
            returnSet.insert(placeName);
        }
    }
    
    return std::set<std::string>(returnSet);
}

std::set<std::string> AnicaLib::getAllActiveCausalPlaces() {
    if (UNLIKELY(lolaPath == "")) {
        throw exceptions::ToolError("loLA", "path not set");
    }
    
    std::set<std::string> returnSet;
    
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::string placeName = (**p).getName();
        if (isActiveCausalPlace(placeName)) {
            returnSet.insert(placeName);
        }
    }
    
    return std::set<std::string>(returnSet);
}

std::set<std::string> AnicaLib::getAllActiveConflictPlaces() {
    if (UNLIKELY(lolaPath == "")) {
        throw exceptions::ToolError("loLA", "path not set");
    }
    
    std::set<std::string> returnSet;
    
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::string placeName = (**p).getName();
        if (isActiveConflictPlace(placeName)) {
            returnSet.insert(placeName);
        }
    }
    
    return std::set<std::string>(returnSet);
}

std::set<std::string> AnicaLib::getAllActivePlaces() {
    if (UNLIKELY(lolaPath == "")) {
        throw exceptions::ToolError("loLA", "path not set");
    }
    
    std::set<std::string> returnSet;
    
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::string placeName = (**p).getName();
        if (isActiveCausalPlace(placeName) || isActiveConflictPlace(placeName)) {
            returnSet.insert(placeName);
        }
    }
    
    return std::set<std::string>(returnSet);
}

bool AnicaLib::isActiveCausalTriple(const pnapi::PetriNet& net, const AnicaLib::TriplePointer* triple)
{
    assert(triple->place != NULL);
    assert(triple->high != NULL);
    assert(triple->low != NULL);
    
    pnapi::PetriNet* taskNet = new pnapi::PetriNet(net);
    newArcs.clear();
    TriplePointer* taskTriple = new TriplePointer(taskNet->findPlace(triple->place->getName()), taskNet->findTransition(triple->high->getName()), taskNet->findTransition(triple->low->getName()));
    TriplePointer* resultTriple = addCausalPattern(*taskNet, taskTriple, true);
    
    int ret = -1;
    
    if (lolaWitnessPath) {
        ret = callLoLA(*taskNet, resultTriple->place, std::string(inputName + "_[" + triple->place->getName() + "," + triple->high->getName() + "," + triple->low->getName() + "].path"));
    }
    else {
        ret = callLoLA(*taskNet, resultTriple->place);
    }
    
    delete taskTriple;
    delete taskNet;
    delete resultTriple;

    switch(ret) {
        case 0:
            return true;
        case 1:
            return false;
        default:
            throw exceptions::ToolError("loLA", "unknown return value");
    }
}

bool AnicaLib::isActiveConflictTriple(const pnapi::PetriNet& net, const AnicaLib::TriplePointer* triple)
{
    assert(triple->place != NULL);
    assert(triple->high != NULL);
    assert(triple->low != NULL);
    
    pnapi::PetriNet* taskNet = new pnapi::PetriNet(net);
    newArcs.clear();
    TriplePointer* taskTriple = new TriplePointer(taskNet->findPlace(triple->place->getName()), taskNet->findTransition(triple->high->getName()), taskNet->findTransition(triple->low->getName()));
    TriplePointer* resultTriple = addConflictPattern(*taskNet, taskTriple, true);
    
    int ret = -1;
    
    if (lolaWitnessPath) {
        ret = callLoLA(*taskNet, resultTriple->place, std::string(inputName + "_[" + triple->place->getName() + "," + triple->high->getName() + "," + triple->low->getName() + "].path"));
    }
    else {
        ret = callLoLA(*taskNet, resultTriple->place);
    }
    
    delete taskTriple;
    delete taskNet;
    delete resultTriple;

    switch(ret) {
        case 0:
            return true;
        case 1:
            return false;
        default:
            throw exceptions::ToolError("loLA", "unknown return value");
    }
}

int AnicaLib::callLoLA(const pnapi::PetriNet& net, const pnapi::Place* goal, const std::string& fileName) const {
    if (UNLIKELY(lolaPath == "")) {
        throw exceptions::ToolError("loLA", "path not set");
    }
	
	std::stringstream ss;
	ss << pnapi::io::lola << net;
	ss << "\n\nFORMULA (" << goal->getName() << " = 1 )\n";

	// select LoLA binary and build LoLA command 
    #if defined(__MINGW32__) 
    // // MinGW does not understand pathnames with "/", so we use the basename 
    const std::string command_line = "\"" + lolaPath + (lolaWitnessPath ? " --path=" + fileName + " " : "") + (lolaVerbose ? "" : " 2> \dev\null"); 
    #else 
    const std::string command_line = lolaPath + (lolaWitnessPath ? " --path=" + fileName + " " : "") + (lolaVerbose ? "" : " 2> /dev/null"); 
    #endif 

	FILE *fp = popen(command_line.c_str(), "w");
	if (UNLIKELY(fp == NULL)) {
	    throw exceptions::ToolError("loLA", "pipe open failure");
	}
	fprintf(fp, "%s", ss.str().c_str());

	return (pclose(fp) / 256);
}


int AnicaLib::callLoLA(const pnapi::PetriNet& net, const pnapi::Place* goal) const {
    if (UNLIKELY(lolaWitnessPath)) {
        throw exceptions::UserError("witness paths require unique file names");
    }
    return callLoLA(net, goal, inputName);
}

AnicaLib::TriplePointer* AnicaLib::addCausalPattern(pnapi::PetriNet& extendedNet, const AnicaLib::TriplePointer* triple, bool insertArcs)
{   
    pnapi::Transition* lC = &extendedNet.createTransition();
    pnapi::Transition* hC = &extendedNet.createTransition();
    pnapi::Place* fired = &extendedNet.createPlace();
    pnapi::Place* goal = &extendedNet.createPlace();

    const std::string tripleName = "<" + triple->place->getName() + "_" + triple->high->getName() + "_" + triple->low->getName() + ">";

    if (represantiveNames) {
        lC->setName("lC_" + tripleName);
        hC->setName("hC_" + tripleName);
        fired->setName("fired_" + tripleName);
        goal->setName("goal_" + tripleName);
    }

    TriplePointer* ret = new TriplePointer(goal, hC, lC);

    PNAPI_FOREACH(prePlace, triple->high->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, hC));
    }
    PNAPI_FOREACH(postPlace, triple->high->getPostset()) {
        newArcs.insert(std::make_pair(hC, *postPlace));
    }

    PNAPI_FOREACH(prePlace, triple->low->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, lC));
    }

    PNAPI_FOREACH(preTransition, triple->place->getPreset()) {
        if (((**preTransition).getName() != triple->low->getName()) && ( (**preTransition).getName() != triple->high->getName() )) {
            pnapi::Place* a = &extendedNet.createPlace("", 1);
            if (represantiveNames) {
                a->setName("p_" + (**preTransition).getName() + "_" + tripleName);
            }
            newArcs.insert(std::make_pair(*preTransition, a));
            newArcs.insert(std::make_pair(a, *preTransition));
            newArcs.insert(std::make_pair(a, hC));
        }
    }

    if (propertyToCheck == PROPERTY_PBNID) {
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

AnicaLib::TriplePointer* AnicaLib::addConflictPattern(pnapi::PetriNet& extendedNet, const AnicaLib::TriplePointer* triple, bool insertArcs)
{   
    pnapi::Transition* lC = &extendedNet.createTransition();
    pnapi::Transition* hC = &extendedNet.createTransition();
    pnapi::Place* fired = &extendedNet.createPlace();
    pnapi::Place* enabled = &extendedNet.createPlace("", 1);
    pnapi::Place* goal = &extendedNet.createPlace();

    const std::string tripleName = "<" + triple->place->getName() + "_" + triple->high->getName() + "_" + triple->low->getName() + ">";

    if (represantiveNames) {
        lC->setName("lC_" + tripleName);
        hC->setName("hC_" + tripleName);
        fired->setName("fired_" + tripleName);
        enabled->setName("enabled_" +tripleName);
        goal->setName("goal_" + tripleName);
    }

    TriplePointer* ret = new TriplePointer(goal, hC, lC);

    PNAPI_FOREACH(prePlace, triple->high->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, hC));
        newArcs.insert(std::make_pair(hC, *prePlace));
    }

    PNAPI_FOREACH(prePlace, triple->low->getPreset()) {
        newArcs.insert(std::make_pair(*prePlace, lC));
    }

    PNAPI_FOREACH(preTransition, triple->place->getPreset()) {
        if (((**preTransition).getName() != triple->low->getName()) && ( (**preTransition).getName() != triple->high->getName() )) {
            pnapi::Place* a = &extendedNet.createPlace("", 1);
            if (represantiveNames) {
                a->setName("p_" + (**preTransition).getName() + "_" + tripleName);
            }
            newArcs.insert(std::make_pair(*preTransition, a));
            newArcs.insert(std::make_pair(a, *preTransition));
            newArcs.insert(std::make_pair(a, hC));
        }
    }

    if (propertyToCheck == PROPERTY_PBNID) {
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
    newArcs.insert(std::make_pair(enabled, const_cast<pnapi::Transition*>(triple->high)));
    newArcs.insert(std::make_pair(const_cast<pnapi::Transition*>(triple->high), enabled));
    newArcs.insert(std::make_pair(enabled, hC));

    if (insertArcs) {
        PNAPI_FOREACH(a, newArcs) {
            extendedNet.createArc(*a->first, *a->second);
        }
        newArcs.clear();
    }

    return ret;
}

size_t AnicaLib::isSecure()
{
    if (UNLIKELY(lolaPath == "")) {
        throw exceptions::ToolError("loLA", "path not set");
    }
    
    size_t ret = 0;
    
    if (UNLIKELY(unassignedTransitions.size() != 0)) {
        throw exceptions::UserError("verification requires complete assigned transitions");
    }
    if (propertyToCheck != PROPERTY_PBNID && downgradeTransitions.size() > 0) {
        throw exceptions::UserError("downgrade transitions requires PBNID");
    }
    
    const size_t transitionsCount = initialNet->getTransitions().size();
    if (highLabeledTransitionsCount == transitionsCount) {
        return ret;
    }
    if (lowLabeledTransitionsCount == transitionsCount) {
        return ret;
    }
       
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::string placeName = (**p).getName();
        if (isActiveCausalPlace(placeName)) {
            ret++;
            if (oneActiveOnly) {
                break;
            }
        }
        if (isActiveConflictPlace(placeName)) {
            ret++;
            if (oneActiveOnly) {
                break;
            }
        }
    }
    return ret;
}

bool AnicaLib::isActiveCausalTriple(const Triple& triple)
{
    if (UNLIKELY(triple.place == "")) {
        throw exceptions::InputError(exceptions::InputError::IE_INCOMPLETE_TRIPLE, "place");
    }
    if (UNLIKELY(triple.high == "")) {
        throw exceptions::InputError(exceptions::InputError::IE_INCOMPLETE_TRIPLE, "high");
    }
    if (UNLIKELY(triple.low == "")) {
        throw exceptions::InputError(exceptions::InputError::IE_INCOMPLETE_TRIPLE, "low");
    }
    
    const AnicaLib::TriplePointer* taskTriple = new AnicaLib::TriplePointer(initialNet->findPlace(triple.place), initialNet->findTransition(triple.high), initialNet->findTransition(triple.low));
 
    const bool ret = isActiveCausalTriple(*initialNet, taskTriple);   
    delete taskTriple;
    
    return ret;
}

bool AnicaLib::isActiveConflictTriple(const Triple& triple)
{
    if (UNLIKELY(triple.place == "")) {
        throw exceptions::InputError(exceptions::InputError::IE_INCOMPLETE_TRIPLE, "place");
    }
    if (UNLIKELY(triple.high == "")) {
        throw exceptions::InputError(exceptions::InputError::IE_INCOMPLETE_TRIPLE, "high");
    }
    if (UNLIKELY(triple.low == "")) {
        throw exceptions::InputError(exceptions::InputError::IE_INCOMPLETE_TRIPLE, "low");
    }
    
    const AnicaLib::TriplePointer* taskTriple = new AnicaLib::TriplePointer(initialNet->findPlace(triple.place), initialNet->findTransition(triple.high), initialNet->findTransition(triple.low));
 
    const bool ret = isActiveConflictTriple(*initialNet, taskTriple);   
    delete taskTriple;
    
    return ret;
}

pnapi::PetriNet* AnicaLib::getCurrentNet() const {
    return new pnapi::PetriNet(*initialNet);
}

pnapi::PetriNet* AnicaLib::getControllerProblem() {
    if (UNLIKELY(downgradeTransitions.size() != 0)) {
        throw exceptions::UserError("controller forbids downgrade transitions");
    }
    
    const std::string HIGH = "_high";
    const std::string LOW = "_low";
    
    pnapi::PetriNet* resultNet = new pnapi::PetriNet(*initialNet);
    
    newArcs.clear();
    setRepresantiveNames(true);
    
    pnapi::Transition* startTransition = &resultNet->createTransition("tStart");
    startTransition->addLabel(resultNet->getInterface().addSynchronousLabel("start"));
    pnapi::Place* preStart = &resultNet->createPlace("preStart", 1);
    resultNet->getFinalCondition().addProposition(*preStart == 0, true);
    newArcs.insert(std::make_pair(preStart, startTransition));
    // update initial marking
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        if ((**p).getTokenCount() > 0) {
            assert((**p).getTokenCount() == 1);
            newArcs.insert(std::make_pair(startTransition, resultNet->findNode((**p).getName())));
            resultNet->findPlace((**p).getName())->setTokenCount(0);
        }
    }
    // controller structure for all transitions
    PNAPI_FOREACH(t, initialNet->getTransitions()) {
        switch ((**t).getConfidence()) {
            case CONFIDENCE_LOW:
                // low labeled
                resultNet->createPlace(std::string((**t).getName() + HIGH), 0);
                resultNet->createPlace(std::string((**t).getName() + LOW), 1);
                break;
            case CONFIDENCE_HIGH:
                // high labeled
                resultNet->createPlace(std::string((**t).getName() + HIGH), 1);
                resultNet->createPlace(std::string((**t).getName() + LOW), 0);
                break;
            case CONFIDENCE_NONE:
                // unlabeled transition
                pnapi::Place* controllerConfigure = &resultNet->createPlace(std::string("configure_" + (**t).getName()), 1);
                pnapi::Transition* controllerMakeHigh = &resultNet->createTransition(std::string("make_high_" + (**t).getName()));
                controllerMakeHigh->addLabel(resultNet->getInterface().addSynchronousLabel(std::string("make_" + (**t).getName() + "_high")));
                newArcs.insert(std::make_pair(&resultNet->createPlace(std::string("activate_make_" + (**t).getName()), 1), controllerMakeHigh));
                newArcs.insert(std::make_pair(controllerConfigure, controllerMakeHigh));
                newArcs.insert(std::make_pair(controllerMakeHigh, controllerConfigure));
                newArcs.insert(std::make_pair(controllerConfigure, startTransition));
                newArcs.insert(std::make_pair(&resultNet->createPlace(std::string((**t).getName() + LOW), 1), controllerMakeHigh));
                newArcs.insert(std::make_pair(controllerMakeHigh, &resultNet->createPlace(std::string((**t).getName() + HIGH), 0)));
                break;
        }
    }
    
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::set<pnapi::Node*> postset = (*p)->getPostset();
        // check potential causal triples
        PNAPI_FOREACH(highTransition, (*p)->getPreset()) {
            const int highConfidence = ((pnapi::Transition*)(*highTransition))->getConfidence();
            const std::string highName = ((pnapi::Transition*)(*highTransition))->getName();
            assert(highConfidence != CONFIDENCE_DOWN);
            if (highConfidence == CONFIDENCE_HIGH || highConfidence == CONFIDENCE_NONE) {
                // current preTransition may be part of a causal triple
                PNAPI_FOREACH(lowTransition, postset) {
                    const int lowConfidence = ((pnapi::Transition*)(*lowTransition))->getConfidence();
                    const std::string lowName = ((pnapi::Transition*)(*lowTransition))->getName();
                    assert(lowConfidence != CONFIDENCE_DOWN);
                    if (lowConfidence == CONFIDENCE_LOW || lowConfidence == CONFIDENCE_NONE) {
                        // potential causal triple
                        
                        AnicaLib::TriplePointer* inputtriple = new AnicaLib::TriplePointer(resultNet->findPlace((*p)->getName()), resultNet->findTransition(highName), resultNet->findTransition(lowName));
                        AnicaLib::TriplePointer* resulttriple = addCausalPattern(*resultNet, inputtriple, false);
                        
                        pnapi::Transition* newHigh = const_cast<pnapi::Transition*>(resulttriple->high);
                        pnapi::Transition* newLow = const_cast<pnapi::Transition*>(resulttriple->low);
                        
                        pnapi::Place * controllerLow = resultNet->findPlace(std::string(lowName + LOW));
                        pnapi::Place * controllerHigh = resultNet->findPlace(std::string(highName + HIGH));
                        newArcs.insert(std::make_pair(controllerHigh, newHigh));
                        newArcs.insert(std::make_pair(newHigh, controllerHigh));
                        newArcs.insert(std::make_pair(controllerLow, newLow));
                        newArcs.insert(std::make_pair(newLow, controllerLow));
                        
                        delete inputtriple;
                        delete resulttriple;
                    }
                }
            }
        } 
        // check potential conflict triples
        PNAPI_FOREACH(highTransition, postset) {
            const int highConfidence = ((pnapi::Transition*)(*highTransition))->getConfidence();
            const std::string highName = ((pnapi::Transition*)(*highTransition))->getName();
            assert(highConfidence != CONFIDENCE_DOWN);
            if (highConfidence == CONFIDENCE_HIGH || highConfidence == CONFIDENCE_NONE) {
                // current highTransition may be part of a causal triple
                PNAPI_FOREACH(lowTransition, postset) {
                    if (lowTransition != highTransition) {
                        // low and high transition must be different ones
                        const int lowConfidence = ((pnapi::Transition*)(*lowTransition))->getConfidence();
                        const std::string lowName = ((pnapi::Transition*)(*lowTransition))->getName();
                        // potential conflict triple
                        
                        AnicaLib::TriplePointer* inputtriple = new AnicaLib::TriplePointer(resultNet->findPlace((*p)->getName()), resultNet->findTransition(highName), resultNet->findTransition(lowName));
                        AnicaLib::TriplePointer* resulttriple = addConflictPattern(*resultNet, inputtriple, false);
                        
                        pnapi::Transition* newHigh = const_cast<pnapi::Transition*>(resulttriple->high);
                        pnapi::Transition* newLow = const_cast<pnapi::Transition*>(resulttriple->low);
                        
                        pnapi::Place * controllerLow = resultNet->findPlace(std::string(lowName + LOW));
                        pnapi::Place * controllerHigh = resultNet->findPlace(std::string(highName + HIGH));
                        newArcs.insert(std::make_pair(controllerHigh, newHigh));
                        newArcs.insert(std::make_pair(newHigh, controllerHigh));
                        newArcs.insert(std::make_pair(controllerLow, newLow));
                        newArcs.insert(std::make_pair(newLow, controllerLow));
                        
                        delete inputtriple;
                        delete resulttriple;
                    }
                }
            }
        }
    }
    
    // add collected arcs
    PNAPI_FOREACH(a, newArcs) {
        resultNet->createArc(*a->first, *a->second);
    }
    
    return resultNet;
}

Cudd* AnicaLib::getCharacterization(char** cuddVariableNames, BDD* cuddOutput, std::map<std::string, int>& cuddVariables)
{
    if (UNLIKELY(downgradeTransitions.size() != 0)) {
        throw exceptions::UserError("characterization forbids downgrade transitions");
    }
    
    Cudd* cuddManager = new Cudd();
	cuddVariables.clear();
	
	cuddManager->AutodynEnable(CUDD_REORDER_GROUP_SIFT_CONV);
    //cuddManager->AutodynEnable(CUDD_REORDER_EXACT);
    *cuddOutput = cuddManager->bddOne();
    size_t i = 0;
    PNAPI_FOREACH(t, initialNet->getTransitions()) {
        cuddVariables[(**t).getName()] = i;
        cuddManager->bddVar(i);
        cuddVariableNames[i] = strdup((**t).getName().c_str());
        // encode given assignments
        switch ((**t).getConfidence()) {
            case CONFIDENCE_HIGH:
                *cuddOutput *= cuddManager->bddVar(i);
                break;
            case CONFIDENCE_LOW:
                *cuddOutput *= !cuddManager->bddVar(i);
                break;
            case CONFIDENCE_NONE:
                break;
            default:
                assert(false);
        }
        i++;
    }
    
    PNAPI_FOREACH(p, initialNet->getPlaces()) {
        const std::set<pnapi::Node*> postset = (*p)->getPostset();
        // check potential causal triples
        PNAPI_FOREACH(highTransition, (*p)->getPreset()) {
            const int highConfidence = ((pnapi::Transition*)(*highTransition))->getConfidence();
            assert(highConfidence != CONFIDENCE_DOWN);
            if (highConfidence == CONFIDENCE_HIGH || highConfidence == CONFIDENCE_NONE) {
                // current preTransition may be part of a causal triple
                PNAPI_FOREACH(lowTransition, postset) {
                    const int lowConfidence = ((pnapi::Transition*)(*lowTransition))->getConfidence();
                    assert(lowConfidence != CONFIDENCE_DOWN);
                    if (lowConfidence == CONFIDENCE_LOW || lowConfidence == CONFIDENCE_NONE) {
                        // potential causal triple
                        const AnicaLib::TriplePointer* taskTriple = new AnicaLib::TriplePointer((pnapi::Place*)*p, (pnapi::Transition*)*highTransition, (pnapi::Transition*)*lowTransition);
                        if (isActiveCausalTriple(*initialNet, taskTriple)) {
                            *cuddOutput *= !(cuddManager->bddVar(cuddVariables[(**highTransition).getName()]) * !cuddManager->bddVar(cuddVariables[(**lowTransition).getName()]));
                        }
                        delete taskTriple;
                    }
                }
            }
        } 
        // check potential conflict triples
        PNAPI_FOREACH(highTransition, postset) {
            const int highConfidence = ((pnapi::Transition*)(*highTransition))->getConfidence();
            assert(highConfidence != CONFIDENCE_DOWN);
            if (highConfidence == CONFIDENCE_HIGH || highConfidence == CONFIDENCE_NONE) {
                // current highTransition may be part of a causal triple
                PNAPI_FOREACH(lowTransition, postset) {
                    if (lowTransition != highTransition) {
                        // low and high transition must be different ones
                        const int lowConfidence = ((pnapi::Transition*)(*lowTransition))->getConfidence();
                        // potential conflict triple
                        const AnicaLib::TriplePointer* taskTriple = new AnicaLib::TriplePointer((pnapi::Place*)*p, (pnapi::Transition*)*highTransition, (pnapi::Transition*)*lowTransition);
                        if (isActiveConflictTriple(*initialNet, taskTriple)) {
                            *cuddOutput *= !(cuddManager->bddVar(cuddVariables[(**highTransition).getName()]) * !cuddManager->bddVar(cuddVariables[(**lowTransition).getName()]));  
                        }
                        delete taskTriple;
                    }
                }
            }
        }
    }   
    
    return cuddManager;   	   
}

pnapi::PetriNet* AnicaLib::colorPotentialPlaces(colorPlaceTask_e task) {
    pnapi::PetriNet* resultNet = new pnapi::PetriNet(*initialNet);
    colorPotentialPlaces(*resultNet, task);
    return resultNet;
}

pnapi::PetriNet* AnicaLib::colorActivePlaces(colorPlaceTask_e task) {
    pnapi::PetriNet* resultNet = new pnapi::PetriNet(*initialNet);
    colorActivePlaces(*resultNet, task);
    return resultNet;
}

pnapi::PetriNet* AnicaLib::colorConfidence(colorConfidenceTask_e task) {
    pnapi::PetriNet* resultNet = new pnapi::PetriNet(*initialNet);
    colorConfidence(*resultNet, task);
    return resultNet;
}

void AnicaLib::clearColors(pnapi::PetriNet& net) {
    PNAPI_FOREACH(node, net.getNodes()) {
        (**node).setColor("");
    }
}

void AnicaLib::colorPotentialPlaces(pnapi::PetriNet& net, colorPlaceTask_e task) {
    clearColors(net);
    
    const bool checkCausal = ((task == COLOR_PLACE_TASK_ALL) || (task == COLOR_PLACE_TASK_CAUSAL));
    const bool checkConflict = ((task == COLOR_PLACE_TASK_ALL) || (task == COLOR_PLACE_TASK_CONFLICT));
    
    PNAPI_FOREACH(p, net.getPlaces()) {
        bool isCausal;
        bool isConflict;
        
        if (checkCausal) {
            isCausal = (isPotentialCausalPlace(*p) > 0); 
        }
        if (checkConflict) {
            isConflict = (isPotentialConflictPlace(*p) > 0); 
        }
        
        switch (task) {
            case COLOR_PLACE_TASK_ALL:
                if (isCausal && isConflict) {
                    (**p).setColor("brown");
                }
                else if (isCausal) {
                    (**p).setColor("blue");
                }
                else if (isConflict) {
                    (**p).setColor("orange");
                }
                break;
            case COLOR_PLACE_TASK_CAUSAL:
                if (isCausal) {
                    (**p).setColor("blue");
                }
                break;
            case COLOR_PLACE_TASK_CONFLICT:
                if (isConflict) {
                    (**p).setColor("orange");
                }
                break;
        }
    }
}

void AnicaLib::colorActivePlaces(pnapi::PetriNet& net, colorPlaceTask_e task) {
    clearColors(net);
    
    const bool checkCausal = ((task == COLOR_PLACE_TASK_ALL) || (task == COLOR_PLACE_TASK_CAUSAL));
    const bool checkConflict = ((task == COLOR_PLACE_TASK_ALL) || (task == COLOR_PLACE_TASK_CONFLICT));
    
    PNAPI_FOREACH(p, net.getPlaces()) {
        bool isCausal;
        bool isConflict;
        
        if (checkCausal) {
            isCausal = (isActiveCausalPlace(net, *p) > 0); 
        }
        if (checkConflict) {
            isConflict = (isActiveConflictPlace(net,*p) > 0); 
        }
        
        switch (task) {
            case COLOR_PLACE_TASK_ALL:
                if (isCausal && isConflict) {
                    (**p).setColor("brown");
                }
                else if (isCausal) {
                    (**p).setColor("blue");
                }
                else if (isConflict) {
                    (**p).setColor("orange");
                }
                break;
            case COLOR_PLACE_TASK_CAUSAL:
                if (isCausal) {
                    (**p).setColor("blue");
                }
                break;
            case COLOR_PLACE_TASK_CONFLICT:
                if (isConflict) {
                    (**p).setColor("orange");
                }
                break;
        }
    }
}

void AnicaLib::colorConfidence(pnapi::PetriNet& net, colorConfidenceTask_e task) {
    clearColors(net);
    
    PNAPI_FOREACH(t, net.getTransitions()) {
	    switch ((**t).getConfidence()) {
		    case CONFIDENCE_LOW:
		        if ((task == COLOR_CONFIDENCE_TASK_LOW) || (task == COLOR_CONFIDENCE_TASK_ALL)) {
			        (**t).setColor("green");
			    }
			    break;
		    case CONFIDENCE_HIGH:
		        if ((task == COLOR_CONFIDENCE_TASK_HIGH) || (task == COLOR_CONFIDENCE_TASK_ALL)) {
			        (**t).setColor("red");
			    }
			    break;
		    case CONFIDENCE_DOWN:
		        if ((task == COLOR_CONFIDENCE_TASK_DOWN) || (task == COLOR_CONFIDENCE_TASK_ALL)) {
			        (**t).setColor("yellow");
			    }
			    break;
	    }
	}
}

}
