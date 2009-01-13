#include "petrinet.h"
#include "formula.h"

namespace pnapi
{

  namespace formula
  {

  /*!
   * \brief   Atomic formula class's public methods
   */
  AtomicFormula::AtomicFormula()
  {
  }

  AtomicFormula::AtomicFormula(Place *p, unsigned int k) :
    place(p), number(k)
  {
  }

  AtomicFormula::~AtomicFormula()
  {
  }

  bool AtomicFormula::evaluate(Marking &m)
  {
    return false;
  }

  void AtomicFormula::setPlace(Place *p)
  {
    place = p;
  }

  void AtomicFormula::setNumber(unsigned int k)
  {
    number = k;
  }

  Place* AtomicFormula::getPlace()
  {
    return place;
  }

  unsigned int AtomicFormula::getNumber()
  {
    return number;
  }

  FormulaEqual::FormulaEqual()
  {
  }

  FormulaEqual::FormulaEqual(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  FormulaEqual::~FormulaEqual()
  {

  }

  bool FormulaEqual::evaluate(Marking &m)
  {
    return m[place] == number;
  }

  FormulaNotEqual::FormulaNotEqual()
  {
  }

  FormulaNotEqual::FormulaNotEqual(Place *p, unsigned int k)
  {
  }

  FormulaNotEqual::~FormulaNotEqual()
  {
  }

  bool FormulaNotEqual::evaluate(Marking &m)
  {
    return m[place] != number;
  }

  FormulaGreater::FormulaGreater()
  {
  }

  FormulaGreater::FormulaGreater(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  FormulaGreater::~FormulaGreater()
  {
  }

  bool FormulaGreater::evaluate(Marking &m)
  {
    return m[place] > number;
  }

  FormulaGreaterEqual::FormulaGreaterEqual()
  {
  }

  FormulaGreaterEqual::FormulaGreaterEqual(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  FormulaGreaterEqual::~FormulaGreaterEqual()
  {
  }

  bool FormulaGreaterEqual::evaluate(Marking &m)
  {
    return m[place] >= number;
  }

  FormulaLess::FormulaLess()
  {
  }

  FormulaLess::FormulaLess(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  FormulaLess::~FormulaLess()
  {
  }

  bool FormulaLess::evaluate(Marking &m)
  {
    return m[place] < number;
  }

  FormulaLessEqual::FormulaLessEqual()
  {
  }

  FormulaLessEqual::FormulaLessEqual(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  FormulaLessEqual::~FormulaLessEqual()
  {
  }

  bool FormulaLessEqual::evaluate(Marking &m)
  {
    return m[place] <= number;
  }

  /************************************************
   *          Unary Boolean Formulas              *
   ************************************************/

  UnaryBooleanFormula::UnaryBooleanFormula()
  {
  }

  UnaryBooleanFormula::UnaryBooleanFormula(Formula *f) :
    sub(f)
  {
  }

  UnaryBooleanFormula::~UnaryBooleanFormula()
  {
  }

  FormulaNot::FormulaNot()
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
   *           Binary Boolean Formulas            *
   ************************************************/

  BinaryBooleanFormula::BinaryBooleanFormula()
  {
  }

  BinaryBooleanFormula::BinaryBooleanFormula(Formula *l, Formula *r) :
    left(l), right(r)
  {
  }

  BinaryBooleanFormula::~BinaryBooleanFormula()
  {
  }

  FormulaAnd::FormulaAnd()
  {
  }

  FormulaAnd::FormulaAnd(Formula *l, Formula *r) :
    BinaryBooleanFormula(l, r)
  {
  }

  FormulaAnd::~FormulaAnd()
  {
  }

  bool FormulaAnd::evaluate(Marking &m)
  {
    return left->evaluate(m) && right->evaluate(m);
  }

  FormulaOr::FormulaOr()
  {
  }

  FormulaOr::FormulaOr(Formula *l, Formula *r) :
    BinaryBooleanFormula(l, r)
  {
  }

  FormulaOr::~FormulaOr()
  {
  }

  bool FormulaOr::evaluate(Marking &m)
  {
    return left->evaluate(m) || right->evaluate(m);
  }


  } /* namespace formula */

} /* namespace pnapi */
