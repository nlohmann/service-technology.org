#include <sstream>

#include <iostream>
using std::cout;

#include "petrinet.h"
#include "marking.h"
#include "formula.h"

using std::stringstream;

namespace pnapi
{

  namespace formula
  {

  /*!
   * Atomic formula class's public methods
   */

  /*!
   * \brief
   */
  AtomicFormula::AtomicFormula(Place &p, unsigned int k) :
    place_(p), number_(k)
  {
  }

  /*!
   * \brief
   */
  AtomicFormula::AtomicFormula(const AtomicFormula &f) :
    place_(f.place_), number_(f.number_)
  {
  }

  /*!
   * \brief
   */
  Place & AtomicFormula::getPlace()
  {
    return place_;
  }

  /*!
   * \brief
   */
  unsigned int AtomicFormula::getNumber()
  {
    return number_;
  }

  /*!
   * \brief
   */
  FormulaEqual::FormulaEqual(Place &p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaEqual::evaluate(Marking &m) const
  {
    return m[&place_] == number_;
  }

  /*!
   * \brief
   */
  const string FormulaEqual::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_.getName() + " = " + snumber;
  }

  /*!
   * \brief
   */
  FormulaEqual * FormulaEqual::flatCopy() const
  {
    return new FormulaEqual(*this);
  }

  /*!
   * \brief
   */
  FormulaEqual * FormulaEqual::deepCopy(map<Place *, Place *> & newP) const
  {
    return new FormulaEqual(*newP[&place_], number_);
  }

  /*!
   * \brief
   */
  FormulaNotEqual::FormulaNotEqual(Place &p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaNotEqual::evaluate(Marking &m) const
  {
    return m[&place_] != number_;
  }

  /*!
   * \brief
   */
  const string FormulaNotEqual::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_.getName() + " != " + snumber;
  }

  /*!
   * \brief
   */
  FormulaNotEqual * FormulaNotEqual::flatCopy() const
  {
    return new FormulaNotEqual(*this);
  }

  /*!
   * \brief
   */
  FormulaNotEqual * FormulaNotEqual::deepCopy(map<Place *, Place *> &newP) const
  {
    return new FormulaNotEqual(*newP[&place_], number_);
  }

  /*!
   * \brief
   */
  FormulaGreater::FormulaGreater(Place &p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaGreater::evaluate(Marking &m) const
  {
    cout << "DEBUG: " << place_.getName() << " (";
    cout << m[&place_] << ") > " << number_ << "?\n";
    return m[&place_] > number_;
  }

  /*!
   * \brief
   */
  const string FormulaGreater::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_.getName() + " > " + snumber;
  }

  /*!
   * \brief
   */
  FormulaGreater * FormulaGreater::flatCopy() const
  {
    return new FormulaGreater(*this);
  }

  /*!
   * \brief
   */
  FormulaGreater * FormulaGreater::deepCopy(map<Place *, Place *> &newP) const
  {
    return new FormulaGreater(*newP[&place_], number_);
  }

  /*!
   * \brief
   */
  FormulaGreaterEqual::FormulaGreaterEqual(Place &p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaGreaterEqual::evaluate(Marking &m) const
  {
    return m[&place_] >= number_;
  }

  /*!
   * \brief
   */
  const string FormulaGreaterEqual::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_.getName() + " >= " + snumber;
  }

  /*!
   * \brief
   */
  FormulaGreaterEqual * FormulaGreaterEqual::flatCopy() const
  {
    return new FormulaGreaterEqual(*this);
  }

  /*!
   * \brief
   */
  FormulaGreaterEqual * FormulaGreaterEqual::deepCopy(map<Place *, Place *> &newP) const
  {
    return new FormulaGreaterEqual(*newP[&place_], number_);
  }

  /*!
   * \brief
   */
  FormulaLess::FormulaLess(Place &p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaLess::evaluate(Marking &m) const
  {
    return m[&place_] < number_;
  }

  /*!
   * \brief
   */
  const string FormulaLess::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_.getName() + " < " + snumber;
  }

  /*!
   * \brief
   */
  FormulaLess * FormulaLess::flatCopy() const
  {
    return new FormulaLess(*this);
  }

  /*!
   * \brief
   */
  FormulaLess * FormulaLess::deepCopy(map<Place *, Place *> &newP) const
  {
    return new FormulaLess(*newP[&place_], number_);
  }

