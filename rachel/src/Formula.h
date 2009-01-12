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

#ifndef __FORMULA_H
#define __FORMULA_H

#include <set>
#include "types.h"


/***********
 * classes *
 ***********/

/// the implicit formula representation (2 or 3 bits)
class FormulaBits {
    public:
        bool S;         ///< every fulfilling assignment contains send events
        bool F;         ///< final occurs in formula
        bool S_1;       ///< S bit is set, but state has not only send edges
        bool S_2;       ///< no send edges and state is neither in S nor in F
        bool F_prime;   ///< F bis is set, but state has no successors
        
        /// constructor -- standard parameters are used to enable std::map
        FormulaBits(const bool S=false,
                    const bool F=false,
                    const bool S_1=false,
                    const bool S_2=false,
                    const bool F_prime=false);
};


/// a formula to be attached to an OG node
class Formula {
    public:
        /// returns a string representation of the formula
        virtual std::string toString() const = 0;
        
        /// returns a Graphviz doz representation of the formula
        virtual std::string toDot(bool noBrackets = true) const = 0;
        
        /// returns true iff given label set satisfies the formula
        virtual bool sat(const std::set<Label> &l) const = 0;
        
        /// returns true if formula contains a final predicate
        virtual bool hasFinal() const = 0;
        
        /// destructor
        virtual ~Formula() {};
};


/// a formula to express a conjunction of two other formulae
class FormulaAND : public Formula {
    private:
        Formula *left;  ///< left sub-formula
        Formula *right; ///< right sub-formula
    
    public:
        FormulaAND(Formula *left, Formula *right);
        std::string toString() const;
        std::string toDot(bool noBrackets = true) const;
        bool sat(const std::set<Label> &l) const;
        bool hasFinal() const;
};


/// a formula to express a disjunction of two other formulae
class FormulaOR : public Formula {
    private:
        Formula *left;  ///< left sub-formula
        Formula *right; ///< right sub-formula
        
    public:
        FormulaOR(Formula *left, Formula *right);
        std::string toString() const;
        std::string toDot(bool noBrackets = true) const;
        bool sat(const std::set<Label> &l) const;
        bool hasFinal() const;
};


/// a formula to express a literal
class FormulaLit : public Formula {
    private:
        Label literal; ///< the lable the litaral consists

    public:    
        FormulaLit(const Label literal);
        std::string toString() const;
        std::string toDot(bool noBrackets = true) const;
        bool sat(const std::set<Label> &l) const;
        bool hasFinal() const;
};


/// a formula to express truth
class FormulaTrue : public Formula {
    public:
        std::string toString() const;
        std::string toDot(bool noBrackets = true) const;
        bool sat(const std::set<Label> &l) const;
        bool hasFinal() const;
};


/// a formula to express falsity
class FormulaFalse : public Formula {
    public:
        std::string toString() const;
        std::string toDot(bool noBrackets = true) const;
        bool sat(const std::set<Label> &l) const;
        bool hasFinal() const;
};


/// a formula to express the final predicate
class FormulaFinal : public Formula {
    public:
        std::string toString() const;
        std::string toDot(bool noBrackets = true) const;
        bool sat(const std::set<Label> &l) const;
        bool hasFinal() const;
};


#endif
