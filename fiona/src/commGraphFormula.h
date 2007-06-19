/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe,             *
 *                            Robert Danitz                                  *
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


#include "enums.h"
#include <string>
#include <set>
#include <map>


class GraphFormulaFixed;
class GraphFormulaCNF;


/**
 * Assignment of literals to truth values to determine the truth value of a
 * GraphFormula under the given assignment. Typical literals are edge
 * labels of IGs and OGs.
 */
class GraphFormulaAssignment {
private:
    /** Type of the container that maps literals to their truth values. */
    typedef std::map<std::string, bool> literal2bool_t;

    /** Maps literals to their truth values. */
    literal2bool_t literal2bool;

public:
    /**
     * Sets the given literal to the given truth value within this
     * GraphFormulaAssignment.
     * @param literal Literal whose truth value should be set.
     * @param value Truth value the given literal should be set to.
     */
    void set(const std::string& literal, bool value);

    /**
     * Sets the given literal to true within this GraphFormulaAssignment.
     * @param literal Literal which should be set to true.
     */
    void setToTrue(const std::string& literal);

    /**
     * Sets the given literal to false within this GraphFormulaAssignment.
     * @param literal Literal which should be set to false.
     */
    void setToFalse(const std::string& literal);

    /**
     * Gets the truth value of the given literal within this
     * GraphFormulaAssignment. If no truth value was previously set for
     * this literal by set() or setToTrue(), then the truth value of the given
     * literal is implicetly false.
     * @param literal Literal whose truth value should be determined.
     * @returns Truth value of the given literal.
     */
    bool get(const std::string& literal) const;
};


/**
 * A formula to be used as an annotation for a state in an OG or IG. This is an
 * abstract base class for all other formula classes. Use those to construct an
 * actual formula.
 */
class GraphFormula {
public:
    /**
     * Destroys the given GraphFormula and all its subformulas.
     */
    virtual ~GraphFormula() {};

    /**
     * Determines whether this GraphFormula satisfies the given
     * GraphFormulaAssignment. Equivalent to value().
     */
    virtual bool satisfies(const GraphFormulaAssignment& assignment) const;

    /**
     * Formats and returns this GraphFormula as a string. This string is
     * suitable for showing the user and for the OG output format. The string
     * representation for the AND and OR operators are determined by
     * GraphFormulaMultiaryAnd::getOperator() and
     * GraphFormulaMultiaryOr::getOperator().
     * @returns The string representation for this GraphFormula.
     */
    virtual std::string asString() const = 0;

    /**
     * Returns the truth value of this GraphFormula under the given
     * GraphFormulaAssignment.
     * @param assignment The GraphFormulaAssignment under which the truth
     *   value of this GraphFormula should be computed. The value of
     *   literals not set in assignment are considered false.
     * @returns The truth value of this GraphFormula under the given
     *   GraphFormulaAssignment.
     */
    virtual bool value(const GraphFormulaAssignment& assignment) const = 0;
    virtual void removeLiteral(const std::string&);
    virtual GraphFormula* getDeepCopy() const = 0;
    threeValueLogic equals();

    /**
     * Returns this formula in conjunctive normal form. The caller is
     * responsible for deleting the newly created and returned formula.
     */
    GraphFormulaCNF *getCNF();
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
    /**
     * Type of the container holding all subformula of this multiary formula.
     */
    typedef std::set<GraphFormula*> subFormulas_t;

    /**
     * Type of the iterator over subformulas.
     */
    typedef subFormulas_t::iterator iterator;

    /**
     * Type of the const iterator over subformulas.
     */
    typedef subFormulas_t::const_iterator const_iterator;

private:
    /**
     * Holds all subformulas of this multiary formula.
     */
    subFormulas_t subFormulas;

public:
    GraphFormulaMultiary();
    /**
     * Constructs a multiary formula from a given subformula by adding the
     * subformula into the set of subformulae.
     * @param newformula The subformula to be added
     */
    GraphFormulaMultiary(GraphFormula* newformula);

    /**
     * Constructs a multiary formula from two given subformulas. This
     * constructor exists to conviently construct binary formulas from a left
     * hand side and a right hand side subformula.
     * @param lhs Left hand side subformula of the to be constructed binary
     *   formula.
     * @param rhs Reft hand side subformula of the to be constructed binary
     *   formula.
     */
    GraphFormulaMultiary(GraphFormula* lhs, GraphFormula* rhs);

