#include "condition.h"

#include <iostream>
using std::cout;
using std::endl;

//using namespace formula;

namespace pnapi
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
    f_(c.f_)
  {
  }

  void Condition::merge(const Condition &c)
  {
    Formula *copy = f_->flatCopy();
    f_ = new FormulaAnd(copy, c.f_);
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
    setFormula(f.flatCopy());
    return *this;
  }

  string Condition::toString() const
  {
    return f_->toString();
  }

}
