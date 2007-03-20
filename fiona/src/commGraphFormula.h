/*****************************************************************************
 * Copyright 2005, 2006, 2007 Jan Bretschneider, Peter Massuthe              *
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
 * \file    commGraphFormula.h
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

#ifndef commGraphFormula_H_
#define commGraphFormula_H_

#include <string>
#include <set>
#include <map>

class CommGraphFormulaFixed;

/**
 * Assignment of literals to truth values to determine the truth value of a
 * CommGraphFormula under the given assignment. Typical literals are edge
 * labels of IGs and OGs.
 */
class CommGraphFormulaAssignment {
private:
    /** Type of the container that maps literals to their truth values. */
    typedef std::map<std::string, bool> literal2bool_t;

    /** Maps literals to their truth values. */
    literal2bool_t literal2bool;

public:
    /**
     * Reverved literal TAU is used for edges in OGs denoting internal steps.
     */
    static const std::string TAU;

    /**
     * Reserved literal FINAL is used notations of nodes in OGs to denote
     * possible final states.
     */
    static const std::string FINAL;

    /**
     * Sets the given literal to the given truth value within this
     * CommGraphFormulaAssignment.
     * @param literal Literal whose truth value should be set.
     * @param value Truth value the given literal should be set to.
     */
    void set(const std::string& literal, bool value);

    /**
     * Sets the given literal to true within this CommGraphFormulaAssignment.
     * @param literal Literal which should be set to true.
     */
    void setToTrue(const std::string& literal);

    /**
     * Gets the truth value of the given literal within this
     * CommGraphFormulaAssignment. If no truth value was previously set for
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
class CommGraphFormula {
public:
    /**
     * Destroys the given CommGraphFormula and all its subformulas.
     */
    virtual ~CommGraphFormula() {};

    /**
     * Determines whether this CommGraphFormula satisfies the given
     * CommGraphFormulaAssignment. Equivalent to value().
     */
    virtual bool satisfies(const CommGraphFormulaAssignment& assignment) const;

    /**
     * Formats and returns this CommGraphFormula as a string. This string is
     * suitable for showing the user and for the OG output format. The string
     * representation for the AND and OR operators are determined by
     * CommGraphFormulaMultiaryAnd::getOperator() and
     * CommGraphFormulaMultiaryOr::getOperator().
     * @returns The string representation for this CommGraphFormula.
     */
    virtual std::string asString() const = 0;

    /**
     * Returns the truth value of this CommGraphFormula under the given
     * CommGraphFormulaAssignment.
     * @param assignment The CommGraphFormulaAssignment under which the truth
     *   value of this CommGraphFormula should be computed. The value of
     *   literals not set in assignment are considered false.
     * @returns The truth value of this CommGraphFormula under the given
     *   CommGraphFormulaAssignment.
     */
    virtual bool value(const CommGraphFormulaAssignment& assignment) const = 0;
};

/**
 * Base class for all multiary \link CommGraphFormula
 * CommGraphFormulas\endlink, such as CommGraphFormulaMultiaryAnd and
 * CommGraphFormulaMultiaryOr. This class exists because the classes
 * CommGraphFormulaMultiaryAnd and CommGraphFormulaMultiaryOr share
 * functionality which should be implemented only once, e.g., the truth of a
 * multiary conjunction is computed analogously to the truth value of a
 * multiary disjunction.
 */
class CommGraphFormulaMultiary : public CommGraphFormula {
private:
    /**
     * Type of the container holding all subformula of this multiary formula.
     */
    typedef std::set<CommGraphFormula*> subFormulas_t;

    /**
     * Holds all subformulas of this multiary formula.
     */
    subFormulas_t subFormulas;
public:
    /**
     * Constructs a multiary formula from two given subformulas. This
     * constructor exists to conviently construct binary formulas from a left
     * hand side and a right hand side subformula.
     * @param lhs Left hand side subformula of the to be constructed binary
     *   formula.
     * @param rhs Reft hand side subformula of the to be constructed binary
     *   formula.
     */
    CommGraphFormulaMultiary(CommGraphFormula* lhs, CommGraphFormula* rhs);

    /**
     * Destroys this CommGraphFormulaMultiary and all its subformulas.
     */
    virtual ~CommGraphFormulaMultiary();
    virtual std::string asString() const;

    /**
     * Returns the string representation of this multiary formula's operator.
     * This representation is used to return the string representation of the
     * whole multiary formula (asString()).
     * @returns The string representation of this multiary formula's operator.
     */
    virtual std::string getOperator() const = 0;
    virtual bool value(const CommGraphFormulaAssignment& assignment) const;