  /*!
   * \brief
   */
  FormulaLessEqual::FormulaLessEqual(Place &p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaLessEqual::evaluate(Marking &m) const
  {
    return m[&place_] <= number_;
  }

  /*!
   * \brief
   */
  const string FormulaLessEqual::toString() const
  {
    string snumber;
    stringstream sstream;
    sstream << number_;
    sstream >> snumber;

    return place_.getName() + " <= " + snumber;
  }

  /*!
   * \brief
   */
  FormulaLessEqual * FormulaLessEqual::flatCopy() const
  {
    return new FormulaLessEqual(*this);
  }

  /*!
   * \brief
   */
  FormulaLessEqual * FormulaLessEqual::deepCopy(map<Place *, Place *> &newP) const
  {
    return new FormulaLessEqual(*newP[&place_], number_);
  }

  /************************************************
   *          Unary Boolean Formulas              *
   ************************************************/

  /*!
   * \brief
   */
  UnaryBooleanFormula::UnaryBooleanFormula(Formula *f) :
    sub_(f)
  {
  }

  /*!
   * \brief
   */
  UnaryBooleanFormula::UnaryBooleanFormula(const UnaryBooleanFormula &f) :
    sub_(f.sub_)
  {
  }

  /*!
   * \brief
   */
  FormulaNot::FormulaNot(Formula *f) :
    UnaryBooleanFormula(f)
  {
  }

  /*!
   * \brief
   */
  FormulaNot::FormulaNot(const Formula & f) :
    UnaryBooleanFormula(f.flatCopy())
  {
  }

  /*!
   * \brief
   */
  bool FormulaNot::evaluate(Marking &m) const
  {
    return !sub_->evaluate(m);
  }

  /*!
   * \brief
   */
  const string FormulaNot::toString() const
  {
    return " NOT ( " + sub_->toString() + " ) ";
  }

  /*!
   * \brief
   */
  FormulaNot * FormulaNot::flatCopy() const
  {
    return new FormulaNot(*this);
  }

  /*!
   * \brief
   */
  FormulaNot * FormulaNot::deepCopy(map<Place *, Place *> &newP) const
  {
    return new FormulaNot(sub_->deepCopy(newP));
  }

  /************************************************
   *           n ary Boolean Formulas            *
   ************************************************/

  /*!
   * \brief
   */
  NaryBooleanFormula::NaryBooleanFormula(Formula *l, Formula *r)
  {
    subs_.push_back(l);
    subs_.push_back(r);
  }

  /*!
   * \brief
   */
  NaryBooleanFormula::NaryBooleanFormula(list<Formula *> &flst) :
    subs_(flst)
  {
  }

  /*!
   * \brief
   */
  NaryBooleanFormula::NaryBooleanFormula(const NaryBooleanFormula &f) :
    subs_(f.subs_)
  {
  }

  /*!
   * \brief
   */
  void NaryBooleanFormula::addSubFormula(Formula *s)
  {
    subs_.push_back(s);
  }

  /*!
   * \brief
   */
  FormulaAnd::FormulaAnd(Formula *l, Formula *r) :
    NaryBooleanFormula(l, r)
  {
  }

  /*!
   * \brief
   */
  FormulaAnd::FormulaAnd(const Formula & l, const Formula & r) :
    NaryBooleanFormula(l.flatCopy(), r.flatCopy())
  {
  }

  /*!
   * \brief
   */
  FormulaAnd::FormulaAnd(list<Formula *> &flst) :
    NaryBooleanFormula(flst)
  {
  }

  /*!
   * \brief
   */
  bool FormulaAnd::evaluate(Marking &m) const
  {
    for (list<Formula *>::const_iterator f = subs_.begin();
        f != subs_.end(); f++)
      if (!(*f)->evaluate(m))
        return false;

    return true;
  }

  /*!
   * \brief
   */
  const string FormulaAnd::toString() const
  {
    string result = " ( ";

    for (list<Formula *>::const_iterator f = subs_.begin();
        f != subs_.end(); f++)
    {
      result.append((*f)->toString());
      list<Formula *>::const_iterator g = (++f)--;
      if (g != subs_.end())
        result.append(" AND ");
    }

    result.append(" ) ");

    return result;
  }

  /*!
   * \brief
   */
  FormulaAnd * FormulaAnd::flatCopy() const
  {
    return new FormulaAnd(*this);
  }

  /*!
   * \brief
   */
  FormulaAnd * FormulaAnd::deepCopy(map<Place *, Place *> &newP) const
  {
    list<Formula *> newSubs;
    for (list<Formula *>::const_iterator f = subs_.begin(); f != subs_.end();
        f++)
    {
      newSubs.push_back((*f)->deepCopy(newP));
    }

    return new FormulaAnd(newSubs);
  }

  /*!
   * \brief
   */
  FormulaOr::FormulaOr(Formula *l, Formula *r) :
    NaryBooleanFormula(l, r)
  {
  }

  /*!
   * \brief
   */
  FormulaOr::FormulaOr(const Formula & l, const Formula & r) :
    NaryBooleanFormula(l.flatCopy(), r.flatCopy())
  {
  }

  /*!
   * \brief
   */
  FormulaOr::FormulaOr(list<Formula *> &flst) :
    NaryBooleanFormula(flst)
  {
  }

  /*!
   * \brief
   */
  bool FormulaOr::evaluate(Marking &m) const
  {
    for (list<Formula *>::const_iterator f = subs_.begin();
        f != subs_.end(); f++)
      if ((*f)->evaluate(m))
        return true;

    return false;
  }

  /*!
   * \brief
   */
  const string FormulaOr::toString() const
  {
    string result = " ( ";

    for (list<Formula *>::const_iterator f = subs_.begin();
        f != subs_.end(); f++)
    {
      result.append((*f)->toString());
      list<Formula *>::const_iterator g = (++f)--;
      if (g != subs_.end())
        result.append(" OR ");
    }

    result.append(" ) ");

    return result;
  }

  /*!
   * \brief
   */
  FormulaOr * FormulaOr::flatCopy() const
  {
    return new FormulaOr(*this);
  }

  /*!
   * \brief
   */
  FormulaOr * FormulaOr::deepCopy(map<Place *, Place *> &newP) const
  {
    list<Formula *> newSubs;
    for (list<Formula *>::const_iterator f = subs_.begin(); f != subs_.end();
        f++)
    {
      newSubs.push_back((*f)->deepCopy(newP));
    }

    return new FormulaOr(newSubs);
  }

  /************************************************
   *                Empty Formula                 *
   ************************************************/

  /*!
   * \brief
   */
  bool True::evaluate(Marking &m) const
  {
    return true;
  }

  /*!
   * \brief
   */
  const string True::toString() const
  {
    return "TRUE";
  }

  /*!
   * \brief
   */
  True * True::flatCopy() const
  {
    return new True();
  }

  /*!
   * \brief
   */
  True * True::deepCopy(map<Place *, Place *> &newP) const
  {
    return new True();
  }

  /*!
   * \brief
   */
  bool False::evaluate(Marking &m) const
  {
    return false;
  }

  /*!
   * \brief
   */
  const string False::toString() const
  {
    return "FALSE";
  }

  /*!
   * \brief
   */
  False * False::flatCopy() const
  {
    return new False();
  }

  /*!
   * \brief
   */
  False * False::deepCopy(map<Place *, Place *> &newP) const
  {
    return new False();
  }


  /*!
   * Overloaded operators for formulas
   */

  /*!
   */
  FormulaEqual operator==(Place & p, unsigned int k)
  {
    return FormulaEqual(p, k);
  }


  /*!
   */
  FormulaNotEqual operator!=(Place & p, unsigned int k)
  {
    return FormulaNotEqual(p, k);
  }


  /*!
   */
  FormulaGreater operator>(Place & p, unsigned int k)
  {
    return FormulaGreater(p, k);
  }


  /*!
   */
  FormulaGreaterEqual operator>=(Place & p, unsigned int k)
  {
    return FormulaGreaterEqual(p, k);
  }


  /*!
   */
  FormulaLess operator<(Place & p, unsigned int k)
  {
    return FormulaLess(p, k);
  }


  /*!
   */
  FormulaLessEqual operator<=(Place & p, unsigned int k)
  {
    return FormulaLessEqual(p, k);
  }


  /*!
   */
  FormulaAnd operator&&(const Formula & f1, const Formula & f2)
  {
    return FormulaAnd(f1, f2);
  }


  /*!
   */
  FormulaOr operator||(const Formula & f1, const Formula & f2)
  {
    return FormulaOr(f1, f2);
  }


  /*!
   */
  FormulaNot operator!(const Formula & f)
  {
    return FormulaNot(f);
  }


  } /* namespace formula */

} /* namespace pnapi */
