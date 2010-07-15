/*****************************************************************************\
 Rachel -- Reparing Service Choreographies

 Copyright (c) 2008, 2009 Niels Lohmann

 Rachel is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Rachel.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef __FORMULA_H
#define __FORMULA_H

#include <set>
#include <string>
#include "types.h"


/***********
 * classes *
 ***********/

/// a formula to be attached to an OG node
class Formula {
    public:
        /// returns a Graphviz doz representation of the formula
        virtual std::string toDot(bool = true) const = 0;

        /// returns true iff given label set satisfies the formula
        virtual bool sat(const std::set<Label>&) const = 0;

        /// destructor
        virtual ~Formula() {}
};


/// a formula to express a conjunction of two other formulae
class FormulaAND : public Formula {
    private:
        Formula* left;   ///< left sub-formula
        Formula* right;  ///< right sub-formula

    public:
        FormulaAND(Formula*, Formula*);
        std::string toDot(bool = true) const;
        bool sat(const std::set<Label>&) const;
};


/// a formula to express a disjunction of two other formulae
class FormulaOR : public Formula {
    private:
        Formula* left;   ///< left sub-formula
        Formula* right;  ///< right sub-formula

    public:
        FormulaOR(Formula*, Formula*);
        std::string toDot(bool = true) const;
        bool sat(const std::set<Label>&) const;
};


/// a formula to express a literal
class FormulaLit : public Formula {
    private:
        Label literal;  ///< the lable the litaral consists

    public:
        explicit FormulaLit(const Label&);
        std::string toDot(bool = true) const;
        bool sat(const std::set<Label>&) const;
};


/// a formula to express truth
class FormulaTrue : public Formula {
    public:
        std::string toDot(bool = true) const;
        bool sat(const std::set<Label>&) const;
};


/// a formula to express falsity
class FormulaFalse : public Formula {
    public:
        std::string toDot(bool = true) const;
        bool sat(const std::set<Label>&) const;
};


/// a formula to express the final predicate
class FormulaFinal : public Formula {
    public:
        std::string toDot(bool = true) const;
        bool sat(const std::set<Label>&) const;
};


#endif
