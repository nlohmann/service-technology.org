/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe,             *
 *                            Robert Danitz                                  *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    commGraphFormula.cc
 *
 * \brief   Implementation of class commGraphFormula. See commGraphFormula.h
 *          for further information.
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "commGraphFormula.h"
#include "debug.h"
#include "enums.h"
//#include <stdlib.h>
#include <cassert>

using namespace std;


const std::string CommGraphFormulaLiteral::FINAL = std::string("final");
const std::string CommGraphFormulaLiteral::TAU = std::string("tau");
const std::string CommGraphFormulaLiteral::TRUE = std::string("true");
const std::string CommGraphFormulaLiteral::FALSE = std::string("false");


void CommGraphFormulaAssignment::set(const std::string& literal, bool value) {
    literal2bool[literal] = value;
}

void CommGraphFormulaAssignment::setToTrue(const std::string& literal) {
    set(literal, true);
}

bool CommGraphFormulaAssignment::get(const std::string& literal) const {
    literal2bool_t::const_iterator literal2bool_iter = literal2bool.find(literal);

    if (literal2bool_iter == literal2bool.end())
        return false;

    return literal2bool_iter->second;
}

bool CommGraphFormula::satisfies(const CommGraphFormulaAssignment& assignment)
    const
{
    return value(assignment);
}


void CommGraphFormula::removeLiteral(const std::string&) {
}


threeValueLogic CommGraphFormula::equals() {
    bool result = false;

    if(dynamic_cast<CommGraphFormulaLiteral*>(this)) {
        if(this->asString() == "true") return TRUE;
        else if(this->asString() == "false") return FALSE;
        else return UNKNOWN; 
    }

    if(dynamic_cast<CommGraphFormulaMultiaryOr*>(this)) {
        result = false;
        for(CommGraphFormulaMultiaryOr::iterator i = dynamic_cast<CommGraphFormulaMultiaryOr*>(this)->begin();
            i != dynamic_cast<CommGraphFormulaMultiaryOr*>(this)->end(); i++) {
            if(dynamic_cast<CommGraphFormula*>(*i)->equals() == TRUE) return TRUE;
            else if(dynamic_cast<CommGraphFormula*>(*i)->equals() == FALSE) result |= false;
            else result = true;
        }
        return result ? UNKNOWN : FALSE; 
    }

    if(dynamic_cast<CommGraphFormulaMultiaryAnd*>(this)) {
        result = true;
        for(CommGraphFormulaMultiaryAnd::iterator i = dynamic_cast<CommGraphFormulaMultiaryAnd*>(this)->begin();
            i != dynamic_cast<CommGraphFormulaMultiaryAnd*>(this)->end(); i++) {
            if(dynamic_cast<CommGraphFormula*>(*i)->equals() == TRUE) result &= true;
            else if(dynamic_cast<CommGraphFormula*>(*i)->equals() == FALSE) return FALSE;
            else result = false;
        }
        return result ? TRUE : UNKNOWN;
    }

    return UNKNOWN;
} 


