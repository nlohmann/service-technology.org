// -*- C++ -*-

/*!
 * \file  formula.h
 */

#ifndef PNAPI_FORMULA_H
#define PNAPI_FORMULA_H

#include "config.h"

#include "myio.h"

namespace pnapi
{

// forward declarations
namespace formula {
class Conjunction;
class Interval;
}
class Marking;
class Node;
class PetriNet;
class Place;

/*!
 * \brief   Final Condition Formulae
 */
namespace formula
{

/*!
 * \brief abstract formula parent class
 */
class Formula
{
public: /* public types */
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
  
public: /* public methods */
  /// destructor
  virtual ~Formula();
  
  /*!
   * \name structural changes
   */
  //@{
  /// removes a place recursively
  virtual bool removePlace(const Place &);
  //@}
  
  /*!
   * \name getter
   */
  //@{
  /// evaluating the formula under the given marking
  virtual bool isSatisfied(const Marking &) const = 0;
  /// create a deep copy of the formula
  virtual Formula * clone(const std::map<const Place *, const Place *> * = NULL) const = 0;
  /// output the formula
  virtual std::ostream & output(std::ostream &) const = 0;
  /// set of concerning places
  virtual std::set<const Place *> getPlaces() const;
  /// set of places implied to be empty
  virtual std::set<const Place *> getEmptyPlaces() const;
  /// get the valid interval of a place
  virtual Interval getPlaceInterval(const Place &) const = 0;
  /// returns formula type
  virtual Type getType() const = 0;
  /// removes negation
  virtual Formula * removeNegation() const;
  /// negates the formula
  virtual Formula * negate() const = 0;
  /// forms formula in disjunctive normal form
  virtual Formula * dnf() const;
  /// evaluates the formula partially and replaces propositions by constants
  virtual Formula * evaluatePlace(const Place &) const;
  /// replace place references
  virtual Formula * replacePlace(const Place &, const Place &) const;
  //@}
};



/**************************************************************************
 ***** Formula Tree Inner Nodes
 **************************************************************************/

/*!
 * \brief abstract operator parent class
 */
class Operator : public Formula
{
protected: /* protected variables */
  /// child formulae
  std::set<const Formula *> children_;
public:
  /*!
   * \name constructors and destructors
   */
  //@{
  /// standard constructor
  Operator();
  /// copy constructor
  Operator(const Formula &);
  /// constructor
  Operator(const Formula &, const Formula &);
  /// constructor
  Operator(const std::set<const Formula *> &,
           const std::map<const Place *, const Place *> * = NULL);
  /// destructor
  ~Operator();
  //@}

  /*!
   * \name structural changes
   */
  //@{
  /// removes a place recursively
  bool removePlace(const Place &);
  //@}
  
  /*!
   * \name getters
   */
  //@{
  /// child formulae
  const std::set<const Formula *> & getChildren() const;
  /// get places concerned by this operator
  std::set<const Place *> getPlaces() const;
  //@}
  
protected: /* protected methods */
  /// simplify child formulae
  virtual void simplifyChildren() = 0;
};

/*!
 * \brief Negation Class
 */
class Negation : public Operator
{
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// copy constructor 
  Negation(const Negation &);
  /// constructor
  Negation(const Formula &);
  /// constructor
  Negation(const std::set<const Formula *> &,
           const std::map<const Place *, const Place *> * = NULL);
  //@}

  /*!
   * \name getter
   */
  //@{
  /// evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  Negation * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// returns formula type
  Type getType() const;
  /// removes negation
  Formula * removeNegation() const;
  /// negates the formula
  Formula * negate() const;
  /// evaluates the formula partially and replaces propositions by constants
  Formula * evaluatePlace(const Place &) const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
  /// replace place references
  Formula * replacePlace(const Place &, const Place &) const;
  //@}
  
protected: /* protected methods */
  /// simplify child formulae
  void simplifyChildren();
};

/*!
 * \brief Conjunction Class
 */
class Conjunction : public Operator
{
  friend std::ostream & pnapi::io::__owfn::output(std::ostream &, const Conjunction &);

public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// constructor
  Conjunction();
  /// copy constructor
  Conjunction(const Conjunction &);
  /// constructor
  Conjunction(const Formula &);
  /// construcotr
  Conjunction(const Formula &, const Formula &);
  /// constructor
  Conjunction(const std::set<const Formula *> &,
              const std::map<const Place *, const Place *> * = NULL);
  //@}

