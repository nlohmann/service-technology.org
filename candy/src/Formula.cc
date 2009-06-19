#include "Formula.h"

// if you need assertions, always include these headers in _this_ order
#include "config.h"
#include <cassert>

using namespace std;

// TRUE and FALSE #defined in cudd package may interfere with
// FormulaLiteral::TRUE and ...::FALSE.
#undef TRUE
#undef FALSE


const std::string FormulaLiteral::FINAL = std::string("final");
const std::string FormulaLiteral::TAU = std::string("tau");
const std::string FormulaLiteral::TRUE = std::string("true");
const std::string FormulaLiteral::FALSE = std::string("false");


//! \brief  Sets the given literal to the given truth value within this
//!         FormulaAssignment.
//! \param literal Literal whose truth value should be set.
//! \param value Truth value the given literal should be set to.
void FormulaAssignment::set(const std::string& literal, bool value) {
    literal2bool[literal] = value;
}


//! \brief Sets the given literal to true within this FormulaAssignment.
//! \param literal Literal which should be set to true.
void FormulaAssignment::setToTrue(const std::string& literal) {
    set(literal, true);
}


//! \brief Sets the given literal to false within this FormulaAssignment.
//! \param literal Literal which should be set to false.
void FormulaAssignment::setToFalse(const std::string& literal) {
    set(literal, false);
}


//! \brief Gets the truth value of the given literal within this
//!        FormulaAssignment. If no truth value was previously set for
//!        this literal by set() or setToTrue(), then the truth value of the given
//!        literal is implicetly false.
//! \param literal Literal whose truth value should be determined.
//! \returns Truth value of the given literal.
bool FormulaAssignment::get(const std::string& literal) const {
    literal2bool_t::const_iterator
            literal2bool_iter = literal2bool.find(literal);

    if (literal2bool_iter == literal2bool.end())
        return false;

    return literal2bool_iter->second;
}


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

// [LUHME XV] Funktionsname umbenennen in "gibt einen Wert einer 3-Wertigen Logik zurück"
// [LUHME XV] nicht mit dynamic_cast, sondern mit virtual Funktionen realisieren
//! \brief computes and returns the value of the formula without an assigment
//! \return returns unknown, true or false
threeValueLogic Formula::equals() {

    if (dynamic_cast<FormulaLiteral*>(this)) {
      // [LUHME XV] WTF? der Rückgabewert wird über einen Stringvergleich mit
      // [LUHME XV] String-Repräsentationen der Werte der Logik während des
      // [LUHME XV] Aufbau des Zustandsraumes ermittelt
      // [LUHME XV] sinnvoller Lösungsansatz: Funktionalität Formel-Auswerten
      // [LUHME XV] und -Reduzieren von String-Darstellung trennen
        if (asString() == FormulaLiteral::TRUE) {
            return TRUE;
        } else if (asString() == FormulaLiteral::FALSE) {
            return FALSE;
        } else {
            return UNKNOWN;
        }
    }

    // assert: this is no FormulaLiteral.

    if (dynamic_cast<FormulaMultiaryOr*>(this)) {
        bool unknownFound = false;
        for (FormulaMultiaryOr::iterator
             i = dynamic_cast<FormulaMultiaryOr*>(this)->begin();
             i != dynamic_cast<FormulaMultiaryOr*>(this)->end(); i++) {

            if (dynamic_cast<Formula*>(*i)->equals() == TRUE) {
                return TRUE;
            }

            if (dynamic_cast<Formula*>(*i)->equals() == UNKNOWN) {
                unknownFound = true;
            }
        }

        return unknownFound ? UNKNOWN : FALSE;
    }

    if (dynamic_cast<FormulaMultiaryAnd*>(this)) {
        bool noUnknownFound = true;
        for (FormulaMultiaryAnd::iterator
             i = dynamic_cast<FormulaMultiaryAnd*>(this)->begin();
             i != dynamic_cast<FormulaMultiaryAnd*>(this)->end(); i++) {

            if (dynamic_cast<Formula*>(*i)->equals() == FALSE) {
                return FALSE;
            }

            if (dynamic_cast<Formula*>(*i)->equals() == UNKNOWN) {
                noUnknownFound = false;
            }
        }
        return noUnknownFound ? TRUE : UNKNOWN;
    }

    assert(false);
    return UNKNOWN;
}


