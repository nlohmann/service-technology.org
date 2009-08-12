#include "settings.h"
#include "Formula.h"

using namespace std;



// ****************************************************************************
// Formula
// ****************************************************************************


//! \brief Determines whether this Formula satisfies the given
//!        FormulaAssignment. Equivalent to value().
bool Formula::satisfies(const FormulaAssignment& assignment) const {

    return value(assignment);
}


//! \brief Placeholder for a virtual function
void Formula::getEventLiterals(set<string>& events) {
}


//! \brief Placeholder for a virtual function
void Formula::removeLiteral(const std::string&) {
}


//! \brief Placeholder for a virtual function
void Formula::removeLiteralForReal(const std::string&) {
}


//! \brief DESCRIPTION
//! \return DESCRIPTION
int Formula::getSubFormulaSize() const {
    // ich habe keine ahnung, warum diese funktion existiert (peter)
    // ich auch nicht (martin)
    assert(false);
    return 0;
}


//! \brief Placeholder for a virtual function
void Formula::removeLiteralByHiding(const std::string&) {
}



// ****************************************************************************
// FormulaMultiary
// ****************************************************************************


//! \brief basic constructor
FormulaMultiary::FormulaMultiary() :
    Formula() {
}


//! \brief Constructs a multiary formula from a given subformula by adding the
//!        subformula into the set of subformulae.
//! \param newformula The subformula to be added
FormulaMultiary::FormulaMultiary(Formula* newformula) {
    subFormulas.push_back(newformula);
}

//! \brief Constructs a multiary formula from two given subformulas. This
//!        constructor exists to conviently construct binary formulas from a left
//!        hand side and a right hand side subformula.
//! \param lhs Left hand side subformula of the to be constructed binary
//!        formula.
//! \param rhs Reft hand side subformula of the to be constructed binary
//!        formula.
FormulaMultiary::FormulaMultiary(Formula* lhs, Formula* rhs) {
    subFormulas.push_back(lhs);
    subFormulas.push_back(rhs);
}


//! \brief deconstructor
FormulaMultiary::~FormulaMultiary() {

    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
         currentFormula != subFormulas.end();
         currentFormula++) {
        delete *currentFormula;
    }

}


//! \brief turns the formula into a string
//! \return the formula as string
std::string FormulaMultiary::asString() const {
    if (subFormulas.empty()) {
        // we found an empty OR (equivalent to FALSE) or AND (equivalent to TRUE)
        return getEmptyFormulaEquivalent().asString();
    }

    std::string formulaString = "";
    subFormulas_t::const_iterator currentFormula = subFormulas.begin();

    if (subFormulas.size() == 1) {
        // we omit parenthesis if only one element in formula
        return (*currentFormula)->asString();
    } else {
        formulaString += '(' + (*currentFormula)->asString();

        while (++currentFormula != subFormulas.end()) {
            formulaString += " " + getOperator() + " " + (*currentFormula)->asString();
        }

        return formulaString + ')';
    }
}


//! \brief computes and returns the value of this formula under the given
//!        assignment
//! \param assignment an assignment for the formulas literals
//! \return true if the formula is true under the assginment, else false
bool FormulaMultiary::value(const FormulaAssignment& assignment) const {
    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
         currentFormula != subFormulas.end(); ++currentFormula) {
        if ((*currentFormula)->value(assignment) != getEmptyFormulaEquivalent().value(assignment)) {
            return !getEmptyFormulaEquivalent().value(assignment);
        }
    }

    return getEmptyFormulaEquivalent().value(assignment);
}


//! \brief adds a given subformula to this multiary
//! \param subformula the subformula to be added
void FormulaMultiary::addSubFormula(Formula* subformula) {
    subFormulas.push_back(subformula);
}


//! \brief removes the subformula at the given iterator position
//! \param subformula iterator at the position to be deleted
//! \return returns an iterator to the formula following the erased one
FormulaMultiary::iterator
FormulaMultiary::removeSubFormula(iterator subformula) {
    return subFormulas.erase(subformula);
}

//! \brief fills the given set of strings with all event-representing
//!        literals from the formula
//! \param events set of strings to fill
void FormulaMultiary::getEventLiterals(set<string>& events)  {


    // iterate over all subformulas
    for (subFormulas_t::iterator iCurrentFormula = subFormulas.begin();
         iCurrentFormula != subFormulas.end();
         iCurrentFormula++) {

        // gather the events of every single subformula
        (*iCurrentFormula)->getEventLiterals(events);
    }

}


