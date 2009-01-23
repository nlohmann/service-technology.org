#include "condition.h"

//using namespace formula;

namespace pnapi
{

  Condition::Condition() :
    f_(*new False())
  {
  }

  Condition::Condition(Formula &f) :
    f_(f)//, net_(n)
  {
  }


  Condition::Condition(const Condition &c) :
    f_(c.f_)//, net_(c.net_)
  {
  }


  void Condition::merge(const Condition &c)
  {
    Formula *copy = f_.flatCopy();
    f_ = *new FormulaAnd(f_, *copy);
  }

  bool Condition::checkFinalMarking(Marking &m)
  {
    return f_.evaluate(m);
  }

}
