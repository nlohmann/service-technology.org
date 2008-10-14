/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008  Niels Lohmann <niels.lohmann@uni-rostock.de>
 
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

#include "config.h"
#include "Formula.h"

/*
 * constructors
 */

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

FormulaLit::FormulaLit(const string _literal) :
    literal(_literal)
{
}


/*
 * string output
 */

string FormulaAND::toString() const {
    return "(" + left->toString() + " AND " + right->toString() + ")";
}

string FormulaOR::toString() const {
    return "(" + left->toString() + " OR " + right->toString() + ")";
}

string FormulaLit::toString() const {
    return literal;
}

string FormulaTrue::toString() const {
    return "TRUE";
}

string FormulaFalse::toString() const {
    return "FALSE";
}

string FormulaFinal::toString() const {
    return "FINAL";
}


/*
 * satisfaction test
 */

bool FormulaAND::sat(const set<string> &l) const {
    return (left->sat(l) && right->sat(l));
}

bool FormulaOR::sat(const set<string> &l) const {
    return (left->sat(l) || right->sat(l));
}

bool FormulaLit::sat(const set<string> &l) const {
    return (l.find(literal) != l.end());
}

bool FormulaTrue::sat(const set<string> &l) const {
    return true;
}

bool FormulaFalse::sat(const set<string> &l) const {
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
bool FormulaFinal::sat(const set<string> &l) const {
    return true;
}
