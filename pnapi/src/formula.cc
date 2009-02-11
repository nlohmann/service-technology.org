#ifndef NDEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

#include <sstream>

#include "petrinet.h"
#include "marking.h"
#include "formula.h"

using std::stringstream;
using std::string;
using std::list;
using std::map;

namespace pnapi
{

  namespace formula
  {

    /**************************************************************************
     ***** Formula: clone() implementations
     **************************************************************************/

    True & True::clone() const
    {
      return *new True(*this);
    }

    False & False::clone() const
    {
      return *new False(*this);
    }

    FormulaEqual & FormulaEqual::clone() const
    {
      return *new FormulaEqual(*this);
    }
    
    FormulaNotEqual & FormulaNotEqual::clone() const
    {
      return *new FormulaNotEqual(*this);
    }

    FormulaGreater & FormulaGreater::clone() const
    {
      return *new FormulaGreater(*this);
    }

    FormulaGreaterEqual & FormulaGreaterEqual::clone() const
    {
      return *new FormulaGreaterEqual(*this);
    }

    FormulaLess & FormulaLess::clone() const
    {
      return *new FormulaLess(*this);
    }

    FormulaLessEqual & FormulaLessEqual::clone() const
    {
      return *new FormulaLessEqual(*this);
    }

    FormulaNot & FormulaNot::clone() const
    {
      return *new FormulaNot(sub_.clone());
    }

    FormulaAnd & FormulaAnd::clone() const
    {
      return *new FormulaAnd(cloneChildren());
    }

    FormulaOr & FormulaOr::clone() const
    {
      return *new FormulaOr(cloneChildren());
    }

    list<const Formula *> NaryBooleanFormula::cloneChildren() const
    {
      list<const Formula *> clones;
      for (list<const Formula *>::const_iterator it = subs_.begin(); 
	   it != subs_.end(); ++it)
	clones.push_back(&(*it)->clone());
      return clones;
    }



    Formula::~Formula()
    {
    }

  
  /*!
   * Atomic formula class's public methods
   */

  /*!
   * \brief
   */
  AtomicFormula::AtomicFormula(const Place & p, unsigned int k) :
    place_(p), number_(k)
  {
  }

  /*!
   * \brief
   */
    /*
  AtomicFormula::AtomicFormula(const AtomicFormula &f) :
    place_(f.place_), number_(f.number_)
  {
  }
    */

