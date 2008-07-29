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

#ifndef __FORMULA_H
#define __FORMULA_H


#include <string>
#include <set>
#include <vector>

using std::string;
using std::set;
using std::vector;


class Formula {
    public:
        virtual string toString() const = 0;
        virtual bool sat(const set<string> &l) const = 0;
        virtual ~Formula() {};
};

class FormulaAND : public Formula {
    private:
        Formula *left;
        Formula *right;
    
    public:
        FormulaAND(Formula *left, Formula *right);
        string toString() const;
        bool sat(const set<string> &l) const;
};

class FormulaOR : public Formula {
    private:
        Formula *left;
        Formula *right;
        
    public:
        FormulaOR(Formula *left, Formula *right);
        string toString() const;
        bool sat(const set<string> &l) const;
};

class FormulaLit : public Formula {
    private:
        string literal;
    
    public:    
        FormulaLit(const string literal);
        string toString() const;
        bool sat(const set<string> &l) const;
};

class FormulaTrue : public Formula {
    public:
        string toString() const;
        bool sat(const set<string> &l) const;
};

class FormulaFalse : public Formula {
    public:
        string toString() const;
        bool sat(const set<string> &l) const;
};

class FormulaFinal : public Formula {
    public:
        string toString() const;
        bool sat(const set<string> &l) const;
};


#endif
