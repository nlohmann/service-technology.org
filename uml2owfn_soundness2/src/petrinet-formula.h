/*****************************************************************************\
  UML2oWFN -- Translating UML2 Activity Diagrams to Petri nets
  Copyright (C) 2008  Dirk Fahland <dirk.fahland@service-technolog.org>,

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    petrinet-formula.h
 *
 * \brief   Petri Net API: formulas over nets
 *
 * \author  Dirk Fahland <fahland@informatik.hu-berlin.de>,
 *
 * \since   2008-05-26
 *
 * \date    \$Date: 2007/06/28 07:38:17 $
 *
 * \note    This file is part of UML2oWFN.
 *
 * \version \$Revision: 1.01 $
 *
 * \ingroup petrinet
 */


#ifndef PETRINETFORMULA_H_
#define PETRINETFORMULA_H_

#include "petrinet.h"

#include <string>
#include <ostream>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>

using std::string;
using std::vector;
using std::deque;
using std::set;
using std::map;
using std::list;
using std::pair;
using std::ostream;

// ------------------ operators and connectives ------------------

/*!
 * \brief enum-type for comparisons
 *
 */
typedef enum
{
	COMPARE_EQUAL,
	COMPARE_LESS,
	COMPARE_GREATER,
	COMPARE_LEQ,
	COMPARE_GEQ,
	COMPARE_NEQ
} comparison_relation;

/*!
 * \brief logical connectives of formulae
 */
typedef enum
{
	LOG_NONE,
	LOG_AND,
	LOG_OR,
	LOG_IMPLIES,
	LOG_EQUIVALENT
} logical_connective;

/*!
 * \brief logical connectives of formulae
 */
typedef enum
{
	LOGQ_NONE,
	LOGQ_CTL_ALLPATH_ALWAYS,
	LOGQ_CTL_EXPATH_ALWAYS,
	LOGQ_CTL_ALLPATH_EVENTUALLY,
	LOGQ_CTL_EXPATH_EVENTUALLY
} logical_quantifier;

// ------------------------- formula -------------------------

/*!
 * \brief empty base-class for all formulae, helps as basic
 *        hook for recursive definitions
 */
class Formula {
	// the contents of a basic formula...
public:
	/// negated formula
	bool negated;

	/// basic constructor
	Formula();

    /// destructor
    virtual ~Formula();

private:
	/// create an output string in LoLA format
	const string output_lola() const;
};

// ------------------------- PN literal -------------------------

/*!
 * \brief a class representing a Petri net literal
 */
class PetriNetLiteral: public Formula
{
	friend class FormulaState;

public:
	/// the place of the literal
	 Place* p;
	/// the relation sign
	comparison_relation rel;
	/// the value to compare against
	int val;

	/// create an output string
	const string toString() const;

	/// create a new Petri net place literal
	PetriNetLiteral(Place *aPlace, comparison_relation aRel, int aVal);

    /// destructor
    virtual ~PetriNetLiteral();

private:
	/// create an output string in LoLA format
	const string output_lola() const;
};


// ------------------------- state formula -------------------------

/*!
 * \brief logical connectives of formulae
 */
typedef enum
{
	FORMAT_LOLA_FORMULA,
	FORMAT_LOLA_STATEPREDICATE
} formula_output_format;


/*!
 * \brief a class representing state-formulae over
 *        a class of literals and state-formulae
 */
class FormulaState: public Formula
{
public:
	/// the sub-formulas of this formula
	set<Formula *> subFormulas;
	/// set the logical connective of this formula
	void set_connective (logical_connective aConn);
	/// set the quantifier of this formula
	void set_quantifier (logical_quantifier aQuant);

	/// set the output format for this formula object
	void set_format (formula_output_format aFormat);

	/// number of sub formulas of this formula
	unsigned int size() const;

  /// outputs the formula
  friend ostream& operator<< (ostream& os, const FormulaState &obj);

  /// constructor for formula with boolean connectives
  FormulaState(logical_connective aConn);
  /// constructor for quantified formula
  FormulaState(logical_quantifier aQuant);

  /// destructor
  virtual ~FormulaState();

private:
	/// their common logical connective
	logical_connective op;
	/// their common logical quantifier
	logical_quantifier quant;

	/// output format for this formula
	formula_output_format format;

  /// output LoLA formula
  void output_lola_complete(ostream *output) const;
  /// output LoLA formula without preceding header
  const string output_lola() const;
};


#endif /*PETRINETFORMULA_H_*/

/*!
 * \defgroup petrinet Petri Net API
 *
 * \author Niels Lohmann <nlohmann@informatik.hu-berlin.de>
 *
 * All functions needed to organize a Petri net representation that can be
 * written to several output file formats and that supports structural
 * reduction rules.
 */
