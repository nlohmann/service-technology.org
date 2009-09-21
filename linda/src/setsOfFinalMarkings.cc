#include <typeinfo>
#include <iostream>
#include "setsOfFinalMarkings.h"

using std::cout;
using std::cerr;
using std::string;

bool PartialMarking::isEquivalentTo(PartialMarking* other) {

	for (std::map<const pnapi::Place*,int>::iterator it = values.begin(); it != values.end(); ++it) {

		if (!(other->defines((*it).first))) {
			return false;
		}

		if (other->values[(*it).first] != (*it).second) {
			return false;
		}

	}


	for (std::map<const pnapi::Place*,int>::iterator it = other->values.begin(); it != other->values.end(); ++it) {

		if (!(defines((*it).first))) {
			return false;
		}

		if (values[(*it).first] != (*it).second) {
			return false;
		}

	}


	return true;
}

PartialMarking* PartialMarking::merge(PartialMarking* m1,PartialMarking* m2) {

	PartialMarking* result = new PartialMarking();

	for (std::map<const pnapi::Place*,int>::iterator it = m1->values.begin(); it != m1->values.end(); ++it) {
		if (m2->defines((*it).first) && m2->values[(*it).first] != (*it).second) {
			delete result;
			return 0;
		}
		result->values[(*it).first] = (*it).second;
	}

	for (std::map<const pnapi::Place*,int>::iterator it = m2->values.begin(); it != m2->values.end(); ++it) {
		if (!result->defines((*it).first)) {
			result->values[(*it).first] = (*it).second;
		}
	}

	return result;

}

void PartialMarking::output() {

	for (std::map<const pnapi::Place*,int>::iterator it = values.begin(); it != values.end(); ++it) {
		std::cerr << (*it).first->getName() << "=" << (*it).second << "; ";
	}
	std::cerr << "\n";

}

string PartialMarking::toString() {

	string result = "";

	for (std::map<const pnapi::Place*,int>::iterator it = values.begin(); it != values.end(); ++it) {
		result += (*it).first->getName() + "=" + intToStr((*it).second) + ", ";
	}

	return result.substr(0,result.length()-2) + ";";
}



void SetOfPartialMarkings::output() {

	for (std::vector<PartialMarking*>::iterator it = partialMarkings.begin(); it != partialMarkings.end(); ++it) {
		std::cout << "\n" << "A new one" << std::endl;

		(*it)->output();
	}

}


void SetOfPartialMarkings::add(SetOfPartialMarkings* aSet) {
	for (std::vector<PartialMarking*>::iterator mIt = aSet->partialMarkings.begin(); mIt != aSet->partialMarkings.end(); ++mIt) {
		protectedInsert(*mIt);
	}

}



SetOfPartialMarkings* SetOfPartialMarkings::intersect(SetOfPartialMarkings* set1, SetOfPartialMarkings* set2) {

	SetOfPartialMarkings* result = new SetOfPartialMarkings();

	for (std::vector<PartialMarking*>::iterator mIt = set1->partialMarkings.begin(); mIt != set1->partialMarkings.end(); ++mIt) {
		for (std::vector<PartialMarking*>::iterator mIt2 = set2->partialMarkings.begin(); mIt2 != set2->partialMarkings.end(); ++mIt2) {

			PartialMarking* temp = PartialMarking::merge(*mIt,*mIt2);
			if(temp != 0) {
				result->partialMarkings.push_back(temp);
			}
		}
	}

	return result;
}

bool SetOfPartialMarkings::contains(PartialMarking* partialMarking) {
	for (std::vector<PartialMarking*>::iterator mIt = partialMarkings.begin(); mIt != partialMarkings.end(); ++mIt) {
			if ((*mIt)->isEquivalentTo(partialMarking)) {
				return true;
			}
	}
	return false;
}

bool SetOfPartialMarkings::protectedInsert(PartialMarking* partialMarking) {
	if (contains(partialMarking)) {
		return false;
	}

	partialMarkings.push_back(partialMarking);
	return true;
}