//! \brief Returns this formula in conjunctive normal form. The caller is
//!        responsible for deleting the newly created and returned formula.
/// \return returns the CNF
FormulaCNF* Formula::getCNF() {

    // CNF(literal) = literal
    if (dynamic_cast<FormulaLiteral*>(this)) {
        FormulaMultiaryOr* clause = new FormulaMultiaryOr;

        clause->addSubFormula(this->getDeepCopy());
        FormulaCNF* cnf = new FormulaCNF;
        cnf->addClause(clause);
        return cnf;
    }

    // CNF(cnf) = cnf
    if (dynamic_cast<FormulaCNF*>(this)) {
        return dynamic_cast<FormulaCNF*>(this)->getDeepCopy();
    }

    // CNF(phi AND psi) = CNF(phi) AND CNF(psi)
    if (dynamic_cast<FormulaMultiaryAnd*>(this)) {
        FormulaCNF* cnf = new FormulaCNF;
        for (FormulaMultiaryAnd::iterator
             i = dynamic_cast<FormulaMultiaryAnd*>(this)->begin();
             i != dynamic_cast<FormulaMultiaryAnd*>(this)->end(); i++) {

            FormulaCNF *temp = dynamic_cast<Formula*>(*i)->getCNF();
            for (FormulaCNF::iterator j = temp->begin(); j != temp->end(); j++) {
                cnf->addClause(dynamic_cast<FormulaMultiaryOr*>(*j)->getDeepCopy());
            }
            delete temp;
        }
        return cnf;
    }

    // CNF(phi OR psi) = ...  things becoming ugly
    if (dynamic_cast<FormulaMultiaryOr*>(this)) {
        FormulaCNF* cnf = new FormulaCNF;
        FormulaMultiaryOr* temp = dynamic_cast<FormulaMultiaryOr*>(this);
        FormulaMultiaryAnd *clause;
        //FormulaCNF *conj = new FormulaCNF;
        bool final = true;

        // first of all, merge all top-level OR's
        temp = temp->merge();

        // all SubFormulas shoud now be either Literal or MultiaryAnd

        // iterate over the MultiaryOr
        for (FormulaMultiaryOr::iterator i = temp->begin(); i != temp->end(); ++i) {
            // CNF((phi1 AND phi2) OR psi) = CNF(phi1 OR psi) AND CNF(phi2 OR psi)
            if (dynamic_cast<FormulaMultiaryAnd*>(*i)) {
                clause = dynamic_cast<FormulaMultiaryAnd*>(*i);
                final = false;

                for (FormulaMultiaryAnd::iterator j = clause->begin();
                     j != clause->end(); ++j) {

                    FormulaMultiaryOr *disj = new FormulaMultiaryOr;
                    disj->addSubFormula(dynamic_cast<Formula*>(*j)->getDeepCopy());

                    for (FormulaMultiaryOr::iterator k = temp->begin();
                         k != temp->end(); ++k) {

                        if (k != i) {
                            disj->addSubFormula(dynamic_cast<Formula*>(*k)->getDeepCopy());
                        }
                    }

                    FormulaCNF *temp_cnf = disj->getCNF();
                    for (FormulaMultiaryAnd::iterator
                         l = temp_cnf->begin(); l != temp_cnf->end(); ++l) {
                        cnf->addClause(dynamic_cast<FormulaMultiaryOr*>(*l)->getDeepCopy());
                    }
                    delete temp_cnf;
                }
            }
        }
        if (final) {
            cnf->addClause(temp->getDeepCopy());
        } else {
        }
        delete temp;
        return cnf;
    }

    return NULL; // should not happen! probably new derivate of Formula
}

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


//! \brief tests if a given formula is already implied in this one,
//!        shall only be used for clauses within cnf!
//! \param op the formula that could be implied
//! \return returns true if the given formula is implied, else false
bool FormulaMultiaryOr::implies(FormulaMultiaryOr *op) {
    bool result;


    // FormulaCNF::simplify() depends on this if clause to remove superfluous
    // true-clauses.
    if (op->equals() == TRUE) {
        return true;
    }

    for (FormulaMultiaryOr::iterator i = this->begin(); i != this->end(); i++) {
        result = false;
        for (FormulaMultiaryOr::iterator j = op->begin(); j != op->end(); j++) {
            result |= (dynamic_cast<FormulaLiteral*>(*i)->asString() == dynamic_cast<FormulaLiteral*>(*j)->asString());
        }
        if (!result) {
            return false;
        }
    }

    return true;
}


//! \brief return an empty formula Equivalent as false
//! \return true empty formula Equivalent
const FormulaFixed FormulaMultiaryOr::emptyFormulaEquivalent = FormulaFalse();


//! \brief return an empty formula Equivalent
//! \return empty formula Equivalent
const FormulaFixed& FormulaMultiaryOr::getEmptyFormulaEquivalent() const {
    return emptyFormulaEquivalent;
}


//! \brief basic constructor
FormulaCNF::FormulaCNF() {
}


//! \brief Constructs a CNF formula containing a clause
//! \param clause_ clause to be added
FormulaCNF::FormulaCNF(FormulaMultiaryOr* clause_) :
    FormulaMultiaryAnd(clause_) {
}


//! \brief Constructs a CNF formula containing the two given clauses
//! \param clause1_ first clause
//! \param clause2_ second clause
FormulaCNF::FormulaCNF(FormulaMultiaryOr* clause1_,
                                 FormulaMultiaryOr* clause2_) :
    FormulaMultiaryAnd(clause1_, clause2_) {
}


