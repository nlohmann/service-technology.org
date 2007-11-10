/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe,             *
 *           Robert Danitz                                                   *
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
 * \file    GraphFormula.cc
 *
 * \brief   Implementation of class GraphFormula. See GraphFormula.h
 *          for further information.
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#include "GraphFormula.h"
#include "debug.h"
#include <cassert>

using namespace std;

// TRUE and FALSE #defined in cudd package may interfere with
// GraphFormulaLiteral::TRUE and ...::FALSE.
#undef TRUE
#undef FALSE

const std::string GraphFormulaLiteral::FINAL = std::string("final");
const std::string GraphFormulaLiteral::TAU = std::string("tau");
const std::string GraphFormulaLiteral::TRUE = std::string("true");
const std::string GraphFormulaLiteral::FALSE = std::string("false");


void GraphFormulaAssignment::set(const std::string& literal, bool value) {
    literal2bool[literal] = value;
}


void GraphFormulaAssignment::setToTrue(const std::string& literal) {
    set(literal, true);
}


void GraphFormulaAssignment::setToFalse(const std::string& literal) {
    set(literal, false);
}


bool GraphFormulaAssignment::get(const std::string& literal) const {
    literal2bool_t::const_iterator
            literal2bool_iter = literal2bool.find(literal);

    if (literal2bool_iter == literal2bool.end())
        return false;

    return literal2bool_iter->second;
}


bool GraphFormula::satisfies(const GraphFormulaAssignment& assignment) const {
    return value(assignment);
}


void GraphFormula::removeLiteral(const std::string&) {
}

void GraphFormula::removeLiteralForReal(const std::string&) {
}

int GraphFormula::getSubFormulaSize() const {
}

void GraphFormula::removeLiteralByHiding(const std::string&) {
}

threeValueLogic GraphFormula::equals() {

    if (dynamic_cast<GraphFormulaLiteral*>(this)) {
        if (asString() == GraphFormulaLiteral::TRUE) {
            return TRUE;
        } else if (asString() == GraphFormulaLiteral::FALSE) {
            return FALSE;
        } else {
            return UNKNOWN;
        }
    }

    // assert: this is no GraphFormulaLiteral.

    if (dynamic_cast<GraphFormulaMultiaryOr*>(this)) {
        bool unknownFound = false;
        for (GraphFormulaMultiaryOr::iterator
             i = dynamic_cast<GraphFormulaMultiaryOr*>(this)->begin();
             i != dynamic_cast<GraphFormulaMultiaryOr*>(this)->end(); i++) {

            if (dynamic_cast<GraphFormula*>(*i)->equals() == TRUE) {
                return TRUE;
            }

            if (dynamic_cast<GraphFormula*>(*i)->equals() == UNKNOWN) {
                unknownFound = true;
            }
        }

        return unknownFound ? UNKNOWN : FALSE;
    }

    if (dynamic_cast<GraphFormulaMultiaryAnd*>(this)) {
        bool noUnknownFound = true;
        for (GraphFormulaMultiaryAnd::iterator
             i = dynamic_cast<GraphFormulaMultiaryAnd*>(this)->begin();
             i != dynamic_cast<GraphFormulaMultiaryAnd*>(this)->end(); i++) {

            if (dynamic_cast<GraphFormula*>(*i)->equals() == FALSE) {
                return FALSE;
            }

            if (dynamic_cast<GraphFormula*>(*i)->equals() == UNKNOWN) {
                noUnknownFound = false;
            }
        }
        return noUnknownFound ? TRUE : UNKNOWN;
    }

    assert(false);
    return UNKNOWN;
}


