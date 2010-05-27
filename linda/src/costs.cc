#include "costs.h"

std::vector<pnapi::Place*>* CostAgent::places;
std::vector<pnapi::Transition*>* CostAgent::transitions;
std::vector<pnapi::Label*>* CostAgent::labels;

lprec* CostAgent::basicStateEquation = 0;
lprec* CostAgent::currentStateEquationWithAssertions = 0;

BinaryTree<pnapi::Place*, std::pair<int, std::pair<int*, REAL*> > >
* CostAgent::lines = new BinaryTree<pnapi::Place*,
std::pair< int, std::pair<int*, REAL*>  > > ();


int CostAgent::getTransitionID(std::string transition) {
	
	for (int i = 1; i < transitions->size(); ++i) {
		if ((*CostAgent::transitions)[i]->getName().compare(transition) == 0) {
			return i;
		}
	}
	assert(false);
}

int CostAgent::getPlaceID(std::string place) {
	for (int i = 0; i < places->size(); ++i) {
		if ((*CostAgent::places)[i]->getName().compare(place) == 0) {
			return i;
		}
	}
	assert(false);
}

int CostAgent::getLabelID(std::string label) {
	for (int i = 0; i < labels->size(); ++i) {
		if ((*CostAgent::labels)[i]->getName().compare(label) == 0) {
			return i;
		}
	}
	assert(false);
}

double* CostAgent::getCleanDoubleArray(int len) {
	double* result = new double[len];
	for (int i = 0; i < len; ++i) {
		result[i] = 0.0;
	}
	return result;
}

ElementalConstraint* ElementalConstraint::convert () { // Converts a label constraint to a transition constraint
	assert(len == CostAgent::labels->size());
	ElementalConstraint* result = new ElementalConstraint(CostAgent::transitions->size());
	for (int i = 1; i < result->len; ++i) {
		std::map<pnapi::Label*, unsigned int> labels = (*CostAgent::transitions)[i]->getLabels();
		for (int j = 0; j < CostAgent::labels->size(); ++j) {
			if (labels.find((*CostAgent::labels)[j]) != labels.end()) {
			unsigned int weight = labels[(*CostAgent::labels)[j]];
			double value = lhs[j];
			result->lhs[i] += ((double) weight)*value;
			}
		}
	}
	result->rhs = rhs;
	result->sign = sign;
	return result;
}

void Request::convertAssertions () {
	std::vector<std::vector<ElementalConstraint*>*>* newassertions = new std::vector<std::vector<ElementalConstraint*>* >();
	for (int i = 0; i < assertions->size(); ++i) {
		newassertions->push_back(new std::vector<ElementalConstraint*>());
		assert((*assertions)[i] != 0);
		std::cerr << (*assertions)[i]->size()<< std::endl;
		for (int j = 0; j < (*assertions)[i]->size(); ++j) {
			(*newassertions)[i]->push_back((*(*assertions)[i])[j]->convert());
		}
	}
	//delete assertions;
	assertions = newassertions;
}

void Request::convertGrants () {
	assert(grants != 0);
	if (grants->size() == 0) return;
	std::vector<Grant*>* newgrants = new std::vector<Grant*>();
	for (int i = 0; i < grants->size(); ++i) {
		Grant* g = (*grants)[i];
		newgrants->push_back(g->convert());		
	}
	delete grants;
	grants = newgrants;
}

Grant* Grant::convert() {
	std::vector<ElementalConstraint*>* newconstraint = new std::vector<ElementalConstraint*>();
	for (int i = 0; i < constraint->size(); ++i) {
		newconstraint->push_back((*constraint)[i]->convert());
	}
	delete constraint;
	constraint = 0;

	assert(variableCosts != 0);
	
	double* newvariablecosts = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());
	for (int i = 1; i < CostAgent::transitions->size(); ++i) {
		std::map<pnapi::Label*, unsigned int> labels = (*CostAgent::transitions)[i]->getLabels();
		for (int j = 0; j < CostAgent::labels->size(); ++j) {
			if (labels.find((*CostAgent::labels)[j]) != labels.end()) {
				unsigned int weight = labels[(*CostAgent::labels)[j]];
				int value = variableCosts[j];
				newvariablecosts[i] += weight*value;
			}
		}
	}
	delete variableCosts;
	variableCosts = 0;
	Grant* newgrant = new Grant();
	newgrant->constraint = newconstraint;
	newgrant->variableCosts = newvariablecosts;
	newgrant->fixCosts = fixCosts;
	
	return newgrant;

}

