/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe              *
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
#include <cassert>

const std::string CommGraphFormulaAssignment::FINAL = std::string("final");
const std::string CommGraphFormulaAssignment::TAU = std::string("tau");
const std::string CommGraphFormulaLiteral::FINAL_L = std::string("final");
const std::string CommGraphFormulaLiteral::TAU_L = std::string("tau");


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


CommGraphFormulaMultiary::CommGraphFormulaMultiary() {
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


CommGraphFormulaMultiaryAnd::CommGraphFormulaMultiaryAnd() {
}


CommGraphFormulaMultiaryAnd::CommGraphFormulaMultiaryAnd(CommGraphFormula* formula) :
   CommGraphFormulaMultiary(formula) {
}


CommGraphFormulaMultiaryAnd::CommGraphFormulaMultiaryAnd(CommGraphFormula* lhs_,
    CommGraphFormula* rhs_) :
    CommGraphFormulaMultiary(lhs_, rhs_) {

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

void CNF_formula::addClause(CommGraphFormulaMultiaryOr* clause) {
	addSubFormula(clause);
}


CommGraphFormulaFixed::CommGraphFormulaFixed(bool value,
    const std::string& asString) :
    _asString(asString),
    _value(value)
{
}

std::string CommGraphFormulaFixed::asString() const
{
    return _asString;
}

bool CommGraphFormulaFixed::value(const CommGraphFormulaAssignment&) const
{
    return _value;
}

CommGraphFormulaTrue::CommGraphFormulaTrue() :
    CommGraphFormulaFixed(true, "true")
{
}

CommGraphFormulaFalse::CommGraphFormulaFalse() :
    CommGraphFormulaFixed(false, "false")
{
}

CommGraphFormulaLiteral::CommGraphFormulaLiteral(const std::string& literal_) :
    literal(literal_)
{
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
    CommGraphFormulaLiteral(CommGraphFormulaLiteral::FINAL_L)
{
}


CommGraphFormulaLiteralTau::CommGraphFormulaLiteralTau() :
    CommGraphFormulaLiteral(CommGraphFormulaLiteral::TAU_L)
{
}