GraphFormulaCNF* GraphFormula::getCNF() {
    trace(TRACE_5, "GraphFormula::getCNF(): " + asString() + " is a\n");

    // CNF(literal) = literal 
    if (dynamic_cast<GraphFormulaLiteral*>(this)) {
        GraphFormulaMultiaryOr* clause = new GraphFormulaMultiaryOr;
        trace(TRACE_5, "literal.\n");

        clause->addSubFormula(this->getDeepCopy());
        GraphFormulaCNF* cnf = new GraphFormulaCNF;
        cnf->addClause(clause);
        return cnf;
    }

    // CNF(cnf) = cnf 
    if (dynamic_cast<GraphFormulaCNF*>(this)) {
        trace(TRACE_5, "CNF.\n");
        return dynamic_cast<GraphFormulaCNF*>(this)->getDeepCopy();
    }

    // CNF(phi AND psi) = CNF(phi) AND CNF(psi) 
    if (dynamic_cast<GraphFormulaMultiaryAnd*>(this)) {
        trace(TRACE_5, "MultiaryAnd. Going in deeper.\n");
        GraphFormulaCNF* cnf = new GraphFormulaCNF;
        for (GraphFormulaMultiaryAnd::iterator
             i = dynamic_cast<GraphFormulaMultiaryAnd*>(this)->begin();
             i != dynamic_cast<GraphFormulaMultiaryAnd*>(this)->end(); i++) {

            GraphFormulaCNF *temp = dynamic_cast<GraphFormula*>(*i)->getCNF();
            for (GraphFormulaCNF::iterator j = temp->begin(); j != temp->end(); j++) {
                cnf->addClause(dynamic_cast<GraphFormulaMultiaryOr*>(*j)->getDeepCopy());
            }
            delete temp;
        }
        return cnf;
    }

    // CNF(phi OR psi) = ...  things becoming ugly 
    if (dynamic_cast<GraphFormulaMultiaryOr*>(this)) {
        trace(TRACE_5, "MultiaryOr. Going Underground.\n");
        GraphFormulaCNF* cnf = new GraphFormulaCNF;
        GraphFormulaMultiaryOr* temp = dynamic_cast<GraphFormulaMultiaryOr*>(this);
        GraphFormulaMultiaryAnd *clause;
        //GraphFormulaCNF *conj = new GraphFormulaCNF;    
        bool final = true;

        // first of all, merge all top-level OR's 
        temp = temp->merge();

        // all SubFormulas shoud now be either Literal or MultiaryAnd

        // iterate over the MultiaryOr
        for (GraphFormulaMultiaryOr::iterator i = temp->begin(); i != temp->end(); ++i) {
            // CNF((phi1 AND phi2) OR psi) = CNF(phi1 OR psi) AND CNF(phi2 OR psi) 
            if (dynamic_cast<GraphFormulaMultiaryAnd*>(*i)) {
                clause = dynamic_cast<GraphFormulaMultiaryAnd*>(*i);
                final = false;

                for (GraphFormulaMultiaryAnd::iterator j = clause->begin();
                     j != clause->end(); ++j) {

                    GraphFormulaMultiaryOr *disj = new GraphFormulaMultiaryOr;
                    disj->addSubFormula(dynamic_cast<GraphFormula*>(*j)->getDeepCopy());

                    for (GraphFormulaMultiaryOr::iterator k = temp->begin();
                         k != temp->end(); ++k) {

                        if (k != i) {
                            disj->addSubFormula(dynamic_cast<GraphFormula*>(*k)->getDeepCopy());
                        }
                    }

                    GraphFormulaCNF *temp_cnf = disj->getCNF();
                    trace(TRACE_5, "Adding Disjunction " + temp_cnf->asString() + "\n");
                    for (GraphFormulaMultiaryAnd::iterator
                         l = temp_cnf->begin(); l != temp_cnf->end(); ++l) {
                        cnf->addClause(dynamic_cast<GraphFormulaMultiaryOr*>(*l)->getDeepCopy());
                    }
                    delete temp_cnf;
                }
            }
        }
        if (final) {
            trace(TRACE_5, "finished.\n");
            cnf->addClause(temp->getDeepCopy());
        } else {
            trace(TRACE_5, "not finished.\n");
        }
        trace(TRACE_5, "Returning: " + cnf->asString() + "\n");
        delete temp;
        return cnf;
    }

    trace(TRACE_5, "You do not see this.\n");
    return NULL; // should not happen! probably new derivate of GraphFormula
}