//! \brief removes the given literal from the formula
//! \param name the name of the literal to be removed
void FormulaMultiary::removeLiteral(const std::string& name) {


    subFormulas_t::iterator iCurrentFormula;
    //cout << "\tanzahl von klauseln: " << subFormulas.size() << endl; int i = 1;
    //cout << "\tremoving literal " << name << " from clause nr " << i++ << endl;

    for (iCurrentFormula = subFormulas.begin();
         iCurrentFormula != subFormulas.end();) {

        // if the considered current formula is a literal, then remove it;
        // call the function recursively, otherwise
        FormulaLiteral* currentFormula = dynamic_cast<FormulaLiteral*> (*iCurrentFormula);
        if (currentFormula != NULL) {

            // the current formula is a literal
            if (currentFormula->asString() == name) {
                // the literal has the right name, so remove it
                delete *iCurrentFormula;
                iCurrentFormula = subFormulas.erase(iCurrentFormula);
            } else {
                iCurrentFormula++;
            }
        } else {
            // the current formula is no literal, so call removeLiteral again
            (*iCurrentFormula)->removeLiteral(name);
            iCurrentFormula++;
        }
    }

}


//! \brief returns the number of subformulas
//! \return number of subformulas
int FormulaMultiary::getSubFormulaSize() const {
    return subFormulas.size();
}


//! \brief removes the given literal from the formula, if this literal is the
//!        only one of a clause, the clause gets removed as well
//! \param name name of the literal to be removed
void FormulaMultiary::removeLiteralForReal(const std::string& name) {


    subFormulas_t::iterator iCurrentFormula;

    std::list<subFormulas_t::iterator> listOfFormulaIterator;

    std::list<subFormulas_t::iterator>::iterator iterOfListOfFormulaIterator;

    //cout << "\tanzahl von klauseln: " << subFormulas.size() << endl; int i = 1;
    //cout << "\tremoving literal " << name << " from clause nr " << i++ << endl;

    for (iCurrentFormula = subFormulas.begin();
         iCurrentFormula != subFormulas.end();) {
        // if the considered current formula is a literal, then remove it;
        // call the function recursively, otherwise
        FormulaLiteral* currentFormula = dynamic_cast<FormulaLiteral*> (*iCurrentFormula);
        if (currentFormula != NULL) {

            // the current formula is a literal
            if (currentFormula->asString() == name) {
                // the literal has the right name, so remove it
                delete *iCurrentFormula;
                iCurrentFormula = subFormulas.erase(iCurrentFormula);
            } else {
                iCurrentFormula++;
            }
        } else {
            if ((*iCurrentFormula)->getSubFormulaSize() > 0) {
                // the current formula is no literal, so call removeLiteral again
                (*iCurrentFormula)->removeLiteralForReal(name);
                if ((*iCurrentFormula)->getSubFormulaSize() == 0) {
                    // this clause just got empty, so we store it in a temporary list
                    // later on we want to remove this clause from the formula
                    listOfFormulaIterator.push_back(iCurrentFormula);
                }
            }
               iCurrentFormula++;
        }
    }

    for (iterOfListOfFormulaIterator = listOfFormulaIterator.begin();
            iterOfListOfFormulaIterator != listOfFormulaIterator.end();
            iterOfListOfFormulaIterator++) {

        // now we remove the clause that we have stored before from the formula
        // otherwise we would get a (false) when evaluating this formula -> this is not our intention
        subFormulas.erase(*iterOfListOfFormulaIterator);

    }

}


//! \brief removes the given literal from the formula by hiding it
//! \param name name of the literal to be removed
void FormulaMultiary::removeLiteralByHiding(const std::string& name) {


    subFormulas_t::iterator iCurrentFormula;
    //cout << "\tanzahl von klauseln: " << subFormulas.size() << endl; int i = 1;
    //cout << "\tremoving literal " << name << " from clause nr " << i++ << endl;

    for (iCurrentFormula = subFormulas.begin();
         iCurrentFormula != subFormulas.end();) {

        // if the considered current formula is a literal, then remove it;
        // call the function recursively, otherwise
        FormulaLiteral* currentFormula = dynamic_cast<FormulaLiteral*> (*iCurrentFormula);
        if (currentFormula != NULL) {
            // the current formula is a literal
            if (currentFormula->asString() == name) {
                // the literal has the right name, so remove it
                delete *iCurrentFormula;
                iCurrentFormula = subFormulas.erase(iCurrentFormula);
            } else {
                iCurrentFormula++;
            }
        } else {
            // the current formula is no literal, so call removeLiteral again
            (*iCurrentFormula)->removeLiteralByHiding(name);
            if ((dynamic_cast<FormulaMultiary*>(*iCurrentFormula))->empty()) {
                delete *iCurrentFormula;
                iCurrentFormula = subFormulas.erase(iCurrentFormula);
            } else {
                iCurrentFormula++;
            }
        }
    }

}


