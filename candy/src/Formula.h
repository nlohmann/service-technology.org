#ifndef Formula_H_
#define Formula_H_

#include "settings.h"
#include <set>


// TRUE and FALSE #defined in cudd package may interfere with
// FormulaLiteral::TRUE and ...::FALSE.
#undef TRUE
#undef FALSE

#define TRUE 1
#define FALSE 0

class FormulaFixed;
class FormulaCNF;


/**
 * Assignment of literals to truth values to determine the truth value of a
 * Formula under the given assignment. Typical literals are edge
 * labels of IGs and OGs.
 */
class FormulaAssignment {
    private:
        /// Type of the container that maps literals to their truth values.
        typedef std::map<std::string, bool> literal2bool_t;

        /// Maps literals to their truth values.
        literal2bool_t literal2bool;

    public:
        /// sets the given literal to the given truth value
        void set(const std::string& literal, bool value);

        /// sets the given literal to true
        void setToTrue(const std::string& literal);

        /// sets the given literal to false
        void setToFalse(const std::string& literal);

        /// returns the bool value of a literal
        bool get(const std::string& literal) const;

};


/**
 * A formula to be used as an annotation for a state in an OG or IG. This is an
 * abstract base class for all other formula classes. Use those to construct an
 * actual formula.
 */
class Formula {
    public:
        /// Destroys the given Formula and all its subformulas.
        virtual ~Formula() { };

        virtual bool satisfies(const FormulaAssignment& assignment) const;

        /// Formats and returns this Formula as a string. This string is
        /// suitable for showing the user and for the OG output format. The string
        /// representation for the AND and OR operators are determined by
        /// FormulaMultiaryAnd::getOperator() and
        /// FormulaMultiaryOr::getOperator().
        /// returns The string representation for this Formula.
        virtual std::string asString() const = 0;

        /// Returns the truth value of this Formula under the given
        /// FormulaAssignment.
        /// @param assignment The FormulaAssignment under which the truth
        ///  value of this Formula should be computed. The value of
        ///  literals not set in assignment are considered false.
        /// @returns The truth value of this Formula under the given
        ///  FormulaAssignment.
        virtual bool value(const FormulaAssignment& assignment) const = 0;

        /// fills the given set of strings with all event-representing literals
        /// from the formula
        virtual void getEventLiterals(std::set<std::string>& events);

        /// removes a literal from the whole formula
        virtual void removeLiteral(const std::string&);

        /// removes a literal from the formula, if this literal is the only one of a clause,
        /// the clause gets removed as well
        /// this function is used in the IG reduction
        virtual void removeLiteralForReal(const std::string&);

        /// removes a literal from the whole formula by hiding
        virtual void removeLiteralByHiding(const std::string&);

        /// copies and returns this Formula
        virtual Formula* getDeepCopy() const = 0;

        /// returns the number of subformulas
        virtual int getSubFormulaSize() const;
};


/**
 * Base class for all multiary \link Formula
 * CommFormulas\endlink, such as FormulaMultiaryAnd and
 * FormulaMultiaryOr. This class exists because the classes
 * FormulaMultiaryAnd and FormulaMultiaryOr share
 * functionality which should be implemented only once, e.g., the truth of a
 * multiary conjunction is computed analogously to the truth value of a
 * multiary disjunction.
 */
class FormulaMultiary : public Formula {
    public:
        /// Type of the container holding all subformula of this multiary formula.
        typedef std::list<Formula*> subFormulas_t;

        /// Type of the iterator over subformulas.
        typedef subFormulas_t::iterator iterator;

        /// Type of the const iterator over subformulas.
        typedef subFormulas_t::const_iterator const_iterator;

    protected:
        /// Holds all subformulas of this multiary formula.
        subFormulas_t subFormulas;

    public:
        /// basic constructor
        FormulaMultiary();

        FormulaMultiary(Formula* newformula);

        FormulaMultiary(Formula* lhs, Formula* rhs);

        /// Destroys this FormulaMultiary and all its subformulas.
        virtual ~FormulaMultiary();

        /// returns a string of this formula
        virtual std::string asString() const;

        /// Returns the string representation of this multiary formula's operator.
        /// This representation is used to return the string representation of the
        /// whole multiary formula (asString()).
        /// @returns The string representation of this multiary formula's operator.
        virtual std::string getOperator() const = 0;

        /// returns the value of this formula under the given assignment
        virtual bool value(const FormulaAssignment& assignment) const;

        /// Returns a formula that is equivalent to this multiary formula if it is
        /// empty. For instance, an empty disjunction is equivalent to false, an
        /// empty conjunction is equivalent to true.
        /// @remarks Using this equivalent formula we can implement value() for
        /// FormulaMultiaryAnd and FormulaMultiaryOr in a single
        /// method here.
        virtual const FormulaFixed& getEmptyFormulaEquivalent() const = 0;

        /// adds a given subformula to this multiary
        void addSubFormula(Formula* subformula);

        /// removes the subformula at the given iterator
        iterator removeSubFormula(iterator subformula);

        /// fills the given set of strings with all event-representing literals
        /// from the formula
        virtual void getEventLiterals(std::set<std::string>& events);

        /// removes a literal from this formula
        virtual void removeLiteral(const std::string&);

        /// removes a literal from the formula, if this literal is the only one of a clause,
        /// the clause gets removed as well
        /// this function is used in the IG reduction
        virtual void removeLiteralForReal(const std::string&);

        /// removes a literal from the formula by hiding
        virtual void removeLiteralByHiding(const std::string&);

        /// returns the number of subformulas
        virtual int getSubFormulaSize() const;

