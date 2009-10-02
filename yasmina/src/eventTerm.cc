

#include <eventTerm.h>

//std::map<std::string,pnapi::Place*> EventTerm::events;
std::set<std::string> EventTerm::events;


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

std::map<std::string const,int>* EventTerm::termToMap(EventTerm* e) {
	EventTerm* temp = e->flatten();
	std::map<std::string const,int>* result = new std::map<std::string const,int>();
	temp->collectR(result);
	return result;
}



void MultiplyTerm::collectR(std::map<std::string const,int>* map) {
	BasicTerm* b = dynamic_cast<BasicTerm*>(term);

	std::map<std::string const,int>& mapRef = *map;

	if (map->find(*b->event) != map->end()) {
		mapRef[*b->event] += factor;
	} else {
		mapRef[*b->event] = factor;
	}
}

void AddTerm::collectR(std::map<std::string const,int>* map) {
	term1->collectR(map);
	term2->collectR(map);
}


std::string EventTerm::toPrettyString(EventTerm* e) {
	std::string result = "";
	std::map<std::string const,int>* map = termToMap(e);
	for (std::map<std::string const,int>::iterator it = map->begin(); it != map->end(); ++it) {
		if ((*it).second >= 0) result += "+";
		std::stringstream out;
		out << (*it).second;
		result += out.str() + "*" + (*it).first;
	}
	return result;
}
