/*****************************************************************************\
 Safira -- Implementing a Set Algebra for Service Behavior

 Copyright (c) 2010 Kathrin Kaschner

 Safira is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Safira is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Safira.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#ifndef _FORMULA_H
#define _FORMULA_H

#include <string>
#include <list>
#include <map>
#include "types.h"
#include "NumPrinter.h"

using namespace std;

/// a formula to be attached to a node
class Formula {
private:
	/// measures the time needed by minisat
	static double full_time;
	vector<vector<int> > clausesVector;

public:

	/// type of the formula
	FormulaType formulaType;

	Formula();
	Formula (const Formula & formula);
	virtual Formula * getCopy() const = 0;
	//virtual Formula& operator = (const Formula &formula);

	/// destructor
	//virtual ~Formula() {};
	virtual ~Formula();

	/// returns true if the formula is satisfiable; returns false otherwise
	bool isSatisfiable();

	/// returns true if given assignment satify the formula; returns false otherwise
	bool isSatisfiable(vector<bool>* assignment);

	/// returns a satisfying assignment if there is one; returns NULL otherwise
	vector<bool> * getSatisfyingAssignment();

	/// calculates the CNF
	list<Clause> calculateCNF();

	/// returns a string representation of the formula
	string toString() const;

	virtual string toString(NumPrinterBase* printer) const = 0;

	/// moves all negations to the inner of the formula
	virtual Formula* moveNegation(bool leadingNot = false) const = 0;

	/// transforms an formula into conjunctive normal form (CNF). Assumption: all negations were already moved to the inner before.
	/// returns a list of clauses
	virtual list<Clause> toCNF( int varId,  int& max) const = 0;

	///return the time needed by minisat
	static double getMinisatTime();

	virtual const Formula* substitute(map<int,Formula*> lits) const;
};


/// a formula to express a conjunction of two other formulae
class FormulaAND :  public Formula {
private:
	const Formula *left;  ///< left sub-formula
	const Formula *right; ///< right sub-formula

public:
	FormulaAND(const Formula *left, const Formula *right);
	FormulaAND(const FormulaAND &formula);
	virtual Formula * getCopy() const;
//	FormulaAND& operator = (const FormulaAND &formula);
	~FormulaAND();
	virtual string toString(NumPrinterBase* printer) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
	const Formula* substitute(map<int,Formula*> lits) const;
};


/// a formula to express a disjunction of two other formulae
class FormulaOR : public Formula {
private:
	const Formula *left;  ///< left sub-formula
	const Formula *right; ///< right sub-formula

public:
	FormulaOR(const Formula *left, const Formula *right);
	FormulaOR(const FormulaOR &formula);
//	FormulaOR& operator = (const FormulaOR &formula);
	~FormulaOR();
	virtual Formula * getCopy() const;
	virtual string toString(NumPrinterBase* printer) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
	const Formula* substitute(map<int,Formula*> lits) const;
};


/// a formula to express a negation of another formula
class FormulaNOT :public Formula {
private:
	const Formula *f; ///< sub-formula

public:
	FormulaNOT(const Formula *f);
	FormulaNOT(const FormulaNOT &formula);
	//FormulaNOT& operator = (const FormulaNOT &formula);
	~FormulaNOT();
	virtual Formula * getCopy() const;
	virtual string toString(NumPrinterBase* printer) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
	const Formula* substitute(map<int,Formula*> lits) const;
};


/// a formula to express a string literal
class FormulaLit :public Formula {
private:
	int number; ///< the number of the literal

public:
	FormulaLit(const int _number);
	FormulaLit(const FormulaLit &formula);
	~FormulaLit(){}
	virtual Formula * getCopy() const;
	virtual string toString(NumPrinterBase* printer) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
};


/// a formula to express a number literal
class FormulaNUM :public Formula {
private:
	int number; ///< the number of the literal

public:
	FormulaNUM(const int literal);
	FormulaNUM(const FormulaNUM &formula);
	~FormulaNUM(){}
	virtual Formula * getCopy() const;
	virtual string toString(NumPrinterBase* printer) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
	const Formula* substitute(map<int,Formula*> lits) const;
};


/// a formula to express truth
class FormulaTrue :public Formula {
public:
	FormulaTrue();
	~FormulaTrue(){}
	virtual Formula * getCopy() const;
	virtual string toString(NumPrinterBase* printer) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;

};


/// a formula to express falsity
class FormulaFalse :public Formula {
public:
	FormulaFalse();
	~FormulaFalse(){}
	virtual Formula * getCopy() const;
	virtual string toString(NumPrinterBase* printer) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
};


/// a formula to express the final predicate
class FormulaFinal : public Formula {
public:
	FormulaFinal();
	~FormulaFinal(){}
	virtual Formula * getCopy() const;
	virtual string toString(NumPrinterBase* printer) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
};


/// a dummy node in the Formula tree. It avoids that nodes below are deleted by the destructor call
/* a dummy node is introduced if a formula h is generated temporarily by existing formulas f and g
 * the call "delete h" will only delete nodes which are neither part of f nor part of g.
 */
class FormulaDummy : public Formula {
private:
	const Formula *f; ///< sub-formula
public:
	FormulaDummy(const Formula *f);
	FormulaDummy(const FormulaDummy &formula);
	~FormulaDummy();
	virtual Formula * getCopy() const;
	virtual string toString(NumPrinterBase* printer) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
	const Formula* substitute(map<int,Formula*> lits) const;
};

#endif
