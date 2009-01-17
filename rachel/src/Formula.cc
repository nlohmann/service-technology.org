/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008, 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
 Rachel is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Rachel (see file COPYING); if not, see http://www.gnu.org/licenses or write to
 the Free Software Foundation,Inc., 51 Franklin Street, Fifth
 Floor, Boston, MA 02110-1301  USA.
\*****************************************************************************/

#include <cassert>
#include <set>

#include "config.h"
#include "Formula.h"

/*
 * constructors
 */
FormulaBits::FormulaBits(bool S, bool F, bool S_1, bool S_2, bool F_prime) :
    S(S), F(F), S_1(S_1), S_2(S_2), F_prime(F_prime)
{}


FormulaAND::FormulaAND(Formula *_left, Formula *_right) : 
    left(_left), right(_right)
{
    assert(_left != NULL);
    assert(_right != NULL);
}

FormulaOR::FormulaOR(Formula *_left, Formula *_right) :
    left(_left), right(_right)
{    
    assert(_left != NULL);
    assert(_right != NULL);
}

FormulaLit::FormulaLit(const Label _literal) :
    literal(_literal)
{
}


/*
 * string output
 */

std::string FormulaAND::toString() const {
    return "(" + left->toString() + " AND " + right->toString() + ")";
}

std::string FormulaOR::toString() const {
    return "(" + left->toString() + " OR " + right->toString() + ")";
}

std::string FormulaLit::toString() const {
    return literal;
}

std::string FormulaTrue::toString() const {
    return "TRUE";
}

std::string FormulaFalse::toString() const {
    return "FALSE";
}

std::string FormulaFinal::toString() const {
    return "FINAL";
}


/*
 * Dot output
 */

std::string FormulaAND::toDot(bool noBrackets) const {
    if (noBrackets)
        return left->toDot(false) + "<FONT> &and; </FONT>" + right->toDot(false);
    else
        return "(" + left->toDot(false) + "<FONT> &and; </FONT>" + right->toDot(false) + ")";
}

std::string FormulaOR::toDot(bool noBrackets) const {
    if (noBrackets)
        return left->toDot(false) + "<FONT> &or; </FONT>" + right->toDot(false);
    else
        return "(" + left->toDot(false) + "<FONT> &or; </FONT>" + right->toDot(false) + ")";
}

std::string FormulaLit::toDot(bool noBrackets) const {
    return literal;
}

std::string FormulaTrue::toDot(bool noBrackets) const {
    return "true";
}

std::string FormulaFalse::toDot(bool noBrackets) const {
    return "false";
}

std::string FormulaFinal::toDot(bool noBrackets) const {
    return "final";
}


/*
 * satisfaction test
 */

bool FormulaAND::sat(const std::set<Label> &l) const {
    return (left->sat(l) && right->sat(l));
}

bool FormulaOR::sat(const std::set<Label> &l) const {
    return (left->sat(l) || right->sat(l));
}

bool FormulaLit::sat(const std::set<Label> &l) const {
    return (l.find(literal) != l.end());
}

bool FormulaTrue::sat(const std::set<Label> &l) const {
    return true;
}

bool FormulaFalse::sat(const std::set<Label> &l) const {
    return false;
}


/*!
 * When calculating the correcting matching between a service automaton and an
 * OG, we require the service automaton's final states to be sink states, i.e.
 * have no successors (see BPM2008 paper). Therefore, evaulation of formulas of
 * nodes in which "final" occurred were not necessary, because in that case, a
 * further correction of the automaton would be suboptimal (because adding more
 * nodes beyond that point would increase the edit distance and will never be
 * necessary due to the requirement that final states are sink states). 
 * 
 * When counting the services characterizes by an OG, however, the evaluation
 * for formulas in which "final" occurs is important. Then, we can set the
 * predicate "final" to true, because it does not change the number of
 * characterized services, but only whether the characterized services have a
 * final state or not.
 *
 * This bug was found by Martin Znamirowski. See <http://gna.org/bugs/?11944>.
 */
bool FormulaFinal::sat(const std::set<Label> &l) const {
    return true; // true by default
}


/*
 * test if formula contains final
 */

bool FormulaAND::hasFinal() const {
    return (left->hasFinal() || right->hasFinal()); // sic!
}

bool FormulaOR::hasFinal() const {
    return (left->hasFinal() || right->hasFinal());
}

bool FormulaLit::hasFinal() const {
    return false;
}

bool FormulaTrue::hasFinal() const { // sic!
    return false;
}

bool FormulaFalse::hasFinal() const {
    return false;
}

bool FormulaFinal::hasFinal() const {
    return true;
}
