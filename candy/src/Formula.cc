#include "settings.h"
#include "Formula.h"

using namespace std;



// ****************************************************************************
// FormulaMultiaryAnd and FormulaMultiaryOr
// ****************************************************************************

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
        return "(empty AND)";

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
        return "(empty OR)";

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
// removeLiteral
// ****************************************************************************

//! \brief removes the given literal from the formula, if this literal is the
//!        only one of a clause, the clause gets removed as well
//! \param literal name of the literal to be removed
void FormulaMultiary::removeLiteral(const string& literal) {

    //DEBUG "\t removeLiteral START: '" << asString() << "', literal '" << literal << "'" END

    for ( subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ) {

        // if the considered current formula is a literal, then remove it;
        // call the function recursively, otherwise
    	//DEBUG "\t removeLiteral: current formula '" << (*i)->asString() << "'" END
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

            FormulaNegation* checkNegation = dynamic_cast<FormulaNegation*> ( *i );
            if ( checkNegation != NULL && checkNegation->size() == 0 ) {

                // a size zero negation formula is useless, so erase it
				subFormulas.erase( i++ );
				//DEBUG "\t removeLiteral: size zero NEGATION removed, result is '" << asString() << "'" END

            } else {

                FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( *i );
                if ( checkMultiary != NULL && checkMultiary->size() == 0 ) {

                    // a size zero multiary formula is useless, so erase it
                    subFormulas.erase( i++ );
                    //DEBUG "\t removeLiteral: size zero MULTIARY removed, result is '" << asString() << "'" END

                } else {

                    // adopt flattened subformula
                    ++i;
                    //DEBUG "\t removeLiteral: nothing to remove, result is '" << asString() << "'" END
                }
            }
        }
    }

    //DEBUG "\t removeLiteral END: '" << asString() << "', literal '" << literal << "'" END
}

void FormulaNegation::removeLiteral(const string& literal) {

    //DEBUG "\t removeLiteral START: '" << asString() << "', literal '" << literal << "'" END

    // if the subformula is a literal, then remove it;
    // call the function recursively, otherwise
    //DEBUG "\t removeLiteral: subformula '" << subFormula->asString() << "'" END
    FormulaLiteral* checkLiteral = dynamic_cast<FormulaLiteral*> ( subFormula );
    if (checkLiteral != NULL) {

        // the subformula is a literal, so delete it if it matches
        if ( literal.compare( checkLiteral->asString() ) == 0 ) {
            clear();
        }

    } else {

        // the current formula is no literal, so call removeLiteral again
        subFormula->removeLiteral( literal );

        FormulaNegation* checkNegation = dynamic_cast<FormulaNegation*> ( subFormula );
        if ( checkNegation != NULL && checkNegation->size() == 0 ) {
            clear();
        } else {

            FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( subFormula );
            if ( checkMultiary != NULL && checkMultiary->size() == 0 ) {
                clear();
            }
        }
    }

    //DEBUG "\t removeLiteral END: '" << asString() << "', literal '" << literal << "'" END
}



// ****************************************************************************
// flatten
// ****************************************************************************

void FormulaMultiaryAnd::flatten() {

    //DEBUG "\tAND flatten START: '" << asString() << "'" END

    // ATTENTION: increment iterator inside because of erase
    for (subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ) {

        // check if subformula is a size one multiary formula
    	//DEBUG "\tAND flatten: current formula '" << (*i)->asString() << "'" END
        FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( *i );
        if ( checkMultiary != NULL && checkMultiary->size() == 1 ) {

        	// a size one multiary formula is useless, so erase it and redirect
        	// the iterator to the new current subformula
			subFormulas.insert( i, checkMultiary->subFormulas.front() );
			subFormulas.erase( i++ );
			--i;
			//DEBUG "\tAND flatten: size one MULTIARY flattened, result is '" << asString() << "'" END
        }


        // check if subformula is a double negation
		FormulaNegation* checkNegation = dynamic_cast<FormulaNegation*> ( *i );
		if ( checkNegation != NULL ) {

            FormulaNegation* checkSubNegation = dynamic_cast<FormulaNegation*> ( checkNegation->subFormula );
            if ( checkSubNegation != NULL ) {
                // a double negation formula is ugly, so erase it and redirect
                // the iterator to the new current subformula
                subFormulas.insert( i, checkSubNegation->subFormula );
                delete checkSubNegation;
                delete checkNegation;
                subFormulas.erase( i++ );
                --i;
                //DEBUG "\tAND flatten: double NEGATION flattened, result is '" << asString() << "'" END
            }
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
            //DEBUG "\tAND flatten: subformulas taken, result is '" << asString() << "'" END

        } else {

			// adopt flattened subformula
			++i;
			//DEBUG "\tAND flatten: flattened used, result is '" << asString() << "'" END
        }
    }

    //DEBUG "\tAND flatten END: '" << asString() << "'" END
}

