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



// ****************************************************************************
// flatten
// ****************************************************************************

void FormulaMultiaryAnd::flatten() {

    cerr << "AND flatten START: '" << asString() << "'" << endl;
    // ATTENTION: increment iterator inside because of erase
    for (subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ) {

        // flatten subformula
        cerr << "\tAND flatten: current formula '" << (*i)->asString() << "'" << endl;
        (*i)->flatten();

        // check if subformula is a size one multiary formula
        cerr << "\tAND flatten: flattened current formula '" << (*i)->asString() << "'" << endl;
        FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( *i );
        if ( checkMultiary != NULL && checkMultiary->size() == 1 ) {

        	// a size one multiary formula is useless, so erase it and redirect
        	// the iterator to the new current subformula
			subFormulas.insert( i, checkMultiary->getFront() );
			subFormulas.erase( i++ );
			--i;
			cerr << "\t\tAND flatten: size one MULTIARY flattened, result is '" << asString() << "'" << endl;
        }

        // check if subformula is a conjunction
        FormulaMultiaryAnd* checkAnd = dynamic_cast<FormulaMultiaryAnd*> ( *i );
        if ( checkAnd != NULL ) {

            // take subsubformulae, insert them before current subformula and
            // erase old subformula
            for ( subFormulas_t::const_iterator j = checkAnd->subFormulas.begin(); j != checkAnd->subFormulas.end(); ++j ) {
                subFormulas.insert( i, *j );
            }
            subFormulas.erase( i++ );
            cerr << "\t\tAND flatten: subformulas taken, result is '" << asString() << "'" << endl;

        } else {

			// adopt flattened subformula
			++i;
			cerr << "\t\tAND flatten: flattened used, result is '" << asString() << "'" << endl;
        }
    }
    cerr << "AND flatten END: '" << asString() << "'" << endl;
}

void FormulaMultiaryOr::flatten() {

    cerr << "OR flatten START: '" << asString() << "'" << endl;
    // ATTENTION: increment iterator inside because of erase
    for (subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ) {

        // flatten subformula
        cerr << "\tOR flatten: current formula '" << (*i)->asString() << "'" << endl;
        (*i)->flatten();

        // check if subformula is a size one multiary formula
		cerr << "\tOR flatten: flattened current formula '" << (*i)->asString() << "'" << endl;
		FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( *i );
		if ( checkMultiary != NULL && checkMultiary->size() == 1 ) {

			// a size one multiary formula is useless, so erase it and redirect
			// the iterator to the new current subformula
			subFormulas.insert( i, checkMultiary->getFront() );
			subFormulas.erase( i++ );
			--i;
			cerr << "\t\tOR flatten: size one MULTIARY flattened, result is '" << asString() << "'" << endl;
		}

        // check if subformula is a disjunction
        cerr << "\tOR flatten: flattened current formula '" << (*i)->asString() << "'" << endl;
        FormulaMultiaryOr* checkOr = dynamic_cast<FormulaMultiaryOr*> ( *i );
        if ( checkOr != NULL ) {

            // take subsubformulae, insert them before current subformula and
            // erase old subformula
            for ( subFormulas_t::const_iterator j = checkOr->subFormulas.begin(); j != checkOr->subFormulas.end(); ++j ) {
                subFormulas.insert( i, *j );
            }
            subFormulas.erase( i++ );
            cerr << "\t\tOR flatten: subformulas taken, result is '" << asString() << "'" << endl;

        } else {

			// adopt flattened subformula
			++i;
			cerr << "\t\tOR flatten: flattened used, result is '" << asString() << "'" << endl;
        }
    }
    cerr << "OR flatten END: '" << asString() << "'" << endl;
}



// ****************************************************************************
// merge
// ****************************************************************************