    /**
     * Destroys this GraphFormulaMultiary and all its subformulas.
     */
    virtual ~GraphFormulaMultiary();

    virtual std::string asString() const;

    /**
     * Returns the string representation of this multiary formula's operator.
     * This representation is used to return the string representation of the
     * whole multiary formula (asString()).
     * @returns The string representation of this multiary formula's operator.
     */
    virtual std::string getOperator() const = 0;

    virtual bool value(const GraphFormulaAssignment& assignment) const;

    /**
     * Returns a formula that is equivalent to this multiary formula if it is
     * empty. For instance, an empty disjunction is equivalent to false, an
     * empty conjunction is equivalent to true.
     * @remarks Using this equivalent formula we can implement value() for
     * GraphFormulaMultiaryAnd and GraphFormulaMultiaryOr in a single
     * method here.
     */
    virtual const GraphFormulaFixed& getEmptyFormulaEquivalent() const = 0;

    void addSubFormula(GraphFormula* subformula);
    void removeSubFormula(iterator subformula);
    virtual void removeLiteral(const std::string&);
    void deepCopyMultiaryPrivateMembersToNewFormula(
        GraphFormulaMultiary* newFormula) const;

    /**
     * Returns an iterator to the first subformula.
     */
    iterator begin();

    /**
     * Returns an iterator to the first subformula.
     */
    const_iterator begin() const;

    /**
     * Returns an iterator to one past the last subformula.
     */
    iterator end();

    /**
     * Returns an iterator to one past the last subformula.
     */
    const_iterator end() const;

    /**
     * Returns true iff this formula has no subformulas.
     */
    bool empty() const;
};


/**
 * A multiary conjunction of \link GraphFormula CommGraphFormulas.\endlink Can
 * have arbitrarily many subformulas, even none.
 */
class GraphFormulaMultiaryAnd : public GraphFormulaMultiary {
private:
    /**
     * Holds a GraphFormula that is equivalent to this
     * GraphFormulaMultiaryAnd if it is empty. This is GraphFormulaTrue
     * because an empty conjunction evaluates to true. Used to implement
     * value() for all \link GraphFormulaMultiary multiary functions
     * \endlink in a single method (GraphFormulaMultiary::value()).
     */
    static const GraphFormulaFixed emptyFormulaEquivalent;
public:
    /** See GraphFormulaMultiary(). */
    GraphFormulaMultiaryAnd();
    GraphFormulaMultiaryAnd(GraphFormula* subformula_);
    GraphFormulaMultiaryAnd(GraphFormula* lhs, GraphFormula* rhs);

    /**
     * Returns the merged equivalent to this formula. Merging gets rid of
     * unnecessary nesting of subformulas. (a*(b*c)) becomes (a*b*c). The
     * caller is responsible for deleting the returned newly created formula.
     */
    GraphFormulaMultiaryAnd* merge();

    virtual GraphFormulaMultiaryAnd* getDeepCopy() const;


    /** Destroys this GraphFormulaMultiaryAnd and all its subformulas. */
    virtual ~GraphFormulaMultiaryAnd() {};
    virtual std::string getOperator() const;
    virtual const GraphFormulaFixed& getEmptyFormulaEquivalent() const;
};


/**
 * A multiary disjunction of \link GraphFormula CommGraphFormulas.\endlink Can
 * have arbitrarily many subformulas, even none.
 */
class GraphFormulaMultiaryOr : public GraphFormulaMultiary {
private:
    /**
     * Analog to GraphFormulaMultiaryOr::emptyFormulaEquivalent. Except: an
     * empty multiary disjunction evaluates to false.
     */
    static const GraphFormulaFixed emptyFormulaEquivalent;
public:
    /** See GraphFormulaMultiary(). */
    GraphFormulaMultiaryOr();
    GraphFormulaMultiaryOr(GraphFormula* subformula_);
    GraphFormulaMultiaryOr(GraphFormula* lhs, GraphFormula* rhs);

