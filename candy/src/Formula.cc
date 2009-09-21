#include "settings.h"
#include "Formula.h"

using namespace std;



// ****************************************************************************
// FormulaMultiary
// ****************************************************************************

//! \brief removes the given literal from the formula, if this literal is the
//!        only one of a clause, the clause gets removed as well
//! \param name name of the literal to be removed
void FormulaMultiary::removeLiteral(const string& literal) {

    cerr << "\t removeLiteral START: '" << asString() << "', literal '" << literal << "'" << endl;

    for ( subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ) {

        // if the considered current formula is a literal, then remove it;
        // call the function recursively, otherwise
    	cerr << "\t removeLiteral: current formula '" << (*i)->asString() << "'" << endl;
        FormulaLiteral* checkLiteral = dynamic_cast<FormulaLiteral*> (*i);
        if (checkLiteral != NULL) {

            // the current formula is a literal, so delete it if it matches
            if ( literal.compare( checkLiteral->asString() ) == 0 ) {
            	subFormulas.erase( i++ );
            } else {
                ++i;
            }

        } else {

			// the current formula is no literal, so call removeLiteral again
			(*i)->removeLiteral( literal );

			FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( *i );
			if ( checkMultiary != NULL && checkMultiary->size() == 0 ) {

				// a size zero multiary formula is useless, so erase it and redirect
				// the iterator to the new current subformula
				subFormulas.erase( i++ );
				cerr << "\t removeLiteral: size zero MULTIARY flattened, result is '" << asString() << "'" << endl;

			} else {

				// adopt flattened subformula
				++i;
				cerr << "\t removeLiteral: nothing to flatten, result is '" << asString() << "'" << endl;
			}
        }
    }

    cerr << "\t removeLiteral END: '" << asString() << "', literal '" << literal << "'" << endl;
}



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
// flatten
// ****************************************************************************

void FormulaMultiaryAnd::flatten() {

    cerr << "\tAND flatten START: '" << asString() << "'" << endl;

    // ATTENTION: increment iterator inside because of erase
    for (subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ) {

        // check if subformula is a size one multiary formula
    	cerr << "\tAND flatten: current formula '" << (*i)->asString() << "'" << endl;
        FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( *i );
        if ( checkMultiary != NULL && checkMultiary->size() == 1 ) {

        	// a size one multiary formula is useless, so erase it and redirect
        	// the iterator to the new current subformula
			subFormulas.insert( i, checkMultiary->subFormulas.front() );
			subFormulas.erase( i++ );
			--i;
			cerr << "\tAND flatten: size one MULTIARY flattened, result is '" << asString() << "'" << endl;
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
            cerr << "\tAND flatten: subformulas taken, result is '" << asString() << "'" << endl;

        } else {

			// adopt flattened subformula
			++i;
			cerr << "\tAND flatten: flattened used, result is '" << asString() << "'" << endl;
        }
    }

    cerr << "\tAND flatten END: '" << asString() << "'" << endl;
}

void FormulaMultiaryOr::flatten() {

    cerr << "\tOR flatten START: '" << asString() << "'" << endl;

    // ATTENTION: increment iterator inside because of erase
    for (subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ) {

        // check if subformula is a size one multiary formula
    	cerr << "\tOR flatten: current formula '" << (*i)->asString() << "'" << endl;
		FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( *i );
		if ( checkMultiary != NULL && checkMultiary->size() == 1 ) {

			// a size one multiary formula is useless, so erase it and redirect
			// the iterator to the new current subformula
			subFormulas.insert( i, checkMultiary->subFormulas.front() );
			subFormulas.erase( i++ );
			--i;
			cerr << "\tOR flatten: size one MULTIARY flattened, result is '" << asString() << "'" << endl;
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
            cerr << "\tOR flatten: subformulas taken, result is '" << asString() << "'" << endl;

        } else {

			// adopt flattened subformula
			++i;
			cerr << "\tOR flatten: flattened used, result is '" << asString() << "'" << endl;
        }
    }

    cerr << "\tOR flatten END: '" << asString() << "'" << endl;
}



// ****************************************************************************
// merge
// ****************************************************************************

