#include "helpers.h"
#include <vector>

class UseCase;
class ElementalConstraint;

class CostAgent {
public:
	static std::vector<pnapi::Transition*>* transitions;
	static int getTransitionID(std::string transition);
	
	static std::vector<pnapi::Place*>* places;
	static int getPlaceID(std::string place);
	
	static std::vector<pnapi::Label*>* labels;
	static int getLabelID(std::string label);
	
	static void buildBasicStateEquation();
	static lprec* basicStateEquation;
	
	static void buildStateEquationWithAssertions(UseCase* usecase, std::vector<ElementalConstraint*>* clause);
	static lprec* currentStateEquationWithAssertions;
	
	static BinaryTree<pnapi::Place*, std::pair<int, std::pair<int*, REAL*> > >* lines;
	
	static double* getCleanDoubleArray(int len);
};

class ElementalConstraint {
public:
	double* lhs;
	double sign;
	double rhs;
	int len;
	ElementalConstraint(int length) {
		len = length;
		lhs = CostAgent::getCleanDoubleArray(len);
		sign = 0;
		rhs = 0;
	}
	ElementalConstraint() {
		lhs = 0;
		len = 0;
		sign = 0;
		rhs = 0;
	}

	ElementalConstraint* convert (); //Converts a label constraint to a transition constraint
};

class UseCase {
public:
	UseCase() {
		marking = CostAgent::getCleanDoubleArray(CostAgent::places->size());
		constraint = new std::vector<ElementalConstraint*>();
		variableCosts = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());
		fixCosts = 0;
		policyLHS = 0;
		policyRHS = 0;
	}
	std::vector<ElementalConstraint*>* constraint; // A conjunction.
	double* marking; // An integer array with an int for each place
	double* variableCosts; // An integer array with an int for each transition
	double fixCosts;
	double policyLHS;
	double policyRHS;
	void output();
};


class CostProfile {
public:
	std::vector<UseCase*>* usecases;
	CostProfile() {
	usecases = new std::vector<UseCase*>();
	}
};

class Grant {
public:
	std::vector<ElementalConstraint*>* constraint;
	double* variableCosts; // An integer array with an int for each label
	double fixCosts;
	Grant() {
		constraint = new std::vector<ElementalConstraint*>();
		variableCosts = CostAgent::getCleanDoubleArray(CostAgent::labels->size());
		fixCosts = 0;
	}
	Grant* convert (); // convert the label constraints to transition constraints
};

class Request {
public:
	std::vector<std::vector<ElementalConstraint*>* >* assertions; // A disjunction (outer vector) of conjunctions (inner vectors).
	std::vector<Grant*>* grants; 
	void convertAssertions (); // convert the label constraints to transitio constraints
	void convertGrants (); // convert the label constraints to transitio constraints

	};

