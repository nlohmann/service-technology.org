/*****************************************************************************\
  Petri net formulas
  
  Copyright (C) 2008  Dirk Fahland
  
  license: to be determined 
\*****************************************************************************/

/*!
 * \file    petrinet-formula.cc
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

#include <cassert>
#include <iostream>
#include <set>
#include <sstream> 
#include <utility>

#include "petrinet-formula.h"
#include "helpers.h"		// helper functions (setUnion, setDifference, max, toString)
#include "pnapi.h"

using std::set;
using std::cerr;
using std::stringstream;
using std::endl;

// ------------------------- formula ------------------------- 

/*!
 * \brief destructor
 */
Formula::~Formula() {
}

/*!
 * \brief constructor of a basic non-negated formula
 */
Formula::Formula() {
	negated = false;
}

/*!
 * \brief returns the empty string
 */
const string Formula::output_lola() const {
	return "";
}

// ------------------------- PN literal -------------------------

/*!
 * \brief destructor
 */
PetriNetLiteral::~PetriNetLiteral() {
}

/*!
 * \brief create a new Petri net literal  p REL val
 */
PetriNetLiteral::PetriNetLiteral(Place *aPlace, comparison_relation aRel, int aVal) :
	rel(COMPARE_GREATER),
	val(0)
{
	p = aPlace;
	rel = aRel;
	val = aVal;
}

/*!
 * \brief return a text representation of the literal in LoLA syntax
 */
const string PetriNetLiteral::output_lola() const {
	stringstream str;
	
	// negated formula?
	if (negated)
		str << "NOT ";
	
	// node name
	str << toLoLAident(p->nodeFullName()) << " ";

	// relation symbol
	string relStr = "";
	switch (rel) {
	case COMPARE_EQUAL:	relStr = "="; break;
	case COMPARE_LESS:	relStr = "<"; break;
	case COMPARE_GREATER: relStr = ">"; break;
	case COMPARE_LEQ: 	relStr = "<="; break;
	case COMPARE_GEQ:	relStr = ">="; break;
	case COMPARE_NEQ:	relStr = "<>"; break;
	default:
		relStr = ">"; break;	// default is ">"
	}
	
	// construct and return: [NOT] pName REL val
	str << relStr << " " << val;
	return str.str();
}

// ------------------------- state formula -------------------------


/*!
 * \brief	create new formula with boolean connectives
 * \param	aConn boolean connective
 */
FormulaState::FormulaState(logical_connective aConn) {
	set_connective(aConn);
}


/*!
 * \brief	create new quantified formula
 * \param	aQuant quantor of the new formula
 */
FormulaState::FormulaState(logical_quantifier aQuant) {
	set_quantifier(aQuant);
}

/*!
 * \brief destructor
 */
FormulaState::~FormulaState() {
}

/*!
 * \brief	set the logical connective for this formula, sets
 * 			quantifier to NONE
 * \param	conn logical connective
 */
void FormulaState::set_connective(logical_connective aConn) {
	op = aConn;
	quant = LOGQ_NONE;
}

/*!
 * \brief	set the quantifier for this formula, sets logical
 * 			connective to NONE
 */
void FormulaState::set_quantifier(logical_quantifier aQuant) {
	quant = aQuant;
	op = LOG_NONE;
}

/*!
 * \brief	set the the formula output format
 * 
 * \param	aFormat
 */
void FormulaState::set_format(formula_output_format aFormat)
{
	format = aFormat;
}

/*!
 * \brief   outputs the formula
 *
 *          Stream the formula object to a given output stream, using the
 *          file format set before uing FormulaState::set_format().
 *
 * \param   os  an output stream to which the net is streamed
 * \param   obj a formula object
 */
ostream& operator<< (ostream& os, const FormulaState &obj)
{
  switch (obj.format)
  {
    case(FORMAT_LOLA_FORMULA):
    case(FORMAT_LOLA_STATEPREDICATE):
      obj.output_lola_complete(&os);
      break;
    default:
      break;
  }
  return os;
}

/*!
 * \brief   outputs the formula as a state formula in LoLA format
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 */
void FormulaState::output_lola_complete(ostream *output) const
{
	assert(output != NULL);
	
	// print header of the output and contents
	switch (format) {
	  case FORMAT_LOLA_FORMULA:
	    (*output) << "FORMULA" << endl << output_lola() << endl;
	    break;
	  case FORMAT_LOLA_STATEPREDICATE:
	    (*output) << "FORMULA" << endl << output_lola() << endl;
	    break;
	}
}

/*!
 * \brief   outputs the formula as a state formula in LoLA format
 *
 * \param   output  output stream
 *
 * \pre     output != NULL
 */
const string FormulaState::output_lola() const
{
	// generate quantifier string
	bool 	quantified = false;
	string 	qStr = "";
	switch (quant) {
		case LOGQ_CTL_ALLPATH_ALWAYS:
			qStr = "ALLPATH ALWAYS "; quantified = true; break;
		case LOGQ_CTL_ALLPATH_EVENTUALLY:
			qStr = "ALLPATH EVENTUALLY "; quantified = true; break;
		case LOGQ_CTL_EXPATH_ALWAYS: 
			qStr = "EXPATH ALWAYS "; quantified = true; break;
		case LOGQ_CTL_EXPATH_EVENTUALLY: 
			qStr = "EXPATH EVENTUALLY "; quantified = true; break;
		case LOGQ_NONE:
		default:
			qStr = ""; break;
	}
	
	// generate predicate string
	stringstream str;	// create string stream
	bool first = true;
	for (set<Formula *>::iterator it = subFormulas.begin(); it != subFormulas.end(); it++)
	{
		string subStr;
		Formula* f = (*it);
		
		// a non-quantified formula connects its operands with the logical connectives
		if (!first && !quantified) {
			string opStr = "";
			switch (this->op) {
			case LOG_AND: 		opStr = "AND"; break;
			case LOG_OR:		opStr = "OR"; break;
			case LOG_IMPLIES: 	opStr = "->"; break;
			case LOG_EQUIVALENT: opStr = "<->"; break;
			case LOG_NONE:
			default:
				opStr = "";			// default operator is none
			}
			str << "\n" << opStr << " ";	// insert operator between two operands 
		}

		// append next operand
		if (typeid(*f) == typeid(FormulaState)) {
			FormulaState* fs = static_cast<FormulaState*>(f);
			subStr = fs->output_lola();
		} else if  (typeid(*f) == typeid(PetriNetLiteral)) {
			PetriNetLiteral* pl = static_cast<PetriNetLiteral*>(f);
			subStr = pl->output_lola();
		} else {
			subStr = "";
		}
		str << subStr;
		
		first = false; // next iteration is not the first iteration, need an operator after this
	}
	
	if (!first)	// is true iff the formula has at least one operand 
	{
		return qStr + "( "+str.str()+" )";	// return formula in parantheses
	}
	else
		return "";
}
