/*
 * eventTerm.cc
 *
 *  Created on: 16.06.2009
 *      Author: Jan
 */

#include <eventTerm.h>




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

int* EventTerm::termToMap(EventTerm* e) {
	EventTerm* temp = e->flatten();
	int* result = new int[NR_OF_EVENTS]();
	for (int i = 0; i < NR_OF_EVENTS; ++i) {
		result[i] = 0;
	}
	temp->collectR(result);
	return result;
}



void MultiplyTerm::collectR(int* map) {
	BasicTerm* b = dynamic_cast<BasicTerm*>(term);
	map[b->event] += factor;
}

void AddTerm::collectR(int* map) {
	term1->collectR(map);
	term2->collectR(map);
}


std::string EventTerm::toPrettyString(EventTerm* e) {
	std::string result = "";
	int* map = termToMap(e);
	for (int i = 0; i < NR_OF_EVENTS; ++i) {
		if (map[i] != 0) {
			if (map[i] >= 0) {
				result += "+";
			}
			result += intToStr(map[i]);
			result += "*";
			result += EVENT_STRINGS[i];
		}

	}
	return result;
}
