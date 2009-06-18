/*
 * eventTerm.cc
 *
 *  Created on: 16.06.2009
 *      Author: Jan
 */

#include <eventTerm.h>

std::map<std::string,pnapi::Place*> EventTerm::events;

std::vector<EventTerm*>* EventTerm::createBasicTermSet(pnapi::PetriNet* net) {

	vector<EventTerm*>* result = new vector<EventTerm*>();
	for (std::set<pnapi::Place*>::iterator it = net->getInterfacePlaces().begin(); it != net->getInterfacePlaces().end(); ++it) {
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
		{
			int nrOfEvents = net->getInterfacePlaces().size();
			int chosen = rand() % (nrOfEvents);
			for (int i = 0; i < chosen; ++i) {
				++it;
			}
			e = new BasicTerm(*it);
			return e;
		}
	case 2 : // Multiply
		{
			int factor = rand() % 10 + 1;
			if (rand() % 2 == 0) factor *= (-1);
			e = new MultiplyTerm(createRandomEventTerm(net),factor);
			return e;
	    }
	case 3 : // Add
		{
			e = new AddTerm(createRandomEventTerm(net),createRandomEventTerm(net));
			return e;
		}
	}

}

MultiplyTerm::~MultiplyTerm() {
	delete term;
}

AddTerm::~AddTerm() {
	delete term1;
	delete term2;
}

BasicTerm::~BasicTerm() {
	// Do nothing!!!
}


EventTerm* MultiplyTerm::flatten() {
	EventTerm* result = term->flatten();
	result = result->multiplyWith(this->factor);
	return result;
}

EventTerm* AddTerm::flatten() {
	return new AddTerm(term1->flatten(),term2->flatten());
}

EventTerm* BasicTerm::flatten() {
	return new MultiplyTerm(new BasicTerm(event),1);
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

