/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe,             *
 *                            Robert Danitz                                  *
 *                                                                           *
 * Copyright 2008             Peter Massuthe, Daniela Weinberg               *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/*!
 * \file    GraphFormula.h
 *
 * \brief   Provides formulas to be used as annotations of nodes in IGs or OGs.
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

#ifndef GraphFormula_H_
#define GraphFormula_H_

#include <string>
#include <list>
#include <map>
#include "mynew.h"


// TRUE and FALSE #defined in cudd package may interfere with
// GraphFormulaLiteral::TRUE and ...::FALSE.
#undef TRUE
#undef FALSE

//! used, for instance, for evaluating node annotation where not all successors
//! are known yet
enum threeValueLogic {FALSE, TRUE, UNKNOWN};

#define TRUE 1
#define FALSE 0

class GraphFormulaFixed;
class GraphFormulaCNF;


/**
 * Assignment of literals to truth values to determine the truth value of a
 * GraphFormula under the given assignment. Typical literals are edge
 * labels of IGs and OGs.
 */
class GraphFormulaAssignment {
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
class GraphFormula {
    public:
        /// Destroys the given GraphFormula and all its subformulas.
        virtual ~GraphFormula() {
        };

        virtual bool satisfies(const GraphFormulaAssignment& assignment) const;

        /// Formats and returns this GraphFormula as a string. This string is
        /// suitable for showing the user and for the OG output format. The string
        /// representation for the AND and OR operators are determined by
        /// GraphFormulaMultiaryAnd::getOperator() and
        /// GraphFormulaMultiaryOr::getOperator().
        /// returns The string representation for this GraphFormula.
        virtual std::string asString() const = 0;

        /// Returns the truth value of this GraphFormula under the given
        /// GraphFormulaAssignment.
        /// @param assignment The GraphFormulaAssignment under which the truth
        ///  value of this GraphFormula should be computed. The value of
        ///  literals not set in assignment are considered false.
        /// @returns The truth value of this GraphFormula under the given
        ///  GraphFormulaAssignment.
        virtual bool value(const GraphFormulaAssignment& assignment) const = 0;

        /// removes a literal from the whole formula
        virtual void removeLiteral(const std::string&);

        /// removes a literal from the formula, if this literal is the only one of a clause,
        /// the clause gets removed as well
        /// this function is used in the IG reduction
        virtual void removeLiteralForReal(const std::string&);

        /// removes a literal from the whole formula by hiding
        virtual void removeLiteralByHiding(const std::string&);

        /// copies and returns this GraphFormula
        virtual GraphFormula* getDeepCopy() const = 0;
        
        /// returns the value of the formula without an assignment
        threeValueLogic equals();

        /// returns the number of subformulas
        virtual int getSubFormulaSize() const;


        /// Returns this formula in conjunctive normal form. The caller is
        /// responsible for deleting the newly created and returned formula.
        GraphFormulaCNF *getCNF();

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphFormula)
#define new NEW_NEW
};


/**
 * Base class for all multiary \link GraphFormula
 * CommGraphFormulas\endlink, such as GraphFormulaMultiaryAnd and
 * GraphFormulaMultiaryOr. This class exists because the classes
 * GraphFormulaMultiaryAnd and GraphFormulaMultiaryOr share
 * functionality which should be implemented only once, e.g., the truth of a
 * multiary conjunction is computed analogously to the truth value of a
 * multiary disjunction.
 */
class GraphFormulaMultiary : public GraphFormula {
    public:
        /// Type of the container holding all subformula of this multiary formula.
        typedef std::list<GraphFormula*> subFormulas_t;

        /// Type of the iterator over subformulas.
        typedef subFormulas_t::iterator iterator;

        /// Type of the const iterator over subformulas.
        typedef subFormulas_t::const_iterator const_iterator;

    private:
        /// Holds all subformulas of this multiary formula.
        subFormulas_t subFormulas;

    public:
        /// basic constructor
        GraphFormulaMultiary();

        GraphFormulaMultiary(GraphFormula* newformula);

        GraphFormulaMultiary(GraphFormula* lhs, GraphFormula* rhs);

        /// Destroys this GraphFormulaMultiary and all its subformulas.
        virtual ~GraphFormulaMultiary();

        /// returns a string of this formula
        virtual std::string asString() const;

