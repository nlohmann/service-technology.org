#ifndef NDEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

#include "condition.h"

using std::string;
using std::map;

using pnapi::formula::Formula;
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
		       map<const Place *, const Place *> * places) :
    formula_(c.formula().clone(places))
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

  void Condition::merge(const Condition & c)
  {
    *this = formula() && c.formula();
  }

  bool Condition::isSatisfied(const Marking & m) const
  {
    return formula_->isSatisfied(m);
  }

  Condition & Condition::operator=(const Formula & f)
  {
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

/*  const std::set<std::string> & Condition::concerningPlaces() const
  {
    return formula_->concerningPlaces();
  }*/

}