SetOfPartialMarkings* SetOfPartialMarkings::create(const pnapi::formula::Formula* const src,unsigned int bound) {
	SetOfPartialMarkings* result = new SetOfPartialMarkings();

	//std::cerr << "Typeid is: " << typeid(*src).name() << std::endl;

	//std::cerr << "Testing equivalence to " << typeid(pnapi::formula::Disjunction).name() << "..." << std::endl;
	if (typeid(*src) ==  typeid(pnapi::formula::Disjunction)) {
//	std::cerr << "Equal! " << std::endl;

		const pnapi::formula::Disjunction* const castedSrc = dynamic_cast<const pnapi::formula::Disjunction* const>(src);

		for (std::set<const pnapi::formula::Formula *>::iterator cIt = castedSrc->children().begin(); cIt != castedSrc->children().end(); ++cIt) {
			SetOfPartialMarkings* current = create((*cIt),bound);
			result->add(current);
		}
	}

	// std::cerr << "Testing equivalence to " << typeid(pnapi::formula::Conjunction).name() << "..." << std::endl;
	if (typeid(*src) ==  typeid(pnapi::formula::Conjunction)) {
		// std::cerr << "Equal! " << std::endl;


		const pnapi::formula::Conjunction* const castedSrc = dynamic_cast<const pnapi::formula::Conjunction* const >(src);
/*
		for (std::set<const pnapi::formula::Formula *>::iterator cIt = castedSrc->children().begin(); cIt != castedSrc->children().end(); ++cIt) {
			SetOfPartialMarkings* current = create((*cIt),bound);
			result->add(current);
		}
*/

		SetOfPartialMarkings* last = create(*(castedSrc->children().begin()),bound);

		for (std::set<const pnapi::formula::Formula *>::iterator cIt = (++castedSrc->children().begin()); cIt != castedSrc->children().end(); ++cIt) {
			SetOfPartialMarkings* current = intersect(last,create(*cIt,bound));
			delete last;
			last = current;
//			last->output();
		}

		result = last;


	}
	// std::cerr << "Testing equivalence to " << typeid(pnapi::formula::FormulaEqual).name() << "..." << std::endl;

	if (typeid(*src) ==  typeid(pnapi::formula::FormulaEqual)) {
	// std::cerr << "Equal! " << std::endl;

		const pnapi::formula::FormulaEqual* const castedSrc = dynamic_cast<const pnapi::formula::FormulaEqual* const >(src);

		PartialMarking* m = new PartialMarking();
		m->values[&(castedSrc->place())] = castedSrc->tokens();
		result->partialMarkings.push_back(m);

//		m->output();

	}

	// std::cerr << "Testing equivalence to " << typeid(pnapi::formula::FormulaGreater).name() << "..." << std::endl;

	if (typeid(*src) ==  typeid(pnapi::formula::FormulaGreater)) {
	// std::cerr << "Equal! " << std::endl;

		const pnapi::formula::FormulaGreater* const castedSrc = dynamic_cast<const pnapi::formula::FormulaGreater* const>(src);


		for (unsigned int i = castedSrc->tokens()+1; i <= bound; ++i) {
			PartialMarking* m = new PartialMarking();
			m->values[&(castedSrc->place())] = i;
			result->partialMarkings.push_back(m);
		}

	}
	// std::cerr << "Testing equivalence to " << typeid(pnapi::formula::FormulaGreaterEqual).name() << "..." << std::endl;

	if (typeid(*src) ==  typeid(pnapi::formula::FormulaGreaterEqual)) {
	// std::cerr << "Equal! " << std::endl;

		const pnapi::formula::FormulaGreaterEqual* const castedSrc = dynamic_cast<const pnapi::formula::FormulaGreaterEqual* const >(src);


		for (unsigned int i = castedSrc->tokens(); i <= bound; ++i) {
			PartialMarking* m = new PartialMarking();
			m->values[&(castedSrc->place())] = i;
			result->partialMarkings.push_back(m);
		}


	}
	// std::cerr << "Testing equivalence to " << typeid(pnapi::formula::FormulaLess).name() << "..." << std::endl;

	if (typeid(*src) ==  typeid(pnapi::formula::FormulaLess)) {
	// std::cerr << "Equal! " << std::endl;
		const pnapi::formula::FormulaLess* const castedSrc = dynamic_cast<const pnapi::formula::FormulaLess* const >(src);

		for (unsigned int i = 0; i < castedSrc->tokens(); ++i) {
			PartialMarking* m = new PartialMarking();
			m->values[&(castedSrc->place())] = i;
			result->partialMarkings.push_back(m);
		}

	}
	// std::cerr << "Testing equivalence to " << typeid(pnapi::formula::FormulaLessEqual).name() << "..." << std::endl;

	if (typeid(*src) ==  typeid(pnapi::formula::FormulaLessEqual)) {
	// std::cerr << "Equal! " << std::endl;
		const pnapi::formula::FormulaLessEqual* const castedSrc = dynamic_cast<const pnapi::formula::FormulaLessEqual* const>(src);

		for (unsigned int i = 0; i <= castedSrc->tokens(); ++i) {
			PartialMarking* m = new PartialMarking();
			m->values[&(castedSrc->place())] = i;
			result->partialMarkings.push_back(m);
		}

	}
	// std::cerr << "Testing equivalence to " << typeid(pnapi::formula::FormulaNotEqual).name() << "..." << std::endl;
	if (typeid(*src) ==  typeid(pnapi::formula::FormulaNotEqual)) {
	// std::cerr << "Equal! " << std::endl;
		const pnapi::formula::FormulaNotEqual* const castedSrc = dynamic_cast<const pnapi::formula::FormulaNotEqual* const >(src);


		for (unsigned int i = castedSrc->tokens(); i >= 0; --i) {
			PartialMarking* m = new PartialMarking();
			m->values[&(castedSrc->place())] = i;
			result->partialMarkings.push_back(m);
		}

		for (unsigned int i = castedSrc->tokens()+1; i <= bound; ++i) {
			PartialMarking* m = new PartialMarking();
			m->values[&(castedSrc->place())] = i;
			result->partialMarkings.push_back(m);
		}

	}



	return result;

}