//! \brief deep copies the private members of this formula to a given one
//! \param newFormula formulamultiary to be copied to
void FormulaMultiary::deepCopyMultiaryPrivateMembersToNewFormula(FormulaMultiary* newFormula) const {
    newFormula->subFormulas.clear();
    for (subFormulas_t::const_iterator iFormula = subFormulas.begin();
         iFormula != subFormulas.end(); ++iFormula) {
        newFormula->subFormulas.push_back((*iFormula)->getDeepCopy());
    }
}

//! \brief returns an iterator to the begin of the subformula list
//! \return iterator to the begin of the subformula list
FormulaMultiary::iterator FormulaMultiary::begin() {
    return subFormulas.begin();
}


//! \brief returns a const iterator to the begin of the subformula list
//! \return const iterator to the begin of the subformula list
FormulaMultiary::const_iterator FormulaMultiary::begin() const {
    return subFormulas.begin();
}


//! \brief returns an iterator to the end of the subformula list
//! \return iterator to the end of the subformula list
FormulaMultiary::iterator FormulaMultiary::end() {
    return subFormulas.end();
}


//! \brief returns a const iterator to the end of the subformula list
//! \return iterator to the begin of the subformula list
FormulaMultiary::const_iterator FormulaMultiary::end() const {
    return subFormulas.end();
}


//! \brief checks whether there are no subformulas
//! \return returns true if there are no subformulas
bool FormulaMultiary::empty() const {
    return subFormulas.empty();
}


//! \brief returns the number of
//! \return iterator to the begin of the subformula list
int FormulaMultiary::size() const {
    return subFormulas.size();
}



// ****************************************************************************
// FormulaMultiaryAnd
// ****************************************************************************


//! \brief basic constructor
FormulaMultiaryAnd::FormulaMultiaryAnd() {
}


//! \brief Constructs a multiary and formula from a given subformula by adding the
//!        subformula into the set of subformulae.
//! \param formula The subformula to be added
FormulaMultiaryAnd::FormulaMultiaryAnd(Formula* formula) :
    FormulaMultiary(formula) {
}


//! \brief Constructs a multiary and formula from two given subformulas. This
//!        constructor exists to conviently construct binary formulas from a left
//!        hand side and a right hand side subformula.
//! \param lhs Left hand side subformula of the to be constructed binary
//!        formula.
//! \param rhs Reft hand side subformula of the to be constructed binary
//!        formula.
FormulaMultiaryAnd::FormulaMultiaryAnd(Formula* lhs_,
                                                 Formula* rhs_) :
    FormulaMultiary(lhs_, rhs_) {
}


//! \brief Returns the merged equivalent to this formula. Merging gets rid of
//!        unnecessary nesting of subformulas. (a*(b*c)) becomes (a*b*c). The
//!        caller is responsible for deleting the returned newly created formula.
//! \return returns the merged equivalent
FormulaMultiaryAnd* FormulaMultiaryAnd::merge() {
    bool final = true;
    FormulaMultiaryAnd *result = new FormulaMultiaryAnd;

    for (FormulaMultiaryAnd::iterator i = begin(); i != end(); i++) {
        if (dynamic_cast<FormulaMultiaryAnd*>(*i)) {
            final = false;
            FormulaMultiaryAnd* temp = dynamic_cast<FormulaMultiaryAnd*>(*i);
            for (FormulaMultiaryAnd::iterator j = temp->begin();
                 j != temp->end(); j++) {
                result->addSubFormula(dynamic_cast<Formula*>(*j)->getDeepCopy());
            }
        } else {
            result->addSubFormula(dynamic_cast<Formula*>(*i)->getDeepCopy());
        }
    }

    if (final) {
        return result;
    }

    FormulaMultiaryAnd* oldResult = result;
    result = result->merge();
    delete oldResult;
    return result;
}


