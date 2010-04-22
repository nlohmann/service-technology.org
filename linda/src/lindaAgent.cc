#include "lindaAgent.h"
#include "cmdline.h"
extern gengetopt_args_info args_info;

pnapi::PetriNet* LindaAgent::mNet = 0;
std::vector<BinaryTree<pnapi::Place*,std::pair<int,int> >*>* LindaAgent::mFinals = 0;
std::vector<ExtendedStateEquation*>* LindaAgent::mSystems = 0;
bool LindaAgent::keepHistory;


bool LindaAgent::initialize(pnapi::PetriNet* net,bool keepHistory) {
	mNet = net;

	mFinals = new std::vector<BinaryTree<pnapi::Place*,std::pair<int,int> >*>();
	mSystems = new std::vector<ExtendedStateEquation*>();


	LindaAgent::keepHistory = keepHistory;

	std::set<pnapi::Label *> labels = net->getInterface().getAsynchronousLabels();

	// Create the global event IDs
	LindaHelpers::NR_OF_EVENTS = labels.size();
	LindaHelpers::EVENT_STRINGS = new std::string[LindaHelpers::NR_OF_EVENTS]();
	LindaHelpers::EVENT_LABELS
	= new pnapi::Label*[LindaHelpers::NR_OF_EVENTS]();

	int counter = 0;
	for (std::set<pnapi::Label *>::iterator it =
	labels.begin(); it != labels.end(); ++it) {
		LindaHelpers::EVENT_STRINGS[counter] = (*it)->getName();
		LindaHelpers::EVENT_LABELS[counter] = (*it);
		++counter;
	}


	return (mNet != 0);
}



uint8_t* LindaAgent::getXORTable(uint8_t firstOfInterest, uint8_t lastOfInterest) {

	uint8_t* eventsOfInterest = new uint8_t(lastOfInterest-firstOfInterest+1);

	for (uint8_t i = firstOfInterest; i <= lastOfInterest; ++i) {
		eventsOfInterest[i] = i;
	}

	uint8_t* result = getXORTable(eventsOfInterest,lastOfInterest-firstOfInterest+1);

	delete eventsOfInterest;

	return result;

}


uint8_t* LindaAgent::getXORTable(uint8_t* eventsOfInterest, uint8_t count) {

	uint8_t bytes = (count*count / 8) + 1;
	uint8_t* storage = new uint8_t[bytes];
	for (size_t i = 0; i < bytes; ++i) {
		// initially, all sending events are not mutually exclusive
		storage[i] = 0;
	}


	for (uint8_t i = 0; i < count; ++i) {
		for (uint8_t j = i + 1; j < count; ++j) {

			EventTermBound* bounds = new EventTermBound[mSystems->size()]();
			EventTerm* term = new AddTerm(new BasicTerm(i), new BasicTerm(j));

			uint8_t counter = 0;
			for (std::vector<ExtendedStateEquation*>::iterator it =
			mSystems->begin(); it != mSystems->end(); ++it) {
				EventTermBound* result = (*it)->evaluate(term);
				bounds[counter++] = *result;

			}

			EventTermBound* b = EventTermBound::merge(bounds, mSystems->size());



			/// Evaluate and write to bit array (both positions!).
			if (b->lowerBounded && b->upperBounded && b->lowerBound <= 1
					&& b->upperBound == 1) {

				const uint8_t actualFirstPosition = i;
				const uint8_t actualSecondPosition = j;

				const uint8_t firstPosition = (actualFirstPosition) * (count) + (actualSecondPosition);
				const uint8_t secondPosition = (actualSecondPosition) * (count) + (actualFirstPosition);

				const uint8_t myFirstByte = firstPosition / 8;
				const uint8_t myFirstBit = firstPosition % 8;

				const uint8_t mySecondByte = secondPosition / 8;
				const uint8_t mySecondBit = secondPosition % 8;

				storage[myFirstByte] += (1 << myFirstBit);
				storage[mySecondByte] += (1 << mySecondBit);
			}

			delete b;
		}
	}
	return storage;

}

