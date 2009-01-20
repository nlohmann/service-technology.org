#include <sstream>

#include "petrinet.h"
#include "marking.h"
#include "formula.h"

using std::stringstream;

namespace pnapi
{

  namespace formula
  {

  /*!
   * \brief   Atomic formula class's public methods
   */
  AtomicFormula::AtomicFormula(Place *p, unsigned int k) :
    place_(p), number_(k)
  {
  }

  AtomicFormula::AtomicFormula(const AtomicFormula &f, Place &p) :
    place_(&p), number_(f.number_)
  {
  }

  bool AtomicFormula::evaluate(Marking &m)
  {
    return false;
  }

  Place* AtomicFormula::getPlace()
  {
    return place_;
  }

  unsigned int AtomicFormula::getNumber()
  {
    return number_;
  }

  FormulaEqual::FormulaEqual(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaEqual::evaluate(Marking &m)
  {
    return m[place_] == number_;
  }

  const string FormulaEqual::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_->getName() + " = " + snumber;
  }

  FormulaNotEqual::FormulaNotEqual(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaNotEqual::evaluate(Marking &m)
  {
    return m[place_] != number_;
  }

  const string FormulaNotEqual::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_->getName() + " != " + snumber;
  }

  FormulaGreater::FormulaGreater(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaGreater::evaluate(Marking &m)
  {
    return m[place_] > number_;
  }

  const string FormulaGreater::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_->getName() + " > " + snumber;
  }

  FormulaGreaterEqual::FormulaGreaterEqual(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaGreaterEqual::evaluate(Marking &m)
  {
    return m[place_] >= number_;
  }

  const string FormulaGreaterEqual::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_->getName() + " >= " + snumber;
  }

  FormulaLess::FormulaLess(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaLess::evaluate(Marking &m)
  {
    return m[place_] < number_;
  }

  const string FormulaLess::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_->getName() + " < " + snumber;
  }

  FormulaLessEqual::FormulaLessEqual(Place *p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  bool FormulaLessEqual::evaluate(Marking &m)
  {
    return m[place_] <= number_;
  }

  const string FormulaLessEqual::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_->getName() + " <= " + snumber;
  }

  /************************************************
   *          Unary Boolean Formulas              *
   ************************************************/
  UnaryBooleanFormula::UnaryBooleanFormula(Formula *f) :
    sub_(f)
  {
  }

  UnaryBooleanFormula::UnaryBooleanFormula(const UnaryBooleanFormula &f) :
    sub_(f.sub_)
  {
  }

  FormulaNot::FormulaNot(Formula *f) :
    UnaryBooleanFormula(f)
  {
  }

  bool FormulaNot::evaluate(Marking &m)
  {
    return !sub_->evaluate(m);
  }

  const string FormulaNot::toString() const
  {
    return " NOT ( " + sub_->toString() + " ) ";
  }

  /************************************************
   *           n ary Boolean Formulas            *
   ************************************************/

  NaryBooleanFormula::NaryBooleanFormula(Formula *l, Formula *r)
  {
    subs_.push_back(l);
    subs_.push_back(r);
  }

  NaryBooleanFormula::NaryBooleanFormula(list<Formula *> &flst) :
    subs_(flst)
  {
  }

  NaryBooleanFormula::NaryBooleanFormula(const NaryBooleanFormula &f) :
    subs_(f.subs_)
  {
  }

  FormulaAnd::FormulaAnd(Formula *l, Formula *r) :
    NaryBooleanFormula(l, r)
  {
  }

  bool FormulaAnd::evaluate(Marking &m)
  {
    for (list<Formula *>::const_iterator f = subs_.begin();
        f != subs_.end(); f++)
      if (!(*f)->evaluate(m))
        return false;

    return true;
  }

  const string FormulaAnd::toString() const
  {
    string result = " ( ";
    list<Formula *>::const_iterator fl = subs_.end()--;

    for (list<Formula *>::const_iterator f = subs_.begin(); f != fl; f++)
      result.append((*f)->toString() + " AND ");

    result.append((*++fl)->toString() + " ) ");

    return result;
  }

  FormulaOr::FormulaOr(Formula *l, Formula *r) :
    NaryBooleanFormula(l, r)
  {
  }

  bool FormulaOr::evaluate(Marking &m)
  {
    for (list<Formula *>::const_iterator f = subs_.begin();
        f != subs_.end(); f++)
      if ((*f)->evaluate(m))
        return true;

    return false;
  }

  const string FormulaOr::toString() const
  {
    string result = " ( ";
    list<Formula *>::const_iterator fl = subs_.end()--;

    for (list<Formula *>::const_iterator f = subs_.begin(); f != fl; f++)
      result.append((*f)->toString() + " OR ");

    result.append((*++fl)->toString() + " ) ");

    return result;
  }

  /************************************************
   *                Empty Formula                 *
   ************************************************/

  bool True::evaluate(Marking &m)
  {
    return true;
  }

  const string True::toString() const
  {
    return "TRUE";
  }


  } /* namespace formula */

} /* namespace pnapi */
