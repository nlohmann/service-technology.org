// -*- C++ -*-

/*!
 * \file  formula.h
 */

#ifndef PNAPI_FORMULA_H
#define PNAPI_FORMULA_H

#include <map>
#include <set>
#include <string>
#include "marking.h"
#include "myio.h"

namespace pnapi
{

// forward declarations
class PetriNet;
class Place;
class Node;

/*!
 * \brief   Final Condition Formulas
 */
namespace formula {

class Formula
{
public:
  
  /// formula types
  enum Type
  {
    F_NEGATION,
    F_CONJUNCTION,
    F_DISJUNCTION,
    F_TRUE,
    F_FALSE,
    F_EQUAL,
    F_NOT_EQUAL,
    F_GREATER,
    F_GREATER_EQUAL,
    F_LESS,
    F_LESS_EQUAL
  };

  /// destructor
  virtual ~Formula();

  /// evaluating the formula under the given marking
  virtual bool isSatisfied(const Marking &) const =0;

  /// create a deep copy of the formula
  virtual Formula * clone(const std::map<const Place *, const Place *> *
      = NULL) const =0;

  /// output the formula
  virtual std::ostream & output(std::ostream &) const =0;

  /// set of concerning places
  virtual std::set<const Place *> places() const;
  
  /// set of places implied to be empty
  virtual std::set<const Place *> emptyPlaces() const;
  
  /// removes a place recursively
  virtual bool removePlace(const Place &);
  
  /// returns formula type
  virtual Type getType() const =0;
  
  /// removes negation
  virtual Formula * removeNegation() const;
  
  /// negates the formula
  virtual Formula * negate() const =0;
  
  /// forms formula in disjunctive normal form
  virtual Formula * dnf() const;

};



/**************************************************************************
 ***** Formula Tree Inner Nodes
 **************************************************************************/

class Operator : public Formula
{
public:

  Operator();

  Operator(const Formula &);

  Operator(const Formula &, const Formula &);

  Operator(const std::set<const Formula *> &,
      const std::map<const Place *, const Place *> * = NULL);

  ~Operator();

  const std::set<const Formula *> & children() const;

  std::set<const Place *> places() const;
  
  /// removes a place recursively
  bool removePlace(const Place &);

protected:
  std::set<const Formula *> children_;

  virtual void simplifyChildren() =0;
};


class Negation : public Operator
{
public:

  Negation(const Negation &);

  Negation(const Formula &);

  Negation(const std::set<const Formula *> &,
      const std::map<const Place *, const Place *> * = NULL);

  bool isSatisfied(const Marking &) const;

  Negation * clone(const std::map<const Place *, const Place *> * = NULL) const;

  std::ostream & output(std::ostream &) const;
  
  /// returns formula type
  Type getType() const;

  /// removes negation
  Formula * removeNegation() const;

  /// negates the formula
  Formula * negate() const;  
  
protected:
  
  void simplifyChildren();
    
};


class Conjunction : public Operator
{
  friend std::ostream & pnapi::io::__owfn::output(std::ostream &, const Conjunction &);

public:

  Conjunction(const Conjunction &);

  Conjunction();

  Conjunction(const Formula &);

  Conjunction(const Formula &, const Formula &);

  Conjunction(const std::set<const Formula *> &,
      const std::map<const Place *, const Place *> * = NULL);

  bool isSatisfied(const Marking &) const;

  Conjunction * clone(const std::map<const Place *, const Place *> *
      = NULL) const;

  std::ostream & output(std::ostream &) const;
  
  /// set of places implied to be empty
  std::set<const Place *> emptyPlaces() const;
  
  /// returns formula type
  Type getType() const;

  /// removes negation
  Formula * removeNegation() const;

  /// negates the formula
  Formula * negate() const;
  
  /// forms formula in disjunctive normal form
  Formula * dnf() const;
  
protected:

  void simplifyChildren();
  
};


class Disjunction : public Operator
{
public:

  Disjunction(const Disjunction &);

  Disjunction(const Formula &, const Formula &);

  Disjunction(const std::set<const Formula *> &,
      const std::map<const Place *, const Place *> * = NULL);

  bool isSatisfied(const Marking &) const;

  Disjunction * clone(const std::map<const Place *, const Place *> *
      = NULL) const;

  std::ostream & output(std::ostream &) const;
  
  /// set of places implied to be empty
  std::set<const Place *> emptyPlaces() const;

  /// returns formula type
  Type getType() const;