CNF_formula *CommGraphFormula::getCNF() {
    CNF_formula *cnf = new CNF_formula;
    trace(TRACE_5, "commGraphFormula::getCNF(): " + asString() + " is a\n");

    // CNF(literal) = literal 
    if(dynamic_cast<CommGraphFormulaLiteral*>(this)) {
        CommGraphFormulaMultiaryOr *clause = new CommGraphFormulaMultiaryOr;
        trace(TRACE_5, "literal.\n");

        clause->addSubFormula(this->getDeepCopy());
        cnf->addClause(clause);
        return cnf; 
    }

    // CNF(cnf) = cnf 
    if(dynamic_cast<CNF_formula*>(this)) {
        trace(TRACE_5, "CNF.\n");
        return dynamic_cast<CNF_formula*>(this)->getDeepCopy();
    }

    // CNF(phi AND psi) = CNF(phi) AND CNF(psi) 
    if(dynamic_cast<CommGraphFormulaMultiaryAnd*>(this)) {
        trace(TRACE_5, "MultiaryAnd. Going in deeper.\n");
        for(CommGraphFormulaMultiaryAnd::iterator i = dynamic_cast<CommGraphFormulaMultiaryAnd*>(this)->begin(); 
            i != dynamic_cast<CommGraphFormulaMultiaryAnd*>(this)->end(); i++) {
            CNF_formula *temp = dynamic_cast<CommGraphFormula*>(*i)->getCNF();
            for(CNF_formula::iterator j = temp->begin(); j != temp->end(); j++) {
                cnf->addClause(dynamic_cast<CommGraphFormulaMultiaryOr*>(*j)->getDeepCopy());
            }
            delete temp;
        }
        return cnf;
    }

    // CNF(phi OR psi) = ...  things becoming ugly 
    if(dynamic_cast<CommGraphFormulaMultiaryOr*>(this)) {
        trace(TRACE_5, "MultiaryOr. Going Underground.\n");
        CommGraphFormulaMultiaryOr *temp = dynamic_cast<CommGraphFormulaMultiaryOr*>(this);
        CommGraphFormulaMultiaryAnd *clause;
        //CNF_formula *conj = new CNF_formula;    
        bool final = true;

        // first of all, merge all top-level OR's 
        temp = temp->merge();

        // all SubFormulas shoud now be either Literal or MultiaryAnd

        // iterate over the MultiaryOr
        for(CommGraphFormulaMultiaryOr::iterator i = temp->begin(); i != temp->end(); ++i) {
            // CNF((phi1 AND phi2) OR psi) = CNF(phi1 OR psi) AND CNF(phi2 OR psi) 
            if(dynamic_cast<CommGraphFormulaMultiaryAnd*>(*i)) {
                clause = dynamic_cast<CommGraphFormulaMultiaryAnd*>(*i); 
                final = false;

                for(CommGraphFormulaMultiaryAnd::iterator j = clause->begin(); j != clause->end(); ++j) {
                    CommGraphFormulaMultiaryOr *disj = new CommGraphFormulaMultiaryOr;
                    disj->addSubFormula(dynamic_cast<CommGraphFormula*>(*j)->getDeepCopy()); 

                    for(CommGraphFormulaMultiaryOr::iterator k = temp->begin(); k != temp->end(); ++k) {
                        if(k != i) {
                            disj->addSubFormula(dynamic_cast<CommGraphFormula*>(*k)->getDeepCopy());
                        }
                    }

                    CNF_formula *temp_cnf = disj->getCNF();
                    trace(TRACE_5, "Adding Disjunction " + temp_cnf->asString() + "\n");
                    for(CommGraphFormulaMultiaryAnd::iterator l = temp_cnf->begin(); l != temp_cnf->end(); ++l) {
                        cnf->addClause(dynamic_cast<CommGraphFormulaMultiaryOr*>(*l)->getDeepCopy()); 
                    }
                    delete temp_cnf;
                }
            }
        }
        if(final) {
            trace(TRACE_5, "finished.\n");
            cnf->addClause(temp->getDeepCopy());
        }
        else {
            trace(TRACE_5, "not finished.\n"); 
        }
        trace(TRACE_5, "Returning: " + cnf->asString() + "\n");
        delete temp;
        return cnf;
    }

    trace(TRACE_5, "You do not see this.\n");
    return NULL; // should not happen! probably new derivate of CommGraphFormula
}


CommGraphFormulaMultiary::CommGraphFormulaMultiary() : CommGraphFormula() {
}


CommGraphFormulaMultiary::CommGraphFormulaMultiary(CommGraphFormula* newformula) {
    subFormulas.insert(newformula);
}


CommGraphFormulaMultiary::CommGraphFormulaMultiary(CommGraphFormula* lhs,
    CommGraphFormula* rhs)
{
    subFormulas.insert(lhs);
    subFormulas.insert(rhs);
}

CommGraphFormulaMultiary::~CommGraphFormulaMultiary() {
	trace(TRACE_5, "CommGraphFormulaMultiary::~CommGraphFormulaMultiary() : start\n");

    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
        currentFormula != subFormulas.end(); ++currentFormula)
    {
        delete *currentFormula;
    }
	trace(TRACE_5, "CommGraphFormulaMultiary::~CommGraphFormulaMultiary() : end\n");
}

std::string CommGraphFormulaMultiary::asString() const
{
    if (subFormulas.empty())
        return getEmptyFormulaEquivalent().asString();

    subFormulas_t::const_iterator currentFormula = subFormulas.begin();
    std::string formulaString = '(' + (*currentFormula)->asString();

    while (++currentFormula != subFormulas.end())
    {
        formulaString += " " + getOperator() + " " +
            (*currentFormula)->asString();
    }

    return formulaString + ')';
}


