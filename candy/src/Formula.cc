#include "settings.h"
#include "Formula.h"

using namespace std;



// ****************************************************************************
// FormulaMultiary
// ****************************************************************************

//! \brief removes the given literal from the formula, if this literal is the
//!        only one of a clause, the clause gets removed as well
//! \param name name of the literal to be removed
void FormulaMultiary::removeLiteral(const std::string& name) {


    subFormulas_t::iterator iCurrentFormula;

    std::list<subFormulas_t::iterator> listOfFormulaIterator;

    std::list<subFormulas_t::iterator>::iterator iterOfListOfFormulaIterator;

    //cout << "\tanzahl von klauseln: " << subFormulas.size() << endl; int i = 1;
    //cout << "\tremoving literal " << name << " from clause nr " << i++ << endl;

    for (iCurrentFormula = subFormulas.begin(); iCurrentFormula != subFormulas.end(); ) {

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



// ****************************************************************************
// FormulaMultiaryAnd and FormulaMultiaryOr
// ****************************************************************************

//! \brief deep copies this formula
//! \return returns copy
FormulaMultiaryAnd* FormulaMultiaryAnd::getDeepCopy() const {

    FormulaMultiaryAnd* newFormula = new FormulaMultiaryAnd();
    for (subFormulas_t::const_iterator i = subFormulas.begin(); i != subFormulas.end(); ++i) {
        newFormula->subFormulas.push_back( (*i)->getDeepCopy() );
    }
    return newFormula;
}

//! \brief deep copies this formula
//! \return returns copy
FormulaMultiaryOr* FormulaMultiaryOr::getDeepCopy() const {

    FormulaMultiaryOr* newFormula = new FormulaMultiaryOr();
    for (subFormulas_t::const_iterator i = subFormulas.begin(); i != subFormulas.end(); ++i) {
        newFormula->subFormulas.push_back( (*i)->getDeepCopy() );
    }
    return newFormula;
}



//! \brief computes and returns the value of this formula under the given
//!        assignment
//! \param assignment an assignment for the formulas literals
//! \return true if the formula is true under the assginment, else false
bool FormulaMultiaryAnd::value(const FormulaAssignment& assignment) const {

    for (subFormulas_t::const_iterator i = subFormulas.begin(); i != subFormulas.end(); ++i) {
        if ((*i)->value(assignment) != true ) {
            return false;
        }
    }
    return true;
}

//! \brief computes and returns the value of this formula under the given
//!        assignment
//! \param assignment an assignment for the formulas literals
//! \return true if the formula is true under the assginment, else false
bool FormulaMultiaryOr::value(const FormulaAssignment& assignment) const {

    for (subFormulas_t::const_iterator i = subFormulas.begin(); i != subFormulas.end(); ++i) {
        if ( (*i)->value(assignment) ) {
            return true;
        }
    }
    return false;
}



//! \brief turns the formula into a string
//! \return the formula as string
string FormulaMultiaryAnd::asString() const {

    //cerr << "AND asString START: size = '" << subFormulas.size() << "'" << endl;
    if ( subFormulas.size() == 0 ) {

        // return dummy value
        //cerr << "AND asString END" << endl;
        return "(empyty AND)";

    } else if ( subFormulas.size() == 1 ) {

        // we omit parenthesis if only one element in formula
        //cerr << "AND asString END" << endl;
        return (*subFormulas.begin())->asString();

    } else {

        // we build the string canonically
        subFormulas_t::const_iterator currentFormula = subFormulas.begin();
        //cerr << "AND asString: current formula '" << (*currentFormula)->asString() << "'" << endl;
        string formulaString = '(' + (*currentFormula)->asString();
        while ( ++currentFormula != subFormulas.end() ) {

            //cerr << "AND asString: current formula '"
            //     << ( *currentFormula != NULL ? (*currentFormula)->asString() : "NULL" ) << "'" << endl;
            formulaString += " * " + (*currentFormula)->asString();
        }
        //cerr << "AND asString END" << endl;
        return formulaString + ')';
    }
}

//! \brief turns the formula into a string
//! \return the formula as string
string FormulaMultiaryOr::asString() const {

    //cerr << "OR asString START: size = '" << subFormulas.size() << "'" << endl;
    if ( subFormulas.size() == 0 ) {

        // return dummy value
        //cerr << "OR asString END" << endl;
        return "(empyty OR)";

    } else if ( subFormulas.size() == 1 ) {

        // we omit parenthesis if only one element in formula
        //cerr << "OR asString END" << endl;
        return (*subFormulas.begin())->asString();

    } else {

        // we build the string canonically
        subFormulas_t::const_iterator currentFormula = subFormulas.begin();
        //cerr << "OR asString: current formula '" << (*currentFormula)->asString() << "'" << endl;
        string formulaString = '(' + (*currentFormula)->asString();
        while ( ++currentFormula != subFormulas.end() ) {

            //cerr << "OR asString: current formula '"
            //     << ( *currentFormula != NULL ? (*currentFormula)->asString() : "NULL" ) << "'" << endl;
            formulaString += " + " + (*currentFormula)->asString();
        }
        //cerr << "OR asString END" << endl;
        return formulaString + ')';
    }
}



// ****************************************************************************
// simplify
// ****************************************************************************

//! \brief Returns the merged equivalent to this formula. Merging gets rid of
//!        unnecessary nesting of subformulas. (a*(b*c)) becomes (a*b*c). The
//!        caller is responsible for deleting the returned newly created formula.
//! \return returns the merged equivalent
Formula* FormulaMultiaryAnd::simplify() {

    cerr << "AND simplify START: '" << asString() << "'" << endl;
    FormulaMultiaryAnd *result = new FormulaMultiaryAnd;

    for (subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); i++) {

        // simplify subformula
        cerr << "\tAND simplify: current formula '" << (*i)->asString() << "'" << endl;
        Formula* simplifiedSub = (*i)->simplify();

        // check if subformula is a conjunction
        cerr << "\tAND simplify: simplified current formula '" << simplifiedSub->asString() << "'" << endl;
        FormulaMultiaryAnd* checkSub = dynamic_cast<FormulaMultiaryAnd*> (simplifiedSub);
        if ( checkSub != NULL ) {

            // copy all subformulas
            for (subFormulas_t::iterator j = checkSub->subFormulas.begin(); j != checkSub->subFormulas.end(); j++) {
                result->subFormulas.push_back( dynamic_cast<Formula*> (*j)->getDeepCopy() );
            }

            // delete old simplified subformula
            delete simplifiedSub;
            cerr << "\t\tAND simplify: subformulas copied, result is '" << result->asString() << "'" << endl;

        } else {

            // adopt simplified subformula
            result->subFormulas.push_back( simplifiedSub );
            cerr << "\t\tAND simplify: simplified used, result is '" << result->asString() << "'" << endl;
        }
    }

    cerr << "AND simplify END: '" << result->asString() << "'" << endl;
    return result;
}

Formula* FormulaMultiaryOr::simplify() {

    cerr << "OR simplify START: '" << asString() << "'" << endl;
    FormulaMultiaryOr *result = new FormulaMultiaryOr;

    for (subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); i++) {

        // simplify subformula
        cerr << "\tOR simplify: current formula '" << (*i)->asString() << "'" << endl;
        Formula* simplifiedSub = (*i)->simplify();

        // check if subformula is a disjunction
        cerr << "\tOR simplify: simplified current formula '" << simplifiedSub->asString() << "'" << endl;
        FormulaMultiaryOr* checkSub = dynamic_cast<FormulaMultiaryOr*> (simplifiedSub);
        if ( checkSub != NULL ) {

            // copy all subformulas
            for ( subFormulas_t::const_iterator j = checkSub->subFormulas.begin(); j != checkSub->subFormulas.end(); ++j ) {
                //result->subFormulas.push_back( dynamic_cast<Formula*> (*j)->getDeepCopy() );
                result->subFormulas.push_back( (*j)->getDeepCopy() );
            }

            // delete old simplified subformula
            delete simplifiedSub;
            cerr << "\t\tOR simplify: subformulas copied, result is '" << result->asString() << "'" << endl;

        } else {

            // adopt simplified subformula
            result->subFormulas.push_back( simplifiedSub );
            cerr << "\t\tOR simplify: simplified used, result is '" << result->asString() << "'" << endl;
        }
    }

    if ( result->size() == 1 ) {

        Formula* newResult = (*(result->subFormulas).begin())->getDeepCopy();
        delete result;

        cerr << "OR simplify END: '" << newResult->asString() << "'" << endl;
        return newResult;
    } else {

        cerr << "OR simplify END: '" << result->asString() << "'" << endl;
        return result;
    }

    // TODO implies aus fiona implementieren und für simplify nutzen
}



//! \brief Returns the merged equivalent to this formula. Merging gets rid of
//!        unnecessary nesting of subformulas. (a+(b+c)) becomes (a+b+c). The
//!        caller is responsible for deleting the returned newly created formula.
//! \return returns the merged equivalent
//FormulaMultiaryOr* FormulaMultiaryOr::simplify() {
//    bool final = true;
//    FormulaMultiaryOr *result = new FormulaMultiaryOr;
//
//    for (subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); i++) {
//        if ( dynamic_cast<FormulaMultiaryOr*>(*i) ) {
//            final = false;
//            FormulaMultiaryOr* temp = dynamic_cast<FormulaMultiaryOr*>(*i);
//            for (subFormulas_t::iterator j = temp->subFormulas.begin();
//                 j != temp->subFormulas.end(); j++) {
//                result->subFormulas.push_back( dynamic_cast<Formula*> (*j)->getDeepCopy() );
//            }
//        } else {
//            result->subFormulas.push_back( dynamic_cast<Formula*> (*i)->getDeepCopy() );
//        }
//    }
//
//    if (final) {
//        return result;
//    }
//
//    //FormulaMultiaryOr* oldResult = result;
//    //result = result->merge();
//    //delete oldResult;
//    return result;
//}


