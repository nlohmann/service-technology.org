#ifndef Formula_H_
#define Formula_H_

#include "settings.h"



// ****************************************************************************
// Assignment
// ****************************************************************************

/**
 * Assignment of literals to truth values to determine the truth value of a
 * Formula under the given assignment.
 */
class FormulaAssignment {
    public:
        /// an assignment is a mapping from literal names to truth values
        typedef map<string, bool> literalValues_t;

    private:
        /// maps literals to their truth values.
        literalValues_t literalValues;

    public:
        /// destroys the given assignment
        virtual ~FormulaAssignment() {};

        /// sets the given literal to the given truth value
        void set(const string& literal, bool value) {
        	literalValues[literal] = value;
        }

        /// returns the bool value of a literal
        bool get(const string& literal) const {
        	literalValues_t::const_iterator i = literalValues.find( literal );
        	return i == literalValues.end() ? false : i->second;
        }
};



// ****************************************************************************
// Formula
// ****************************************************************************

/**
 * A formula to be used as an annotation for a state in an OG or IG. This is an
 * abstract base class for all other formula classes. Use those to construct an
 * actual formula.
 */
class Formula {
    public:
        /// destroys the given formula
        virtual ~Formula() {};

        /// destroys all subformulas of this formula
        virtual void clear() = 0;

        /// returns the number of subformulas
        virtual int size() const = 0;

        /// returns the truth value of this formula under the given assignment
        virtual bool value(const FormulaAssignment& assignment) const = 0;

        /// formats and returns this formula as a string, which is
        /// suitable for showing the user and for the OG output format
        virtual string asString() const = 0;

        /// removes a literal from the formula
        /// if this literal is the only one of a clause, the clause gets
        /// removed as well
        virtual void removeLiteral(const string& literal) = 0;

        /// simplifies the formula
        virtual void simplify() = 0;

        /// returns true iff this formula implies the given formula
        virtual bool implies(Formula* conclusion) const = 0;
};



// ****************************************************************************
// Multiary Formula
// ****************************************************************************

/**
 * Base class for all multiary formula.
 */
class FormulaMultiary : public Formula {
    public:
        /// type of the container holding all subformula of this multiary formula
        typedef list<Formula*> subFormulas_t;

        /// holds all subformulas of this multiary formula
        subFormulas_t subFormulas;

    public:
        /// basic constructor
        FormulaMultiary() : Formula() {};

        /// constructor with one new formula
        FormulaMultiary(Formula* newformula) : Formula() {
            subFormulas.push_back(newformula);
        };

        /// binary constructor
        FormulaMultiary(Formula* lhs, Formula* rhs) : Formula() {
            subFormulas.push_back(lhs);
            subFormulas.push_back(rhs);
        };

        /// destroys this FormulaMultiary
        virtual ~FormulaMultiary() {};

        /// destroys all subformulas of this FormulaMultiary
        virtual void clear() {
            for (subFormulas_t::const_iterator i = subFormulas.begin(); i != subFormulas.end(); ++i) {
                delete *i;
            }
        };

        /// Returns the the number of formulas, the multiary formula consists of.
        virtual int size() const {
            return subFormulas.size();
        };

        /// removes a literal from the formula, if this literal is the only one of a clause,
        /// the clause gets removed as well
        /// this function is used in the IG reduction
        virtual void removeLiteral(const string&);
};


/**
 * A multiary conjunction of \link Formula CommFormulas.\endlink Can
 * have arbitrarily many subformulas, even none.
 */
class FormulaMultiaryAnd : public FormulaMultiary {
    public:
        /// constructors
        FormulaMultiaryAnd() : FormulaMultiary() {};
        FormulaMultiaryAnd(Formula* sub) : FormulaMultiary(sub) {};
        FormulaMultiaryAnd(Formula* lhs, Formula* rhs) : FormulaMultiary(lhs, rhs) {};

        /// destroys this FormulaMultiaryAnd
        virtual ~FormulaMultiaryAnd() {};

        /// returns the value of this formula under given assignment
        virtual bool value(const FormulaAssignment& assignment) const;

        /// returns the formula as a string
        virtual string asString() const;

        /// simplifies the formula
        virtual void simplify();

        /// flattens this multiary formula
        void flatten();

        /// merge this multiary formula
        void merge();

        /// returns true iff this formula implies the given formula
        virtual bool implies(Formula* conclusion) const;
};


/**
 * A multiary disjunction of \link Formula CommFormulas.\endlink Can
 * have arbitrarily many subformulas, even none.
 */
class FormulaMultiaryOr : public FormulaMultiary {
    public:
        /// constructors
        FormulaMultiaryOr() : FormulaMultiary() {};
        FormulaMultiaryOr(Formula* sub) : FormulaMultiary(sub) {};
        FormulaMultiaryOr(Formula* lhs, Formula* rhs) : FormulaMultiary(lhs, rhs) {};