bool CommGraphFormulaMultiary::value(
    const CommGraphFormulaAssignment& assignment) const
{
    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
        currentFormula != subFormulas.end(); ++currentFormula)
    {
        if ((*currentFormula)->value(assignment) !=
            getEmptyFormulaEquivalent().value(assignment))
        {
            return !getEmptyFormulaEquivalent().value(assignment);
        }
    }

    return getEmptyFormulaEquivalent().value(assignment);
}


void CommGraphFormulaMultiary::addSubFormula(CommGraphFormula* subformula) {
	subFormulas.insert(subformula);
}


void CommGraphFormulaMultiary::removeLiteral(const std::string& name) {
	
	trace(TRACE_5, "CommGraphFormulaMultiary::removeLiteral(const std::string& name) : start\n");

	subFormulas_t::iterator iCurrentFormula;
	//cout << "\tanzahl von klauseln: " << subFormulas.size() << endl; int i = 1;
	//cout << "\tremoving literal " << name << " from clause nr " << i++ << endl;	
	
	for (iCurrentFormula = subFormulas.begin();
	     iCurrentFormula != subFormulas.end(); ) {
		
		// if the considered current formula is a literal, then remove it;
		// call the function recursively, otherwise
		CommGraphFormulaLiteral* currentFormula = dynamic_cast<CommGraphFormulaLiteral*> (*iCurrentFormula);
		if (currentFormula != NULL) {
			// the current formula is a literal
			if (currentFormula->asString() == name) {
				// the literal has the right name, so remove it
				subFormulas_t::iterator iOldFormula = iCurrentFormula++;
				delete *iOldFormula;
				subFormulas.erase(iOldFormula);
			} else {
				iCurrentFormula++;
			}
		} else {
			// the current formula is no literal, so call removeLiteral again
			(*iCurrentFormula)->removeLiteral(name);
			iCurrentFormula++;
		}
	}

	trace(TRACE_5, "CommGraphFormulaMultiary::removeLiteral(const std::string& name) : end\n");
}

void CommGraphFormulaMultiary::deepCopyMultiaryPrivateMembersToNewFormula(
    CommGraphFormulaMultiary* newFormula) const
{
    newFormula->subFormulas.clear();
    for (subFormulas_t::const_iterator iFormula = subFormulas.begin();
         iFormula != subFormulas.end(); ++iFormula)
    {
        newFormula->subFormulas.insert((*iFormula)->getDeepCopy());
    }
}

CommGraphFormulaMultiary::iterator CommGraphFormulaMultiary::begin()
{
    return subFormulas.begin();
}

CommGraphFormulaMultiary::const_iterator CommGraphFormulaMultiary::begin() const
{
    return subFormulas.begin();
}

CommGraphFormulaMultiary::iterator CommGraphFormulaMultiary::end()
{
    return subFormulas.end();
}

CommGraphFormulaMultiary::const_iterator CommGraphFormulaMultiary::end() const
{
    return subFormulas.end();
}

bool CommGraphFormulaMultiary::empty() const
{
    return subFormulas.empty();
}

CommGraphFormulaMultiaryAnd::CommGraphFormulaMultiaryAnd() {
}


CommGraphFormulaMultiaryAnd::CommGraphFormulaMultiaryAnd(CommGraphFormula* formula) :
   CommGraphFormulaMultiary(formula) {
}


CommGraphFormulaMultiaryAnd::CommGraphFormulaMultiaryAnd(CommGraphFormula* lhs_,
    CommGraphFormula* rhs_) :
    CommGraphFormulaMultiary(lhs_, rhs_) {

}


CommGraphFormulaMultiaryAnd* CommGraphFormulaMultiaryAnd::merge() {
    bool final = true;
    CommGraphFormulaMultiaryAnd *result = new CommGraphFormulaMultiaryAnd;

    trace(TRACE_5, "commGraphFormulaMultiaryAnd::merge: " + asString() + " merged to ");
    for(CommGraphFormulaMultiaryAnd::iterator i = begin(); i != end(); i++) {
        if(dynamic_cast<CommGraphFormulaMultiaryAnd*>(*i)) {
            final = false;
            CommGraphFormulaMultiaryAnd *temp = dynamic_cast<CommGraphFormulaMultiaryAnd*>(*i);
            for(CommGraphFormulaMultiaryAnd::iterator j = temp->begin();
                j != temp->end(); j++) {
            result->addSubFormula(dynamic_cast<CommGraphFormula*>(*j)->getDeepCopy());
            }
        }
        else {
            result->addSubFormula(dynamic_cast<CommGraphFormula*>(*i)->getDeepCopy());
        }
    }
    trace(TRACE_5, result->asString() + "\n");

    if (final) {
        return result;
    }

    CommGraphFormulaMultiaryAnd* oldResult = result;
    result = result->merge();
    delete oldResult;
    return result;
}