void UseCase::output () {
	std::cerr << "Use case output" << std::endl;
	std::cerr << "Constraints (" << constraint->size() << ")" << std::endl;
	for (int i = 0; i < constraint->size(); ++i) {
		ElementalConstraint* c = (*constraint)[i];
		for (int j = 1; j < c->len; ++j) {
			std::cerr <<  c->lhs[j] << "*"<< (*CostAgent::transitions)[j]->getName() << " "; 
		}
		std::cerr <<  c->sign << " " << c->rhs << std::endl;
	}
	std::cerr << "Marking" << std::endl;
	for (int i = 0; i < CostAgent::places->size(); ++i) {
		std::cerr << (*CostAgent::places)[i]->getName() << ": "<< marking[i] << "; ";
	}
	std::cerr << std::endl;
	std::cerr << "Cost function" << std::endl;
	for (int j = 1; j < CostAgent::transitions->size(); ++j) {
		std::cerr <<  variableCosts[j] << "*" << (*CostAgent::transitions)[j]->getName() << " "; 
	}	
	
	std::cerr << fixCosts <<std::endl;

	std::cerr << "Policy" << std::endl;
	std::cerr << "(" << policyLHS << ", " << policyRHS << ")" << std::endl;	

}


void CostAgent::buildBasicStateEquation() {
	int START_TRANSITIONS = 1;
	int NR_OF_COLS = transitions->size() -1 ;
	
	delete_lp(basicStateEquation);
	basicStateEquation = make_lp(0, NR_OF_COLS);
	
	// If for some reason the lp system is null, we exit.
	if (basicStateEquation== NULL) {
		abort(1, "Unable to create new LP model");
	}

	// Set debug and verbose of this linear problem to FALSE
	set_debug(basicStateEquation, FALSE);
	set_verbose(basicStateEquation, FALSE);

	// Enter the "add row mode" for faster row insertion
	set_add_rowmode(basicStateEquation, TRUE);

	for (int placecounter = 0; placecounter < places->size(); ++placecounter) {
		
		pnapi::Place* place = (*places)[placecounter];
		
		int* transCol;
		REAL* transVal;
		
		// See how many transitions are connected to this place
		int number_of_transitions_for_this_place =
		place->getPresetArcs().size() + place->getPostsetArcs().size();

		// See if this line was already created before (and thus is stored in the static member).
		BinaryTreeNode<pnapi::Place*, std::pair<int , std::pair<int*, REAL*> > >* line =
		lines->find(place);

		// Counter for the columns with values.
		int tr = 0;

		// If the line did not exist before...
		if (line == 0) {


			// Allocate memory for the arrays
			transCol = new int[number_of_transitions_for_this_place]();
			transVal = new REAL[number_of_transitions_for_this_place]();


			// We now iterate over the preset of the place to retrieve all transitions positively effecting the place.
			for (std::set<pnapi::Arc *>::iterator pIt =
			place->getPresetArcs().begin(); pIt
			!= place->getPresetArcs().end(); ++pIt) {

				pnapi::Transition& t = (*pIt)->getTransition();

				// We add the transition, with the weight as a factor.
				transCol[tr] = START_TRANSITIONS
				+ getTransitionID(t.getName());
				transVal[tr] = (*pIt)->getWeight();
				++tr;
			}

			// We now iterate over the postset of the place to retrieve all transitions negatively effecting the place.
			for (std::set<pnapi::Arc *>::iterator pIt =
			place->getPostsetArcs().begin(); pIt
			!= place->getPostsetArcs().end(); ++pIt) {

				pnapi::Transition& t = (*pIt)->getTransition();
				if (place->getPreset().find(&t) != place->getPreset().end()) {
					int thisID = START_TRANSITIONS +  getTransitionID(t.getName());
					for (int i = 0; i < tr; ++i) {
						if (transCol[i] == thisID) {
							transVal[i] +=  -1 * (int) (*pIt)->getWeight();
						}
					}

				} else {

					// We add the transition, with the weight as a factor.
					transCol[tr] = START_TRANSITIONS
					+  getTransitionID(t.getName());
					transVal[tr] = -1 * (int) (*pIt)->getWeight();

					++tr;

				}
			}

			// Store this line for further use in the static member
			lines->insert(place, std::pair<int, std::pair<int*, REAL*> > (tr, std::pair<int*, REAL*>(transCol, transVal)),
			false);

		} else { // The line already exist, we just set our pointers to those in the static member
			tr = line->value.first;
			transCol = line->value.second.first;
			transVal = line->value.second.second;
		}
		
		add_constraintex(basicStateEquation, tr, transVal, transCol, GE, 0);
		
	}

	set_add_rowmode(basicStateEquation, FALSE);
	
	print_lp(basicStateEquation);
	
}

void CostAgent::buildStateEquationWithAssertions(UseCase* usecase, std::vector<ElementalConstraint*>* clause) {
	if (clause->size() == 0) return;
	delete_lp(currentStateEquationWithAssertions);
	currentStateEquationWithAssertions = copy_lp(basicStateEquation);
	lprec* se = currentStateEquationWithAssertions;
	set_add_rowmode(se, TRUE);
	for (int i = 0; i < CostAgent::places->size(); ++i) {
		set_rh(se,i+1,usecase->marking[i]);
	}
	// print_lp(se);
	for (int i = 0; i < clause->size(); ++i) {
			std::cerr << add_constraint(se,(*clause)[i]->lhs,(*clause)[i]->sign,(*clause)[i]->rhs) << std::endl;
	}
	set_add_rowmode(se, FALSE);
	print_lp(se);
}