    /**
     * Returns a formula that is equivalent to this multiary formula if it is
     * empty. For instance, an empty disjunction is equivalent to false, an
     * empty conjunction is equivalent to true.
     * @remarks Using this equivalent formula we can implement value() for
     * CommGraphFormulaMultiaryAnd and CommGraphFormulaMultiaryOr in a single
     * method here.
     */
    virtual const CommGraphFormulaFixed& getEmptyFormulaEquivalent() const = 0;
};

/**
 * A multiary conjunction of \link CommGraphFormula CommGraphFormulas.\end Can
 * have arbitrarily many subformulas, even none.
 */
class CommGraphFormulaMultiaryAnd : public CommGraphFormulaMultiary {
private:
    /**
     * Holds a CommGraphFormula that is equivalent to this
     * CommGraphFormulaMultiaryAnd if it is empty. This is CommGraphFormulaTrue
     * because an empty conjunction evaluates to true. Used to implement
     * value() for all \link CommGraphFormulaMultiary multiary functions
     * \endlink in a single method (CommGraphFormulaMultiary::value()).
     */
    static const CommGraphFormulaFixed emptyFormulaEquivalent;
public:
    /** See CommGraphFormulaMultiary(). */
    CommGraphFormulaMultiaryAnd(CommGraphFormula* lhs, CommGraphFormula* rhs);

    /** Destroys this CommGraphFormulaMultiaryAnd and all its subformulas. */
    virtual ~CommGraphFormulaMultiaryAnd() {};
    virtual std::string getOperator() const;
    virtual const CommGraphFormulaFixed& getEmptyFormulaEquivalent() const;
};

/**
 * A multiary disjunction of \link CommGraphFormula CommGraphFormulas.\end Can
 * have arbitrarily many subformulas, even none.
 */
class CommGraphFormulaMultiaryOr : public CommGraphFormulaMultiary {
private:
    /**
     * Analog to CommGraphFormulaMultiaryOr::emptyFormulaEquivalent. Except: an
     * empty multiary disjunction evaluates to false.
     */
    static const CommGraphFormulaFixed emptyFormulaEquivalent;
public:
    /** See CommGraphFormulaMultiary(). */
    CommGraphFormulaMultiaryOr(CommGraphFormula* lhs, CommGraphFormula* rhs);

    /** Destroys this CommGraphFormulaMultiaryOr and all its subformulas. */
    virtual ~CommGraphFormulaMultiaryOr() {};
    virtual std::string getOperator() const;
    virtual const CommGraphFormulaFixed& getEmptyFormulaEquivalent() const;
};

/**
 * Base class for all formulas that have a fixed value, i.e., a value that does
 * not depend on an assignment. This class exists, because CommGraphFormulaTrue
 * and CommGraphFormulaFalse need a common base class, so we can use them to
 * implement CommGraphFormulaMultiary::value() uniformly for all multiary
 * formulas.
 */
class CommGraphFormulaFixed : public CommGraphFormula {
private:
    /** String representation of this CommGraphFormulaFixed. */
    std::string _asString;

    /** Fixed truth value of this CommGraphFormulaFixed. */
    bool _value;
public:
    /**
     * Creates a formula with a given fixed value and string reprensentation.
     * @param value Fixed truth value of this formula.
     * @param asString String representation of this formula.
     */
    CommGraphFormulaFixed(bool value, const std::string& asString);

    /* Destroys this CommGraphFormulaFixed. */
    virtual ~CommGraphFormulaFixed() {};
    virtual bool value(const CommGraphFormulaAssignment& assignment) const;
    virtual std::string asString() const;
};

/**
 * The constant formula 'true'.
 */
class CommGraphFormulaTrue : public CommGraphFormulaFixed {
public:
    CommGraphFormulaTrue();
    virtual ~CommGraphFormulaTrue() {};
};

/**
 * The constant formula 'false'.
 */
class CommGraphFormulaFalse : public CommGraphFormulaFixed {
public:
    CommGraphFormulaFalse();
    virtual ~CommGraphFormulaFalse() {};
};

/**
 * A literal within a CommGraphFormula.
 */
class CommGraphFormulaLiteral : public CommGraphFormula {
private:
    /** The string representation of this literal. */
    std::string literal;
public:
    /**
     * Constructs a literal with the given string representation.
     * @param literal String representation of this literal.
     */
    CommGraphFormulaLiteral(const std::string& literal);
    virtual ~CommGraphFormulaLiteral() {};
    virtual bool value(const CommGraphFormulaAssignment& assignment) const;
    virtual std::string asString() const;
};

#endif
