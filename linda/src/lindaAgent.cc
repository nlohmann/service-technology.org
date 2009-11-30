#include "lindaAgent.h"

pnapi::PetriNet* LindaAgent::mNet = 0;
std::vector<PartialMarking*>* LindaAgent::mFinals = 0;
std::vector<ExtendedStateEquation*>* LindaAgent::mSystems = 0;
bool LindaAgent::keepHistory;

bool LindaAgent::initialize(pnapi::PetriNet* net,
		std::map<std::string, uint8_t>& name2id,
		std::map<uint8_t, std::string>& id2name,
		bool keepHistory) {
	mNet = net;

	LindaAgent::keepHistory = keepHistory;

	mFinals = new std::vector<PartialMarking*>();
	mSystems = new std::vector<ExtendedStateEquation*>();

	// Sync linda's order with wendy's order.
	LindaHelpers::NR_OF_EVENTS = mNet->getInterfacePlaces().size();
	LindaHelpers::EVENT_PLACES
	= new pnapi::Place*[LindaHelpers::NR_OF_EVENTS]();
	LindaHelpers::EVENT_STRINGS = new std::string[LindaHelpers::NR_OF_EVENTS]();

	for (std::set<pnapi::Place *>::iterator it =
		net->getInterfacePlaces().begin(); it
		!= net->getInterfacePlaces().end(); ++it) {
		int wendyID = name2id[(*it)->getName()] - 1;
		LindaHelpers::EVENT_PLACES[wendyID] = (*it);
		LindaHelpers::EVENT_STRINGS[wendyID] = (*it)->getName();

	}

	return (mNet != 0);
}

bool LindaAgent::initialize(pnapi::PetriNet* net,bool keepHistory) {
	mNet = net;

	mFinals = new std::vector<PartialMarking*>();
	mSystems = new std::vector<ExtendedStateEquation*>();


	LindaAgent::keepHistory = keepHistory;

	// Create the global event IDs
	LindaHelpers::NR_OF_EVENTS = net->getInterfacePlaces().size();
	LindaHelpers::EVENT_STRINGS = new std::string[LindaHelpers::NR_OF_EVENTS]();
	LindaHelpers::EVENT_PLACES
	= new pnapi::Place*[LindaHelpers::NR_OF_EVENTS]();

	int counter = 0;
	for (std::set<pnapi::Place *>::iterator it =
		net->getInterfacePlaces().begin(); it
		!= net->getInterfacePlaces().end(); ++it) {
		LindaHelpers::EVENT_STRINGS[counter] = (*it)->getName();
		LindaHelpers::EVENT_PLACES[counter] = (*it);
		++counter;
	}


	return (mNet != 0);
}


bool LindaAgent::addFinalMarking(
		std::map<const pnapi::Place*, unsigned int>* finalMarking) {
	PartialMarking* p = new PartialMarking();

	for (std::set<pnapi::Place*>::iterator it =
		mNet->getInternalPlaces().begin(); it
		!= mNet->getInternalPlaces().end(); ++it) {
		if (finalMarking->find(*it) != finalMarking->end()) {
			p->values.insert(*it, (*finalMarking)[*it], false);
		} else {
			p->values.insert(*it, 0, false);
		}
	}

	return addFinalMarking(p);


}

bool LindaAgent::addFinalMarking(PartialMarking* finalMarking) {


	ExtendedStateEquation* e = new ExtendedStateEquation(mNet, finalMarking, keepHistory);
	if (e->constructLP()) {
		mFinals->push_back(finalMarking);
		mSystems->push_back(e);
		return true;
	}

	return false;

}

/*
bool LindaAgent::addFinalMarkingsFromFinalCondition(uint8_t bound) {
	SetOfPartialMarkings* fSet = SetOfPartialMarkings::create(mNet,
			&(mNet->finalCondition().formula()), bound);

	for (int i = 0; i < fSet->size(); ++i) {
		addFinalMarking(fSet->partialMarkings[i]);
	}

	return true;
}
*/