void FormulaMultiaryOr::flatten() {

    //DEBUG "\tOR flatten START: '" << asString() << "'" END

    // ATTENTION: increment iterator inside because of erase
    for (subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ) {

        // check if subformula is a size one multiary formula
    	//DEBUG "\tOR flatten: current formula '" << (*i)->asString() << "'" END
		FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( *i );
		if ( checkMultiary != NULL && checkMultiary->size() == 1 ) {

			// a size one multiary formula is useless, so erase it and redirect
			// the iterator to the new current subformula
			subFormulas.insert( i, checkMultiary->subFormulas.front() );
			subFormulas.erase( i++ );
			--i;
			//DEBUG "\tOR flatten: size one MULTIARY flattened, result is '" << asString() << "'" END
		}


        // check if subformula is a double negation
		FormulaNegation* checkNegation = dynamic_cast<FormulaNegation*> ( *i );
		if ( checkNegation != NULL ) {

            FormulaNegation* checkSubNegation = dynamic_cast<FormulaNegation*> ( checkNegation->subFormula );
            if ( checkSubNegation != NULL ) {
                // a double negation formula is ugly, so erase it and redirect
                // the iterator to the new current subformula
                subFormulas.insert( i, checkSubNegation->subFormula );
                delete checkSubNegation;
                delete checkNegation;
                subFormulas.erase( i++ );
                --i;
                //DEBUG "\tOR flatten: double NEGATION flattened, result is '" << asString() << "'" END
            }
		}


        // check if subformula is a disjunction
        //DEBUG "\tOR flatten: flattened current formula '" << (*i)->asString() << "'" END
        FormulaMultiaryOr* checkOr = dynamic_cast<FormulaMultiaryOr*> ( *i );
        if ( checkOr != NULL ) {

            // take subsubformulae, insert them before current subformula and
            // erase old subformula
            for ( subFormulas_t::const_iterator j = checkOr->subFormulas.begin(); j != checkOr->subFormulas.end(); ++j ) {
                subFormulas.insert( i, *j );
            }
            subFormulas.erase( i++ );
            //DEBUG "\tOR flatten: subformulas taken, result is '" << asString() << "'" END

        } else {

			// adopt flattened subformula
			++i;
			//DEBUG "\tOR flatten: flattened used, result is '" << asString() << "'" END
        }
    }

    //DEBUG "\tOR flatten END: '" << asString() << "'" END
}

void FormulaNegation::flatten() {

    //DEBUG "\tNOT flatten START: '" << asString() << "'" END

    // check if subformula is a size one multiary formula
    //DEBUG "\tNOT flatten: subformula '" << subFormula->asString() << "'" END
    FormulaMultiary* checkMultiary = dynamic_cast<FormulaMultiary*> ( subFormula );
    if ( checkMultiary != NULL && checkMultiary->size() == 1 ) {

        // a size one multiary formula is useless, so erase it
        subFormula = checkMultiary->subFormulas.front();
        delete checkMultiary;
        //DEBUG "\tNOT flatten: size one MULTIARY flattened, result is '" << asString() << "'" END
    }


    // check if subformula is a double negation
    FormulaNegation* checkNegation = dynamic_cast<FormulaNegation*> ( subFormula );
    if ( checkNegation != NULL ) {

        FormulaNegation* checkSubNegation = dynamic_cast<FormulaNegation*> ( checkNegation->subFormula );
        if ( checkSubNegation != NULL ) {
            // a double negation formula is ugly, so erase it
            subFormula = checkSubNegation->subFormula;
            delete checkSubNegation;
            delete checkNegation;
            //DEBUG "\tNOT flatten: double NEGATION flattened, result is '" << asString() << "'" END
        }
    }

    //DEBUG "\tNOT flatten END: '" << asString() << "'" END
}



// ****************************************************************************
// merge
// ****************************************************************************

void FormulaMultiaryAnd::merge() {

	//DEBUG "\tAND merge START: '" << asString() << "'" END

	// iterate over all subformulas twice: first we determine the current
	// assumption and then we check all subformulae for being an conclusion
	for ( subFormulas_t::iterator assumption = subFormulas.begin(); assumption != subFormulas.end(); ++assumption ) {

		//DEBUG "\tAND merge: current formula '" << (*assumption)->asString() << "'" END
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
					//DEBUG "\tAND merge: erase formula '" << (*conclusion)->asString() << "'" END
					subFormulas.erase( conclusion++ );
				} else {
					//DEBUG "\tAND merge: skipped formula '" << (*conclusion)->asString() << "'" END
					++conclusion;
				}
			}
		}
	}

	//DEBUG "\tAND merge END: '" << asString() << "'" END
}

