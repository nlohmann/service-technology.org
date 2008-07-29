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

bool FormulaFinal::sat(const set<string> &l) const {
    return (l.find("FINAL") != l.end());
}