  /*!
   * \name getter
   */
  //@{
  ///evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  Conjunction * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// set of places implied to be empty
  std::set<const Place *> getEmptyPlaces() const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
  /// returns formula type
  Type getType() const;
  /// removes negation
  Formula * removeNegation() const;
  /// negates the formula
  Formula * negate() const;
  /// forms formula in disjunctive normal form
  Formula * dnf() const;
  /// evaluates the formula partially and replaces propositions by constants
  Formula * evaluatePlace(const Place &) const;
  /// replace place references
  Formula * replacePlace(const Place &, const Place &) const;
  //@}
  
protected: /* protected methods */
  /// simplify child formulae
  void simplifyChildren();
  
};

/*!
 * \brief Disjunction Class
 */
class Disjunction : public Operator
{
public: /* public methods */
  /*!
   * \name constructors and destructors
   */
  //@{
  /// copy constructor
  Disjunction(const Disjunction &);
  /// constructor
  Disjunction(const Formula &, const Formula &);
  /// constructor
  Disjunction(const std::set<const Formula *> &,
              const std::map<const Place *, const Place *> * = NULL);
  //@}

  /*!
   * \name getters
   */
  //@{
  ///evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  Disjunction * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// set of places implied to be empty
  std::set<const Place *> getEmptyPlaces() const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
  /// returns formula type
  Type getType() const;
  /// removes negation
  Formula * removeNegation() const;
  /// negates the formula
  Formula * negate() const;
  /// forms formula in disjunctive normal form
  Formula * dnf() const;
  /// evaluates the formula partially and replaces propositions by constants
  Formula * evaluatePlace(const Place &) const;
  /// replace place references
  Formula * replacePlace(const Place &, const Place &) const;
  //@}
  
protected: /* protected methods */
  /// simplify child formulae
  void simplifyChildren();
  
private: /* private methods */
  /// constructor needed by evaluatePlace and replacePlace
  Disjunction();
};



/**************************************************************************
 ***** Formula Tree Leaves
 **************************************************************************/

/*!
 * \brief Abstract Proposition Parent Class
 */ 
class Proposition : public Formula
{
protected: /* protected variables */
  /// concerned place
  const Place & place_;
  /// mentioned number of tokens
  const unsigned int tokens_;
  
public: /* public methods */
  /// constructor
  Proposition(const Place &, unsigned int,
              const std::map<const Place *, const Place *> *);

  /// removes a place recursively
  bool removePlace(const Place &);
  
