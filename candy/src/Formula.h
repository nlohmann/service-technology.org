#ifndef Formula_H_
#define Formula_H_

#include "settings.h"
//#include <set>



// ****************************************************************************
// Assignment
// ****************************************************************************

/**
 * Assignment of literals to truth values to determine the truth value of a
 * Formula under the given assignment. Typical literals are edge
 * labels of IGs and OGs.
 */
class FormulaAssignment {
    public:
        typedef map<string, bool> literalValues_t;

    private:
        /// Maps literals to their truth values.
        literalValues_t literalValues;

    public:
        /// sets the given literal to the given truth value
        void set(const string& literal, bool value) {
        	literalValues[literal] = value;
        }

        /// sets the given literal to true
        void setToTrue(const string& literal) {
        	set(literal, true);
        }

        /// sets the given literal to false
        void setToFalse(const string& literal) {
        	set(literal, false);;
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
        /// Destroys the given Formula and all its subformulas.
        virtual ~Formula() {};

        /// copies and returns this Formula
        virtual Formula* getDeepCopy() const = 0;

        /// Returns the truth value of this Formula under the given
        /// FormulaAssignment.
        /// @param assignment The FormulaAssignment under which the truth
        ///  value of this Formula should be computed. The value of
        ///  literals not set in assignment are considered false.
        /// @returns The truth value of this Formula under the given
        ///  FormulaAssignment.
        virtual bool value(const FormulaAssignment& assignment) const = 0;

        /// Formats and returns this formula as a string. This string is
        /// suitable for showing the user and for the OG output format.
        /// returns The string representation for this Formula.
        virtual string asString() const = 0;

        /// returns the number of subformulas
        virtual int size() const = 0;

        /// removes a literal from the formula, if this literal is the only one of a clause,
        /// the clause gets removed as well
        virtual void removeLiteral(const string& literal) {};

        /// returns a simplified version of this formula
        virtual Formula* simplify() = 0;

        virtual void flatten() = 0;
        virtual void merge() = 0;
        virtual void clear() {};
        virtual bool implies(Formula* conclusion) const = 0;

};



// ****************************************************************************
// Multiary Formulas
// ****************************************************************************

/**
 * Base class for all multiary \link Formula
 * CommFormulas\endlink, such as FormulaMultiaryAnd and
 * FormulaMultiaryOr. This class exists because the classes
 * FormulaMultiaryAnd and FormulaMultiaryOr share
 * functionality which should be implemented only once.
 */
class FormulaMultiary : public Formula {
    public:
        /// Type of the container holding all subformula of this multiary formula.
        typedef list<Formula*> subFormulas_t;

    protected:
        /// Holds all subformulas of this multiary formula.
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

        /// Destroys this FormulaMultiary and all its subformulas.
        virtual ~FormulaMultiary() {
            // WARNING remove later to avoid memory leaks
            //for (subFormulas_t::const_iterator i = subFormulas.begin(); i != subFormulas.end(); ++i) {
            //    delete *i;
            //}
        };

        virtual void clear() {
            for (subFormulas_t::const_iterator i = subFormulas.begin(); i != subFormulas.end(); ++i) {
                delete *i;
            }
        };

        /// Returns the the number of formulas, the multiary formula consists of.
        virtual int size() const {
            return subFormulas.size();
        };

        virtual Formula* getFront() {
            return subFormulas.front();
        }

        /// removes a literal from the formula, if this literal is the only one of a clause,
        /// the clause gets removed as well
        /// this function is used in the IG reduction
        virtual void removeLiteral(const std::string&);
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

        /// Destroys this FormulaMultiaryAnd and all its subformulas. */
        virtual ~FormulaMultiaryAnd() {};

        /// deep copies the formula
        virtual FormulaMultiaryAnd* getDeepCopy() const;

        /// returns the value of this formula under given assignment
        virtual bool value(const FormulaAssignment& assignment) const;

        /// returns the formula as a string
        virtual string asString() const;

        /// Returns the merged equivalent to this formula.
        //virtual FormulaMultiaryAnd* merge();

        virtual Formula* simplify();
        virtual void flatten();
        virtual void merge();
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

        /// Destroys this FormulaMultiaryOr and all its subformulas
        virtual ~FormulaMultiaryOr() {};

        /// returns a deep copy of this formula
        virtual FormulaMultiaryOr* getDeepCopy() const;

        /// returns the value of this formula under given assignment
        virtual bool value(const FormulaAssignment& assignment) const;

        /// returns the formula as a string
        virtual string asString() const;

        /// Returns the merged equivalent to this formula. Merging gets rid of
        /// unnecessary nesting of subformulas. (a+(b+c)) becomes (a+b+c). The
        /// caller is responsible for deleting the returned newly created formula.
        //virtual FormulaMultiaryOr* merge();

        // TODO implement
        virtual Formula* simplify();
        virtual void flatten();
        virtual void merge();
        virtual bool implies(Formula* conclusion) const;
};



// ****************************************************************************
// Literals and Constants
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

        /// returns a deep copy of this formula
        virtual FormulaLiteral* getDeepCopy() const {
        	return new FormulaLiteral(*this);
        }

        /// returns the value of the literal in the given asisgnment
        virtual bool value(const FormulaAssignment& assignment) const {
        	return assignment.get(_literal);
        }

        /// returns the name of the literal
        virtual string asString() const {
        	return _literal;
        }

        /// returns the size of this formula
        virtual int size() const {
            return 1;
        }

        // TODO necessary?
        /// returns a simplified version of this formula
        virtual Formula* simplify() {
        	return new FormulaLiteral(*this);
        }

        virtual void flatten() {};
        virtual void merge() {};
        // TODO implement for true and false
        virtual bool implies(Formula* conclusion) const;
};


/**
 * Base class for all formulas that have a fixed value, i.e., a value that does
 * not depend on an assignment. This class exists, because FormulaTrue
 * and FormulaFalse need a common base class, so we can use them to
 * implement FormulaMultiary::value() uniformly for all multiary
 * formulas.
 */
class FormulaFixed : public FormulaLiteral {
    private:
        /// Fixed truth value of this FormulaFixed.
        bool _value;
    public:
        /// Creates a formula with a given fixed value and string reprensentation.
        FormulaFixed(bool value, const string& asString) : FormulaLiteral(asString), _value(value) {};

        /// Destroys this FormulaFixed.
		virtual ~FormulaFixed() {};

        /// returns a deep copy of this formula
        virtual FormulaFixed* getDeepCopy() const {
        	return new FormulaFixed(*this);
        }

        /// returns the prefixed value of this formula
        virtual bool value(const FormulaAssignment& assignment) const {
        	return _value;
        }

        // TODO necessary?
        /// returns a simplified version of this formula
        virtual Formula* simplify() {
            return new FormulaFixed(*this);
        }
};


/**
 * The constant formula 'TRUE'.
 */
class FormulaTrue : public FormulaFixed {
    public:
        /// basic constructor
        FormulaTrue() : FormulaFixed(true, "TRUE") {};

        /// basic deconstructor
        virtual ~FormulaTrue() {};
};


/**
 * The constant formula 'FALSE'.
 */
class FormulaFalse : public FormulaFixed {
    public:
        /// basic constructor
        FormulaFalse() : FormulaFixed(false, "FALSE") {};

        /// basic deconstructor
        virtual ~FormulaFalse() {};
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
};

#endif
