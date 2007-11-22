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


//! \brief  Sets the given literal to the given truth value within this
//!         GraphFormulaAssignment.
//! \param literal Literal whose truth value should be set.
//! \param value Truth value the given literal should be set to.
void GraphFormulaAssignment::set(const std::string& literal, bool value) {
    literal2bool[literal] = value;
}


//! \brief Sets the given literal to true within this GraphFormulaAssignment.
//! \param literal Literal which should be set to true.
void GraphFormulaAssignment::setToTrue(const std::string& literal) {
    set(literal, true);
}


//! \brief Sets the given literal to false within this GraphFormulaAssignment.
//! \param literal Literal which should be set to false.
void GraphFormulaAssignment::setToFalse(const std::string& literal) {
    set(literal, false);
}


//! \brief Gets the truth value of the given literal within this
//!        GraphFormulaAssignment. If no truth value was previously set for
//!        this literal by set() or setToTrue(), then the truth value of the given
//!        literal is implicetly false.
//! \param literal Literal whose truth value should be determined.
//! \returns Truth value of the given literal.
bool GraphFormulaAssignment::get(const std::string& literal) const {
    literal2bool_t::const_iterator
            literal2bool_iter = literal2bool.find(literal);

    if (literal2bool_iter == literal2bool.end())
        return false;

    return literal2bool_iter->second;
}


//! \brief Determines whether this GraphFormula satisfies the given
//!        GraphFormulaAssignment. Equivalent to value().
bool GraphFormula::satisfies(const GraphFormulaAssignment& assignment) const {
    return value(assignment);
}


//! \brief Placeholder for a virtual function
void GraphFormula::removeLiteral(const std::string&) {
}


//! \brief Placeholder for a virtual function
void GraphFormula::removeLiteralForReal(const std::string&) {
}


//! \brief DESCRIPTION
//! \return DESCRIPTION
int GraphFormula::getSubFormulaSize() const {
    // ich habe keine ahnung, warum diese funktion existiert (peter)
    // ich auch nicht (martin)
    assert(false);
    return 0;
}


//! \brief Placeholder for a virtual function
void GraphFormula::removeLiteralByHiding(const std::string&) {
}

//! \brief computes and returns the value of the formula without an assigment
//! \return returns unknown, true or false
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


//! \brief Returns this formula in conjunctive normal form. The caller is
//!        responsible for deleting the newly created and returned formula.
/// \return returns the CNF
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

//! \brief basic constructor
GraphFormulaMultiary::GraphFormulaMultiary() :
    GraphFormula() {
}


//! \brief Constructs a multiary formula from a given subformula by adding the
//!        subformula into the set of subformulae.
//! \param newformula The subformula to be added
GraphFormulaMultiary::GraphFormulaMultiary(GraphFormula* newformula) {
    subFormulas.push_back(newformula);
}

//! \brief Constructs a multiary formula from two given subformulas. This
//!        constructor exists to conviently construct binary formulas from a left
//!        hand side and a right hand side subformula.
//! \param lhs Left hand side subformula of the to be constructed binary
//!        formula.
//! \param rhs Reft hand side subformula of the to be constructed binary
//!        formula.
GraphFormulaMultiary::GraphFormulaMultiary(GraphFormula* lhs, GraphFormula* rhs) {
    subFormulas.push_back(lhs);
    subFormulas.push_back(rhs);
}


//! \brief deconstructor
GraphFormulaMultiary::~GraphFormulaMultiary() {
    trace(TRACE_5, "GraphFormulaMultiary::~GraphFormulaMultiary() : start\n");

    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
         currentFormula != subFormulas.end(); ++currentFormula) {
        delete *currentFormula;
    }
    trace(TRACE_5, "GraphFormulaMultiary::~GraphFormulaMultiary() : end\n");
}


//! \brief turns the formula into a string
//! \return the formula as string
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


