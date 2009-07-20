#include "config.h"
#include <cassert>

#ifndef NDEBUG
#include <iostream>
#include "io.h"
using std::cout;
using std::endl;
using pnapi::io::util::operator<<;
#endif

#include "marking.h"
#include "condition.h"

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
    return formula_->isSatisfied(m);
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
    formula_ = formulaTrue ? (Formula*) new FormulaTrue
                           : (Formula*) new FormulaFalse;
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
      *this = Conjunction(propositions);
    else
      *this = formula() || Conjunction(propositions);

    for (set<const Formula *>::iterator it = propositions.begin();
	 it != propositions.end(); ++it)
      delete *it;
  }

  std::set<const Place *> Condition::concerningPlaces() const
  {
    return formula_->places(true);
  }

  void Condition::negate()
  {
    *this = !formula();
  }

}
