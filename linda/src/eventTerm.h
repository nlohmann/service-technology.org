/*
 * eventTerm.h
 *
 *  Created on: 16.06.2009
 *      Author: Jan
 */

#ifndef EVENTTERM_H_
#define EVENTTERM_H_

#include <pnapi/pnapi.h>
#include "helpers.h"
#include <cstdlib>
#include <vector>
#include <set>
#include <ctime>
#include <string>

enum TermType {TypeAbstract,TypeAddTerm,TypeMultiplyTerm,TypeBasicTerm};

class EventTerm {
protected:
public:
	virtual void collectR(std::map<pnapi::Place* const,int>*) = 0;
	virtual std::string toString() = 0;
	virtual EventTerm* flatten() = 0;
	virtual EventTerm* multiplyWith(int) = 0;
	static std::map<pnapi::Place* const,int>* termToMap(EventTerm*);
	static std::string toPrettyString(EventTerm*);
	static std::vector<EventTerm*>* createBasicTermSet(pnapi::PetriNet*);
	static EventTerm* createRandomEventTerm(pnapi::PetriNet*);
	static std::map<std::string,pnapi::Place*> events;
	virtual ~EventTerm() {}
};

class AddTerm : public EventTerm {
public:
	virtual void collectR(std::map<pnapi::Place* const,int>*);
	EventTerm* term1;
	EventTerm* term2;
	std::string toString() {
		return "( " + term1->toString() + " + " + term2->toString() + " )";
	}
	AddTerm(EventTerm* t1, EventTerm* t2) : term1(t1),term2(t2) {};
	virtual EventTerm* flatten();
	virtual EventTerm* multiplyWith(int);
	virtual ~AddTerm();
};

class MultiplyTerm : public EventTerm {
public:
	virtual void collectR(std::map<pnapi::Place* const,int>*);
	EventTerm* term;
	int factor;
	MultiplyTerm(EventTerm* t, int f) : term(t),factor(f) {};
	virtual std::string toString() {
		return "( " + intToStr(factor) + " * " + term->toString() + " )";
	}
	virtual EventTerm* flatten();
	virtual EventTerm* multiplyWith(int);
	virtual ~MultiplyTerm();
};

class BasicTerm : public EventTerm {
public:
	virtual void collectR(std::map<pnapi::Place* const,int>*)  {};

	BasicTerm(pnapi::Place* e) : event(e) {}
	BasicTerm(std::string* s) : event(events[*s]) {	}

	pnapi::Place* const event;
	virtual std::string toString() {
		if (event == 0) return "dummy";
		return event->getName();
	}
	virtual EventTerm* flatten();
	virtual EventTerm* multiplyWith(int);
	~BasicTerm();
};

class EventTermBound {
public:
	bool lowerBounded;
	bool upperBounded;
	int lowerBound;
	int upperBound;
	EventTermBound() {lowerBounded = false; upperBounded = false;}
	EventTermBound(bool lowerBounded,bool upperBounded,int lowerBound,int upperBound) {
		this->lowerBounded = lowerBounded;
		this->upperBounded = upperBounded;
		this->lowerBound = lowerBound;
		this->upperBound = upperBound;
	}
	bool intersectionEmpty (EventTermBound* other) {
		if ((!lowerBounded && !upperBounded) || (!other->upperBounded && !other->lowerBounded)) return false;
		return (other->upperBounded && lowerBounded && lowerBound > other->upperBound) || (other->lowerBounded && upperBounded && upperBound < other->lowerBound);
	}

	bool contains(EventTermBound* other) {
		return 	(!lowerBounded || (other->lowerBounded && lowerBound <= other->lowerBound)) && (!upperBounded || (other->upperBounded && upperBound >= other->upperBound));
	}

	bool isDecided () {
		if (lowerBounded && upperBounded && lowerBound == upperBound) return true;

		return false;
	}

	void output (EventTerm* term, bool as_given) {
		std::cout << "\t" << getLowerBoundString() << " <= ";
		if (as_given) {
			std::cout << term->toString();
		} else {
			std::cout << EventTerm::toPrettyString(term);
		}
		std::cout << " <= "<< getUpperBoundString() << "\n";
	}

	std::string getLowerBoundString() {
		if (lowerBounded) {
			return intToStr(lowerBound);
		} else {
			return "unbounded";
		}
	}
	std::string getUpperBoundString() {
		if (upperBounded) {
			return intToStr(upperBound);
		} else {
			return "unbounded";
		}
	}

};

class EventTermConstraint {
public:
	EventTermBound* vals;
	EventTerm* e;
	EventTermConstraint(EventTerm* e, EventTermBound* vals) {
		this->vals = vals;
		this->e = e;
	}

	EventTerm* getEventTerm() {
		return e;
	}

// vals 0 <= bla <= 0
// toCheck -5 <= blubb <= 10

	bool contradicts(EventTermBound* toCheck) {
		return toCheck->intersectionEmpty(vals);
	}

	static const int is_true = 1;
	static const int is_false = 0;
	static const int is_maybe = -1;


	unsigned int holds(EventTermBound* toCheck) {




		if (toCheck->intersectionEmpty(vals)) {
			return is_false;
		}
		if (vals->contains(toCheck)) {
			return is_true;
		}
		return is_maybe;
	}



	std::string toString() {
		std::string result = "";
		if (vals->lowerBounded) {
			result += intToStr(vals->lowerBound);
		} else {
			result += "unbounded";
		}
		result += " <= " + e->toString() + " <= ";
		if (vals->upperBounded) {
			result += intToStr(vals->upperBound);
		} else {
			result += "unbounded";
		}
		return result;
	}


};


#endif /* EVENTTERM_H_ */
