#include "costs.h"

std::vector<pnapi::Place*>* CostAgent::places;
std::vector<pnapi::Transition*>* CostAgent::transitions;
std::vector<pnapi::Label*>* CostAgent::labels;

int CostAgent::getTransitionID(std::string transition) {
	for (int i = 0; i < transitions->size(); ++i) {
		if ((*CostAgent::transitions)[i]->getName() == transition) {
			return i;
		}
	}
	assert(false);
}

int CostAgent::getPlaceID(std::string place) {
	for (int i = 0; i < places->size(); ++i) {
		if ((*CostAgent::places)[i]->getName() == place) {
			return i;
		}
	}
	assert(false);
}

int CostAgent::getLabelID(std::string label) {
	for (int i = 0; i < labels->size(); ++i) {
		if ((*CostAgent::labels)[i]->getName() == label) {
			return i;
		}
	}
	assert(false);
}

int* CostAgent::getCleanIntArray(int len) {
	int* result = new int[len];
	for (int i = 0; i < len; ++i) {
		result[i] = 0;
	}
	return result;
}

ElementalConstraint* ElementalConstraint::convert () { // Converts a label constraint to a transition constraint
	assert(len == CostAgent::labels->size());
	ElementalConstraint* result = new ElementalConstraint(CostAgent::transitions->size());
	for (int i = 0; i < result->len; ++i) {
		std::map<pnapi::Label*, unsigned int> labels = (*CostAgent::transitions)[i]->getLabels();
		for (int j = 0; j < CostAgent::labels->size(); ++j) {
			unsigned int weight = labels[(*CostAgent::labels)[j]];
			int value = lhs[j];
			result->lhs[i] += weight*value;
		}
	}
	return result;
}


void Grant::convertConstraints () {
	std::vector<ElementalConstraint*>* newconstraint = new std::vector<ElementalConstraint*>(constraint->size());
	for (int i = 0; i < constraint->size(); ++i) {
		newconstraint->push_back((*constraint)[i]->convert());
	}
	delete constraint;
	constraint = newconstraint;
}

void Request::convertAssertions () {
	std::vector<std::vector<ElementalConstraint*>*>* newassertions = new std::vector<std::vector<ElementalConstraint*>* >(assertions->size());
	for (int i = 0; i < assertions->size(); ++i) {
		newassertions->push_back(new std::vector<ElementalConstraint*>((*assertions)[i]->size()));
		for (int j = 0; j < (*assertions)[i]->size(); ++j) {
			(*newassertions)[i]->push_back((*(*assertions)[i])[j]->convert());
		}
		delete (*assertions)[i];
	}
	delete assertions;
	assertions = newassertions;
}