        /// Returns the string representation of this multiary formula's operator.
        /// This representation is used to return the string representation of the
        /// whole multiary formula (asString()).
        /// @returns The string representation of this multiary formula's operator.
        virtual std::string getOperator() const = 0;

        /// returns the value of this formula under the given assignment
        virtual bool value(const GraphFormulaAssignment& assignment) const;

        /// Returns a formula that is equivalent to this multiary formula if it is
        /// empty. For instance, an empty disjunction is equivalent to false, an
        /// empty conjunction is equivalent to true.
        /// @remarks Using this equivalent formula we can implement value() for
        /// GraphFormulaMultiaryAnd and GraphFormulaMultiaryOr in a single
        /// method here.
        virtual const GraphFormulaFixed& getEmptyFormulaEquivalent() const = 0;

        /// adds a given subformula to this multiary
        void addSubFormula(GraphFormula* subformula);
        
        /// removes the subformula at the given iterator
        iterator removeSubFormula(iterator subformula);

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
        void deepCopyMultiaryPrivateMembersToNewFormula(GraphFormulaMultiary* newFormula) const;

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

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphFormulaMultiary)
#define new NEW_NEW
};


/**
 * A multiary conjunction of \link GraphFormula CommGraphFormulas.\endlink Can
 * have arbitrarily many subformulas, even none.
 */
class GraphFormulaMultiaryAnd : public GraphFormulaMultiary {
    private:
        /// Holds a GraphFormula that is equivalent to this
        /// GraphFormulaMultiaryAnd if it is empty. This is GraphFormulaTrue
        /// because an empty conjunction evaluates to true. Used to implement
        /// value() for all \link GraphFormulaMultiary multiary functions
        /// \endlink in a single method (GraphFormulaMultiary::value()).
        static const GraphFormulaFixed emptyFormulaEquivalent;
    public:
        /// See GraphFormulaMultiary()
        GraphFormulaMultiaryAnd();
        GraphFormulaMultiaryAnd(GraphFormula* subformula_);
        GraphFormulaMultiaryAnd(GraphFormula* lhs, GraphFormula* rhs);

        /// Returns the merged equivalent to this formula. 
        GraphFormulaMultiaryAnd* merge();

        /// deep copies the formula
        virtual GraphFormulaMultiaryAnd* getDeepCopy() const;

        /// Destroys this GraphFormulaMultiaryAnd and all its subformulas. */
        virtual ~GraphFormulaMultiaryAnd() { };

        /// returns the fitting operator
        virtual std::string getOperator() const;
        
        /// returns a constant empty formula equivalent
        virtual const GraphFormulaFixed& getEmptyFormulaEquivalent() const;

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphMultiaryAnd)
#define new NEW_NEW
};


/**
 * A multiary disjunction of \link GraphFormula CommGraphFormulas.\endlink Can
 * have arbitrarily many subformulas, even none.
 */
class GraphFormulaMultiaryOr : public GraphFormulaMultiary {
    private:
        /// Analog to GraphFormulaMultiaryOr::emptyFormulaEquivalent. Except: an
        /// empty multiary disjunction evaluates to false.
        static const GraphFormulaFixed emptyFormulaEquivalent;
    public:
        /// See GraphFormulaMultiary()
        GraphFormulaMultiaryOr();
        GraphFormulaMultiaryOr(GraphFormula* subformula_);
        GraphFormulaMultiaryOr(GraphFormula* lhs, GraphFormula* rhs);

        /// Returns the merged equivalent to this formula. Merging gets rid of
        /// unnecessary nesting of subformulas. (a+(b+c)) becomes (a+b+c). The
        /// caller is responsible for deleting the returned newly created formula.
        GraphFormulaMultiaryOr* merge();

        /// checks whether the given formula is already implied by this one
        bool implies(GraphFormulaMultiaryOr *);
        
        /// returns a deep copy of this formula
        virtual GraphFormulaMultiaryOr* getDeepCopy() const;

        /// Destroys this GraphFormulaMultiaryOr and all its subformulas
        virtual ~GraphFormulaMultiaryOr() { };
        
        /// returns "+"
        virtual std::string getOperator() const;
        
        /// returns an empty formula equivalent
        virtual const GraphFormulaFixed& getEmptyFormulaEquivalent() const;

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphFormulaMultiaryOr)
#define new NEW_NEW
};


/**
 * A multiary conjunction of \link GraphFormula CommGraphFormulasMultiaryOr.\endlink
 * Each disjunction is called "clause" and has a set of literals.
 * This class exists for special function "addClause", etc.
 */
