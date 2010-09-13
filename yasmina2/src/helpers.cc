#include "helpers.h"

unsigned int getBit(unsigned int field, unsigned int position) {
 	  unsigned int mask = 1 << position;
	  return ((mask & field) == mask);
}

// Static member storing all identifiers.
std::map<std::string, int> Universe::identifiers = std::map<std::string, int>();

int Universe::addIdentifier (std::string* identifier) {
  if (identifiers.find(*identifier) != identifiers.end()) {
    return identifiers[*identifier];
  } else {
    int val = identifiers.size();
    identifiers[*identifier] = val+1;
  }
  return identifiers.size();
}

std::string Universe::getIdentifier (int numID) {
  std::string s = "";
  if (identifiers.size() >= numID) {
    for (std::map<std::string, int>::iterator it = identifiers.begin(); it != identifiers.end(); ++it) {
      if ((*it).second == numID) {
        s = (*it).first;
        return s;
      }
    }
  }
  return s;
}
  
int Universe::getNumID(std::string* identifier) {
  return identifiers[*identifier];
}

int Universe::getUniverseSize() {
  return identifiers.size();
}

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

Result::Result(std::vector<int>* composite, int res) {
  models = composite; result = res;      
}

std::string* Result::toString() {
  std::string* resultstring = new std::string();
  for (int i = 0; i < models->size(); ++i) {
    if (i != 0) {
      (*resultstring) += " (+) ";
    }
    (*resultstring) += "M" + intToStr((*models)[i]+1);
  }
  (*resultstring) += ": ";
  if (result == 0) {
    (*resultstring) += "maybe";
  } else if (result == 1) {
    (*resultstring) += "no";
  } else if (result == -1) {
    (*resultstring) += "undefined";
  }
  return resultstring; 
}


StateEquation::StateEquation(pnapi::PetriNet* net) {
  ServiceModel();
  
  // Get interface 
  std::set<pnapi::Label *> labels = net->getInterface().getAsynchronousLabels();
  std::map<pnapi::Label*, int> labelMap;
  for (std::set<pnapi::Label *>::iterator it =
	    labels.begin(); it != labels.end(); ++it) {
	    std::string* labelName = new std::string((*it)->getName());
      int id = Universe::addIdentifier(labelName);
      delete labelName;
      labelMap[*it] = id;
      if ((*it)->getType() == 1) {
        this->inputs.push_back(id);
      } else {
        this->outputs.push_back(id);
      }
  }
  // Add all transitions as identifiers and store their numIDs in a map.
  std::map<pnapi::Transition*, int> transitionMap;
  
  for (std::set<pnapi::Transition*>::iterator it =
				net->getTransitions().begin(); it
				!= net->getTransitions().end(); ++it) {
    transitionMap[(*it)] = Universe::addIdentifier(&(*it)->getName());
  }

  // Create the lhs of the state equation which is equal in any final marking
  
  std::map<pnapi::Place*, std::pair<std::vector<int>*, std::vector<int>*> > lhs;
  
  for (std::set<pnapi::Place*>::iterator it =
        net->getPlaces().begin(); it
        != net->getPlaces().end(); ++it) {

    std::vector<int>* domain = new std::vector<int>();
    std::vector<int>* values = new std::vector<int>();            

    for (std::set<pnapi::Arc *>::iterator pIt =
        (*it)->getPresetArcs().begin(); pIt
        != (*it)->getPresetArcs().end(); ++pIt) {
        pnapi::Transition& t = (*pIt)->getTransition();
        domain->push_back(transitionMap[&t]);
        values->push_back((*pIt)->getWeight());
    }

    for (std::set<pnapi::Arc *>::iterator pIt =
        (*it)->getPostsetArcs().begin(); pIt
        != (*it)->getPostsetArcs().end(); ++pIt) {
        pnapi::Transition& t = (*pIt)->getTransition();
        int id = transitionMap[&t];
        bool set = false;
        for (int x = 0; x < domain->size(); ++x) {
          if (!set && (*domain)[x] == id) {
            (*values)[x] -= (*pIt)->getWeight();
          }
        }
        
        if (!set) {
          domain->push_back(transitionMap[&t]);
          values->push_back(-1*(*pIt)->getWeight());
        }
    }
    lhs[*it] = std::pair<std::vector<int>*, std::vector<int>* >(domain, values);    

  }
  
  // Now compute constraints for each label
  
  std::vector<Constraint*> labelConstraints;
  // for each label, add a constraint
  for (std::set<pnapi::Label *>::iterator it =
    labels.begin(); it != labels.end(); ++it) {
    std::vector<int>* domain = new std::vector<int>();
    std::vector<int>* values = new std::vector<int>();            
    int id = labelMap[(*it)];
    domain->push_back(id);
    values->push_back(-1);      
    for (std::set<pnapi::Transition*>::iterator tIt = (*it)->getTransitions().begin(); tIt != (*it)->getTransitions().end(); ++tIt) {
      domain->push_back(transitionMap[*tIt]);
      values->push_back(1);
    }
    CustomConstraint* newConstraint = new CustomConstraint(domain,values,EQ,0);
    labelConstraints.push_back(newConstraint);
  }

  
  // Now we need final markings.

  status("Computing final markings...");
	std::vector<std::map<pnapi::Place*, std::pair<int,int> > > translated = translateFinalCondition(net);
	
	status("Computed %i final markings",translated.size());
	
	for (int i = 0; i < translated.size(); ++i) {
    
    this->constraints.push_back(std::vector<Constraint*>());
    int current = this->constraints.size()-1;
    
    // For each place, add the constraints. 
    for (std::set<pnapi::Place*>::iterator it =
				  net->getPlaces().begin(); it
				  != net->getPlaces().end(); ++it) {
      
      std::pair<int, int>& node = translated  [i][(*it)];

      int op = node.first;
			int value = node.second - (*it)->getTokenCount();

      CustomConstraint* newConstraint = new CustomConstraint(lhs[(*it)].first,lhs[(*it)].second,op,value);
      this->constraints[current].push_back(newConstraint);
    }
    
    // Add the constraint for each label
    for (int i = 0; i < labelConstraints.size(); ++i) {
      this->constraints[current].push_back(labelConstraints[i]);
    }
    
  }


}
  
