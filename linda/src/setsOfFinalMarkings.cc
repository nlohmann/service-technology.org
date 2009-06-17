#include <typeinfo>
#include <iostream>
#include "setsOfFinalMarkings.h"

using std::cout;
using std::cerr;

bool PartialMarking::isEquivalentTo(PartialMarking* other) {

/*	for (std::map<const pnapi::Place*,int>::iterator it = values.begin(); it != values.end(); ++it) {
		if ((other->defines((*it).first)) && (other->values[(*it).first] != (*it).second)) {
			return false;
		}
	}
*/

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
		std::cout << (*it).first->getName() << "=" << (*it).second << "; ";
	}
	std::cout << "\n";

}


void SetOfPartialMarkings::output() {

	for (std::vector<PartialMarking*>::iterator it = partialMarkings.begin(); it != partialMarkings.end(); ++it) {
		std::cout << "\n" << "A new one" << std::endl;

		(*it)->output();
	}

}


SetOfPartialMarkings* SetOfPartialMarkings::unite(std::vector<SetOfPartialMarkings*>& sets) {

	SetOfPartialMarkings* result = new SetOfPartialMarkings();

	for (std::vector<SetOfPartialMarkings*>::iterator setIt = sets.begin(); setIt != sets.end(); ++setIt) {
		for (std::vector<PartialMarking*>::iterator mIt = (*setIt)->partialMarkings.begin(); mIt != (*setIt)->partialMarkings.end(); ++mIt) {
			result->protectedInsert(*mIt);
		}
	}

	return result;
}

SetOfPartialMarkings* SetOfPartialMarkings::unite(SetOfPartialMarkings* set1, SetOfPartialMarkings* set2) {

	SetOfPartialMarkings* result = new SetOfPartialMarkings();

	for (std::vector<PartialMarking*>::iterator mIt = set1->partialMarkings.begin(); mIt != set1->partialMarkings.end(); ++mIt) {
		result->partialMarkings.push_back(*mIt);
	}

	for (std::vector<PartialMarking*>::iterator mIt = set2->partialMarkings.begin(); mIt != set2->partialMarkings.end(); ++mIt) {
		result->protectedInsert(*mIt);
	}


	return result;
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


	if (typeid(*src) ==  typeid(pnapi::formula::Disjunction)) {



		const pnapi::formula::Disjunction* const castedSrc = dynamic_cast<const pnapi::formula::Disjunction* const>(src);

		for (std::set<const pnapi::formula::Formula *>::iterator cIt = castedSrc->children().begin(); cIt != castedSrc->children().end(); ++cIt) {
			SetOfPartialMarkings* current = create((*cIt),bound);
			result->add(current);
		}
	} else if (typeid(*src) ==  typeid(pnapi::formula::Conjunction)) {


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


	} 	else if (typeid(*src) ==  typeid(pnapi::formula::FormulaEqual)) {

		const pnapi::formula::FormulaEqual* const castedSrc = dynamic_cast<const pnapi::formula::FormulaEqual* const >(src);

		PartialMarking* m = new PartialMarking();
		m->values[&(castedSrc->place())] = castedSrc->tokens();
		result->partialMarkings.push_back(m);

//		m->output();

	} else if (typeid(*src) ==  typeid(pnapi::formula::FormulaGreater)) {

		const pnapi::formula::FormulaGreater* const castedSrc = dynamic_cast<const pnapi::formula::FormulaGreater* const>(src);


		for (unsigned int i = castedSrc->tokens()+1; i <= bound; ++i) {
			PartialMarking* m = new PartialMarking();
			m->values[&(castedSrc->place())] = i;
			result->partialMarkings.push_back(m);
		}

	} else if (typeid(*src) ==  typeid(pnapi::formula::FormulaGreaterEqual)) {

		const pnapi::formula::FormulaGreaterEqual* const castedSrc = dynamic_cast<const pnapi::formula::FormulaGreaterEqual* const >(src);


		for (unsigned int i = castedSrc->tokens(); i <= bound; ++i) {
			PartialMarking* m = new PartialMarking();
			m->values[&(castedSrc->place())] = i;
			result->partialMarkings.push_back(m);
		}


	} else if (typeid(*src) ==  typeid(pnapi::formula::FormulaLess)) {
		const pnapi::formula::FormulaLess* const castedSrc = dynamic_cast<const pnapi::formula::FormulaLess* const >(src);

		for (unsigned int i = 0; i < castedSrc->tokens(); ++i) {
			PartialMarking* m = new PartialMarking();
			m->values[&(castedSrc->place())] = i;
			result->partialMarkings.push_back(m);
		}

	} else if (typeid(*src) ==  typeid(pnapi::formula::FormulaLessEqual)) {
		const pnapi::formula::FormulaLessEqual* const castedSrc = dynamic_cast<const pnapi::formula::FormulaLessEqual* const>(src);

		for (unsigned int i = 0; i <= castedSrc->tokens(); ++i) {
			PartialMarking* m = new PartialMarking();
			m->values[&(castedSrc->place())] = i;
			result->partialMarkings.push_back(m);
		}

	} else if (typeid(*src) ==  typeid(pnapi::formula::FormulaNotEqual)) {
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
