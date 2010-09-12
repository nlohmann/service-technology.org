#include "helpers.h"

unsigned int getBit(unsigned int field, unsigned int position) {
 	  unsigned int mask = 1 << position;
	  return ((mask & field) == mask);
}

// Static member storing all identifiers.
std::map<std::string, int> Universe::identifiers = std::map<std::string, int>();

CustomConstraint::CustomConstraint(std::vector<int>* aDomain, std::vector<int>* someValues, int op, int rhs) {
  domainsize = aDomain->size(); 
  domain = new int[domainsize];
  values = new REAL[domainsize];  
  for (int i = 0; i < domainsize; ++i) {
    domain[i] = (*aDomain)[i]; 
    values[i] = (REAL) (*someValues)[i]; 
  }
  this->op = op;
  this->rhs = rhs;
}

UnboundedConstraint::UnboundedConstraint(std::vector<int>* aDomain, std::vector<int>* someValues) {
  domainsize = aDomain->size(); 
  domain = new int[domainsize];
  values = new REAL[domainsize];  
  for (int i = 0; i < domainsize; ++i) {
    domain[i] = (*aDomain)[i]; 
    values[i] = (REAL) (*someValues)[i]; 
  }
}

LeftBoundedConstraint::LeftBoundedConstraint(std::vector<int>* aDomain, std::vector<int>* someValues, int aBound) {
  domainsize = aDomain->size(); 
  domain = new int[domainsize];
  values = new REAL[domainsize];
  bound = (REAL) aBound;
  
  for (int i = 0; i < domainsize; ++i) {
    domain[i] = (*aDomain)[i]; 
    values[i] = (REAL) (*someValues)[i]; 
  }
}

RightBoundedConstraint::RightBoundedConstraint(std::vector<int>* aDomain, std::vector<int>* someValues, int aBound) {
  domainsize = aDomain->size(); 
  domain = new int[domainsize];
  values = new REAL[domainsize];
  bound = (REAL) aBound;
  for (int i = 0; i < domainsize; ++i) {
    domain[i] = (*aDomain)[i]; 
    values[i] = (REAL) (*someValues)[i]; 
  }
}

BoundedConstraint::BoundedConstraint(std::vector<int>* aDomain, std::vector<int>* someValues, int aLeftBound, int aRightBound) {
  domainsize = aDomain->size(); 
  domain = new int[domainsize];
  values = new REAL[domainsize];
  lBound = (REAL) aLeftBound;
  rBound = (REAL) aRightBound;

  for (int i = 0; i < domainsize; ++i) {
    domain[i] = (*aDomain)[i]; 
    values[i] = (REAL) (*someValues)[i]; 
  }
}

int CustomConstraint::addToLP(lprec* lp) {
  add_constraintex(lp, domainsize, values, domain, op, rhs);
  return 1;
}


int LeftBoundedConstraint::addToLP(lprec* lp) {
  add_constraintex(lp, domainsize, values, domain, GE, bound);
  return 1;
}

int RightBoundedConstraint::addToLP(lprec* lp) {
  add_constraintex(lp, domainsize, values, domain, LE, bound);
  return 1;
}

int BoundedConstraint::addToLP(lprec* lp) {
  add_constraintex(lp, domainsize, values, domain, GE, lBound);
  add_constraintex(lp, domainsize, values, domain, LE, rBound);
  return 2;
}


// pair of an array of markings and a number of markings,
// each marking is a binary tree mapping a place pointer to a pair of a lp-sign and the rhs of the constraint
std::pair<BinaryTree<pnapi::Place*,std::pair<int,int> >**,int> translateFinalCondition(pnapi::PetriNet* net) {

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

std::vector<ServiceModel*> YasminaLog::models = std::vector<ServiceModel*>();
std::vector<Result> YasminaLog::results = std::vector<Result>();
std::string* YasminaLog::toString(bool reportSyntacticallyIncompatible){
  
  std::string* result = new std::string("MODELS\n");
  
  for (int i = 0; i < models.size(); ++i) {
    (*result) += "    M" + intToStr(models[i]->alias) + ": " + models[i]->name + ";\n";
  }  
  (*result) += "RESULTS\n";

  for (int i = 0; i < results.size(); ++i) {
    if (reportSyntacticallyIncompatible || (results[i].result != -1)) {
      std::string* current = results[i].toString(); 
      (*result) += "    " + (*current) + ";\n";
      delete current;
    }
  }  
  return result;
}

std::string intToStr(int i) {
  std::stringstream out;
  out << i;
  return out.str();
}

