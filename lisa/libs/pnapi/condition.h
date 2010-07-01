// -*- C++ -*-

/*!
 * \file  condition.h
 */

#ifndef PNAPI_CONDITION_H
#define PNAPI_CONDITION_H

#include "config.h"

#include "formula.h"

#include <map>
#include <set>
#include <string>

namespace pnapi
{

// forward declarations
class Place;
class Marking;


/*!
 * \brief Final Condition Class
 */
class Condition
{
private: /* private variables */
  /// the formula
  formula::Formula * formula_;
  
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// constructor
  Condition();
  /// copy constructor
  Condition(const Condition &, const std::map<const Place *, const Place *> &);
  /// destructor
  virtual ~Condition();
  //@}

  /*!
   * \name structural changes
   */
  //@{
  /// standard assignment operator
  Condition & operator=(const formula::Formula &);
  /// standard assignment operator for boolean values
  Condition & operator=(bool);
  /// conjuncts two conditions
  void conjunct(const Condition &, const std::map<const Place *, const Place *> &);
  /// adds a proposition
  void addProposition(const formula::Proposition &, bool = true);
  /// creates a condition for the given marking and adds it
  void addMarking(const Marking &);
  /// removes a place recursively
  void removePlace(const Place &);
  /// negates the formula
  void negate();
  /// calculate disjunctive normal form
  void dnf();
  /// forces all places not mentioned by the formula to be empty
  void allOtherPlacesEmpty(PetriNet&);
  /// evaluates the formula partially and replaces propositions by constants
  void evaluatePlace(const Place &);
  /// replace place references
  void replacePlace(const Place &, const Place &);
  //@}
  
  /*!
   * \name getters
   */
  //@{
  /// returns the formula
  const formula::Formula & getFormula() const;
  /// checks whether the formula is satisfied under a given marking
  bool isSatisfied(const Marking &) const;
  /// returns a set of places, which don't have to be empty
  std::set<const Place *> concerningPlaces() const;
  /// get valid interval of a place
  formula::Interval getPlaceInterval(const Place &) const;
  /// compares formula with a boolean
  bool operator==(bool) const;
  //@}

private: /* private methods */
  /// no default copy constructor!
  Condition(const Condition &);
  /// no default copy assignment operator!
  Condition & operator=(const Condition &);
  
  /// removes all negations
  void removeNegation();
};

} /* namespace pnapi */ 

#endif /* PNAPI_CONDITION_H */
