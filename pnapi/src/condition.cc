#include "condition.h"

//using namespace formula;

namespace pnapi
{

  Condition::Condition() :
    f_(*new formula::True())
  {
  }


  Condition::Condition(Formula &f) :
    f_(f)
  {
  }


  Condition::Condition(const Condition &c) :
    f_(c.f_)
  {
  }


  void Condition::merge(const Condition &c)
  {
    Formula *copy = f_.clone();
    f_ = *new FormulaAnd(&f_, copy);
  }

}
