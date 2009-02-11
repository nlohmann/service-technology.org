#ifndef NDEBUG
#include <iostream>
using std::cout;
using std::endl;
#endif

#include "formula.h"
#include "condition.h"

using std::string;
using std::map;

namespace pnapi
{

  namespace formula
  {

    Condition::Condition() :
      f_(new True())
    {
    }

    Condition::Condition(Formula *f) :
      f_(f)
    {
    }


    Condition::Condition(const Condition &c) :
      f_(c.f_->flatCopy())
    {
    }

    Condition::Condition(const Condition &c, map<const Place *, Place *> &newP) :
      f_(c.f_->deepCopy(newP))
    {
    }

    void Condition::merge(const Condition &c)
    {
      //Formula *copy = f_->flatCopy();
      f_ = new FormulaAnd(*f_, *c.f_);
    }

    bool Condition::checkFinalMarking(Marking &m)
    {
      return f_->evaluate(m);
    }

    void Condition::setFormula(Formula *f)
    {
      f_ = f;
    }

    Condition & Condition::operator=(const Condition & c)
    {
      return (*new Condition(c));
    }

    Condition & Condition::operator=(const Formula & f)
    {
      f_ = &f.clone();
      return *this;
    }

    string Condition::toString() const
    {
      return f_->toString();
    }

  }

}