CommGraphFormulaMultiaryAnd* CommGraphFormulaMultiaryAnd::getDeepCopy() const
{
    CommGraphFormulaMultiaryAnd* newFormula =
        new CommGraphFormulaMultiaryAnd(*this);

    deepCopyMultiaryPrivateMembersToNewFormula(newFormula);

    return newFormula;
}


std::string CommGraphFormulaMultiaryAnd::getOperator() const
{
    return "*";
}


const CommGraphFormulaFixed
CommGraphFormulaMultiaryAnd::emptyFormulaEquivalent = CommGraphFormulaTrue();


const CommGraphFormulaFixed&
CommGraphFormulaMultiaryAnd::getEmptyFormulaEquivalent() const
{
    return emptyFormulaEquivalent;
}


CommGraphFormulaMultiaryOr::CommGraphFormulaMultiaryOr() {
}


CommGraphFormulaMultiaryOr::CommGraphFormulaMultiaryOr(CommGraphFormula* formula) :
   CommGraphFormulaMultiary(formula) {
}


CommGraphFormulaMultiaryOr::CommGraphFormulaMultiaryOr(CommGraphFormula* lhs_,
    CommGraphFormula* rhs_) : CommGraphFormulaMultiary(lhs_, rhs_)
{
}


std::string CommGraphFormulaMultiaryOr::getOperator() const
{
    return "+";
}

CommGraphFormulaMultiaryOr* CommGraphFormulaMultiaryOr::getDeepCopy() const
{
    CommGraphFormulaMultiaryOr* newFormula =
        new CommGraphFormulaMultiaryOr(*this);

    deepCopyMultiaryPrivateMembersToNewFormula(newFormula);

    return newFormula;
}


CommGraphFormulaMultiaryOr* CommGraphFormulaMultiaryOr::merge() {
    bool final = true;
    CommGraphFormulaMultiaryOr *result = new CommGraphFormulaMultiaryOr;

    trace(TRACE_5, "commGraphFormulaMultiaryOr::merge: " + asString() + " merged to ");
    for(CommGraphFormulaMultiaryOr::iterator i = begin(); i != end(); i++) {
        if(dynamic_cast<CommGraphFormulaMultiaryOr*>(*i)) {
            final = false;
            CommGraphFormulaMultiaryOr *temp = dynamic_cast<CommGraphFormulaMultiaryOr*>(*i); 
            for(CommGraphFormulaMultiaryOr::iterator j = temp->begin();
                j != temp->end(); j++) {
            result->addSubFormula(dynamic_cast<CommGraphFormula*>(*j)->getDeepCopy()); 
            }
        }
        else {
            result->addSubFormula(dynamic_cast<CommGraphFormula*>(*i)->getDeepCopy());
        }
    }
    trace(TRACE_5, result->asString() + "\n");

    if (final) {
        return result;
    }

    CommGraphFormulaMultiaryOr* oldResult = result;
    result = result->merge();
    delete oldResult;
    return result;
}


// shall only be used for clauses within cnf!
bool CommGraphFormulaMultiaryOr::implies(CommGraphFormulaMultiaryOr *op) {
    bool result;

    trace(TRACE_5, this->asString() + " -> " + op->asString() + " ? ... "); 

    for(CommGraphFormulaMultiaryOr::iterator i = this->begin(); i != this->end(); i++) {
        result = false;
        for(CommGraphFormulaMultiaryOr::iterator j = op->begin(); j != op->end(); j++) {
	    result |= (dynamic_cast<CommGraphFormulaLiteral*>(*i)->asString() 
                == dynamic_cast<CommGraphFormulaLiteral*>(*j)->asString()); 
        }
        if(!result) {
            trace(TRACE_5, "false.\n"); 
            return false;
        }
    }

    trace(TRACE_5, "true.\n");
    return true;
}


