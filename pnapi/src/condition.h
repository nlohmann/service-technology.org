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
    Condition(Formula *f);

    /// flat copy constructor
    Condition(const Condition &c);

    /// deep copy constructor
    Condition(const Condition &c, map<Place *, Place *> &newP);

    /// destructor
    virtual ~Condition() {}

    void merge(const Condition &c);

    bool checkFinalMarking(Marking &m);

    void setFormula(Formula *f);

    Condition & operator=(const Condition &);

    Condition & operator=(const Formula &);

    string toString() const;

  private:
    Formula *f_;
  };

} /* end of namespace pnapi */


#endif /* CONDITION_H */