//! \brief deep copies this formula
//! \return returns copy
FormulaMultiaryAnd* FormulaMultiaryAnd::getDeepCopy() const {
    FormulaMultiaryAnd* newFormula = new FormulaMultiaryAnd(*this);

    deepCopyMultiaryPrivateMembersToNewFormula(newFormula);

    return newFormula;
}


//! \brief returns the "and"-operator specifying the virtual function
//! \return returns "*"
std::string FormulaMultiaryAnd::getOperator() const {
    return "*";
}


//! \brief return an empty formula Equivalent as true
//! \return true empty formula Equivalent
const FormulaFixed FormulaMultiaryAnd::emptyFormulaEquivalent = FormulaTrue();

//! \brief return an empty formula Equivalent
//! \return empty formula Equivalent
const FormulaFixed& FormulaMultiaryAnd::getEmptyFormulaEquivalent() const {
    return emptyFormulaEquivalent;
}



// ****************************************************************************
// FormulaMultiaryOr
// ****************************************************************************


//! \brief basic constructor
FormulaMultiaryOr::FormulaMultiaryOr() {
}


//! \brief Constructs a multiary or formula from a given subformula by adding the
//!        subformula into the set of subformulae.
//! \param formula The subformula to be added
FormulaMultiaryOr::FormulaMultiaryOr(Formula* formula) :
    FormulaMultiary(formula) {
}


//! \brief Constructs a multiary or formula from two given subformulas. This
//!        constructor exists to conviently construct binary formulas from a left
//!        hand side and a right hand side subformula.
//! \param lhs Left hand side subformula of the to be constructed binary
//!        formula.
//! \param rhs Reft hand side subformula of the to be constructed binary
//!        formula.
FormulaMultiaryOr::FormulaMultiaryOr(Formula* lhs_,
                                               Formula* rhs_) :
    FormulaMultiary(lhs_, rhs_) {
}


//! \brief returns the "or"-operator specifying the virtual function
//! \return returns "+"
std::string FormulaMultiaryOr::getOperator() const {
    return "+";
}


//! \brief deep copies this formula
//! \return returns copy
FormulaMultiaryOr* FormulaMultiaryOr::getDeepCopy() const {
    FormulaMultiaryOr* newFormula =new FormulaMultiaryOr(*this);

    deepCopyMultiaryPrivateMembersToNewFormula(newFormula);

    return newFormula;
}

//! \brief adds a given subformula to this clause
//! \param subformula the subformula to be added
void FormulaMultiaryOr::addSubFormula(Formula* subformula) {

    if (dynamic_cast<FormulaLiteral*>(subformula)) {
	    for (FormulaMultiaryOr::iterator i = begin(); i != end(); i++) {
	    	if (dynamic_cast<FormulaLiteral*>(*i) &&
	    			(dynamic_cast<FormulaLiteral*>(*i)->asString() ==
	    			dynamic_cast<FormulaLiteral*>(subformula)->asString())) {
	    		return ;
	    	}
	    }
    }
    subFormulas.push_back(subformula);

}

//! \brief Returns the merged equivalent to this formula. Merging gets rid of
//!        unnecessary nesting of subformulas. (a+(b+c)) becomes (a+b+c). The
//!        caller is responsible for deleting the returned newly created formula.
//! \return returns the merged equivalent
FormulaMultiaryOr* FormulaMultiaryOr::merge() {
    bool final = true;
    FormulaMultiaryOr *result = new FormulaMultiaryOr;

    for (FormulaMultiaryOr::iterator i = begin(); i != end(); i++) {
        if (dynamic_cast<FormulaMultiaryOr*>(*i)) {
            final = false;
            FormulaMultiaryOr* temp = dynamic_cast<FormulaMultiaryOr*>(*i);
            for (FormulaMultiaryOr::iterator j = temp->begin();
                 j != temp->end(); j++) {
                result->addSubFormula(dynamic_cast<Formula*>(*j)->getDeepCopy());
            }
        } else {
            result->addSubFormula(dynamic_cast<Formula*>(*i)->getDeepCopy());
        }
    }

    if (final) {
        return result;
    }

    FormulaMultiaryOr* oldResult = result;
    result = result->merge();
    delete oldResult;
    return result;
}


//! \brief return an empty formula Equivalent as false
//! \return true empty formula Equivalent
const FormulaFixed FormulaMultiaryOr::emptyFormulaEquivalent = FormulaFalse();


//! \brief return an empty formula Equivalent
//! \return empty formula Equivalent
const FormulaFixed& FormulaMultiaryOr::getEmptyFormulaEquivalent() const {
    return emptyFormulaEquivalent;
}
