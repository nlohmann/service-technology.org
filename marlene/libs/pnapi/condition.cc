#include "config.h"
#include <cassert>

#ifndef NDEBUG
#include <iostream>
#include "myio.h"
using std::cout;
using std::endl;
using pnapi::io::util::operator<<;
#endif

#include "marking.h"
#include "condition.h"
#include "petrinet.h"
#include "util.h"

using std::string;
using std::map;
using std::set;

using pnapi::formula::Formula;
using pnapi::formula::Proposition;
using pnapi::formula::FormulaTrue;
using pnapi::formula::FormulaFalse;
using pnapi::formula::FormulaEqual;
using pnapi::formula::FormulaNotEqual;
using pnapi::formula::FormulaGreater;
using pnapi::formula::FormulaGreaterEqual;
using pnapi::formula::FormulaLess;
using pnapi::formula::FormulaLessEqual;
using pnapi::formula::Conjunction;
using pnapi::formula::Disjunction;
using pnapi::formula::Negation;

namespace pnapi
{

/**************************************************************************
 ***** Overloaded operators for formulas
 *************************************************************************/

FormulaEqual operator==(const Place & p, unsigned int k)
{
  return FormulaEqual(p, k);
}

FormulaNotEqual operator!=(const Place & p, unsigned int k)
{
  return FormulaNotEqual(p, k);
}

FormulaGreater operator>(const Place & p, unsigned int k)
{
  return FormulaGreater(p, k);
}

FormulaGreaterEqual operator>=(const Place & p, unsigned int k)
{
  return FormulaGreaterEqual(p, k);
}

FormulaLess operator<(const Place & p, unsigned int k)
{
  return FormulaLess(p, k);
}

FormulaLessEqual operator<=(const Place & p, unsigned int k)
{
  return FormulaLessEqual(p, k);
}

Conjunction operator&&(const Formula & f1, const Formula & f2)
{
  return Conjunction(f1, f2);
}

Disjunction operator||(const Formula & f1, const Formula & f2)
{
  return Disjunction(f1, f2);
}

Negation operator!(const Formula & f)
{
  return Negation(f);
}



/**************************************************************************
 ***** Condition implementation
 *************************************************************************/

Condition::Condition() :
  formula_(new FormulaTrue())
  {
  }

Condition::Condition(const Condition & c,
                     const map<const Place *, const Place *> & places) :
  formula_(c.formula().clone(&places))
{
}

Condition::~Condition()
{
  delete formula_;
}

const Formula & Condition::formula() const
{
  return *formula_;
}

void Condition::merge(const Condition & c,
    const map<const Place *, const Place *> & placeMapping)
{
  Formula * f = c.formula().clone(&placeMapping);
  *this = formula() && *f;
  delete f;
}

bool Condition::isSatisfied(const Marking & m) const
{
  bool interfaceEmpty = true;
  for(set<Place*>::iterator p = m.getPetriNet().getInterfacePlaces().begin();
       p != m.getPetriNet().getInterfacePlaces().end(); ++p)
  {
    interfaceEmpty = interfaceEmpty && (m[**p] == 0);
  }
  
  return (interfaceEmpty && formula_->isSatisfied(m));
}

Condition & Condition::operator=(const Formula & f)
{
  assert(formula_ != &f);

  delete formula_;
  formula_ = f.clone();
  return *this;
}

Condition & Condition::operator=(bool formulaTrue)
{
  delete formula_;
  formula_ = formulaTrue ? reinterpret_cast<Formula*>(new FormulaTrue)
      : reinterpret_cast<Formula*>(new FormulaFalse);
      return *this;
}

void Condition::addProposition(const Proposition & p, bool conjunct)
{
  if (conjunct)
    *this = formula() && p;
  else
    *this = formula() || p;
}

void Condition::addMarking(const Marking & m)
{
  set<const Formula *> propositions;
  for (map<const Place *, unsigned int>::const_iterator it = m.begin();
        it != m.end(); ++it)
    propositions.insert(new FormulaEqual(*it->first, it->second));

  if (dynamic_cast<FormulaTrue *>(formula_) != NULL)
    *this = Conjunction(propositions, NULL);
  else
    *this = formula() || Conjunction(propositions, NULL);

  for (set<const Formula *>::iterator it = propositions.begin();
        it != propositions.end(); ++it)
    delete *it;
}

std::set<const Place *> Condition::concerningPlaces() const
{
  return formula_->places();
}

/*!
 * \breif removes a place recursively
 */
void Condition::removePlace(const Place & p)
{
  if(formula_->removePlace(p))
  {
    delete formula_;
    formula_ = new FormulaTrue();
  }
}

void Condition::negate()
{
  *this = !formula();
}

/*!
 * \brief forces all places not concerned by the formula to be empty
 */
void Condition::allOtherPlacesEmpty(PetriNet & net)
{
  set<const Place*> coveredPlaces = formula_->places();
  set<const Place*> allPlaces;
  for(set<Place*>::iterator p = net.getInternalPlaces().begin();
       p != net.getInternalPlaces().end(); ++p)
  {
    allPlaces.insert(*p);
  }
  set<const Place*> remainingPlaces = util::setDifference(allPlaces, coveredPlaces);
  
  for(set<const Place*>::iterator p = remainingPlaces.begin();
       p != remainingPlaces.end(); ++p)
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

}
