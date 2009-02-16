// -*- C++ -*-

#ifndef PNAPI_CONDITION_H
#define PNAPI_CONDITION_H

#include <map>
#include <set>
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

    /// returns the formula
    const formula::Formula & formula() const;

    /// standard assignment operator
    Condition & operator=(const formula::Formula &);

    /// standard assignment operator for boolean values
    Condition & operator=(bool);

    /// checks whether the formula is satisfied under marking m
    bool isSatisfied(const Marking &) const;

    /// merges two condition with each other
    void merge(const Condition &);

    /// returns a set of places, which are contained in the condition
    const std::set<std::string> & concerningPlaces() const;


  private:

    /// the formula
    formula::Formula * formula_;

  };

}

#endif