    /**
     * Returns the merged equivalent to this formula. Merging gets rid of
     * unnecessary nesting of subformulas. (a+(b+c)) becomes (a+b+c). The
     * caller is responsible for deleting the returned newly created formula.
     */
    GraphFormulaMultiaryOr* merge();

    bool implies(GraphFormulaMultiaryOr *);
    virtual GraphFormulaMultiaryOr* getDeepCopy() const;

    /** Destroys this GraphFormulaMultiaryOr and all its subformulas. */
    virtual ~GraphFormulaMultiaryOr() {};
    virtual std::string getOperator() const;
    virtual const GraphFormulaFixed& getEmptyFormulaEquivalent() const;
};


/**
 * A multiary conjunction of \link GraphFormula CommGraphFormulasMultiaryOr.\endlink
 * Each disjunction is called "clause" and has a set of literals.
 * This class exists for special function "addClause", etc.
 */
class GraphFormulaCNF : public GraphFormulaMultiaryAnd {
private:
public:
    GraphFormulaCNF();
    //** constructs a new CNF and adds the clause to the CNF
    GraphFormulaCNF(GraphFormulaMultiaryOr* clause);
    GraphFormulaCNF(GraphFormulaMultiaryOr* clause1, GraphFormulaMultiaryOr* clause2);

    //** destroys the CNF and all its clauses
    virtual ~GraphFormulaCNF() {};

    virtual GraphFormulaCNF* getDeepCopy() const;

    //** adds a clause to the CNF
    void addClause(GraphFormulaMultiaryOr* clause);

    bool implies(GraphFormulaCNF *);

    /**
     * Simplifies the formula by removing redundant clauses.
     */
    void simplify();
};


/**
 * A literal within a GraphFormula.
 */
class GraphFormulaLiteral : public GraphFormula {
private:
    /** The string representation of this literal. */
    std::string literal;
public:
    /**
     * Reserved literal TAU is used for edges in OGs denoting internal steps.
     */
    static const std::string TAU;

    /**
     * Reserved literal FINAL is used notations of nodes in OGs to denote
     * possible final states.
     */
    static const std::string FINAL;

    /**
     * Reserved literal TRUE is used for the value 'true'.
     */
    static const std::string TRUE;

    /**
     * Reserved literal FALSE is used for the value 'false'.
     */
    static const std::string FALSE;

    /**
     * Constructs a literal with the given string representation.
     * @param literal String representation of this literal.
     */
    GraphFormulaLiteral(const std::string& literal);
    virtual GraphFormulaLiteral* getDeepCopy() const;
    virtual ~GraphFormulaLiteral() {};
    virtual bool value(const GraphFormulaAssignment& assignment) const;
    virtual std::string asString() const;
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
    /** Fixed truth value of this GraphFormulaFixed. */
    bool _value;
public:
    /**
     * Creates a formula with a given fixed value and string reprensentation.
     * @param value Fixed truth value of this formula.
     * @param asString String representation of this formula.
     */
    GraphFormulaFixed(bool value, const std::string& asString);

    virtual GraphFormulaFixed* getDeepCopy() const;

    /* Destroys this GraphFormulaFixed. */
    virtual ~GraphFormulaFixed() {};
    virtual bool value(const GraphFormulaAssignment& assignment) const;
};


/**
 * The constant formula 'true'.
 */
class GraphFormulaTrue : public GraphFormulaFixed {
public:
    GraphFormulaTrue();
    virtual ~GraphFormulaTrue() {};
};


/**
 * The constant formula 'false'.
 */
class GraphFormulaFalse : public GraphFormulaFixed {
public:
    GraphFormulaFalse();
    virtual ~GraphFormulaFalse() {};
};


/**
 * A special literal FINAL.
 */
class GraphFormulaLiteralFinal : public GraphFormulaLiteral {
public:
    /**
     * Constructs a literal with the given string representation.
     */
    GraphFormulaLiteralFinal();
    virtual ~GraphFormulaLiteralFinal() {};
};


/**
 * A special literal TAU.
 */
class GraphFormulaLiteralTau : public GraphFormulaLiteral {
public:
    /**
     * Constructs a literal with the given string representation.
     */
    GraphFormulaLiteralTau();
    virtual ~GraphFormulaLiteralTau() {};
};
#endif
