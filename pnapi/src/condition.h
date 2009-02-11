// -*- C++ -*-

#ifndef PNAPI_CONDITION_H
#define PNAPI_CONDITION_H

#include <map>
#include <string>

namespace pnapi
{

  // forward declarations
  class Place;
  class Marking;

  namespace formula
  {

    // forward declarations
    class Formula;


    /*!
     */
    class Condition
    {
    public:
      Condition();
      Condition(Formula *f);

      /// flat copy constructor
      Condition(const Condition &c);

      /// deep copy constructor
      Condition(const Condition &c, std::map<const Place *, Place *> &newP);

      /// destructor
      virtual ~Condition() {}

      void merge(const Condition &c);

      bool checkFinalMarking(Marking &m);

      void setFormula(Formula *f);

      Condition & operator=(const Condition &);

      Condition & operator=(const Formula &);

      std::string toString() const;

    private:
      Formula *f_;
    };

  }

}

#endif
