

#ifndef EVENTTERM_H_
#define EVENTTERM_H_

#include <pnapi.h>
#include "config.h"
//#include "helpers.h"
#include <cstdlib>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <fstream>

#include <ctime>
#include <string>

using std::map;

//extern const std::string intToStrJan(const int);
//std::string getText(FILE*);

enum TermType {TypeAbstract,TypeAddTerm,TypeMultiplyTerm,TypeBasicTerm};



class EventTerm {
protected:
public:
	virtual void collectR(std::map<std::string const,int>*) = 0;
	virtual std::string toString() = 0;
	virtual EventTerm* flatten() = 0;
	virtual EventTerm* multiplyWith(int) = 0;
	static std::map<std::string const,int>* termToMap(EventTerm*);
	static std::string toPrettyString(EventTerm*);
	//static std::vector<EventTerm*>* createBasicTermSet(pnapi::PetriNet*);
	//static EventTerm* createRandomEventTerm(pnapi::PetriNet*);
	static std::set<std::string> events;
	virtual ~EventTerm() {}
};

class AddTerm : public EventTerm {
public:
	virtual void collectR(std::map<std::string const,int>*);
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
	virtual void collectR(std::map<std::string const,int>*);
	EventTerm* term;
	int factor;
	MultiplyTerm(EventTerm* t, int f) : term(t),factor(f) {};
	virtual std::string toString() {
		std::string s;
		std::stringstream out;
		out << factor;
		return "( " + out.str() + " * " + term->toString() + " )";
	}
	virtual EventTerm* flatten();
	virtual EventTerm* multiplyWith(int);
	virtual ~MultiplyTerm();
};

class BasicTerm : public EventTerm {
public:
	virtual void collectR(std::map<std::string const,int>*)  {};

	//BasicTerm(pnapi::Place* e) : event(e) {}
	BasicTerm(std::string *s) : event(s) {	}
	std::string* const event;
	virtual std::string toString() {
		if (event == 0) return "dummy";
		return *event;
	}
	virtual EventTerm* flatten();
	virtual EventTerm* multiplyWith(int);
	~BasicTerm();
};






#endif /* EVENTTERM_H_ */
