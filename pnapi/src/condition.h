// -*- C++ -*-

#ifndef PNAPI_CONDITION_H
#define PNAPI_CONDITION_H

#include <map>
#include <string>

#include "formula.h"

namespace pnapi
{

  // forward declarations
  class Place;
  class Marking;
  namespace formula
  {
    class Formula;
    class Negation;
    class Conjunction;
    class Disjunction;
    class FormulaEqual;
    class FormulaNotEqual;
    class FormulaGreater;
    class FormulaGreaterEqual;
    class FormulaLess;
    class FormulaLessEqual;
  }


  /// formula construction operator
  formula::Negation operator!(const formula::Formula &);

  /// formula construction operator
  formula::Conjunction operator&&(const formula::Formula &, 
				  const formula::Formula &);

  /// formula construction operator
  formula::Disjunction operator||(const formula::Formula &, 
				  const formula::Formula &);

  /// formula construction operator
  formula::FormulaEqual operator==(const Place &, unsigned int);

  /// formula construction operator
  formula::FormulaNotEqual operator!=(const Place &, unsigned int);

  /// formula construction operator
  formula::FormulaGreater operator>(const Place &, unsigned int);

  /// formula construction operator
  formula::FormulaGreaterEqual operator>=(const Place &, unsigned int);

  /// formula construction operator
  formula::FormulaLess operator<(const Place &, unsigned int);

  /// formula construction operator
  formula::FormulaLessEqual operator<=(const Place &, unsigned int);


  /*!
   */
  class Condition
  {
  public:
      
    /// constructor
    Condition();

    /// copy constructor
    Condition(const Condition &, 
	      std::map<const Place *, const Place *> * = NULL);

    /// destructor
    virtual ~Condition();

    const formula::Formula & formula() const;

    Condition & operator=(const formula::Formula &);

    Condition & operator=(bool);

    bool isSatisfied(const Marking &) const;

    void merge(const Condition &);


  private:
      
    /// the formula
    formula::Formula * formula_;

  };

}

#endif