        /// destroys this FormulaMultiaryOr
        virtual ~FormulaMultiaryOr() {};

        /// returns the value of this formula under given assignment
        virtual bool value(const FormulaAssignment& assignment) const;

        /// returns the formula as a string
        virtual string asString() const;

        /// simplifies the formula
        virtual void simplify();

        /// flattens this multiary formula
        void flatten();

        /// merge this multiary formula
		void merge();

		/// returns true iff this formula implies the given formula
        virtual bool implies(Formula* conclusion) const;
};



// ****************************************************************************
// Negation
// ****************************************************************************

/**
 * A negated formula.
 */
class FormulaNegation : public Formula {
    public:

        /// the un-negated subformula
        Formula* subFormula;
    public:

        /// Constructs a literal with the given string representation.
        FormulaNegation(Formula* sub) : Formula() {
            subFormula = sub;
        };

        /// basic deconstructor
		virtual ~FormulaNegation() {};

        /// destroys the subformula of this FormulaNegation
        virtual void clear() {
            delete subFormula;
            subFormula = NULL;
        };

        /// returns the value of the literal in the given asisgnment
        virtual bool value(const FormulaAssignment& assignment) const {
            return not subFormula->value( assignment );
        }

        /// returns the name of the literal
        virtual string asString() const {
            return subFormula != NULL ? "(~" + subFormula->asString() + ")" : "(empty not)";
        }

        /// returns the size of this formula
        virtual int size() const {
            return ( subFormula != NULL ? 1 : 0);
        }

        /// removes a literal from the formula
        virtual void removeLiteral(const string& literal);

        /// simplifies the formula
        virtual void simplify();

        /// flattens this negation formula
        void flatten();

        /// returns true iff this formula implies the given formula
        virtual bool implies(Formula* conclusion) const;
};



// ****************************************************************************
// Literal
// ****************************************************************************

/**
 * A literal within a Formula.
 */
class FormulaLiteral : public Formula {
    private:
        /// The string representation of this literal.
        string _literal;
    public:

        /// Constructs a literal with the given string representation.
        FormulaLiteral(const string& literal) : _literal(literal) {};

        /// basic deconstructor
		virtual ~FormulaLiteral() {};

        /// clear all subformulae (i.e. nothing)
        virtual void clear() {};

        /// returns the size of this formula
        virtual int size() const {
            return 1;
        }

        /// returns the value of the literal in the given asisgnment
        virtual bool value(const FormulaAssignment& assignment) const {
        	return assignment.get(_literal);
        }

        /// returns the name of the literal
        virtual string asString() const {
        	return _literal;
        }

        /// removes a literal from the formula
        /// but we cannot remove ourself
        virtual void removeLiteral(const string& literal) {};

        /// simplifies the formula
        /// but we are a literal and as simple as possible
        virtual void simplify() {};

        /// returns true iff this formula implies the given formula
        virtual bool implies(Formula* conclusion) const;
};



// ****************************************************************************
// Constants
// ****************************************************************************

/**
 * The constant formula 'TRUE'.
 */
class FormulaTrue : public FormulaLiteral {
    public:
        /// basic constructor
        FormulaTrue() : FormulaLiteral("TRUE") {};

        /// basic deconstructor
        virtual ~FormulaTrue() {};

        /// returns a deep copy of this formula
		virtual FormulaTrue* getDeepCopy() const {
			return new FormulaTrue(*this);
		}

		/// returns the prefixed value of this formula
		virtual bool value(const FormulaAssignment& assignment) const {
			return true;
		}
};


/**
 * The constant formula 'FALSE'.
 */
class FormulaFalse : public FormulaLiteral {
    public:
        /// basic constructor
        FormulaFalse() : FormulaLiteral("FALSE") {};

        /// basic deconstructor
        virtual ~FormulaFalse() {};

        /// returns a deep copy of this formula
		virtual FormulaFalse* getDeepCopy() const {
			return new FormulaFalse(*this);
		}

		/// returns the prefixed value of this formula
		virtual bool value(const FormulaAssignment& assignment) const {
			return false;
		}
};


/**
 * A special literal FINAL.
 */
class FormulaFinal : public FormulaLiteral {
    public:
        /// Constructs a literal with the given string representation.
        FormulaFinal() : FormulaLiteral("FINAL") {};

        /// basic deconstructor
        virtual ~FormulaFinal() {};

        /// returns a deep copy of this formula
		virtual FormulaFinal* getDeepCopy() const {
			return new FormulaFinal(*this);
		}

		/// returns the prefixed value of this formula
		virtual bool value(const FormulaAssignment& assignment) const {
			return true;
		}
};

#endif