uint8_t* LindaAgent::getXORTable(uint8_t firstOfInterest, uint8_t lastOfInterest) {

	uint8_t* eventsOfInterest = new uint8_t(lastOfInterest-firstOfInterest+1);

	for (uint8_t i = firstOfInterest; i <= lastOfInterest; ++i) {
		eventsOfInterest[i] = i;
	}

	uint8_t* result = getXORTable(eventsOfInterest,lastOfInterest-firstOfInterest+1);

	delete eventsOfInterest;

	return result;

}


uint8_t* LindaAgent::getXORTable(uint8_t* eventsOfInterest, uint8_t count) {

	uint8_t bytes = (count*count / 8) + 1;
	uint8_t* storage = new uint8_t[bytes];
	for (size_t i = 0; i < bytes; ++i) {
		// initially, all sending events are not mutually exclusive
		storage[i] = 0;
	}


	for (uint8_t i = 0; i < count; ++i) {
		for (uint8_t j = i + 1; j < count; ++j) {

			EventTermBound* bounds = new EventTermBound[mSystems->size()]();
			EventTerm* term = new AddTerm(new BasicTerm(i), new BasicTerm(j));

			uint8_t counter = 0;
			for (std::vector<ExtendedStateEquation*>::iterator it =
				mSystems->begin(); it != mSystems->end(); ++it) {
				EventTermBound* result = (*it)->evaluate(term);
				bounds[counter++] = *result;

			}

			EventTermBound* b = EventTermBound::merge(bounds, mSystems->size());



			/// Evaluate and write to bit array (both positions!).
			if (b->lowerBounded && b->upperBounded && b->lowerBound <= 1
					&& b->upperBound == 1) {

				const uint8_t actualFirstPosition = i;
				const uint8_t actualSecondPosition = j;

				const uint8_t firstPosition = (actualFirstPosition) * (count) + (actualSecondPosition);
				const uint8_t secondPosition = (actualSecondPosition) * (count) + (actualFirstPosition);

				const uint8_t myFirstByte = firstPosition / 8;
				const uint8_t myFirstBit = firstPosition % 8;

				const uint8_t mySecondByte = secondPosition / 8;
				const uint8_t mySecondBit = secondPosition % 8;

				storage[myFirstByte] += (1 << myFirstBit);
				storage[mySecondByte] += (1 << mySecondBit);
			}

			delete b;
		}
	}
	return storage;

}

void LindaAgent::finalize() {

	// Cached incidence matrix is not needed anymore
	delete ExtendedStateEquation::lines;
	delete[] ExtendedStateEquation::eventLines;
	delete ExtendedStateEquation::transitionID;


	for (int i = 0; i < mFinals->size(); ++i) {
		delete (*mFinals)[i];
		(*mFinals)[i] = 0;
	}

	delete mFinals;

	for (int i = 0; i < mSystems->size(); ++i) {
		delete (*mSystems)[i];
		(*mSystems)[i] = 0;
	}

	delete mSystems;

	delete ExtendedStateEquation::lines;
	delete[] ExtendedStateEquation::eventLines;
	delete ExtendedStateEquation::transitionID;
}

bool LindaAgent::addFinalMarkingsFromFinalCondition(uint8_t bound) {
	// Calculate the final markings from the final condition

	SetOfPartialMarkings* fSet = SetOfPartialMarkings::create(mNet,
			&(mNet->finalCondition().formula()), bound);

	status("Computed final markings:");
	for (std::vector<PartialMarking*>::iterator finalMarkingIt =
		fSet->partialMarkings.begin(); finalMarkingIt
		!= fSet->partialMarkings.end(); ++finalMarkingIt) {
		status("    %s", (*finalMarkingIt)->toString().c_str());

		ExtendedStateEquation* XSE = new ExtendedStateEquation(mNet,(*finalMarkingIt));
		if (XSE->constructLP()) {
			mSystems->push_back(XSE);
			mFinals->push_back(*finalMarkingIt);
		}


		// TODO: Access to args_info::showlp

	}





}


