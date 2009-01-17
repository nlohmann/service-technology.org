#include "petrinet.h"
#include "formula.h"

namespace pnapi
{

  namespace formula
  {

  /*!
   * \brief   Atomic formula class's public methods
   */
  AtomicFormula::AtomicFormula(Place *p, unsigned int k) :
    place(p), number(k)
  {
  }

  bool AtomicFormula::evaluate(Marking &m)
  {
    return false;
  }

  Place* AtomicFormula::getPlace()
  {
    return place;
  }

  unsigned int AtomicFormula::getNumber()
  {
    return number;
  }

  FormulaEqual::FormulaEqual(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaEqual::evaluate(Marking &m)
  {
    return m[place] == number;
  }

  bool FormulaNotEqual::evaluate(Marking &m)
  {
    return m[place] != number;
  }

  FormulaGreater::FormulaGreater(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaGreater::evaluate(Marking &m)
  {
    return m[place] > number;
  }

  FormulaGreaterEqual::FormulaGreaterEqual(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaGreaterEqual::evaluate(Marking &m)
  {
    return m[place] >= number;
  }

  FormulaLess::FormulaLess(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaLess::evaluate(Marking &m)
  {
    return m[place] < number;
  }

  FormulaLessEqual::FormulaLessEqual(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaLessEqual::evaluate(Marking &m)
  {
    return m[place] <= number;
  }

  /************************************************
   *          Unary Boolean Formulas              *
   ************************************************/
  UnaryBooleanFormula::UnaryBooleanFormula(Formula *f) :
    sub(f)
  {
  }

  FormulaNot::FormulaNot(Formula *f) :
    UnaryBooleanFormula(f)
  {
  }

  bool FormulaNot::evaluate(Marking &m)
  {
    return !sub->evaluate(m);
  }

  /************************************************
   *           n ary Boolean Formulas            *
   ************************************************/

  NaryBooleanFormula::NaryBooleanFormula(Formula *l, Formula *r)
  {
    subs.push_back(l);
    subs.push_back(r);
  }

  FormulaAnd::FormulaAnd(Formula *l, Formula *r) :
    NaryBooleanFormula(l, r)
  {
  }

  bool FormulaAnd::evaluate(Marking &m)
  {
    for (list<Formula *>::const_iterator f = subs.begin(); f != subs.end(); f++)
      if (!(*f)->evaluate(m))
        return false;

    return true;
  }

  FormulaOr::FormulaOr(Formula *l, Formula *r) :
    NaryBooleanFormula(l, r)
  {
  }

  bool FormulaOr::evaluate(Marking &m)
  {
    for (list<Formula *>::const_iterator f = subs.begin(); f != subs.end(); f++)
      if ((*f)->evaluate(m))
        return true;

    return false;
  }

  /************************************************
   *                Empty Formula                 *
   ************************************************/

  bool True::evaluate(Marking &m)
  {
    return true;
  }


  } /* namespace formula */

} /* namespace pnapi */
