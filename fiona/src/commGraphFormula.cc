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

CommGraphFormulaBinary::CommGraphFormulaBinary(CommGraphFormula* lhs_,
    CommGraphFormula* rhs_) : lhs(lhs_), rhs(rhs_)
{
}

CommGraphFormulaBinary::~CommGraphFormulaBinary()
{
    delete lhs;
    delete rhs;
}

bool CommGraphFormulaBinary::lhsValue(
    const CommGraphFormulaAssignment& assignment) const
{
    return lhs->value(assignment);
}

bool CommGraphFormulaBinary::rhsValue(
    const CommGraphFormulaAssignment& assignment) const
{
    return rhs->value(assignment);
}

std::string CommGraphFormulaBinary::asString() const
{
    return "(" + lhs->asString() + " " + getOperator() + " " +
        rhs->asString() + ")";
}

CommGraphFormulaBinaryAnd::CommGraphFormulaBinaryAnd(CommGraphFormula* lhs_,
    CommGraphFormula* rhs_) : CommGraphFormulaBinary(lhs_, rhs_)
{
}

bool CommGraphFormulaBinaryAnd::value(
    const CommGraphFormulaAssignment& assignment) const
{
    if (!lhsValue(assignment))
        return false;

    return rhsValue(assignment);
}

std::string CommGraphFormulaBinaryAnd::getOperator() const
{
    return "*";
}

CommGraphFormulaBinaryOr::CommGraphFormulaBinaryOr(CommGraphFormula* lhs_,
    CommGraphFormula* rhs_) : CommGraphFormulaBinary(lhs_, rhs_)
{
}

bool CommGraphFormulaBinaryOr::value(
    const CommGraphFormulaAssignment& assignment) const
{
    if (lhsValue(assignment))
        return true;

    return rhsValue(assignment);
}

std::string CommGraphFormulaBinaryOr::getOperator() const
{
    return "+";
}

bool CommGraphFormulaTrue::value(const CommGraphFormulaAssignment&) const
{
    return true;
}

std::string CommGraphFormulaTrue::asString() const
{
    return "true";
}

bool CommGraphFormulaFalse::value(const CommGraphFormulaAssignment&) const
{
    return false;
}

std::string CommGraphFormulaFalse::asString() const
{
    return "false";
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