//! \brief computes and returns the value of this formula under the given
//!        assignment
//! \param assignment an assignment for the formulas literals
//! \return true if the formula is true under the assginment, else false
bool GraphFormulaMultiary::value(const GraphFormulaAssignment& assignment) const {
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
void GraphFormulaMultiary::addSubFormula(GraphFormula* subformula) {
    subFormulas.push_back(subformula);
}


//! \brief removes the subformula at the given iterator position
//! \param subformula iterator at the position to be deleted
//! \return returns an iterator to the formula following the erased one
GraphFormulaMultiary::iterator
GraphFormulaMultiary::removeSubFormula(iterator subformula) {
    return subFormulas.erase(subformula);
}


//! \brief removes the given literal from the formula
//! \param name the name of the literal to be removed
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


//! \brief returns the number of subformulas
//! \return number of subformulas
int GraphFormulaMultiary::getSubFormulaSize() const {
	return subFormulas.size();
}


//! \brief removes the given literal from the formula, if this literal is the 
//!        only one of a clause, the clause gets removed as well
//! \param name name of the literal to be removed 
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


//! \brief removes the given literal from the formula by hiding it
//! \param name name of the literal to be removed 
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


//! \brief deep copies the private members of this formula to a given one
//! \param newFormula formulamultiary to be copied to
void GraphFormulaMultiary::deepCopyMultiaryPrivateMembersToNewFormula(GraphFormulaMultiary* newFormula) const {
    newFormula->subFormulas.clear();
    for (subFormulas_t::const_iterator iFormula = subFormulas.begin();
         iFormula != subFormulas.end(); ++iFormula) {
        newFormula->subFormulas.push_back((*iFormula)->getDeepCopy());
    }
}

//! \brief returns an iterator to the begin of the subformula list
//! \return iterator to the begin of the subformula list
GraphFormulaMultiary::iterator GraphFormulaMultiary::begin() {
    return subFormulas.begin();
}


//! \brief returns a const iterator to the begin of the subformula list
//! \return const iterator to the begin of the subformula list
GraphFormulaMultiary::const_iterator GraphFormulaMultiary::begin() const {
    return subFormulas.begin();
}


//! \brief returns an iterator to the end of the subformula list
//! \return iterator to the end of the subformula list
GraphFormulaMultiary::iterator GraphFormulaMultiary::end() {
    return subFormulas.end();
}


//! \brief returns a const iterator to the end of the subformula list
//! \return iterator to the begin of the subformula list
GraphFormulaMultiary::const_iterator GraphFormulaMultiary::end() const {
    return subFormulas.end();
}


//! \brief checks whether there are no subformulas
//! \return returns true if there are no subformulas
bool GraphFormulaMultiary::empty() const {
    return subFormulas.empty();
}


//! \brief returns the number of 
//! \return iterator to the begin of the subformula list
int GraphFormulaMultiary::size() const {
    return subFormulas.size();
}


//! \brief basic constructor 
GraphFormulaMultiaryAnd::GraphFormulaMultiaryAnd() {
}


//! \brief Constructs a multiary and formula from a given subformula by adding the
//!        subformula into the set of subformulae.
//! \param formula The subformula to be added
GraphFormulaMultiaryAnd::GraphFormulaMultiaryAnd(GraphFormula* formula) :
    GraphFormulaMultiary(formula) {
}


//! \brief Constructs a multiary and formula from two given subformulas. This
//!        constructor exists to conviently construct binary formulas from a left
//!        hand side and a right hand side subformula.
//! \param lhs Left hand side subformula of the to be constructed binary
//!        formula.
//! \param rhs Reft hand side subformula of the to be constructed binary
//!        formula.
GraphFormulaMultiaryAnd::GraphFormulaMultiaryAnd(GraphFormula* lhs_,
                                                 GraphFormula* rhs_) :
    GraphFormulaMultiary(lhs_, rhs_) {
}


//! \brief Returns the merged equivalent to this formula. Merging gets rid of
//!        unnecessary nesting of subformulas. (a*(b*c)) becomes (a*b*c). The
//!        caller is responsible for deleting the returned newly created formula.
//! \return returns the merged equivalent
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


//! \brief deep copies this formula
//! \return returns copy
GraphFormulaMultiaryAnd* GraphFormulaMultiaryAnd::getDeepCopy() const {
    GraphFormulaMultiaryAnd* newFormula = new GraphFormulaMultiaryAnd(*this);

    deepCopyMultiaryPrivateMembersToNewFormula(newFormula);

    return newFormula;
}


//! \brief returns the "and"-operator specifying the virtual function
//! \return returns "*"
std::string GraphFormulaMultiaryAnd::getOperator() const {
    return "*";
}


//! \brief return an empty formula Equivalent as true
//! \return true empty formula Equivalent
const GraphFormulaFixed GraphFormulaMultiaryAnd::emptyFormulaEquivalent = GraphFormulaTrue();

//! \brief return an empty formula Equivalent
//! \return empty formula Equivalent
const GraphFormulaFixed& GraphFormulaMultiaryAnd::getEmptyFormulaEquivalent() const {
    return emptyFormulaEquivalent;
}

//! \brief basic constructor 
GraphFormulaMultiaryOr::GraphFormulaMultiaryOr() {
}


//! \brief Constructs a multiary or formula from a given subformula by adding the
//!        subformula into the set of subformulae.
//! \param formula The subformula to be added
GraphFormulaMultiaryOr::GraphFormulaMultiaryOr(GraphFormula* formula) :
    GraphFormulaMultiary(formula) {
}


//! \brief Constructs a multiary or formula from two given subformulas. This
//!        constructor exists to conviently construct binary formulas from a left
//!        hand side and a right hand side subformula.
//! \param lhs Left hand side subformula of the to be constructed binary
//!        formula.
//! \param rhs Reft hand side subformula of the to be constructed binary
//!        formula.
GraphFormulaMultiaryOr::GraphFormulaMultiaryOr(GraphFormula* lhs_,
                                               GraphFormula* rhs_) :
    GraphFormulaMultiary(lhs_, rhs_) {
}


//! \brief returns the "or"-operator specifying the virtual function
//! \return returns "+"
std::string GraphFormulaMultiaryOr::getOperator() const {
    return "+";
}


//! \brief deep copies this formula
//! \return returns copy
GraphFormulaMultiaryOr* GraphFormulaMultiaryOr::getDeepCopy() const {
    GraphFormulaMultiaryOr* newFormula =new GraphFormulaMultiaryOr(*this);

    deepCopyMultiaryPrivateMembersToNewFormula(newFormula);

    return newFormula;
}


//! \brief Returns the merged equivalent to this formula. Merging gets rid of
//!        unnecessary nesting of subformulas. (a+(b+c)) becomes (a+b+c). The
//!        caller is responsible for deleting the returned newly created formula.
//! \return returns the merged equivalent
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


//! \brief tests if a given formula is already implied in this one,
//!        shall only be used for clauses within cnf!
//! \param op the formula that could be implied
//! \return returns true if the given formula is implied, else false
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


//! \brief return an empty formula Equivalent as false
//! \return true empty formula Equivalent
const GraphFormulaFixed GraphFormulaMultiaryOr::emptyFormulaEquivalent = GraphFormulaFalse();


//! \brief return an empty formula Equivalent
//! \return empty formula Equivalent
const GraphFormulaFixed& GraphFormulaMultiaryOr::getEmptyFormulaEquivalent() const {
    return emptyFormulaEquivalent;
}


//! \brief basic constructor 
GraphFormulaCNF::GraphFormulaCNF() {
}


//! \brief Constructs a CNF formula containing a clause
//! \param clause_ clause to be added 
GraphFormulaCNF::GraphFormulaCNF(GraphFormulaMultiaryOr* clause_) :
    GraphFormulaMultiaryAnd(clause_) {
}


//! \brief Constructs a CNF formula containing the two given clauses
//! \param clause1_ first clause
//! \param clause2_ second clause
GraphFormulaCNF::GraphFormulaCNF(GraphFormulaMultiaryOr* clause1_,
                                 GraphFormulaMultiaryOr* clause2_) :
    GraphFormulaMultiaryAnd(clause1_, clause2_) {
}


//! \brief deep copies this formula
//! \return returns copy
GraphFormulaCNF* GraphFormulaCNF::getDeepCopy() const {
    GraphFormulaMultiaryAnd* newFormula =GraphFormulaMultiaryAnd::getDeepCopy();

    return static_cast<GraphFormulaCNF*>(newFormula);
}


//! \brief adds a clause to the CNF
//! \param clause clause to be addded
void GraphFormulaCNF::addClause(GraphFormulaMultiaryOr* clause) {
    addSubFormula(clause);
}


//! \brief checks whether the current CNF formula implies the given one via op
//! \param op the rhs of the implication
//! \fn bool GraphFormulaCNF::implies(GraphFormulaCNF *op)
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


//! \brief Simplifies the formula by removing redundant clauses.
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


//! \brief Creates a formula with a given fixed value and string reprensentation.
//! \param value Fixed truth value of this formula.
//! \param asString String representation of this formula.
GraphFormulaFixed::GraphFormulaFixed(bool value, const std::string& asString) :
    GraphFormulaLiteral(asString), _value(value) {
}


//! \brief returns the fixed value of the formula
//! \param GraphFormulaAssignment An assignment to keep the structure of value
//! \return returns the fixed value
bool GraphFormulaFixed::value(const GraphFormulaAssignment&) const {
    return _value;
}


//! \brief deep copies this formula
//! \return returns copy
GraphFormulaFixed* GraphFormulaFixed::getDeepCopy() const {
    return new GraphFormulaFixed(*this);
}


//! \brief constructs the constant true formula
GraphFormulaTrue::GraphFormulaTrue() :
    GraphFormulaFixed(true, GraphFormulaLiteral::TRUE) {
}


//! \brief constructs the constant false formula
GraphFormulaFalse::GraphFormulaFalse() :
    GraphFormulaFixed(false, GraphFormulaLiteral::FALSE) {
}


//! \brief constructs an atomic literal formula
//! \param literal_ name of the literal
GraphFormulaLiteral::GraphFormulaLiteral(const std::string& literal_) :
    literal(literal_) {
}


//! \brief deep copies this formula
//! \return returns copy
GraphFormulaLiteral* GraphFormulaLiteral::getDeepCopy() const {
    return new GraphFormulaLiteral(*this);
}


//! \brief returns the value of this literal under the given assignment
//! \param assignment an assignment
//! \return true if the literal is true under the assginment, else false
bool GraphFormulaLiteral::value(const GraphFormulaAssignment& assignment) const {
    return assignment.get(literal);
}


//! \brief returns the name of the literal
//! \return name of the literal
std::string GraphFormulaLiteral::asString() const {
    return literal;
}


//! \brief basic constructor
GraphFormulaLiteralFinal::GraphFormulaLiteralFinal() :
    GraphFormulaLiteral(GraphFormulaLiteral::FINAL) {
}


//! \brief basic constructor
GraphFormulaLiteralTau::GraphFormulaLiteralTau() :
    GraphFormulaLiteral(GraphFormulaLiteral::TAU) {
}
