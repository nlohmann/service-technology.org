#include "settings.h"
#include "Formula.h"

using namespace std;



// ****************************************************************************
// FormulaMultiary
// ****************************************************************************

//! \brief deconstructor
FormulaMultiary::~FormulaMultiary() {

    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
         currentFormula != subFormulas.end();
         currentFormula++) {
        delete *currentFormula;
    }

}


//! \brief removes the given literal from the formula, if this literal is the
//!        only one of a clause, the clause gets removed as well
//! \param name name of the literal to be removed
void FormulaMultiary::removeLiteral(const std::string& name) {


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

            if ((*iCurrentFormula)->size() > 0) {

                // the current formula is no literal, so call removeLiteral again
                (*iCurrentFormula)->removeLiteral(name);
                if ((*iCurrentFormula)->size() == 0) {
                    // this clause just got empty, so we store it in a temporary list
                    // later on we want to remove this clause from the formula
                    listOfFormulaIterator.push_back(iCurrentFormula);


                    // TODO why not this
                    //delete *iCurrentFormula;
                    //iCurrentFormula = subFormulas.erase( iCurrentFormula );
                }
            }
            // TODO check with above
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


//    subFormulas_t::iterator iCurrentFormula;
//    //cout << "\tanzahl von klauseln: " << subFormulas.size() << endl; int i = 1;
//    //cout << "\tremoving literal " << name << " from clause nr " << i++ << endl;
//
//    for (iCurrentFormula = subFormulas.begin();
//         iCurrentFormula != subFormulas.end();) {
//
//        // if the considered current formula is a literal, then remove it;
//        // call the function recursively, otherwise
//        FormulaLiteral* currentFormula = dynamic_cast<FormulaLiteral*> (*iCurrentFormula);
//        if (currentFormula != NULL) {
//            // the current formula is a literal
//            if (currentFormula->asString() == name) {
//                // the literal has the right name, so remove it
//                delete *iCurrentFormula;
//                iCurrentFormula = subFormulas.erase(iCurrentFormula);
//            } else {
//                iCurrentFormula++;
//            }
//        } else {
//            // the current formula is no literal, so call removeLiteral again
//            (*iCurrentFormula)->removeLiteralByHiding(name);
//            if ( (dynamic_cast<FormulaMultiary*>(*iCurrentFormula))->size() == 0 ) {
//                delete *iCurrentFormula;
//                iCurrentFormula = subFormulas.erase(iCurrentFormula);
//            } else {
//                iCurrentFormula++;
//            }
//        }
//    }
//
//}


//! \brief deep copies the private members of this formula to a given one
//! \param newFormula formulamultiary to be copied to
void FormulaMultiary::deepCopyMultiaryPrivateMembersToNewFormula(FormulaMultiary* newFormula) const {

    newFormula->subFormulas.clear();
    for (subFormulas_t::const_iterator iFormula = subFormulas.begin();
         iFormula != subFormulas.end(); ++iFormula) {
        newFormula->subFormulas.push_back((*iFormula)->getDeepCopy());
    }
}


//! \brief returns the number of
//! \return iterator to the begin of the subformula list
int FormulaMultiary::size() const {
    return subFormulas.size();
}



// ****************************************************************************
// FormulaMultiaryAnd
// ****************************************************************************


//! \brief turns the formula into a string
//! \return the formula as string
string FormulaMultiaryAnd::asString() const {

    if (subFormulas.empty()) {
        return "(empyty AND)";
    }

    std::string formulaString = "";
    subFormulas_t::const_iterator currentFormula = subFormulas.begin();

    if (subFormulas.size() == 1) {

        // we omit parenthesis if only one element in formula
        return (*currentFormula)->asString();
    } else {

        formulaString += '(' + (*currentFormula)->asString();

        while (++currentFormula != subFormulas.end()) {
            formulaString += " * " + (*currentFormula)->asString();
        }

        return formulaString + ')';
    }
}
//! \brief computes and returns the value of this formula under the given
//!        assignment
//! \param assignment an assignment for the formulas literals
//! \return true if the formula is true under the assginment, else false
bool FormulaMultiaryAnd::value(const FormulaAssignment& assignment) const {
    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
         currentFormula != subFormulas.end(); ++currentFormula) {
        if ((*currentFormula)->value(assignment) != true ) {
            return false;
        }
    }

    return true;
}


//! \brief Returns the merged equivalent to this formula. Merging gets rid of
//!        unnecessary nesting of subformulas. (a*(b*c)) becomes (a*b*c). The
//!        caller is responsible for deleting the returned newly created formula.
//! \return returns the merged equivalent
FormulaMultiaryAnd* FormulaMultiaryAnd::merge() {
    bool final = true;
    FormulaMultiaryAnd *result = new FormulaMultiaryAnd;

    for (FormulaMultiaryAnd::iterator i = subFormulas.begin(); i != subFormulas.end(); i++) {
        if (dynamic_cast<FormulaMultiaryAnd*>(*i)) {
            final = false;
            FormulaMultiaryAnd* temp = dynamic_cast<FormulaMultiaryAnd*>(*i);
            for (FormulaMultiaryAnd::iterator j = temp->subFormulas.begin();
                 j != temp->subFormulas.end(); j++) {
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



// ****************************************************************************
// FormulaMultiaryOr
// ****************************************************************************


//! \brief turns the formula into a string
//! \return the formula as string
string FormulaMultiaryOr::asString() const {

    if (subFormulas.empty()) {
        return "empty OR";
    }

    std::string formulaString = "";
    subFormulas_t::const_iterator currentFormula = subFormulas.begin();

    if (subFormulas.size() == 1) {
        // we omit parenthesis if only one element in formula
        return (*currentFormula)->asString();
    } else {
        formulaString += '(' + (*currentFormula)->asString();

        while (++currentFormula != subFormulas.end()) {
            formulaString += " + " + (*currentFormula)->asString();
        }

        return formulaString + ')';
    }
}


//! \brief computes and returns the value of this formula under the given
//!        assignment
//! \param assignment an assignment for the formulas literals
//! \return true if the formula is true under the assginment, else false
bool FormulaMultiaryOr::value(const FormulaAssignment& assignment) const {
    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
         currentFormula != subFormulas.end(); ++currentFormula) {
        if ( (*currentFormula)->value(assignment) ) {
            return true;
        }
    }

    return false;
}


//! \brief Returns the merged equivalent to this formula. Merging gets rid of
//!        unnecessary nesting of subformulas. (a+(b+c)) becomes (a+b+c). The
//!        caller is responsible for deleting the returned newly created formula.
//! \return returns the merged equivalent
FormulaMultiaryOr* FormulaMultiaryOr::merge() {
    bool final = true;
    FormulaMultiaryOr *result = new FormulaMultiaryOr;

    for (FormulaMultiaryOr::iterator i = subFormulas.begin(); i != subFormulas.end(); i++) {
        if (dynamic_cast<FormulaMultiaryOr*>(*i)) {
            final = false;
            FormulaMultiaryOr* temp = dynamic_cast<FormulaMultiaryOr*>(*i);
            for (FormulaMultiaryOr::iterator j = temp->subFormulas.begin();
                 j != temp->subFormulas.end(); j++) {
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


//! \brief deep copies this formula
//! \return returns copy
FormulaMultiaryOr* FormulaMultiaryOr::getDeepCopy() const {

    FormulaMultiaryOr* newFormula =new FormulaMultiaryOr(*this);
    deepCopyMultiaryPrivateMembersToNewFormula(newFormula);
    return newFormula;
}