void FormulaMultiaryOr::merge() {

	//DEBUG "\tOR merge START: '" << asString() << "'" END

	// iterate over all subformulas twice: first we determine the current
	// conclusion and then we check all subformulae for being an assumption
	for ( subFormulas_t::iterator conclusion = subFormulas.begin(); conclusion != subFormulas.end(); ++conclusion ) {

		//DEBUG "\tOR merge: current formula '" << (*conclusion)->asString() << "'" END
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
					//DEBUG "\tOR merge: erase formula '" << (*assumption)->asString() << "'" END
					subFormulas.erase( assumption++ );
				} else {
					//DEBUG "\tOR merge: skipped formula '" << (*assumption)->asString() << "'" END
					++assumption;
				}
			}
		}
	}

	//DEBUG "\tOR merge END: '" << asString() << "'" END
}



// ****************************************************************************
// simplify
// ****************************************************************************

void FormulaMultiaryAnd::simplify() {

	//DEBUG "AND simplify START: '" << asString() << "'" END

	// simplify recursively all subformula first
	for ( subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ++i ) {
		//DEBUG "\tAND simplify: current formula '" << (*i)->asString() << "'" END
		(*i)->simplify();
		//DEBUG "\tAND simplify: simplified current formula '" << (*i)->asString() << "'" END
	}

    // step 1: flatten
    // iterate over all subformula and check them for being a size one mulitary
    // formula, a conjunction or a double negation
	flatten();

    // step 2: merge
	// iterate over all subformulas twice: first we determine the current
    // assumption and then we check all subformulae for being a conclusion
	merge();

    //DEBUG "AND simplify END: '" << asString() << "'" END
}

void FormulaMultiaryOr::simplify() {

	//DEBUG "OR simplify START: '" << asString() << "'" END

	// simplify recursively all subformula first
	for ( subFormulas_t::iterator i = subFormulas.begin(); i != subFormulas.end(); ++i ) {
		//DEBUG "\tOR simplify: current formula '" << (*i)->asString() << "'" END
		(*i)->simplify();
		//DEBUG "\tOR simplify: simplified current formula '" << (*i)->asString() << "'" END
	}

	// step 1: flatten
	// iterate over all subformula and check them for being a size one mulitary
	// formula, a disjunction or a double negation
	flatten();

	// step 2: merge
	// iterate over all subformulas twice: first we determine the current
	// conclusion and then we check all subformulae for being an assumption
	merge();

    //DEBUG "OR simplify END: '" << asString() << "'" END
}

void FormulaNegation::simplify() {

	//DEBUG "NOT simplify START: '" << asString() << "'" END

	// simplify subformula first
    //DEBUG "\tNOT simplify: subformula '" << subFormula->asString() << "'" END
    subFormula->simplify();
    //DEBUG "\tNOT simplify: simplified subformula '" << subFormula->asString() << "'" END

	// only step 1: flatten
	// iterate over all subformula and check them for being a size one mulitary
	// formula or a double negation
	flatten();

    //DEBUG "NOT simplify END: '" << asString() << "'" END
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


bool FormulaNegation::implies(Formula* conclusion) const {

    assert( conclusion != NULL );

    // a negation always implies TRUE
	FormulaTrue* formulaTrue = dynamic_cast<FormulaTrue*> ( conclusion );
	if ( formulaTrue != NULL ) {
        return true;
    }

	// a negation B is implied by a negation A iff unnegated B implies unnegated A
    // B => A iff ~A => ~B
	FormulaNegation* negation = dynamic_cast<FormulaNegation*> ( conclusion );
	if ( negation != NULL ) {
        return ( (negation->subFormula)->implies(this->subFormula) );
	} else {
        // this implies only negations
        return false;
    }
}


bool FormulaLiteral::implies(Formula* conclusion) const {

	assert( conclusion != NULL );

    // a literal always implies TRUE
	FormulaTrue* formulaTrue = dynamic_cast<FormulaTrue*> ( conclusion );
	if ( formulaTrue != NULL ) {
        return true;
    }

	// a literal implies a literal only if it is the same literal
	FormulaLiteral* literal = dynamic_cast<FormulaLiteral*> ( conclusion );
	if ( literal != NULL ) {
		return _literal.compare( literal->_literal ) == 0;
	} else {
		return false;
	}
}