void FormulaMultiaryAnd::merge() {

	cerr << "\tAND merge START: '" << asString() << "'" << endl;

	// iterate over all subformulas twice: first we determine the current
	// assumption and then we check all subformulae for being an conclusion
	for ( subFormulas_t::iterator assumption = subFormulas.begin(); assumption != subFormulas.end(); ++assumption ) {

		cerr << "\tAND merge: current formula '" << (*assumption)->asString() << "'" << endl;
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

	cerr << "\tAND merge END: '" << asString() << "'" << endl;
}

void FormulaMultiaryOr::merge() {

	cerr << "\tOR merge START: '" << asString() << "'" << endl;

	// iterate over all subformulas twice: first we determine the current
	// conclusion and then we check all subformulae for being an assumption
	for ( subFormulas_t::iterator conclusion = subFormulas.begin(); conclusion != subFormulas.end(); ++conclusion ) {

		cerr << "\tOR merge: current formula '" << (*conclusion)->asString() << "'" << endl;
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

	cerr << "\tOR merge END: '" << asString() << "'" << endl;
}



// ****************************************************************************
// simplify
// ****************************************************************************

void FormulaMultiaryAnd::simplify() {

	cerr << "AND simplify START: '" << asString() << "'" << endl;

	// simplify recursively all subformula first
	for ( subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ++i ) {
		cerr << "\tAND simplify: current formula '" << (*i)->asString() << "'" << endl;
		(*i)->simplify();
		cerr << "\tAND simplify: simplified current formula '" << (*i)->asString() << "'" << endl;
	}

    // step 1: merge
	// iterate over all subformulas twice: first we determine the current
    // assumption and then we check all subformulae for being a conclusion
	merge();

    // step 2: flatten
    // iterate over all subformula and check them for being a size one mulitary
    // formula, a conjunction or a double negation
	flatten();

    cerr << "AND simplify END: '" << asString() << "'" << endl;
}

void FormulaMultiaryOr::simplify() {

	cerr << "OR merge START: '" << asString() << "'" << endl;

	// simplify recursively all subformula first
	for ( subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ++i ) {
		cerr << "\tOR simplify: current formula '" << (*i)->asString() << "'" << endl;
		(*i)->simplify();
		cerr << "\tOR simplify: simplified current formula '" << (*i)->asString() << "'" << endl;
	}

	// step 1: merge
	// iterate over all subformulas twice: first we determine the current
	// assumption and then we check all subformulae for being a conclusion
	merge();

	// step 2: flatten
	// iterate over all subformula and check them for being a size one mulitary
	// formula, a disjunction or a double negation
	flatten();

    cerr << "OR merge END: '" << asString() << "'" << endl;
}



// ****************************************************************************
// implies
// ****************************************************************************

bool FormulaMultiaryAnd::implies(Formula* conclusion) const {

	assert( conclusion != NULL );

	// a literal is implied by a conjunction A iff some subformula of A implies the literal
	FormulaLiteral* literal = dynamic_cast<FormulaLiteral*> ( conclusion );
	if ( literal != NULL ) {
		for ( subFormulas_t::const_iterator i = subFormulas.begin(); i != subFormulas.end(); ++i ) {
			if ( (*i)->implies(literal) ) {
				return true;
			}
		}
		return false;
	}

	// a conjunction A is implied by this iff every subformula of A is implied by this
	FormulaMultiaryAnd* conjunction = dynamic_cast<FormulaMultiaryAnd*> ( conclusion );
	if ( conjunction != NULL ) {
		for ( subFormulas_t::const_iterator i = conjunction->subFormulas.begin(); i != conjunction->subFormulas.end(); ++i ) {
			if ( not this->implies(*i) ) {
				return false;
			}
		}
		return true;
	}

	// a disjunction A is implied by this iff some subformula of A is implied by this
	FormulaMultiaryOr* disjunction = dynamic_cast<FormulaMultiaryOr*> ( conclusion );
	if ( disjunction != NULL ) {
		for ( subFormulas_t::const_iterator i = disjunction->subFormulas.begin(); i != disjunction->subFormulas.end(); ++i ) {
			if ( this->implies(*i) ) {
				return true;
			}
		}
		return false;
	}

	// this implies only literals, conjunctions and disjunctions
	return false;
}

bool FormulaMultiaryOr::implies(Formula* conclusion) const {

	assert( conclusion != NULL );

	// a literal is implied by a disjunction A iff all subformula of A imply the literal
	FormulaLiteral* literal = dynamic_cast<FormulaLiteral*> ( conclusion );
	if ( literal != NULL ) {
		for ( subFormulas_t::const_iterator i = subFormulas.begin(); i != subFormulas.end(); ++i ) {
			if ( not (*i)->implies(literal) ) {
				return false;
			}
		}
		return true;
	}

	// a conjunction A is implied by this iff every subformula of A is implied by this
	FormulaMultiaryAnd* conjunction = dynamic_cast<FormulaMultiaryAnd*> ( conclusion );
	if ( conjunction != NULL ) {
		for ( subFormulas_t::const_iterator i = conjunction->subFormulas.begin(); i != conjunction->subFormulas.end(); ++i ) {
			if ( not this->implies(*i) ) {
				return false;
			}
		}
		return true;
	}

	// a disjunction A is implied by this iff some subformula of A is implied by this
	FormulaMultiaryOr* disjunction = dynamic_cast<FormulaMultiaryOr*> ( conclusion );
	if ( disjunction != NULL ) {
		for ( subFormulas_t::const_iterator i = disjunction->subFormulas.begin(); i != disjunction->subFormulas.end(); ++i ) {
			if ( this->implies(*i) ) {
				return true;
			}
		}
		return false;
	}

	// this implies only literals, conjunctions and disjunctions
	return false;
}

bool FormulaLiteral::implies(Formula* conclusion) const {

	assert( conclusion != NULL );

	// a literal implies a literal only
	FormulaLiteral* literal = dynamic_cast<FormulaLiteral*> ( conclusion );
	if ( literal != NULL ) {
		return _literal.compare( literal->_literal ) == 0;
	} else {
		return false;
	}
}