  /*!
   * \brief
   */
  const Place & AtomicFormula::getPlace()
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
  FormulaEqual::FormulaEqual(const Place & p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaEqual::evaluate(const Marking & m) const
  {
    return m[place_] == number_;
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
  FormulaEqual * FormulaEqual::deepCopy(map<const Place *, Place *> & newP) const
  {
    return new FormulaEqual(*newP.find(&place_)->second, number_);
  }
  
  /*!
   * \brief
   */
  FormulaNotEqual::FormulaNotEqual(const Place & p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaNotEqual::evaluate(const Marking &m) const
  {
    return m[place_] != number_;
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
  FormulaNotEqual * FormulaNotEqual::deepCopy(map<const Place *, Place *> &newP) const
  {
    return new FormulaNotEqual(*newP[&place_], number_);
  }

  /*!
   * \brief
   */
  FormulaGreater::FormulaGreater(const Place & p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaGreater::evaluate(const Marking &m) const
  {
    cout << "DEBUG: " << place_.getName() << " (";
    cout << m[place_] << ") > " << number_ << "?\n";
    return m[place_] > number_;
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
  FormulaGreater * FormulaGreater::deepCopy(map<const Place *, Place *> &newP) const
  {
    return new FormulaGreater(*newP[&place_], number_);
  }

  /*!
   * \brief
   */
  FormulaGreaterEqual::FormulaGreaterEqual(const Place & p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaGreaterEqual::evaluate(const Marking &m) const
  {
    return m[place_] >= number_;
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
  FormulaGreaterEqual * FormulaGreaterEqual::deepCopy(map<const Place *, Place *> &newP) const
  {
    return new FormulaGreaterEqual(*newP[&place_], number_);
  }

  /*!
   * \brief
   */
  FormulaLess::FormulaLess(const Place & p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaLess::evaluate(const Marking &m) const
  {
    return m[place_] < number_;
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
  FormulaLess * FormulaLess::deepCopy(map<const Place *, Place *> &newP) const
  {
    return new FormulaLess(*newP[&place_], number_);
  }

  /*!
   * \brief
   */
  FormulaLessEqual::FormulaLessEqual(const Place & p, unsigned int k) :
    AtomicFormula(p, k)
  {
  }

  /*!
   * \brief
   */
  bool FormulaLessEqual::evaluate(const Marking &m) const
  {
    return m[place_] <= number_;
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
  FormulaLessEqual * FormulaLessEqual::deepCopy(map<const Place *, Place *> &newP) const
  {
    return new FormulaLessEqual(*newP[&place_], number_);
  }

  /************************************************
   *          Unary Boolean Formulas              *
   ************************************************/

  /*!
   * \brief
   */
  UnaryBooleanFormula::UnaryBooleanFormula(const Formula & f) :
    sub_(f)
  {
  }

  /*!
   * \brief
   */
    /*
  UnaryBooleanFormula::UnaryBooleanFormula(const UnaryBooleanFormula &f) :
    sub_(f.sub_)
  {
  }
    */

  FormulaNot::FormulaNot(const Formula & f) :
    UnaryBooleanFormula(f)
  {
  }

  bool FormulaNot::evaluate(const Marking & m) const
  {
    return !sub_.evaluate(m);
  }

  const string FormulaNot::toString() const
  {
    return " NOT ( " + sub_.toString() + " ) ";
  }

  FormulaNot * FormulaNot::flatCopy() const
  {
    return new FormulaNot(*this);
  }

  FormulaNot * FormulaNot::deepCopy(map<const Place *, Place *> &newP) const
  {
    return new FormulaNot(*sub_.deepCopy(newP));
  }

  /************************************************
   *           n ary Boolean Formulas            *
   ************************************************/

  /*!
   * \brief
   */
  NaryBooleanFormula::NaryBooleanFormula(const Formula & l, const Formula & r)
  {
    subs_.push_back(&l);
    subs_.push_back(&r);
  }

  /*!
   * \brief
   */
  NaryBooleanFormula::NaryBooleanFormula(const list<const Formula *> & flst) :
    subs_(flst)
  {
  }

  /*!
   * \brief
   */
    /*
  NaryBooleanFormula::NaryBooleanFormula(const NaryBooleanFormula &f) :
    subs_(f.subs_)
  {
  }
    */

  /*!
   * \brief
   */
  void NaryBooleanFormula::addSubFormula(Formula *s)
  {
    subs_.push_back(s);
  }



    /**************************************************************************
     ***** Formula: AND
     **************************************************************************/

  FormulaAnd::FormulaAnd(const Formula & l, const Formula & r) :
    NaryBooleanFormula(l, r)
  {
  }

  FormulaAnd::FormulaAnd(const list<const Formula *> & flst) :
    NaryBooleanFormula(flst)
  {
  }

  bool FormulaAnd::evaluate(const Marking & m) const
  {
    for (list<const Formula *>::const_iterator f = subs_.begin();
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

    for (list<const Formula *>::const_iterator f = subs_.begin();
        f != subs_.end(); f++)
    {
      result.append((*f)->toString());
      list<const Formula *>::const_iterator g = (++f)--;
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
  FormulaAnd * FormulaAnd::deepCopy(map<const Place *, Place *> &newP) const
  {
    list<const Formula *> newSubs;
    for (list<const Formula *>::const_iterator f = subs_.begin(); f != subs_.end();
        f++)
    {
      newSubs.push_back((*f)->deepCopy(newP));
    }

    return new FormulaAnd(newSubs);
  }



    /**************************************************************************
     ***** Formula: OR
     **************************************************************************/

  FormulaOr::FormulaOr(const Formula & l, const Formula & r) :
    NaryBooleanFormula(l, r)
  {
  }

  FormulaOr::FormulaOr(const list<const Formula *> & flst) :
    NaryBooleanFormula(flst)
  {
  }

  bool FormulaOr::evaluate(const Marking & m) const
  {
    for (list<const Formula *>::const_iterator f = subs_.begin();
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

    for (list<const Formula *>::const_iterator f = subs_.begin();
        f != subs_.end(); f++)
    {
      result.append((*f)->toString());
      list<const Formula *>::const_iterator g = (++f)--;
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
  FormulaOr * FormulaOr::deepCopy(map<const Place *, Place *> &newP) const
  {
    list<const Formula *> newSubs;
    for (list<const Formula *>::const_iterator f = subs_.begin(); f != subs_.end();
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
  bool True::evaluate(const Marking &m) const
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
  True * True::deepCopy(map<const Place *, Place *> &newP) const
  {
    return new True();
  }

  /*!
   * \brief
   */
  bool False::evaluate(const Marking &m) const
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
  False * False::deepCopy(map<const Place *, Place *> &newP) const
  {
    return new False();
  }



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

    FormulaAnd operator&&(const Formula & f1, const Formula & f2)
    {
      return FormulaAnd(f1, f2);
    }

    FormulaOr operator||(const Formula & f1, const Formula & f2)
    {
      return FormulaOr(f1, f2);
    }

    FormulaNot operator!(const Formula & f)
    {
      return FormulaNot(f);
    }

  }

}
