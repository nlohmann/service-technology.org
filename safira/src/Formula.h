#ifndef _FORMULA_H
#define _FORMULA_H

#include <string>
#include <vector>
#include <set>
#include <list>
#include "types.h"

using namespace std;

/// a formula to be attached to a node
class Formula {
public:

	/// type of the formula
	FormulaType formulaType;

	/// returns true if the formula is satisfiable; returns false otherwise
	/// fVar: number of Variables in the formula to be checked
	bool isSatisfiable(int fVar);

	/// destructor
	//virtual ~Formula() {};
	virtual ~Formula();

	/// returns a string representation of the formula
	virtual string toString() const = 0;

	/// returns a string representation of the formula as conjunction of (x <-> a OP b)-expressions
	virtual string toStringCNF( int varId = 0,  int max = 0) const = 0;

	/// moves all negations to the inner of the formula
	virtual Formula* moveNegation(bool leadingNot = false) const = 0;

	/// transforms an formula into conjunctive normal form (CNF). Assumption: all negations were already moved to the inner before.
	/// returns a list of clauses
	virtual list<Clause> toCNF( int varId = 1,  int max = 1) const = 0;
};


/// a formula to express a conjunction of two other formulae
class FormulaAND :  public Formula {
private:
	const Formula *left;  ///< left sub-formula
	const Formula *right; ///< right sub-formula

public:
	FormulaAND(const Formula *left, const Formula *right);
	~FormulaAND();
	string toString() const;
	string toStringCNF(int varId, int max) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int max) const;
};

/// a formula to express a disjunction of two other formulae
class FormulaOR : public Formula {
private:
	const Formula *left;  ///< left sub-formula
	const Formula *right; ///< right sub-formula


public:
	FormulaOR(const Formula *left, const Formula *right);
	~FormulaOR();
	string toString() const;
	string toStringCNF(int varId, int max) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int max) const;
};

/// a formula to express a negation of another formula
class FormulaNOT :public Formula {
private:
	const Formula *f; ///< sub-formula


public:
	FormulaNOT(const Formula *f);
	~FormulaNOT();
	string toString() const;
	string toStringCNF(int varId, int max) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int max) const;
};

/// a formula to express a string literal
class FormulaLit :public Formula {
private:
	int number; ///< the number of the literal

public:
	FormulaLit(const int _number);
	~FormulaLit(){};
	string toString() const;
	string toStringCNF(int varId, int max) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int max) const;
};


/// a formula to express a number literal
class FormulaNUM :public Formula {
private:
	int number; ///< the number of the literal

public:
	FormulaNUM(const int literal);
	~FormulaNUM(){};
	string toString() const;
	string toStringCNF(int varId, int max) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int max) const;
};


/// a formula to express truth
class FormulaTrue :public Formula {
public:
	FormulaTrue();
	~FormulaTrue(){};
	string toString() const;
	string toStringCNF(int varId, int max) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int max) const;
};

/// a formula to express falsity
class FormulaFalse :public Formula {
protected:
public:
	FormulaFalse();
	~FormulaFalse(){};
	string toString() const;
	string toStringCNF(int varId, int max) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int max) const;
};

/// a formula to express the final predicate
class FormulaFinal : public Formula {
public:
	FormulaFinal();
	~FormulaFinal(){};
	string toString() const;
	string toStringCNF(int varId, int max) const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int max) const;
};


#endif
