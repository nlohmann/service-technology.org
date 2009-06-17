/*
 * eventTerm.cc
 *
 *  Created on: 16.06.2009
 *      Author: Jan
 */

#include <eventTerm.h>

std::vector<EventTerm*>* EventTerm::createBasicTermSet(pnapi::PetriNet* net) {

	const std::set<pnapi::Place*> placeSet = net->getInterfacePlaces();
	vector<EventTerm*>* result = new vector<EventTerm*>();
	for (std::set<pnapi::Place*>::iterator it = placeSet.begin(); it != placeSet.end(); ++it) {
		BasicTerm* b = new BasicTerm((*it));
		result->push_back(b);
	}

	return result;

}

EventTerm* EventTerm::createRandomEventTerm(pnapi::PetriNet* net) {


	int decision = rand() % 3 + 1;
	std::set<pnapi::Place*>::iterator it = net->getInterfacePlaces().begin();
	EventTerm* e;
	switch (decision) {
	case 1 : // Basic Term
		int nrOfEvents = net->getInterfacePlaces().size();
		int chosen = rand() % (nrOfEvents);
		for (int i = 0; i < chosen; ++i) {
			++it;
		}
		e = new BasicTerm(*it);
		return e;
	case 2 : // Multiply
		int factor = rand() % 10 + 1;
		if (rand() % 2 == 0) factor *= (-1);
		e = new MultiplyTerm(createRandomEventTerm(net),factor);
		return e;
	case 3 : // Add
		e = new AddTerm(createRandomEventTerm(net),createRandomEventTerm(net));
		return e;
	}

}

EventTerm* MultiplyTerm::flatten() {
	term = term->flatten();
	term = term->multiplyWith(this->factor);
	return term;
}

EventTerm* AddTerm::flatten() {
	term1 = term1->flatten();
	term2 = term2->flatten();
	return this;
}

EventTerm* BasicTerm::flatten() {
	EventTerm* result = new MultiplyTerm(this,1);
	return result;
}

EventTerm* MultiplyTerm::multiplyWith(int k) {
	factor *= k;
	return this;
}

EventTerm* AddTerm::multiplyWith(int k) {
	term1 = term1->multiplyWith(k);
	term2 = term2->multiplyWith(k);
	return this;
}

EventTerm* BasicTerm::multiplyWith(int k) {
	EventTerm* result = new MultiplyTerm(this,k);
	return result;
}

std::map<pnapi::Place* const,int>* EventTerm::termToMap(EventTerm* e) {
	EventTerm* temp = e->flatten();
	std::map<pnapi::Place* const,int>* result = new std::map<pnapi::Place* const,int>();
	temp->collectR(result);
	return result;
}



void MultiplyTerm::collectR(std::map<pnapi::Place* const,int>* map) {
	BasicTerm* b = dynamic_cast<BasicTerm*>(term);

	std::map<pnapi::Place* const,int>& mapRef = *map;

	if (map->find(b->event) != map->end()) {
		mapRef[b->event] += factor;
	} else {
		mapRef[b->event] = factor;
	}
}

void AddTerm::collectR(std::map<pnapi::Place* const,int>* map) {
	term1->collectR(map);
	term2->collectR(map);
}

