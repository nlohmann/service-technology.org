#include <eventTerm.h>

std::string MultiplyTerm::toString() {
	return "( " + LindaHelpers::intToStr(factor) + " * " + term->toString()
			+ " )";
}

MultiplyTerm::~MultiplyTerm() {
	delete term;
}

std::string AddTerm::toString() {
	return "( " + term1->toString() + " + " + term2->toString() + " )";
}

AddTerm::~AddTerm() {
	delete term1;
	delete term2;
}

std::string BasicTerm::toString() {
	return LindaHelpers::EVENT_STRINGS[event];
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
	return new AddTerm(term1->flatten(), term2->flatten());
}

EventTerm* BasicTerm::flatten() {
	return new MultiplyTerm(new BasicTerm(event), 1);
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
	EventTerm* result = new MultiplyTerm(this, k);
	return result;
}

int* EventTerm::termToMap(EventTerm* e) {
	EventTerm* temp = e->flatten();
	int* result = new int[LindaHelpers::NR_OF_EVENTS]();
	for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
		result[i] = 0;
	}
	temp->collectR(result);
	return result;
}

void MultiplyTerm::collectR(int* map) {
	BasicTerm* b = dynamic_cast<BasicTerm*> (term);
	map[b->event] += factor;
}

void AddTerm::collectR(int* map) {
	term1->collectR(map);
	term2->collectR(map);
}

std::string EventTerm::toPrettyString(EventTerm* e) {
	std::string result = "";
	int* map = termToMap(e);
	for (int i = 0; i < LindaHelpers::NR_OF_EVENTS; ++i) {
		if (map[i] != 0) {
			if (map[i] >= 0) {
				result += "+";
			}
			result += LindaHelpers::intToStr(map[i]);
			result += "*";
			result += LindaHelpers::EVENT_STRINGS[i];
		}

	}
	return result;
}

EventTermBound* EventTermBound::merge(EventTermBound* termArray,
		unsigned int size) {
	bool lset = false;
	bool uset = false;
	bool ldone = false;
	bool udone = false;
	bool lbounded = false;
	bool ubounded = false;
	int lbound = 0;
	int ubound = 0;

	for (int i = 0; i < size; ++i) {
		if (!ldone) { // We have not yet found an unbounded for lower.
			if (!termArray[i].lowerBounded) { // unbounded term!
				lbounded = false;
				ldone = true;
			} else if (!lset) { // bounded term for sure and bound was not set yet.
				lset = true;
				lbounded = true;
				lbound = termArray[i].lowerBound;
			} else if (lset) { // bounded term for sure and bound was already set
				if (lbound > termArray[i].lowerBound) {
					lbound = termArray[i].lowerBound;
				}
			}

		}

		if (!udone) {
			if (!termArray[i].upperBounded) {
				ubounded = false;
				udone = true;
			} else if (!uset) {
				uset = true;
				ubounded = true;
				ubound = termArray[i].upperBound;
			} else if (uset) {
				if (ubound > termArray[i].upperBound) {
					ubound = termArray[i].upperBound;
				}
			}

		}
	}


	return new EventTermBound(lbounded,ubounded,lbound,ubound);

}

bool EventTermBound::intersectionEmpty(EventTermBound* other) {
	if ((!lowerBounded && !upperBounded) || (!other->upperBounded
			&& !other->lowerBounded))
		return false;
	return (other->upperBounded && lowerBounded && lowerBound
			> other->upperBound) || (other->lowerBounded && upperBounded
			&& upperBound < other->lowerBound);
}

bool EventTermBound::contains(EventTermBound* other) {
	return (!lowerBounded || (other->lowerBounded && lowerBound
			<= other->lowerBound)) && (!upperBounded || (other->upperBounded
			&& upperBound >= other->upperBound));
}

bool EventTermBound::isDecided() {
	if (lowerBounded && upperBounded && lowerBound == upperBound)
		return true;
	return false;
}

void EventTermBound::output(EventTerm* term, bool as_given) {
	std::cerr << "\t" << getLowerBoundString() << " <= ";
	if (as_given) {
		std::cerr << term->toString();
	} else {
		std::cerr << EventTerm::toPrettyString(term);
	}
	std::cerr << " <= " << getUpperBoundString() << std::endl;
}

std::string EventTermBound::getLowerBoundString() {
	if (lowerBounded) {
		return LindaHelpers::intToStr(lowerBound);
	} else {
		return "unbounded";
	}
}

std::string EventTermBound::getUpperBoundString() {
	if (upperBounded) {
		return LindaHelpers::intToStr(upperBound);
	} else {
		return "unbounded";
	}
}

EventTermConstraint::EventTermConstraint(EventTerm* e, EventTermBound* vals) {
	this->vals = vals;
	this->e = e;
}

EventTerm* EventTermConstraint::getEventTerm() {
	return e;
}

bool EventTermConstraint::contradicts(EventTermBound* toCheck) {
	return toCheck->intersectionEmpty(vals);
}

unsigned int EventTermConstraint::holds(EventTermBound* toCheck) {

	if (toCheck->intersectionEmpty(vals)) {
		return is_false;
	}
	if (vals->contains(toCheck)) {
		return is_true;
	}
	return is_maybe;
}

std::string EventTermConstraint::toString() {
	std::string result = "";
	if (vals->lowerBounded) {
		result += LindaHelpers::intToStr(vals->lowerBound);
	} else {
		result += "unbounded";
	}
	result += " <= " + e->toString() + " <= ";
	if (vals->upperBounded) {
		result += LindaHelpers::intToStr(vals->upperBound);
	} else {
		result += "unbounded";
	}
	return result;
}
/*

void RawTerm::setValue(int id, int newValue) {
	data->insert(id,newValue,true);
}

int RawTerm::getValue(int id) {
	BinaryTreeNode<int,int>* b = data->find(id);
	if (b != 0) return b->value;
	return 0;
}

void RawTerm::addValue(int id, int addend) {
	int newVal = getValue(id);
	setValue(id,newVal);
}*/