std::vector<std::map<pnapi::Place*, std::pair<int,int> > > StateEquation::translateFinalCondition(pnapi::PetriNet* net) {

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
	
  std::vector<std::map<pnapi::Place*, std::pair<int,int> > > result = 	std::vector<std::map<pnapi::Place*, std::pair<int,int> > >(size);
  
	int i = 0;
	for (std::set<const pnapi::formula::Formula *>::iterator dIt =
	dForm.getChildren().begin(); dIt
	!= dForm.getChildren().end(); ++dIt) {
		
		
		if ((*dIt)->getType() != pnapi::formula::Formula::F_CONJUNCTION) {
			continue;
		}
		const pnapi::formula::Conjunction* cForm = dynamic_cast<const pnapi::formula::Conjunction*>(*dIt);

		result[i] = std::map<pnapi::Place*, std::pair<int, int> >();
		for (std::set<const pnapi::formula::Formula *>::iterator cIt =
		cForm->getChildren().begin(); cIt
		!= cForm->getChildren().end(); ++cIt) {
			
			const pnapi::formula::Formula* src = *cIt;
			
			// p1 = j means a new partial marking M with M(p1) = j
			if (typeid(*src) == typeid(pnapi::formula::FormulaEqual)) {

				const pnapi::formula::FormulaEqual
				* const castedSrc = dynamic_cast<const pnapi::formula::FormulaEqual* const > (src);

				result[i][const_cast<pnapi::Place*>(&(castedSrc->getPlace()))] = std::pair<int,int>(EQ, castedSrc->getTokens());
				
			}

			// p1 > j means a new partial marking Mi for each 1 <= i <= bound - j where Mi(p1) = i+j+1
			else if (typeid(*src)
					== typeid(pnapi::formula::FormulaGreater)) {

				const pnapi::formula::FormulaGreater
				* const castedSrc =
				dynamic_cast<const pnapi::formula::FormulaGreater* const > (src);

				result[i][const_cast<pnapi::Place*>(&(castedSrc->getPlace()))] = std::pair<int,int>(GE, castedSrc->getTokens()+1);

			}

			// p1 >= j means a new partial marking Mi for each 0 <= i <= bound - j where Mi(p1) = i+j
			else if (typeid(*src)
					== typeid(pnapi::formula::FormulaGreaterEqual)) {

				const pnapi::formula::FormulaGreaterEqual
				* const castedSrc =
				dynamic_cast<const pnapi::formula::FormulaGreaterEqual* const > (src);
				result[i][const_cast<pnapi::Place*>(&(castedSrc->getPlace()))] = std::pair<int,int>(GE, castedSrc->getTokens());
			}

			// p1 < j means a new partial marking Mi for each 0 <= i <= j-1 where Mi(p1) = i
			else if (typeid(*src)
					== typeid(pnapi::formula::FormulaLess)) {
				const pnapi::formula::FormulaLess
				* const castedSrc =
				dynamic_cast<const pnapi::formula::FormulaLess* const > (src);
				result[i][const_cast<pnapi::Place*>(&(castedSrc->getPlace()))] = std::pair<int,int>(LE, castedSrc->getTokens()-1);
			}

			// p1 <= j means a new partial marking Mi for each 0 <= i <= j where Mi(p1) = i
			else if (typeid(*src)
					== typeid(pnapi::formula::FormulaLessEqual)) {
				const pnapi::formula::FormulaLessEqual
				* const castedSrc =
				dynamic_cast<const pnapi::formula::FormulaLessEqual* const > (src);
				result[i][const_cast<pnapi::Place*>(&(castedSrc->getPlace()))] = std::pair<int,int>(LE, castedSrc->getTokens());
			} else {
				//assert(false);
				
			}
			
		}
		
		++i;
		
	}
	
	return result;
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

