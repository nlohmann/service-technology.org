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
private:
	/// measures the time needed by minisat
	static double full_time;
	vector<vector<int> > clausesVector;

public:

	/// type of the formula
	FormulaType formulaType;

	/// returns true if the formula is satisfiable; returns false otherwise
	bool isSatisfiable();
	list<Clause> calculateCNF();

	Formula();
	Formula (const Formula & formula);
	virtual Formula * getCopy() const = 0;
	//virtual Formula& operator = (const Formula &formula);

	/// destructor
	//virtual ~Formula() {};
	virtual ~Formula();

	/// returns a string representation of the formula
	virtual string toString() const = 0;

	/// moves all negations to the inner of the formula
	virtual Formula* moveNegation(bool leadingNot = false) const = 0;

	/// transforms an formula into conjunctive normal form (CNF). Assumption: all negations were already moved to the inner before.
	/// returns a list of clauses
	virtual list<Clause> toCNF( int varId,  int& max) const = 0;

	///return the time needed by minisat
	static double getMinisatTime();
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
	string toString() const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
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
	string toString() const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
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
	string toString() const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
};

/// a formula to express a string literal
class FormulaLit :public Formula {
private:
	int number; ///< the number of the literal

public:
	FormulaLit(const int _number);
	FormulaLit(const FormulaLit &formula);
	~FormulaLit(){};
	virtual Formula * getCopy() const;
	string toString() const;
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
	~FormulaNUM(){};
	virtual Formula * getCopy() const;
	virtual string toString() const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
};


/// a formula to express truth
class FormulaTrue :public Formula {
public:
	FormulaTrue();
	~FormulaTrue(){};
	virtual Formula * getCopy() const;
	string toString() const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
};

/// a formula to express falsity
class FormulaFalse :public Formula {
protected:
public:
	FormulaFalse();
	~FormulaFalse(){};
	virtual Formula * getCopy() const;
	string toString() const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
};

/// a formula to express the final predicate
class FormulaFinal : public Formula {
public:
	FormulaFinal();
	~FormulaFinal(){};
	virtual Formula * getCopy() const;
	string toString() const;
	Formula* moveNegation(bool leadingNot) const;
	list<Clause> toCNF(int varId, int& max) const;
};


#endif
