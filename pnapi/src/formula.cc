#include "petrinet.h"
#include "formula.h"

namespace pnapi
{

namespace formula 
{

/*!
 * \brief   Formula class's public methods
 */
Formula::Formula()
{
}

Formula::Formula(Formula *f) :
  sub(f)
{

}

Formula::~Formula()
{
}

bool Formula::evaluate(Marking &m)
{
  return sub->evaluate(m);
}

ftype Formula::getType()
{
  return NONE;
}

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

ftype AtomicFormula::getType()
{
  return NONE;
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
  //FIXME: return m[place->getMarkingID()] == number;
  return false;
}

ftype FormulaEqual::getType()
{
  return EQ;
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
  // FIXME: return m[place->getMarkingID()] != number;
  return false;
}

ftype FormulaNotEqual::getType()
{
  return NEQ;
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
  //FIXME: return m[place->getMarkingID()] > number;
  return false;
}

ftype FormulaGreater::getType()
{
  return G;
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
  // FIXME: return m[place->getMarkingID()] >= number;
  return false;
}

ftype FormulaGreaterEqual::getType()
{
  return GEQ;
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
  // FIXME: return m[place->getMarkingID()] < number;
  return false;
}

ftype FormulaLess::getType()
{
  return L;
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
  //FIXME: return m[place->getMarkingID()] <= number;
  return false;
}

ftype FormulaLessEqual::getType()
{
  return LEQ;
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

void UnaryBooleanFormula::setSubFormula(Formula *s)
{
  sub = s;
}

Formula* UnaryBooleanFormula::getSubFormula()
{
  return sub;
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

ftype FormulaNot::getType()
{
  return NOT;
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

void BinaryBooleanFormula::setLeft(Formula *l)
{
  left = l;
}

void BinaryBooleanFormula::setRight(Formula *r)
{
  right = r;
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

ftype FormulaAnd::getType()
{
  return AND;
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

ftype FormulaOr::getType()
{
  return OR;
}


} /* namespace formula */

} /* namespace pnapi */