GraphFormulaMultiary::GraphFormulaMultiary() :
    GraphFormula() {
}


GraphFormulaMultiary::GraphFormulaMultiary(GraphFormula* newformula) {
    subFormulas.push_back(newformula);
}


GraphFormulaMultiary::GraphFormulaMultiary(GraphFormula* lhs, GraphFormula* rhs) {
    subFormulas.push_back(lhs);
    subFormulas.push_back(rhs);
}


GraphFormulaMultiary::~GraphFormulaMultiary() {
    trace(TRACE_5, "GraphFormulaMultiary::~GraphFormulaMultiary() : start\n");

    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
         currentFormula != subFormulas.end(); ++currentFormula) {
        delete *currentFormula;
    }
    trace(TRACE_5, "GraphFormulaMultiary::~GraphFormulaMultiary() : end\n");
}


std::string GraphFormulaMultiary::asString() const {
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


bool GraphFormulaMultiary::value(const GraphFormulaAssignment& assignment) const {
    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
         currentFormula != subFormulas.end(); ++currentFormula) {
        if ((*currentFormula)->value(assignment) != getEmptyFormulaEquivalent().value(assignment)) {
            return !getEmptyFormulaEquivalent().value(assignment);
        }
    }

    return getEmptyFormulaEquivalent().value(assignment);
}


void GraphFormulaMultiary::addSubFormula(GraphFormula* subformula) {
    subFormulas.push_back(subformula);
}


GraphFormulaMultiary::iterator
GraphFormulaMultiary::removeSubFormula(iterator subformula) {
    return subFormulas.erase(subformula);
}


