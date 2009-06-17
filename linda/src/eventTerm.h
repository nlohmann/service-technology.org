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

enum TermType {TypeAbstract,TypeAddTerm,TypeMultiplyTerm,TypeBasicTerm};

class EventTerm {
protected:
public:
	virtual void collectR(std::map<pnapi::Place* const,int>*) = 0;
	virtual std::string toString() = 0;
	virtual EventTerm* flatten() = 0;
	virtual EventTerm* multiplyWith(int) = 0;
	static std::map<pnapi::Place* const,int>* termToMap(EventTerm*);
	static std::vector<EventTerm*>* createBasicTermSet(pnapi::PetriNet*);
	static EventTerm* createRandomEventTerm(pnapi::PetriNet*);
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
};

class BasicTerm : public EventTerm {
public:
	virtual void collectR(std::map<pnapi::Place* const,int>*)  {};

	BasicTerm(pnapi::Place* e) : event(e) {}

	pnapi::Place* const event;
	virtual std::string toString() {
		return event->getName();
	}
	virtual EventTerm* flatten();
	virtual EventTerm* multiplyWith(int);
};







#endif /* EVENTTERM_H_ */
