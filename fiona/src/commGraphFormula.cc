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
#include <cassert>

const std::string CommGraphFormulaAssignment::TAU = std::string("tau");
const std::string CommGraphFormulaAssignment::FINAL = std::string("final");

void CommGraphFormulaAssignment::set(const std::string& label, bool value) {
    label2bool[label] = value;
}

void CommGraphFormulaAssignment::setToTrue(const std::string& label) {
    set(label, true);
}

bool CommGraphFormulaAssignment::get(const std::string& label) const {
    label2bool_t::const_iterator label2bool_iter = label2bool.find(label);

    if (label2bool_iter == label2bool.end())
        return false;

    return label2bool_iter->second;
}

bool CommGraphFormula::satisfies(const CommGraphFormulaAssignment& assignment) const {
    return value(assignment) == true;
}

CommGraphFormulaMultiary::CommGraphFormulaMultiary(CommGraphFormula* lhs,
    CommGraphFormula* rhs)
{
    subFormulas.insert(lhs);
    subFormulas.insert(rhs);
}

CommGraphFormulaMultiary::~CommGraphFormulaMultiary()
{
    for (subFormulas_t::const_iterator currentFormula = subFormulas.begin();
        currentFormula != subFormulas.end(); ++currentFormula)
    {
        delete *currentFormula;
    }
}

std::string CommGraphFormulaMultiary::asString() const
{
    if (subFormulas.empty())
        return getEmptyFormulaEquivalent().asString();

    subFormulas_t::const_iterator currentFormula = subFormulas.begin();
    std::string formulaString = '(' + (*currentFormula)->asString();

    while (++currentFormula != subFormulas.end())
    {
        formulaString += getOperator() + " " +
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

CommGraphFormulaMultiaryAnd::CommGraphFormulaMultiaryAnd(
    CommGraphFormula* lhs_, CommGraphFormula* rhs_) :
    CommGraphFormulaMultiary(lhs_, rhs_)
{
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

CommGraphFormulaProposition::CommGraphFormulaProposition(
    const std::string& proposition_) : proposition(proposition_)
{
}

bool CommGraphFormulaProposition::value(
    const CommGraphFormulaAssignment& assignment) const
{
    return assignment.get(proposition);
}

std::string CommGraphFormulaProposition::asString() const
{
    return proposition;
}