void LindaAgent::finalize() {

	// Cached incidence matrix is not needed anymore
	delete ExtendedStateEquation::lines;
	delete[] ExtendedStateEquation::eventLines;
	delete ExtendedStateEquation::transitionID;


	for (int i = 0; i < mFinals->size(); ++i) {
		delete (*mFinals)[i];
		(*mFinals)[i] = 0;
	}

	delete mFinals;

	for (int i = 0; i < mSystems->size(); ++i) {
		delete (*mSystems)[i];
		(*mSystems)[i] = 0;
	}

	delete mSystems;

	delete ExtendedStateEquation::lines;
	delete[] ExtendedStateEquation::eventLines;
	delete ExtendedStateEquation::transitionID;
}

std::pair<BinaryTree<pnapi::Place*,std::pair<int,int> >**,int> LindaAgent::translateFinalCondition(pnapi::PetriNet* net) {

	pnapi::Condition& cond = net->getFinalCondition();
	cond.dnf();
	const pnapi::formula::Formula& form = cond.getFormula();
	pnapi::formula::FormulaFalse* falseDummy = new pnapi::formula::FormulaFalse; 
	pnapi::formula::FormulaTrue* trueDummy = new pnapi::formula::FormulaTrue; 
	pnapi::formula::Conjunction* conjDummy = new pnapi::formula::Conjunction(form,*trueDummy);
	pnapi::formula::Disjunction* disjDummy = new pnapi::formula::Disjunction(form,*falseDummy);
	pnapi::formula::Disjunction* disjDummy2 = new pnapi::formula::Disjunction(*conjDummy,*falseDummy);

	pnapi::formula::Disjunction dForm = pnapi::formula::Disjunction(*falseDummy,*falseDummy);

	if (typeid(form) == typeid(pnapi::formula::Disjunction)) {
		dForm = dynamic_cast<const pnapi::formula::Disjunction&>(form);
		} else if (typeid(form) == typeid(pnapi::formula::Conjunction)) {
		dForm = *disjDummy;
		} else {
		dForm = *disjDummy2;
		}
	int size = dForm.getChildren().size();
	
	BinaryTree<pnapi::Place*,std::pair<int,int> >** result = new BinaryTree<pnapi::Place*,std::pair<int,int> >*[size];
	
	int i = 0;
	for (std::set<const pnapi::formula::Formula *>::iterator dIt =
	dForm.getChildren().begin(); dIt
	!= dForm.getChildren().end(); ++dIt) {
		
		
		if ((*dIt)->getType() != pnapi::formula::Formula::F_CONJUNCTION) {
			continue;
		}
		const pnapi::formula::Conjunction* cForm = dynamic_cast<const pnapi::formula::Conjunction*>(*dIt);

		result[i] = new BinaryTree<pnapi::Place*,std::pair<int,int> >;
		for (std::set<const pnapi::formula::Formula *>::iterator cIt =
		cForm->getChildren().begin(); cIt
		!= cForm->getChildren().end(); ++cIt) {
			
			const pnapi::formula::Formula* src = *cIt;
			
			// p1 = j means a new partial marking M with M(p1) = j
			if (typeid(*src) == typeid(pnapi::formula::FormulaEqual)) {

				const pnapi::formula::FormulaEqual
				* const castedSrc = dynamic_cast<const pnapi::formula::FormulaEqual* const > (src);

				result[i]->insert(const_cast<pnapi::Place*>(&(castedSrc->getPlace())), std::pair<int,int>(EQ, castedSrc->getTokens()), false);
				
			}

			// p1 > j means a new partial marking Mi for each 1 <= i <= bound - j where Mi(p1) = i+j+1
			else if (typeid(*src)
					== typeid(pnapi::formula::FormulaGreater)) {

				const pnapi::formula::FormulaGreater
				* const castedSrc =
				dynamic_cast<const pnapi::formula::FormulaGreater* const > (src);

				result[i]->insert(const_cast<pnapi::Place*>(&(castedSrc->getPlace())), std::pair<int,int>(GE, castedSrc->getTokens()+1), false);

			}

			// p1 >= j means a new partial marking Mi for each 0 <= i <= bound - j where Mi(p1) = i+j
			else if (typeid(*src)
					== typeid(pnapi::formula::FormulaGreaterEqual)) {

				const pnapi::formula::FormulaGreaterEqual
				* const castedSrc =
				dynamic_cast<const pnapi::formula::FormulaGreaterEqual* const > (src);
				result[i]->insert(const_cast<pnapi::Place*>(&(castedSrc->getPlace())), std::pair<int,int>(GE, castedSrc->getTokens()), false);
			}

			// p1 < j means a new partial marking Mi for each 0 <= i <= j-1 where Mi(p1) = i
			else if (typeid(*src)
					== typeid(pnapi::formula::FormulaLess)) {
				const pnapi::formula::FormulaLess
				* const castedSrc =
				dynamic_cast<const pnapi::formula::FormulaLess* const > (src);
				result[i]->insert(const_cast<pnapi::Place*>(&(castedSrc->getPlace())), std::pair<int,int>(LE, castedSrc->getTokens()-1), false);
			}

			// p1 <= j means a new partial marking Mi for each 0 <= i <= j where Mi(p1) = i
			else if (typeid(*src)
					== typeid(pnapi::formula::FormulaLessEqual)) {
				const pnapi::formula::FormulaLessEqual
				* const castedSrc =
				dynamic_cast<const pnapi::formula::FormulaLessEqual* const > (src);
				result[i]->insert(const_cast<pnapi::Place*>(&(castedSrc->getPlace())), std::pair<int,int>(LE, castedSrc->getTokens()), false);
			} else {
				//assert(false);
				
			}
			
		}
		
		++i;
		
	}
	
	std::pair<BinaryTree<pnapi::Place*,std::pair<int,int> >**,int> res(result,size);
	
	return res;
}