void FormulaMultiaryAnd::merge() {

	cerr << "AND merge START: '" << asString() << "'" << endl;

	// merge all subformula and flatten them
	for ( subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ++i ) {

		// merge recursively all subformula
		cerr << "\tAND merge: current formula '" << (*i)->asString() << "'" << endl;
		(*i)->merge();
	}

    // iterate over all subformulas twice: first we determine the current
    // assumption and then we check all subformulae for being a conclusion
    for ( subFormulas_t::iterator assumption = subFormulas.begin(); assumption != subFormulas.end(); ++assumption ) {

        for ( subFormulas_t::iterator conclusion = subFormulas.begin(); conclusion != subFormulas.end(); ) {

            // check if we have the same subformula as assumption and
            // conclusion: this has to be skipped because it would be a
            // trivial implication
            if ( assumption == conclusion ) {
                ++conclusion;
            } else {

                // check if assumption implies conclusion, which means in a
                // CONJUNCTION the conclusion is redundant
                if ( (*assumption)->implies(*conclusion) ) {
                	cerr << "\tAND merge: erase formula '" << (*conclusion)->asString() << "'" << endl;
                    subFormulas.erase( conclusion++ );
                } else {
                	cerr << "\tAND merge: skipped formula '" << (*conclusion)->asString() << "'" << endl;
                    ++conclusion;
                }
            }
        }
    }

    cerr << "AND merge END: '" << asString() << "'" << endl;
}

void FormulaMultiaryOr::merge() {

	cerr << "OR merge START: '" << asString() << "'" << endl;
	for ( subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ++i ) {

		// merge recursively all subformula first
		(*i)->merge();

		// check if subformula is a size one conjunction
		FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( *i );
		if ( checkMultiary != NULL && checkMultiary->size() == 1 ) {

			// take only subformula and insert it before current subformula
			subFormulas.insert( i, checkMultiary->getFront() );
			subFormulas.erase( i++ );
			cerr << "\t\tOR merge: size one MULTIARY merged, result is '" << asString() << "'" << endl;
		}
	}

    // iterate over all subformulas twice: first we determine the current
    // conclusion and then we check all subformulae for being an assumption
    for ( subFormulas_t::iterator conclusion = subFormulas.begin(); conclusion != subFormulas.end(); ++conclusion ) {

        for ( subFormulas_t::iterator assumption = subFormulas.begin(); assumption != subFormulas.end(); ) {

            // check if we have the same subformula as assumption and
            // conclusion: this has to be skipped because it would be a
            // trivial implication
            if ( assumption == conclusion ) {
                ++assumption;
            } else {

                // check if assumption implies conclusion, which means in a
                // DISJUNCTION the assumption is redundant
                if ( (*assumption)->implies(*conclusion) ) {
                	cerr << "\tOR merge: erase formula '" << (*assumption)->asString() << "'" << endl;
                    subFormulas.erase( assumption++ );
                } else {
                	cerr << "\tOR merge: skipped formula '" << (*assumption)->asString() << "'" << endl;
                    ++assumption;
                }
            }
        }
    }
    cerr << "OR merge END: '" << asString() << "'" << endl;
}



// ****************************************************************************
// implies
// ****************************************************************************


bool FormulaMultiaryAnd::implies(Formula* conclusion) const {
    return false;
}

bool FormulaMultiaryOr::implies(Formula* conclusion) const {

    // check if the conclusion is a literal
    FormulaLiteral* checkLiteral = dynamic_cast<FormulaLiteral*> ( conclusion );
    if ( checkLiteral != NULL ) {

        for ( subFormulas_t::const_iterator i = subFormulas.begin(); i != subFormulas.end(); ++i ) {

            if ( not (*i)->implies(checkLiteral) ) {
                return false;
            }
        }
        return true;

    } else {
        // TODO fuck
        return false;
    }
}

bool FormulaLiteral::implies(Formula* conclusion) const {

    // check if the conclusion is a literal
    FormulaLiteral* checkLiteral = dynamic_cast<FormulaLiteral*> ( conclusion );
    if ( checkLiteral != NULL ) {

        return ( _literal.compare(checkLiteral->_literal) == 0 );
    } else {
        // TODO stimmt das auch for AND und OR?
        // a literal doesn't imply anything differant than a literal
        return false;
    }
}