//! \brief deep copies this formula
//! \return returns copy
FormulaCNF* FormulaCNF::getDeepCopy() const {
    FormulaMultiaryAnd* newFormula =FormulaMultiaryAnd::getDeepCopy();

    return static_cast<FormulaCNF*>(newFormula);
}


//! \brief adds a clause to the CNF
//! \param clause clause to be addded
void FormulaCNF::addClause(FormulaMultiaryOr* clause) {
    addSubFormula(clause);
}


//! \brief checks whether the current CNF formula implies the given one via op
//! \param op the rhs of the implication
//! \fn bool FormulaCNF::implies(FormulaCNF *op)
//! Note: all literals have to occur non-negated!
bool FormulaCNF::implies(FormulaCNF *op) {
    bool result;

    if (op == NULL) {
        return false;
    }


    if (this->equals() == FALSE)
        return true;
    if (op->equals() == TRUE)
        return true;

    for (FormulaCNF::iterator i = op->begin(); i != op->end(); i++) {
        // iterate over rhs operand
        if ((dynamic_cast<Formula*>(*i)->asString() != "final") &&(dynamic_cast<Formula*>(*i)->asString() != "true")&&(dynamic_cast<Formula*>(*i)->asString() != "false")) {
            result = false;

            for (FormulaCNF::iterator j = begin(); j != end(); j++) {
                // iterate over lhs operand
                result |= (dynamic_cast<FormulaMultiaryOr*>(*j)->implies(dynamic_cast<FormulaMultiaryOr*>(*i)));
            }
            if (!result) {
                return false;
            }
        }
    }

    return true;
}


//! \brief Simplifies the formula by removing redundant clauses.
void FormulaCNF::simplify() {
    for (FormulaCNF::const_iterator iLhs = begin(); iLhs != end(); ++iLhs) {
        FormulaMultiaryOr* lhs =dynamic_cast<FormulaMultiaryOr*>(*iLhs);
        assert(lhs != NULL);
        FormulaCNF::iterator iRhs = begin();
        while (iRhs != end()) {
            if (iLhs == iRhs) {
                ++iRhs;
                continue;
            }

            FormulaMultiaryOr* rhs =dynamic_cast<FormulaMultiaryOr*>(*iRhs);
            assert(rhs != NULL);
            if (lhs->implies(rhs)) {
                delete *iRhs;
                iRhs = removeSubFormula(iRhs);
            } else {
                ++iRhs;
            }
        }
    }
}


//! \brief Creates a formula with a given fixed value and string reprensentation.
//! \param value Fixed truth value of this formula.
//! \param asString String representation of this formula.
FormulaFixed::FormulaFixed(bool value, const std::string& asString) :
    FormulaLiteral(asString), _value(value) {
}


//! \brief returns the fixed value of the formula
//! \param FormulaAssignment An assignment to keep the structure of value
//! \return returns the fixed value
bool FormulaFixed::value(const FormulaAssignment&) const {
    return _value;
}


//! \brief deep copies this formula
//! \return returns copy
FormulaFixed* FormulaFixed::getDeepCopy() const {
    return new FormulaFixed(*this);
}


//! \brief constructs the constant true formula
FormulaTrue::FormulaTrue() :
    FormulaFixed(true, FormulaLiteral::TRUE) {
}


//! \brief constructs the constant false formula
FormulaFalse::FormulaFalse() :
    FormulaFixed(false, FormulaLiteral::FALSE) {
}


//! \brief constructs an atomic literal formula
//! \param literal_ a reference to a string containing the name of the literal
FormulaLiteral::FormulaLiteral(const std::string& literal_) :
    literal(literal_) {
}


//! \brief deconstructor
FormulaLiteral::~FormulaLiteral() {

}


//! \brief deep copies this formula
//! \return returns copy
FormulaLiteral* FormulaLiteral::getDeepCopy() const {
    return new FormulaLiteral(*this);
}


//! \brief returns the value of this literal under the given assignment
//! \param assignment an assignment
//! \return true if the literal is true under the assginment, else false
bool FormulaLiteral::value(const FormulaAssignment& assignment) const {
    return assignment.get(literal);
}


//! \brief returns the name of the literal
//! \return name of the literal
std::string FormulaLiteral::asString() const {
    return literal;
}


//! \brief adds this literal to the given set
//! \param events set of strings to fill
void FormulaLiteral::getEventLiterals(set<string>& events)  {

    // If this literal represent an event, copy it into the set of events
    if (literal != FormulaLiteral::TRUE &&
        literal != FormulaLiteral::FALSE &&
        literal != FormulaLiteral::FINAL &&
        literal != FormulaLiteral::TAU) {

        string copy(literal);
        events.insert(copy);
    }
}


//! \brief basic constructor
FormulaLiteralFinal::FormulaLiteralFinal() :
    FormulaLiteral(FormulaLiteral::FINAL) {
}