  /// removes negation
  Formula * removeNegation() const;

  /// negates the formula
  Formula * negate() const;
  
  /// forms formula in disjunctive normal form
  Formula * dnf() const;
  
protected:
  void simplifyChildren();
  
};



/**************************************************************************
 ***** Formula Tree Leaves
 **************************************************************************/

class Proposition : public Formula
{
public:

  Proposition(const Place &, unsigned int,
      const std::map<const Place *, const Place *> *);

  const Place & place() const;

  unsigned int tokens() const;

  std::set<const Place *> places() const;
  
  /// removes a place recursively
  bool removePlace(const Place &);
  
protected:
  const Place & place_;
  const unsigned int tokens_;
};


class FormulaTrue : public Formula
{
public:

  bool isSatisfied(const Marking &) const;

  FormulaTrue * clone(const std::map<const Place *, const Place *> *
      = NULL) const;

  std::ostream & output(std::ostream &) const;
  
  /// returns formula type
  Type getType() const;
  
  /// negates the formula
  Formula * negate() const;

};


class FormulaFalse : public Formula
{
public:
  bool isSatisfied(const Marking &) const;

  FormulaFalse * clone(const std::map<const Place *, const Place *> *
      = NULL) const;

  std::ostream & output(std::ostream &) const;

  /// returns formula type
  Type getType() const;
  
  /// negates the formula
  Formula * negate() const;
  
};


class FormulaEqual : public Proposition
{
public:

  FormulaEqual(const Place &, unsigned int,
      const std::map<const Place *, const Place *> * = NULL);

  bool isSatisfied(const Marking &) const;

  FormulaEqual * clone(const std::map<const Place *, const Place *> *
      = NULL) const;

  std::ostream & output(std::ostream &) const;
  
  /// set of places implied to be empty
  std::set<const Place *> emptyPlaces() const;
  
  /// returns formula type
  Type getType() const;
  
  /// negates the formula
  Formula * negate() const;
    
};


class FormulaNotEqual : public Proposition
{
public:

  FormulaNotEqual(const Place &, unsigned int,
      const std::map<const Place *, const Place *> * = NULL);

  bool isSatisfied(const Marking &) const;

  FormulaNotEqual * clone(const std::map<const Place *, const Place *> *
      = NULL) const;

  std::ostream & output(std::ostream &) const;
  
  /// returns formula type
  Type getType() const;
  
  /// removes negation
  Formula * removeNegation() const;
  
  /// negates the formula
  Formula * negate() const;
  
};


class FormulaGreater : public Proposition
{
public:
  FormulaGreater(const Place &, unsigned int,
      const std::map<const Place *, const Place *> * = NULL);

  bool isSatisfied(const Marking &) const;

  FormulaGreater * clone(const std::map<const Place *, const Place *> *
      = NULL) const;

  std::ostream & output(std::ostream &) const;
  
  /// returns formula type
  Type getType() const;
  
  /// negates the formula
  Formula * negate() const;
  
};


class FormulaGreaterEqual : public Proposition
{
public:
  FormulaGreaterEqual(const Place &, unsigned int,
      const std::map<const Place *, const Place *> * = NULL);

  bool isSatisfied(const Marking &) const;

  FormulaGreaterEqual * clone(const std::map<const Place *, const Place *> *
      = NULL) const;

  std::ostream & output(std::ostream &) const;
  
  /// returns formula type
  Type getType() const;
  
  /// negates the formula
  Formula * negate() const;
  
};


class FormulaLess : public Proposition
{
public:
  FormulaLess(const Place &, unsigned int,
      const std::map<const Place *, const Place *> * = NULL);

  bool isSatisfied(const Marking &) const;

  FormulaLess * clone(const std::map<const Place *, const Place *> *
      = NULL) const;

  std::ostream & output(std::ostream &) const;
  
  /// returns formula type
  Type getType() const;
  
  /// negates the formula
  Formula * negate() const;
  
};


class FormulaLessEqual : public Proposition
{
public:
  FormulaLessEqual(const Place &, unsigned int,
      const std::map<const Place *, const Place *> * = NULL);

  bool isSatisfied(const Marking &) const;

  FormulaLessEqual * clone(const std::map<const Place *, const Place *> *
      = NULL) const;

  std::ostream & output(std::ostream &) const;
  
  /// returns formula type
  Type getType() const;
  
  /// negates the formula
  Formula * negate() const;
  
};

} /* namespace formula */

} /* namespace pnapi */

#endif