bool LindaAgent::addFinalMarkingsFromFinalCondition(uint8_t bound) {
	// Calculate the final markings from the final condition

	status("Computing final markings...");
	std::pair<BinaryTree<pnapi::Place*,std::pair<int,int> >**,int> translated = translateFinalCondition(mNet);
	
	status("Computed %i final markings",translated.second);
	
	for (int i = 0; i < translated.second; ++i) {
		
		ExtendedStateEquation* XSE = new ExtendedStateEquation(mNet,translated.first[i]);
		if (XSE->constructLP()) {
			mSystems->push_back(XSE);
			mFinals->push_back(translated.first[i]);
			status("    Final marking found: %s",
			 LindaAgent::getFinalMarkingString(mFinals->size()-1).c_str());
			if (args_info.show_lp_flag) {
					XSE->output();
			}
		}
	}
}

std::string LindaAgent::getFinalMarkingString(int i) {
		std::string result = "";
		bool first = true;
		for (std::set<pnapi::Place*>::iterator it = mNet->getPlaces().begin(); it != mNet->getPlaces().end(); ++it) {
		if (first) {
			first = false;
		} else {
			result += ", ";
		}
		int value = 0;
		int op = GE;
		BinaryTreeNode<pnapi::Place*,std::pair<int,int> >* node = (*mFinals)[i]->find(*it);
		if (node != 0) {
			op = node->value.first;
			value = node->value.second;
		}
		
		result += (*it)->getName();
		if (op == GE) {
		result += ">=";
		}

		else if (op == EQ) {
		result += "=";
		}

		else if (op == LE) {
		result += "<=";
		}
		
		result += LindaHelpers::intToStr(value);
		
		}
		return result + ";";
		
}