class GraphFormulaCNF : public GraphFormulaMultiaryAnd {
    private:
    public:
        /// basic constructor
        GraphFormulaCNF();
        
        /// constructs a new CNF and adds the clause to the CNF
        GraphFormulaCNF(GraphFormulaMultiaryOr* clause);

        /// constructs a new CNF and adds two clauses to the CNF
        GraphFormulaCNF(GraphFormulaMultiaryOr* clause1,
                        GraphFormulaMultiaryOr* clause2);

        /// destroys the CNF and all its clauses
        virtual ~GraphFormulaCNF() {
        };

        /// deep copies this formula
        virtual GraphFormulaCNF* getDeepCopy() const;

        /// adds a clause to the CNF
        void addClause(GraphFormulaMultiaryOr* clause);

        /// checks whether the given CNF is implied by this one
        bool implies(GraphFormulaCNF *);

        /// Simplifies the formula by removing redundant clauses.
        void simplify();

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphFormulaCNF)
#define new NEW_NEW
};


/**
 * A literal within a GraphFormula.
 */
class GraphFormulaLiteral : public GraphFormula {
    private:
        /// The string representation of this literal.
        std::string literal;
    public:
        /// Reserved literal TAU is used for edges in OGs denoting internal steps.
        static const std::string TAU;

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
        GraphFormulaLiteral(const std::string& literal);

        /// returns a deep copy of this formula
        virtual GraphFormulaLiteral* getDeepCopy() const;
        
        /// basic deconstructor
        virtual ~GraphFormulaLiteral() {
        };
        
        /// returns the value of the literal in the given asisgnment
        virtual bool value(const GraphFormulaAssignment& assignment) const;
        
        /// returns the name of the literal
        virtual std::string asString() const;

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphFormulaLiteral)
#define new NEW_NEW
};


/**
 * Base class for all formulas that have a fixed value, i.e., a value that does
 * not depend on an assignment. This class exists, because GraphFormulaTrue
 * and GraphFormulaFalse need a common base class, so we can use them to
 * implement GraphFormulaMultiary::value() uniformly for all multiary
 * formulas.
 */
class GraphFormulaFixed : public GraphFormulaLiteral {
    private:
        /// Fixed truth value of this GraphFormulaFixed.
        bool _value;
    public:
        /// Creates a formula with a given fixed value and string reprensentation.
        GraphFormulaFixed(bool value, const std::string& asString);

        /// returns a deep copy of this formula
        virtual GraphFormulaFixed* getDeepCopy() const;

        /// Destroys this GraphFormulaFixed.
        virtual ~GraphFormulaFixed() {
        };
        
        /// returns the prefixed value of this formula
        virtual bool value(const GraphFormulaAssignment& assignment) const;

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphFormulaFixed)
#define new NEW_NEW
};


/**
 * The constant formula 'true'.
 */
class GraphFormulaTrue : public GraphFormulaFixed {
    public:
        /// basic constructor
        GraphFormulaTrue();

        /// basic deconstructor
        virtual ~GraphFormulaTrue() {
        };

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphFormulaTrue)
#define new NEW_NEW
};


/**
 * The constant formula 'false'.
 */
class GraphFormulaFalse : public GraphFormulaFixed {
    public:
        /// basic constructor
        GraphFormulaFalse();

        /// basic deconstructor
        virtual ~GraphFormulaFalse() {
        };

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphFormulaFalse)
#define new NEW_NEW
};


/**
 * A special literal FINAL.
 */
class GraphFormulaLiteralFinal : public GraphFormulaLiteral {
    public:
        /// Constructs a literal with the given string representation.
        GraphFormulaLiteralFinal();
        
        /// basic deconstructor
        virtual ~GraphFormulaLiteralFinal() {
        };

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphFormulaLiteralFinal)
#define new NEW_NEW
};


/**
 * A special literal TAU.
 */
class GraphFormulaLiteralTau : public GraphFormulaLiteral {
    public:
        /// Constructs a literal with the given string representation.
        GraphFormulaLiteralTau();

        /// basic deconstructor
        virtual ~GraphFormulaLiteralTau() {
        };

#undef new
        /// Provides user defined operator new. Needed to trace all new operations
        /// on this class.
        NEW_OPERATOR(GraphFormulaLiteralTau)
#define new NEW_NEW
};
#endif