void GraphFormulaMultiary::removeLiteral(const std::string& name) {

    trace(TRACE_3, "GraphFormulaMultiary::removeLiteral(const std::string& name) : start\n");

    subFormulas_t::iterator iCurrentFormula;
    //cout << "\tanzahl von klauseln: " << subFormulas.size() << endl; int i = 1;
    //cout << "\tremoving literal " << name << " from clause nr " << i++ << endl;	

    for (iCurrentFormula = subFormulas.begin();
         iCurrentFormula != subFormulas.end();) {

        // if the considered current formula is a literal, then remove it;
        // call the function recursively, otherwise
        GraphFormulaLiteral* currentFormula = dynamic_cast<GraphFormulaLiteral*> (*iCurrentFormula);
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

    trace(TRACE_3, "GraphFormulaMultiary::removeLiteral(const std::string& name) : end\n");
}

int GraphFormulaMultiary::getSubFormulaSize() const {
	return subFormulas.size();
}

// \descr 
void GraphFormulaMultiary::removeLiteralForReal(const std::string& name) {

    trace(TRACE_3, "GraphFormulaMultiary::removeLiteralForReal(const std::string& name) : start\n");

    subFormulas_t::iterator iCurrentFormula;
    
    std::list<subFormulas_t::iterator> listOfFormulaIterator;
    
    std::list<subFormulas_t::iterator>::iterator iterOfListOfFormulaIterator;
    
    //cout << "\tanzahl von klauseln: " << subFormulas.size() << endl; int i = 1;
    //cout << "\tremoving literal " << name << " from clause nr " << i++ << endl;	

    for (iCurrentFormula = subFormulas.begin();
         iCurrentFormula != subFormulas.end();) {
        // if the considered current formula is a literal, then remove it;
        // call the function recursively, otherwise
        GraphFormulaLiteral* currentFormula = dynamic_cast<GraphFormulaLiteral*> (*iCurrentFormula);
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

    trace(TRACE_3, "GraphFormulaMultiary::removeLiteral(const std::string& name) : end\n");
}


void GraphFormulaMultiary::removeLiteralByHiding(const std::string& name) {

    trace(TRACE_5, "GraphFormulaMultiary::removeLiteralByHiding(const std::string& name) : start\n");

    subFormulas_t::iterator iCurrentFormula;
    //cout << "\tanzahl von klauseln: " << subFormulas.size() << endl; int i = 1;
    //cout << "\tremoving literal " << name << " from clause nr " << i++ << endl;

    for (iCurrentFormula = subFormulas.begin();
         iCurrentFormula != subFormulas.end();) {

        // if the considered current formula is a literal, then remove it;
        // call the function recursively, otherwise
        GraphFormulaLiteral* currentFormula = dynamic_cast<GraphFormulaLiteral*> (*iCurrentFormula);
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
            if ((dynamic_cast<GraphFormulaMultiary*>(*iCurrentFormula))->empty()) {
                delete *iCurrentFormula;
                iCurrentFormula = subFormulas.erase(iCurrentFormula);
            } else {
                iCurrentFormula++;
            }
        }
    }

    trace(TRACE_5, "GraphFormulaMultiary::removeLiteralByHiding(const std::string& name) : end\n");
}


void GraphFormulaMultiary::deepCopyMultiaryPrivateMembersToNewFormula(GraphFormulaMultiary* newFormula) const {
    newFormula->subFormulas.clear();
    for (subFormulas_t::const_iterator iFormula = subFormulas.begin();
         iFormula != subFormulas.end(); ++iFormula) {
        newFormula->subFormulas.push_back((*iFormula)->getDeepCopy());
    }
}


GraphFormulaMultiary::iterator GraphFormulaMultiary::begin() {
    return subFormulas.begin();
}


GraphFormulaMultiary::const_iterator GraphFormulaMultiary::begin() const {
    return subFormulas.begin();
}


GraphFormulaMultiary::iterator GraphFormulaMultiary::end() {
    return subFormulas.end();
}


GraphFormulaMultiary::const_iterator GraphFormulaMultiary::end() const {
    return subFormulas.end();
}


bool GraphFormulaMultiary::empty() const {
    return subFormulas.empty();
}

int GraphFormulaMultiary::size() const {
    return subFormulas.size();
}

GraphFormulaMultiaryAnd::GraphFormulaMultiaryAnd() {
}


GraphFormulaMultiaryAnd::GraphFormulaMultiaryAnd(GraphFormula* formula) :
    GraphFormulaMultiary(formula) {
}


GraphFormulaMultiaryAnd::GraphFormulaMultiaryAnd(GraphFormula* lhs_,
                                                 GraphFormula* rhs_) :
    GraphFormulaMultiary(lhs_, rhs_) {
}


GraphFormulaMultiaryAnd* GraphFormulaMultiaryAnd::merge() {
    bool final = true;
    GraphFormulaMultiaryAnd *result = new GraphFormulaMultiaryAnd;

    trace(TRACE_5, "GraphFormulaMultiaryAnd::merge: " + asString() + " merged to ");
    for (GraphFormulaMultiaryAnd::iterator i = begin(); i != end(); i++) {
        if (dynamic_cast<GraphFormulaMultiaryAnd*>(*i)) {
            final = false;
            GraphFormulaMultiaryAnd* temp = dynamic_cast<GraphFormulaMultiaryAnd*>(*i);
            for (GraphFormulaMultiaryAnd::iterator j = temp->begin();
                 j != temp->end(); j++) {
                result->addSubFormula(dynamic_cast<GraphFormula*>(*j)->getDeepCopy());
            }
        } else {
            result->addSubFormula(dynamic_cast<GraphFormula*>(*i)->getDeepCopy());
        }
    }
    trace(TRACE_5, result->asString() + "\n");

    if (final) {
        return result;
    }

    GraphFormulaMultiaryAnd* oldResult = result;
    result = result->merge();
    delete oldResult;
    return result;
}


GraphFormulaMultiaryAnd* GraphFormulaMultiaryAnd::getDeepCopy() const {
    GraphFormulaMultiaryAnd* newFormula = new GraphFormulaMultiaryAnd(*this);

    deepCopyMultiaryPrivateMembersToNewFormula(newFormula);

    return newFormula;
}


std::string GraphFormulaMultiaryAnd::getOperator() const {
    return "*";
}


const GraphFormulaFixed GraphFormulaMultiaryAnd::emptyFormulaEquivalent = GraphFormulaTrue();


const GraphFormulaFixed&GraphFormulaMultiaryAnd::getEmptyFormulaEquivalent() const {
    return emptyFormulaEquivalent;
}


GraphFormulaMultiaryOr::GraphFormulaMultiaryOr() {
}


GraphFormulaMultiaryOr::GraphFormulaMultiaryOr(GraphFormula* formula) :
    GraphFormulaMultiary(formula) {
}


GraphFormulaMultiaryOr::GraphFormulaMultiaryOr(GraphFormula* lhs_,
                                               GraphFormula* rhs_) :
    GraphFormulaMultiary(lhs_, rhs_) {
}


std::string GraphFormulaMultiaryOr::getOperator() const {
    return "+";
}


GraphFormulaMultiaryOr* GraphFormulaMultiaryOr::getDeepCopy() const {
    GraphFormulaMultiaryOr* newFormula =new GraphFormulaMultiaryOr(*this);

    deepCopyMultiaryPrivateMembersToNewFormula(newFormula);

    return newFormula;
}


GraphFormulaMultiaryOr* GraphFormulaMultiaryOr::merge() {
    bool final = true;
    GraphFormulaMultiaryOr *result = new GraphFormulaMultiaryOr;

    trace(TRACE_5, "GraphFormulaMultiaryOr::merge: " + asString() + " merged to ");
    for (GraphFormulaMultiaryOr::iterator i = begin(); i != end(); i++) {
        if (dynamic_cast<GraphFormulaMultiaryOr*>(*i)) {
            final = false;
            GraphFormulaMultiaryOr* temp = dynamic_cast<GraphFormulaMultiaryOr*>(*i);
            for (GraphFormulaMultiaryOr::iterator j = temp->begin();
                 j != temp->end(); j++) {
                result->addSubFormula(dynamic_cast<GraphFormula*>(*j)->getDeepCopy());
            }
        } else {
            result->addSubFormula(dynamic_cast<GraphFormula*>(*i)->getDeepCopy());
        }
    }
    trace(TRACE_5, result->asString() + "\n");

    if (final) {
        return result;
    }

    GraphFormulaMultiaryOr* oldResult = result;
    result = result->merge();
    delete oldResult;
    return result;
}


// shall only be used for clauses within cnf!
bool GraphFormulaMultiaryOr::implies(GraphFormulaMultiaryOr *op) {
    bool result;

    trace(TRACE_5, this->asString() + " -> " + op->asString() + " ? ... ");

    // GraphFormulaCNF::simplify() depends on this if clause to remove superfluous
    // true-clauses.
    if (op->equals() == TRUE) {
        return true;
    }

    for (GraphFormulaMultiaryOr::iterator i = this->begin(); i != this->end(); i++) {
        result = false;
        for (GraphFormulaMultiaryOr::iterator j = op->begin(); j != op->end(); j++) {
            result |= (dynamic_cast<GraphFormulaLiteral*>(*i)->asString() == dynamic_cast<GraphFormulaLiteral*>(*j)->asString());
        }
        if (!result) {
            trace(TRACE_5, "false.\n");
            return false;
        }
    }

    trace(TRACE_5, "true.\n");
    return true;
}


const GraphFormulaFixed GraphFormulaMultiaryOr::emptyFormulaEquivalent = GraphFormulaFalse();


const GraphFormulaFixed&GraphFormulaMultiaryOr::getEmptyFormulaEquivalent() const {
    return emptyFormulaEquivalent;
}


GraphFormulaCNF::GraphFormulaCNF() {
}


GraphFormulaCNF::GraphFormulaCNF(GraphFormulaMultiaryOr* clause_) :
    GraphFormulaMultiaryAnd(clause_) {
}


GraphFormulaCNF::GraphFormulaCNF(GraphFormulaMultiaryOr* clause1_,
                                 GraphFormulaMultiaryOr* clause2_) :
    GraphFormulaMultiaryAnd(clause1_, clause2_) {
}


GraphFormulaCNF* GraphFormulaCNF::getDeepCopy() const {
    GraphFormulaMultiaryAnd* newFormula =GraphFormulaMultiaryAnd::getDeepCopy();

    return static_cast<GraphFormulaCNF*>(newFormula);
}


void GraphFormulaCNF::addClause(GraphFormulaMultiaryOr* clause) {
    addSubFormula(clause);
}


//! \fn bool GraphFormulaCNF::implies(GraphFormulaCNF *op)
//! \param op the rhs of the implication
//! \brief checks whether the current CNF formula implies the given one via op
//! Note: all literals have to occur non-negated!
bool GraphFormulaCNF::implies(GraphFormulaCNF *op) {
    bool result;

    if (op == NULL) {
        return false;
    }

    trace(TRACE_5, "bool GraphFormulaCNF::implies(GraphFormulaCNF *op)\n");
    trace(TRACE_5, asString() + " => "+ op->asString() + "?\n");

    if (this->equals() == FALSE)
        return true;
    if (op->equals() == TRUE)
        return true;

    for (GraphFormulaCNF::iterator i = op->begin(); i != op->end(); i++) {
        // iterate over rhs operand
        if ((dynamic_cast<GraphFormula*>(*i)->asString() != "final") &&(dynamic_cast<GraphFormula*>(*i)->asString() != "true")&&(dynamic_cast<GraphFormula*>(*i)->asString() != "false")) {
            result = false;

            for (GraphFormulaCNF::iterator j = begin(); j != end(); j++) {
                // iterate over lhs operand
                result |= (dynamic_cast<GraphFormulaMultiaryOr*>(*j)->implies(dynamic_cast<GraphFormulaMultiaryOr*>(*i)));
            }
            if (!result) {
                trace(TRACE_5, "Implication failed.\n");
                return false;
            }
        }
    }

    trace(TRACE_5, "Implication succesfull.\n");
    return true;
}


void GraphFormulaCNF::simplify() {
    for (GraphFormulaCNF::const_iterator iLhs = begin(); iLhs != end(); ++iLhs) {
        GraphFormulaMultiaryOr* lhs =dynamic_cast<GraphFormulaMultiaryOr*>(*iLhs);
        assert(lhs != NULL);
        GraphFormulaCNF::iterator iRhs = begin();
        while (iRhs != end()) {
            if (iLhs == iRhs) {
                ++iRhs;
                continue;
            }

            GraphFormulaMultiaryOr* rhs =dynamic_cast<GraphFormulaMultiaryOr*>(*iRhs);
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


GraphFormulaFixed::GraphFormulaFixed(bool value, const std::string& asString) :
    GraphFormulaLiteral(asString), _value(value) {
}


bool GraphFormulaFixed::value(const GraphFormulaAssignment&) const {
    return _value;
}


GraphFormulaFixed* GraphFormulaFixed::getDeepCopy() const {
    return new GraphFormulaFixed(*this);
}


GraphFormulaTrue::GraphFormulaTrue() :
    GraphFormulaFixed(true, GraphFormulaLiteral::TRUE) {
}


GraphFormulaFalse::GraphFormulaFalse() :
    GraphFormulaFixed(false, GraphFormulaLiteral::FALSE) {
}


GraphFormulaLiteral::GraphFormulaLiteral(const std::string& literal_) :
    literal(literal_) {
}


GraphFormulaLiteral* GraphFormulaLiteral::getDeepCopy() const {
    return new GraphFormulaLiteral(*this);
}


bool GraphFormulaLiteral::value(const GraphFormulaAssignment& assignment) const {
    return assignment.get(literal);
}


std::string GraphFormulaLiteral::asString() const {
    return literal;
}


GraphFormulaLiteralFinal::GraphFormulaLiteralFinal() :
    GraphFormulaLiteral(GraphFormulaLiteral::FINAL) {
}


GraphFormulaLiteralTau::GraphFormulaLiteralTau() :
    GraphFormulaLiteral(GraphFormulaLiteral::TAU) {
}
