#ifndef CONDITION_H
#define CONDITION_H

#include "formula.h"
#include "marking.h"

using namespace pnapi::formula;


namespace pnapi
{

  class Condition
  {
  public:
    Condition();
    Condition(Formula &f);
    Condition(const Condition &c);
    virtual ~Condition() {}

    void merge(const Condition &c);

    bool checkFinalMarking(Marking &m);

  private:
    Formula &f_;
  };

} /* end of namespace pnapi */


#endif /* CONDITION_H */
