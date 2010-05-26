#include "helpers.h"
#include <vector>

class CostAgent {
public:
	static std::vector<pnapi::Transition*>* transitions;
	static int getTransitionID(std::string transition);
	
	static std::vector<pnapi::Place*>* places;
	static int getPlaceID(std::string place);
	
	static std::vector<pnapi::Label*>* labels;
	static int getLabelID(std::string label);
	
	
	static int* getCleanIntArray(int len);
};

class ElementalConstraint {
public:
	int* lhs;
	int sign;
	int rhs;
	int len;
	ElementalConstraint(int length) {
		len = length;
		lhs = CostAgent::getCleanIntArray(len);
		sign = 0;
		rhs = 0;
	}
	ElementalConstraint* convert (); //Converts a label constraint to a transition constraint
};

class UseCase {
public:
	UseCase() {
		marking = CostAgent::getCleanIntArray(CostAgent::places->size());
		constraint = new std::vector<ElementalConstraint*>();
		variableCosts = CostAgent::getCleanIntArray(CostAgent::transitions->size());
		fixCosts = 0;
		policyLHS = 0;
		policyRHS = 0;
	}
	std::vector<ElementalConstraint*>* constraint; // A conjunction.
	int* marking; // An integer array with an int for each place
	int* variableCosts; // An integer array with an int for each transition
	int fixCosts;
	int policyLHS;
	int policyRHS;
};


class CostProfile {
public:
	std::vector<UseCase*> usecases;
};

class Grant {
public:
	std::vector<ElementalConstraint*>* constraint;
	int* variableCosts; // An integer array with an int for each label
	int fixCosts;
	Grant() {
		constraint = new std::vector<ElementalConstraint*>();
		variableCosts = CostAgent::getCleanIntArray(CostAgent::labels->size());
		fixCosts = 0;
	}
	void convertConstraints (); // convert the label constraints to transition constraints
};

class Request {
public:
	std::vector<std::vector<ElementalConstraint*>* >* assertions; // A disjunction (outer vector) of conjunctions (inner vectors).
	std::vector<Grant*>* grants; 
	void convertAssertions (); // convert the label constraints to transitio constraints
};

