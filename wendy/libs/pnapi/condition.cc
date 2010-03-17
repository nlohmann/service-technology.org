/*!
 * \file  condition.cc
 */

#include "config.h"

#include "condition.h"
#include "formula.h"
#include "marking.h"
#include "petrinet.h"
#include "util.h"

#ifndef NDEBUG
#include <iostream>

using std::cout;
using std::endl;
using pnapi::io::util::operator<<;
#endif /* NDEBUG */

using std::string;
using std::map;
using std::set;



namespace pnapi
{

using formula::Formula;
using formula::Proposition;
using formula::FormulaTrue;
using formula::FormulaFalse;
using formula::FormulaEqual;
using formula::FormulaNotEqual;
using formula::FormulaGreater;
using formula::FormulaGreaterEqual;
using formula::FormulaLess;
using formula::FormulaLessEqual;
using formula::Conjunction;
using formula::Disjunction;
using formula::Negation;


/**************************************************************************
 ***** Condition implementation
 *************************************************************************/

/*!
 * \brief constructor
 */
Condition::Condition() :
  formula_(new FormulaTrue())
{
}

/*!
 * \brief copy constructor
 */
Condition::Condition(const Condition & c,
                     const std::map<const Place *, const Place *> & places) :
  formula_(c.getFormula().clone(&places))
{
}

/*!
 * \brief destructor
 */                     
Condition::~Condition()
{
  delete formula_;
}

/*!
 * \brief returns the formula
 */
const Formula & Condition::getFormula() const
{
  return *formula_;
}

/*!
 * \brief conjuncts two conditions
 */
void Condition::conjunct(const Condition & c,
                         const std::map<const Place *, const Place *> & placeMapping)
{
  Formula * f = c.getFormula().clone(&placeMapping);
  *this = ((*formula_) && (*f));
  delete f;
}

/*!
 * \brief checks whether the formula is satisfied under a given marking
 */
bool Condition::isSatisfied(const Marking & m) const
{
  return formula_->isSatisfied(m);
}

/*!
 * \brief standard assignment operator
 */
Condition & Condition::operator=(const Formula & f)
{
  assert(formula_ != &f);

  delete formula_;
  formula_ = f.clone();
  
  return *this;
}

/*!
 * \brief standard assignment operator for boolean values
 */
Condition & Condition::operator=(bool formulaTrue)
{
  delete formula_;
  formula_ = (formulaTrue ?
              static_cast<Formula *>(new FormulaTrue) :
              static_cast<Formula *>(new FormulaFalse));
  
  return *this;
}

/*!
 * \brief adds a proposition
 * 
 * \param p the proposition
 * \param conjunct if true, the proposition is added by conjunction (default),
 *                 else the proposition is added by discunction
 */
void Condition::addProposition(const Proposition & p, bool conjunct)
{
  if (conjunct)
  {
    (*this) = ((*formula_) && p);
  }
  else
  {
    (*this) = ((*formula_) || p);
  }
}

/*!
 * \brief creates a condition for the given marking and adds it
 */
void Condition::addMarking(const Marking & m)
{
  set<const Formula *> propositions;
  set<Place *> places = m.getPetriNet().getPlaces();
  
  PNAPI_FOREACH(p, places)
  {
    propositions.insert(new FormulaEqual(**p, m[**p]));
  }

  if (dynamic_cast<FormulaTrue *>(formula_) != NULL)
  {
    (*this) = Conjunction(propositions, NULL);
  }
  else
  {
    (*this) = ((*formula_) || Conjunction(propositions, NULL));
  }

  PNAPI_FOREACH(it, propositions)
  {
    delete *it;
  }
}

/*!
 * \brief returns a set of places, which don't have to be empty
 */
std::set<const Place *> Condition::concerningPlaces() const
{
  return formula_->getPlaces();
}

/*!
 * \brief removes a place recursively
 */
void Condition::removePlace(const Place & p)
{
  if(formula_->removePlace(p))
  {
    delete formula_;
    formula_ = new FormulaTrue();
  }
}

/*!
 * \brief negates the formula
 */
void Condition::negate()
{
  (*this) = !getFormula();
}

/*!
 * \brief forces all places not mentioned by the formula to be empty
 */
void Condition::allOtherPlacesEmpty(PetriNet & net)
{
  set<const Place *> coveredPlaces = formula_->getPlaces();
  set<const Place *> allPlaces;
  PNAPI_FOREACH(p, net.getPlaces())
  {
    allPlaces.insert(*p);
  }
  
  set<const Place *> remainingPlaces = util::setDifference(allPlaces, coveredPlaces);
  
  PNAPI_FOREACH(p, remainingPlaces)
  {
    (*this) = (*formula_ && (**p == 0));
  }
}

/*!
 * \brief removes all negations
 */
void Condition::removeNegation()
{
  Formula * tmp = formula_->removeNegation();
  delete formula_;
  formula_ = tmp;
}

/*!
 * \brief calculate disjunctive normal form
 */
void Condition::dnf()
{
  removeNegation();
  Formula * tmp = formula_->dnf();
  delete formula_;
  formula_ = tmp;
}

} /* namespace pnapi */