  /// get the place concerned by this proposition
  const Place & getPlace() const;
  /// get the tokens mentioned by this proposition
  unsigned int getTokens() const;
  /// get places concerned by this operator
  std::set<const Place *> getPlaces() const;
};

/*!
 * \brief FormulaTrue Class
 */
class FormulaTrue : public Formula
{
public: /* public methods */
  ///evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  FormulaTrue * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// returns formula type
  Type getType() const;
  /// negates the formula
  Formula * negate() const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
};

/*!
 * \brief FormulaFalse Class
 */
class FormulaFalse : public Formula
{
public: /* public methods */
  ///evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  FormulaFalse * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// returns formula type
  Type getType() const;
  /// negates the formula
  Formula * negate() const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
};

/*!
 * \brief FormulaEqual Class
 */
class FormulaEqual : public Proposition
{
public: /* public methods */
  /// constructor
  FormulaEqual(const Place &, unsigned int,
               const std::map<const Place *, const Place *> * = NULL);
  ///evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  FormulaEqual * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// returns formula type
  Type getType() const;
  /// negates the formula
  Formula * negate() const;
  /// evaluates the formula partially and replaces propositions by constants
  Formula * evaluatePlace(const Place &) const;
  /// set of places implied to be empty
  std::set<const Place *> getEmptyPlaces() const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
  /// replace place references
  Formula * replacePlace(const Place &, const Place &) const;
};

/*!
 * \brief FormulaNotEqual Class
 */
class FormulaNotEqual : public Proposition
{
public: /* public methods */
  /// constructor
  FormulaNotEqual(const Place &, unsigned int,
                  const std::map<const Place *, const Place *> * = NULL);
  ///evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  FormulaNotEqual * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// returns formula type
  Type getType() const;
  /// removes negation
  Formula * removeNegation() const;
  /// negates the formula
  Formula * negate() const;
  /// evaluates the formula partially and replaces propositions by constants
  Formula * evaluatePlace(const Place &) const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
  /// replace place references
  Formula * replacePlace(const Place &, const Place &) const;
};

/*!
 * \brief FormulaGreater Class
 */
class FormulaGreater : public Proposition
{
public: /* public methods */
  /// constructor
  FormulaGreater(const Place &, unsigned int,
                 const std::map<const Place *, const Place *> * = NULL);
  ///evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  FormulaGreater * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// returns formula type
  Type getType() const;
  /// negates the formula
  Formula * negate() const;
  /// evaluates the formula partially and replaces propositions by constants
  Formula * evaluatePlace(const Place &) const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
  /// replace place references
  Formula * replacePlace(const Place &, const Place &) const;
};

/*!
 * \brief FormulaGreaterEqual Class
 */
class FormulaGreaterEqual : public Proposition
{
public: /* public methods */
  /// constructor
  FormulaGreaterEqual(const Place &, unsigned int,
                      const std::map<const Place *, const Place *> * = NULL);
  ///evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  FormulaGreaterEqual * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// returns formula type
  Type getType() const;
  /// negates the formula
  Formula * negate() const;
  /// evaluates the formula partially and replaces propositions by constants
  Formula * evaluatePlace(const Place &) const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
  /// replace place references
  Formula * replacePlace(const Place &, const Place &) const;
};

/*!
 * \brief FormulaLess Class
 */
class FormulaLess : public Proposition
{
public: /* public methods */
  /// constructor
  FormulaLess(const Place &, unsigned int,
              const std::map<const Place *, const Place *> * = NULL);
  ///evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  FormulaLess * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// returns formula type
  Type getType() const;
  /// negates the formula
  Formula * negate() const;
  /// evaluates the formula partially and replaces propositions by constants
  Formula * evaluatePlace(const Place &) const;
  /// set of places implied to be empty
  std::set<const Place *> getEmptyPlaces() const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
  /// replace place references
  Formula * replacePlace(const Place &, const Place &) const;
};

/*!
 * \brief FormulaLessEqual Class
 */
class FormulaLessEqual : public Proposition
{
public: /* public methods */
  /// constructor
  FormulaLessEqual(const Place &, unsigned int,
                   const std::map<const Place *, const Place *> * = NULL);
  ///evaluating the formula under the given marking
  bool isSatisfied(const Marking &) const;
  /// create a deep copy of the formula
  FormulaLessEqual * clone(const std::map<const Place *, const Place *> * = NULL) const;
  /// output the formula
  std::ostream & output(std::ostream &) const;
  /// returns formula type
  Type getType() const;
  /// negates the formula
  Formula * negate() const;
  /// evaluates the formula partially and replaces propositions by constants
  Formula * evaluatePlace(const Place &) const;
  /// get the valid interval of a place
  Interval getPlaceInterval(const Place &) const;
  /// set of places implied to be empty
  std::set<const Place *> getEmptyPlaces() const;
  /// replace place references
  Formula * replacePlace(const Place &, const Place &) const;
};

/*!
 * \brief interval of the marking of a place to fullfill the condition
 * 
 * The lower bound ranges from 0 to any positive number. The upper bound
 * ranges from any positive number to -2, whereas 0 means there a at most
 * 0 token allowed, -2 means there is no limit and -1 means, there are at most
 * less than 0 token allowed, which means the final condition equals to false
 * at any given marking for the place in question.
 */
class Interval
{
private: /* private variables */
  /// lower bound
  int lower_;
  /// upper bound
  int upper_;
  /// excluded intervals
  std::map<int, int> excludes_;
  
public: /* public methods */
  /// constructor
  Interval(int = 0, int = -2);
  /// exclude an interval
  void exclude(int, int);
  /// union of two intervals
  Interval & operator||(const Interval &);
  /// intersection of two intervals
  Interval & operator&&(const Interval &);
  
  /// whether inteval contains the given value
  bool isIn(int) const;
  /// upper bound
  int getUpper() const;
  /// lower bound
  int getLower() const;
};


} /* namespace formula */


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

} /* namespace pnapi */

#endif /* PNAPI_FORMULA_H */
