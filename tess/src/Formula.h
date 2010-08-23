/*****************************************************************************\
 Tess -- Selecting Test Cases for Services

 Copyright (c) 2010 Kathrin Kaschner

 Tess is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Tess is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Tess.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef _FORMULA_H
#define _FORMULA_H

#include <string>
#include <list>
#include <map>
#include <list>
#include <set>
#include "types.h"
//#include "NumPrinter.h"

using namespace std;

/// a formula to be attached to a node
class Formula {

public:

	/// type of the formula
	FormulaType formulaType;

	/// Constructors
	Formula();

	/// destructor
	virtual ~Formula();

	/// returns a string representation of the formula
	virtual string toString() const = 0;

	/// Returns the disjunctive normal form of this formula
	virtual list< set<int> > toDnf () = 0;

	/// Returns the minimal disjunctive normal form of this formula
	virtual list< set<int> > toMinimalDnf() = 0;

	/// makes the formula flat
	virtual void merge ();

	/// list of sub-formulas of the flatted formula
	list <Formula*> formulas;



protected:
	bool merged;

};


/// a formula to express a conjunction of two other formulae
class FormulaAND :  public Formula {
protected:
	Formula *left;  ///< left sub-formula
	Formula *right; ///< right sub-formula

	list< set<int> > combine(list< set<int> > firstDnf, list< set<int> > lastDnf);

public:
	FormulaAND(Formula *left, Formula *right);
	~FormulaAND();
	virtual string toString() const;

	/// Returns the disjunctive normal form of this formula
	virtual list< set<int> > toDnf ();

	/// Returns the minimal disjunctive normal form of this formula
	virtual list< set<int> > toMinimalDnf();

	/// makes the formula flat
	void merge ();
};


/// a formula to express a disjunction of two other formulae
class FormulaOR : public Formula {
protected:
	Formula *left;  ///< left sub-formula
	Formula *right; ///< right sub-formula

public:
	FormulaOR(Formula *left, Formula *right);
	~FormulaOR();
	virtual string toString() const;

	/// Returns the disjunctive normal form of this formula
	virtual list< set<int> > toDnf ();

	/// Returns the minimal disjunctive normal form of this formula
	virtual list< set<int> > toMinimalDnf();

	/// makes the formula flat
	void merge();
};


/// a formula to express a string literal
class FormulaLit :public Formula {
protected:
	int id; ///< the number of the literal

public:
	FormulaLit(const int _number);
	~FormulaLit(){}
	virtual string toString() const;

	/// Returns the disjunctive normal form of this formula
	virtual list< set<int> > toDnf ();

	/// Returns the minimal disjunctive normal form of this formula
	virtual list< set<int> > toMinimalDnf();
};


/// a formula to express a number literal
class FormulaNUM :public Formula {
protected:
	int id; ///< the number of the literal

public:
	FormulaNUM(const int literal);
	~FormulaNUM(){}
	virtual string toString() const;

	/// Returns the disjunctive normal form of this formula
	virtual list< set<int> > toDnf ();

	/// Returns the minimal disjunctive normal form of this formula
	virtual list< set<int> > toMinimalDnf();
};


/// a formula to express truth
class FormulaTrue :public Formula {
protected:
	int id;

public:
	FormulaTrue();
	~FormulaTrue(){}
	virtual string toString() const;

	/// Returns the disjunctive normal form of this formula
	virtual list< set<int> > toDnf ();

	/// Returns the minimal disjunctive normal form of this formula
	virtual list< set<int> > toMinimalDnf();
};



/// a formula to express the final predicate
class FormulaFinal : public Formula {
protected:
	int id;

public:
	FormulaFinal();
	~FormulaFinal(){}
	virtual string toString() const;

	/// Returns the disjunctive normal form of this formula
	virtual list< set<int> > toDnf ();

	/// Returns the minimal disjunctive normal form of this formula
	virtual list< set<int> > toMinimalDnf();
};


#endif