const CommGraphFormulaFixed
CommGraphFormulaMultiaryOr::emptyFormulaEquivalent = CommGraphFormulaFalse();


const CommGraphFormulaFixed&
CommGraphFormulaMultiaryOr::getEmptyFormulaEquivalent() const
{
    return emptyFormulaEquivalent;
}


CNF_formula::CNF_formula() {
}


CNF_formula::CNF_formula(CommGraphFormulaMultiaryOr* clause_) :
    CommGraphFormulaMultiaryAnd(clause_) {
    
}

CNF_formula::CNF_formula(CommGraphFormulaMultiaryOr* clause1_, CommGraphFormulaMultiaryOr* clause2_) :
    CommGraphFormulaMultiaryAnd(clause1_, clause2_) {
    
}

CNF_formula* CNF_formula::getDeepCopy() const
{
    CommGraphFormulaMultiaryAnd* newFormula =
        CommGraphFormulaMultiaryAnd::getDeepCopy();

    return static_cast<CNF_formula*>(newFormula);
}


void CNF_formula::addClause(CommGraphFormulaMultiaryOr* clause) {
	addSubFormula(clause);
}

//! \fn bool CNF_formula::implies(CNF_formula *op) 
//! \param op the rhs of the implication 
//! \brief checks whether the current CNF formula implies the given one via op
//!        Note: all literals have to occur non-negated! 
bool CNF_formula::implies(CNF_formula *op) {
    bool result;

    if(op == NULL) return false;
	
    trace(TRACE_5, "bool CNF_formula::implies(CNF_formula *op)\n");
    trace(TRACE_5, asString() + " => " + op->asString() + "?\n");

    if(this->equals() == FALSE) return true;
    if(op->equals() == TRUE) return true;

    for(CNF_formula::iterator i = op->begin(); i != op->end(); i++) {
       	// iterate over rhs operand
        if((dynamic_cast<CommGraphFormula*>(*i)->asString() != "final") && 
           (dynamic_cast<CommGraphFormula*>(*i)->asString() != "true") && 
           (dynamic_cast<CommGraphFormula*>(*i)->asString() != "false")) {
            result = false;

            for(CNF_formula::iterator j = begin(); j != end(); j++) {
                // iterate over lhs operand
                result |= (dynamic_cast<CommGraphFormulaMultiaryOr*>(*j)->implies(
                    dynamic_cast<CommGraphFormulaMultiaryOr*>(*i))); 
            }
            if(!result) {
            trace(TRACE_5, "Implication failed.\n");
            return false;
            }
        }
    }
 
    trace(TRACE_5, "Implication succesfull.\n");
    return true;
}


CommGraphFormulaFixed::CommGraphFormulaFixed(bool value,
    const std::string& asString) :
    CommGraphFormulaLiteral(asString),
    _value(value)
{
}

bool CommGraphFormulaFixed::value(const CommGraphFormulaAssignment&) const
{
    return _value;
}

CommGraphFormulaFixed* CommGraphFormulaFixed::getDeepCopy() const
{
    return new CommGraphFormulaFixed(*this);
}

CommGraphFormulaTrue::CommGraphFormulaTrue() :
    CommGraphFormulaFixed(true, CommGraphFormulaLiteral::TRUE)
{
}

CommGraphFormulaFalse::CommGraphFormulaFalse() :
    CommGraphFormulaFixed(false, CommGraphFormulaLiteral::FALSE)
{
}

CommGraphFormulaLiteral::CommGraphFormulaLiteral(const std::string& literal_) :
    literal(literal_)
{
}

CommGraphFormulaLiteral* CommGraphFormulaLiteral::getDeepCopy() const
{
    return new CommGraphFormulaLiteral(*this);
}

bool CommGraphFormulaLiteral::value(
    const CommGraphFormulaAssignment& assignment) const
{
    return assignment.get(literal);
}

std::string CommGraphFormulaLiteral::asString() const
{
    return literal;
}


CommGraphFormulaLiteralFinal::CommGraphFormulaLiteralFinal() :
    CommGraphFormulaLiteral(CommGraphFormulaLiteral::FINAL)
{
}


CommGraphFormulaLiteralTau::CommGraphFormulaLiteralTau() :
    CommGraphFormulaLiteral(CommGraphFormulaLiteral::TAU)
{
}