        /// copies te private members of this multiary to a given formula
        void deepCopyMultiaryPrivateMembersToNewFormula(FormulaMultiary* newFormula) const;

        /// Returns an iterator to the first subformula.
        iterator begin();

        /// Returns an iterator to the first subformula.
        const_iterator begin() const;

        /// Returns an iterator to one past the last subformula.
        iterator end();

        /// Returns an iterator to one past the last subformula.
        const_iterator end() const;

        /// Returns true iff this formula has no subformulas.
        bool empty() const;

        /// Returns the the number of formulas, the multiary formula consists of.
        int size() const;
};


/**
 * A multiary conjunction of \link Formula CommFormulas.\endlink Can
 * have arbitrarily many subformulas, even none.
 */
class FormulaMultiaryAnd : public FormulaMultiary {
    private:
        /// Holds a Formula that is equivalent to this
        /// FormulaMultiaryAnd if it is empty. This is FormulaTrue
        /// because an empty conjunction evaluates to true. Used to implement
        /// value() for all \link FormulaMultiary multiary functions
        /// \endlink in a single method (FormulaMultiary::value()).
        static const FormulaFixed emptyFormulaEquivalent;
    public:
        /// See FormulaMultiary()
        FormulaMultiaryAnd();
        FormulaMultiaryAnd(Formula* subformula_);
        FormulaMultiaryAnd(Formula* lhs, Formula* rhs);

        /// Returns the merged equivalent to this formula.
        FormulaMultiaryAnd* merge();

        /// deep copies the formula
        virtual FormulaMultiaryAnd* getDeepCopy() const;

        /// Destroys this FormulaMultiaryAnd and all its subformulas. */
        virtual ~FormulaMultiaryAnd() { };

        /// returns the fitting operator
        virtual std::string getOperator() const;

        /// returns a constant empty formula equivalent
        virtual const FormulaFixed& getEmptyFormulaEquivalent() const;
};


/**
 * A multiary disjunction of \link Formula CommFormulas.\endlink Can
 * have arbitrarily many subformulas, even none.
 */
class FormulaMultiaryOr : public FormulaMultiary {
    private:
        /// Analog to FormulaMultiaryOr::emptyFormulaEquivalent. Except: an
        /// empty multiary disjunction evaluates to false.
        static const FormulaFixed emptyFormulaEquivalent;
    public:
        /// See FormulaMultiary()
        FormulaMultiaryOr();
        FormulaMultiaryOr(Formula* subformula_);
        FormulaMultiaryOr(Formula* lhs, Formula* rhs);

        /// Returns the merged equivalent to this formula. Merging gets rid of
        /// unnecessary nesting of subformulas. (a+(b+c)) becomes (a+b+c). The
        /// caller is responsible for deleting the returned newly created formula.
        FormulaMultiaryOr* merge();

        /// returns a deep copy of this formula
        virtual FormulaMultiaryOr* getDeepCopy() const;

        /// Destroys this FormulaMultiaryOr and all its subformulas
        virtual ~FormulaMultiaryOr() { };

        /// returns "+"
        virtual std::string getOperator() const;

        /// returns an empty formula equivalent
        virtual const FormulaFixed& getEmptyFormulaEquivalent() const;

        /// intelligently adds a new subformula to the clause, if exactly the same
        /// subformula already exists, the new one is not added
        virtual void addSubFormula(Formula*);
};


/**
 * A literal within a Formula.
 */
class FormulaLiteral : public Formula {
    private:
        /// The string representation of this literal.
        std::string literal;
    public:

        /// Reserved literal FINAL is used notations of nodes in OGs to denote
        /// possible final states.
        static const std::string FINAL;

        /// Reserved literal TRUE is used for the value 'true'.
#undef TRUE /* TRUE may interfere with macro in cudd package. */
        static const std::string TRUE;
#define TRUE 1

        /// Reserved literal FALSE is used for the value 'false'.
#undef FALSE /* FALSE may interfere with macro in cudd package. */
        static const std::string FALSE;
#define FALSE 0

        /// Constructs a literal with the given string representation.
        FormulaLiteral(const std::string& literal);

        /// returns a deep copy of this formula
        virtual FormulaLiteral* getDeepCopy() const;

        /// basic deconstructor
        virtual ~FormulaLiteral();

        /// returns the value of the literal in the given asisgnment
        virtual bool value(const FormulaAssignment& assignment) const;

        /// returns the name of the literal
        virtual std::string asString() const;
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
        FormulaFixed(bool value, const std::string& asString);

        /// returns a deep copy of this formula
        virtual FormulaFixed* getDeepCopy() const;

        /// Destroys this FormulaFixed.
        virtual ~FormulaFixed() {
        };

        /// returns the prefixed value of this formula
        virtual bool value(const FormulaAssignment& assignment) const;
};


/**
 * The constant formula 'true'.
 */
class FormulaTrue : public FormulaFixed {
    public:
        /// basic constructor
        FormulaTrue();

        /// basic deconstructor
        virtual ~FormulaTrue() {
        };
};


/**
 * The constant formula 'false'.
 */
class FormulaFalse : public FormulaFixed {
    public:
        /// basic constructor
        FormulaFalse();

        /// basic deconstructor
        virtual ~FormulaFalse() {
        };
};


/**
 * A special literal FINAL.
 */
class FormulaLiteralFinal : public FormulaLiteral {
    public:
        /// Constructs a literal with the given string representation.
        FormulaLiteralFinal();

        /// basic deconstructor
        virtual ~FormulaLiteralFinal() {
        };
};


#endif
