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

const std::string OGFromFileFormulaAssignment::TAU = std::string("tau");
const std::string OGFromFileFormulaAssignment::FINAL = std::string("final");

void OGFromFileFormulaAssignment::set(const std::string& label, bool value) {
    label2bool[label] = value;
}

void OGFromFileFormulaAssignment::setToTrue(const std::string& label) {
    set(label, true);
}

bool OGFromFileFormulaAssignment::get(const std::string& label) const {
    label2bool_t::const_iterator label2bool_iter = label2bool.find(label);

    if (label2bool_iter == label2bool.end())
        return false;

    return label2bool_iter->second;
}

bool OGFromFileFormula::satisfies(const OGFromFileFormulaAssignment& assignment) const {
    return value(assignment) == true;
}

OGFromFileFormulaBinary::OGFromFileFormulaBinary(OGFromFileFormula* lhs_,
    OGFromFileFormula* rhs_) : lhs(lhs_), rhs(rhs_)
{
}

OGFromFileFormulaBinary::~OGFromFileFormulaBinary()
{
    delete lhs;
    delete rhs;
}

bool OGFromFileFormulaBinary::lhsValue(
    const OGFromFileFormulaAssignment& assignment) const
{
    return lhs->value(assignment);
}

bool OGFromFileFormulaBinary::rhsValue(
    const OGFromFileFormulaAssignment& assignment) const
{
    return rhs->value(assignment);
}

std::string OGFromFileFormulaBinary::asString() const
{
    return "(" + lhs->asString() + " " + getOperator() + " " +
        rhs->asString() + ")";
}

OGFromFileFormulaBinaryAnd::OGFromFileFormulaBinaryAnd(OGFromFileFormula* lhs_,
    OGFromFileFormula* rhs_) : OGFromFileFormulaBinary(lhs_, rhs_)
{
}

bool OGFromFileFormulaBinaryAnd::value(
    const OGFromFileFormulaAssignment& assignment) const
{
    if (!lhsValue(assignment))
        return false;

    return rhsValue(assignment);
}

std::string OGFromFileFormulaBinaryAnd::getOperator() const
{
    return "*";
}

OGFromFileFormulaBinaryOr::OGFromFileFormulaBinaryOr(OGFromFileFormula* lhs_,
    OGFromFileFormula* rhs_) : OGFromFileFormulaBinary(lhs_, rhs_)
{
}

bool OGFromFileFormulaBinaryOr::value(
    const OGFromFileFormulaAssignment& assignment) const
{
    if (lhsValue(assignment))
        return true;

    return rhsValue(assignment);
}

std::string OGFromFileFormulaBinaryOr::getOperator() const
{
    return "+";
}

bool OGFromFileFormulaTrue::value(const OGFromFileFormulaAssignment&) const
{
    return true;
}

std::string OGFromFileFormulaTrue::asString() const
{
    return "true";
}

bool OGFromFileFormulaFalse::value(const OGFromFileFormulaAssignment&) const
{
    return false;
}

std::string OGFromFileFormulaFalse::asString() const
{
    return "false";
}

OGFromFileFormulaProposition::OGFromFileFormulaProposition(
    const std::string& proposition_) : proposition(proposition_)
{
}

bool OGFromFileFormulaProposition::value(
    const OGFromFileFormulaAssignment& assignment) const
{
    return assignment.get(proposition);
}

std::string OGFromFileFormulaProposition::asString() const
{
    return proposition;
}
