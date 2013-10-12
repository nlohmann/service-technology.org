/*!
 * \file  formula.cc
 */

#include "config.h"

#include "pnapi-assert.h"

#include "component.h"
#include "formula.h"
#include "marking.h"
#include "util.h"

#include <iostream>

using std::cout;
using std::cerr;
using std::endl;
using std::map;
using std::set;
using std::ostream;
using std::vector;

using pnapi::io::util::operator<<;



namespace pnapi
{

/**************************************************************************
 ***** Overloaded operators for formulas
 *************************************************************************/

/*!
 * \brief formula construction operator
 */
formula::FormulaEqual operator==(const Place & p, unsigned int k)
{
  return formula::FormulaEqual(p, k);
}

/*!
 * \brief formula construction operator
 */
formula::FormulaNotEqual operator!=(const Place & p, unsigned int k)
{
  return formula::FormulaNotEqual(p, k);
}

/*!
 * \brief formula construction operator
 */
formula::FormulaGreater operator>(const Place & p, unsigned int k)
{
  return formula::FormulaGreater(p, k);
}

/*!
 * \brief formula construction operator
 */
formula::FormulaGreaterEqual operator>=(const Place & p, unsigned int k)
{
  return formula::FormulaGreaterEqual(p, k);
}

/*!
 * \brief formula construction operator
 */
formula::FormulaLess operator<(const Place & p, unsigned int k)
{
  return formula::FormulaLess(p, k);
}

/*!
 * \brief formula construction operator
 */
formula::FormulaLessEqual operator<=(const Place & p, unsigned int k)
{
  return formula::FormulaLessEqual(p, k);
}

/*!
 * \brief formula construction operator
 */
formula::Conjunction operator&&(const formula::Formula & f1, const formula::Formula & f2)
{
  return formula::Conjunction(f1, f2);
}

/*!
 * \brief formula construction operator
 */
formula::Disjunction operator||(const formula::Formula & f1, const formula::Formula & f2)
{
  return formula::Disjunction(f1, f2);
}

/*!
 * \brief formula construction operator
 */
formula::Negation operator!(const formula::Formula & f)
{
  return formula::Negation(f);
}


namespace formula
{

/**************************************************************************
 ***** Constructor() implementation
 **************************************************************************/

/*!
 * \brief constructor
 */
Operator::Operator()
{
}

/*!
 * \brief constructor
 */
Operator::Operator(const Formula & f)
{
  children_.insert(f.clone());
}

/*!
 * \brief constructor
 */
Operator::Operator(const Formula & l, const Formula & r)
{
  children_.insert(l.clone());
  children_.insert(r.clone());

#ifndef NDEBUG
  // consistency check
  const Proposition * child1 = NULL;
  PNAPI_FOREACH(it, children_)
  {
    const Proposition * child2 = dynamic_cast<const Proposition *>(*it);
    if (child2 != NULL)
    {
      if (child1 != NULL)
      {
        PNAPI_ASSERT(&child1->getPlace().getPetriNet() == &child2->getPlace().getPetriNet());
      }
      child1 = child2;
    }
  }
#endif
}

/*!
 * \brief constructor
 */
Operator::Operator(const std::set<const Formula *> & children,
                   const std::map<const Place *, const Place *> * places)
{
  PNAPI_FOREACH(it, children)
  {
    children_.insert((*it)->clone(places));
  }

#ifndef NDEBUG
  // consistency check
  const Proposition * child1 = NULL;
  PNAPI_FOREACH(it, children_)
  {
    const Proposition * child2 = dynamic_cast<const Proposition *>(*it);
    if (child2 != NULL)
    {
      if (child1 != NULL)
        PNAPI_ASSERT(&child1->getPlace().getPetriNet() == &child2->getPlace().getPetriNet());
      child1 = child2;
    }
  }
#endif
}

/*!
 * \brief constructor
 */
Negation::Negation(const Formula & f) :
  Operator(f)
{
}

/*!
 * \brief constructor
 */
Negation::Negation(const std::set<const Formula *> & children,
                   const std::map<const Place *, const Place *> * places) :
  Operator(children, places)
{
}

/*!
 * \brief constructor
 */
Negation::Negation(const Negation & n) :
  Operator(n.children_)
{
}

/*!
 * \brief constructor
 */
Conjunction::Conjunction() :
  Operator()
{
}

/*!
 * \brief constructor
 */
Conjunction::Conjunction(const Formula & f) :
  Operator(f)
{
  simplifyChildren();
}

/*!
 * \brief constructor
 */
Conjunction::Conjunction(const Formula & l, const Formula & r) :
  Operator(l, r)
{
  simplifyChildren();
}

/*!
 * \brief constructor
 */
Conjunction::Conjunction(const std::set<const Formula *> & children,
                         const std::map<const Place *, const Place *> * places) :
  Operator(children, places)
{
  simplifyChildren();
}

/*!
 * \brief constructor
 */
Conjunction::Conjunction(const Conjunction & c) :
  Operator(c.children_)
{
  simplifyChildren();
}

/*!
 * \brief constructor
 */
Disjunction::Disjunction(const Formula & l, const Formula & r) :
  Operator(l, r)
{
  simplifyChildren();
}

/*!
 * \brief constructor
 */
Disjunction::Disjunction(const std::set<const Formula *> & children,
                         const std::map<const Place *, const Place *> * places) :
  Operator(children, places)
{
  simplifyChildren();
}

/*!
 * \brief constructor
 */
Disjunction::Disjunction(const Disjunction & d) :
  Operator(d.children_)
{
  simplifyChildren();
}

/// constructor needed by evaluatePlace and replacePlace
Disjunction::Disjunction()
{
}

/*!
 * \brief constructor
 */
Proposition::Proposition(const Place & p, unsigned int k,
                         const std::map<const Place *, const Place *> * places) :
  place_((places == NULL) ? p :
         (PNAPI_ASSERT((places->find(&p) != places->end()) && (places->find(&p)->second != NULL)),
          (*places->find(&p)->second))),
  tokens_(k)
{
}

/*!
 * \brief constructor
 */
FormulaEqual::FormulaEqual(const Place & p, unsigned int k,
                           const std::map<const Place *, const Place *> * places) :
  Proposition(p, k, places)
{
}

/*!
 * \brief constructor
 */                           
FormulaNotEqual::FormulaNotEqual(const Place & p, unsigned int k,
                                 const std::map<const Place *, const Place *> * places) :
  Proposition(p, k, places)
{
}

/*!
 * \brief constructor
 */
FormulaGreater::FormulaGreater(const Place & p, unsigned int k,
                               const std::map<const Place *, const Place *> * places) :
  Proposition(p, k, places)
{
}

/*!
 * \brief constructor
 */
FormulaGreaterEqual::FormulaGreaterEqual(const Place & p, unsigned int k,
                                         const std::map<const Place *, const Place *> * places) :
  Proposition(p, k, places)
{
}

/*!
 * \brief constructor
 */
FormulaLess::FormulaLess(const Place & p, unsigned int k,
                         const std::map<const Place *, const Place *> * places) :
  Proposition(p, k, places)
{
}

/*!
 * \brief constructor
 */
FormulaLessEqual::FormulaLessEqual(const Place & p, unsigned int k,
                                   const std::map<const Place *, const Place *> * places) :
  Proposition(p, k, places)
{
}


/**************************************************************************
 ***** ~Destructor() implementation
 **************************************************************************/

/*!
 * \brief destructor
 */
Formula::~Formula()
{
}

/*!
 * \brief destructor
 */
Operator::~Operator()
{
  PNAPI_FOREACH(it, children_)
  {
    delete (*it);
  }
}

/**************************************************************************
 ***** clone() implementation
 **************************************************************************/

/*!
 * \brief create a deep copy of the formula
 */
Negation * Negation::clone(const std::map<const Place *, const Place *> * places) const
{
  return new Negation(children_, places);
}

/*!
 * \brief create a deep copy of the formula
 */
Conjunction * Conjunction::clone(const std::map<const Place *, const Place *> * places) const
{
  return new Conjunction(children_, places);
}

/*!
 * \brief create a deep copy of the formula
 */
Disjunction * Disjunction::clone(const std::map<const Place *, const Place *> * places) const
{
  return new Disjunction(children_, places);
}

/*!
 * \brief create a deep copy of the formula
 */
FormulaTrue * FormulaTrue::clone(const std::map<const Place *, const Place *> *) const
{
  return new FormulaTrue();
}

/*!
 * \brief create a deep copy of the formula
 */
FormulaFalse * FormulaFalse::clone(const std::map<const Place *, const Place *> *) const
{
  return new FormulaFalse();
}

/*!
 * \brief create a deep copy of the formula
 */
FormulaEqual * FormulaEqual::clone(const std::map<const Place *, const Place *> * places) const
{
  return new FormulaEqual(place_, tokens_, places);
}

/*!
 * \brief create a deep copy of the formula
 */
FormulaNotEqual * FormulaNotEqual::clone(const std::map<const Place *, const Place *> * places) const
{
  return new FormulaNotEqual(place_, tokens_, places);
}

/*!
 * \brief create a deep copy of the formula
 */
FormulaGreater * FormulaGreater::clone(const std::map<const Place *, const Place *> * places) const
{
  return new FormulaGreater(place_, tokens_, places);
}

/*!
 * \brief create a deep copy of the formula
 */
FormulaGreaterEqual * FormulaGreaterEqual::clone(const std::map<const Place *, const Place *> * places) const
{
  return new FormulaGreaterEqual(place_, tokens_, places);
}

/*!
 * \brief create a deep copy of the formula
 */
FormulaLess * FormulaLess::clone(const std::map<const Place *, const Place *> * places) const
{
  return new FormulaLess(place_, tokens_, places);
}

/*!
 * \brief create a deep copy of the formula
 */
FormulaLessEqual * FormulaLessEqual::clone(const std::map<const Place *, const Place *> * places) const
{
  return new FormulaLessEqual(place_, tokens_, places);
}



/**************************************************************************
 ***** isSatisfied() implementation
 **************************************************************************/

/*!
 * \brief evaluating the formula under the given marking
 */
bool Negation::isSatisfied(const Marking & m) const
{
  return !(*children_.begin())->isSatisfied(m);
}

/*!
 * \brief checks, whether this conjunction is satisfied by a given marking
 * \pre   the formula is unfolded
 */
bool Conjunction::isSatisfied(const Marking & m) const
{
  PNAPI_FOREACH(f, children_)
  {
    if (!(*f)->isSatisfied(m))
      return false;
  }

  return true;
}

/*!
 * \brief evaluating the formula under the given marking
 */
bool Disjunction::isSatisfied(const Marking & m) const
{
  PNAPI_FOREACH(f, children_)
  {
    if ((*f)->isSatisfied(m))
      return true;
  }
  
  return false;
}

/*!
 * \brief evaluating the formula under the given marking
 */
bool FormulaTrue::isSatisfied(const Marking &) const
{
  return true;
}

/*!
 * \brief evaluating the formula under the given marking
 */
bool FormulaFalse::isSatisfied(const Marking &) const
{
  return false;
}

/*!
 * \brief evaluating the formula under the given marking
 */
bool FormulaEqual::isSatisfied(const Marking & m) const
{
  return m[place_] == tokens_;
}

/*!
 * \brief evaluating the formula under the given marking
 */
bool FormulaNotEqual::isSatisfied(const Marking & m) const
{
  return m[place_] != tokens_;
}

/*!
 * \brief evaluating the formula under the given marking
 */
bool FormulaGreater::isSatisfied(const Marking & m) const
{
  return m[place_] > tokens_;
}

/*!
 * \brief evaluating the formula under the given marking
 */
bool FormulaGreaterEqual::isSatisfied(const Marking & m) const
{
  return m[place_] >= tokens_;
}

/*!
 * \brief evaluating the formula under the given marking
 */
bool FormulaLess::isSatisfied(const Marking & m) const
{
  return m[place_] < tokens_;
}

/*!
 * \brief evaluating the formula under the given marking
 */
bool FormulaLessEqual::isSatisfied(const Marking & m) const
{
  return m[place_] <= tokens_;
}


/**************************************************************************
 ***** remove place implementation
 **************************************************************************/

/*!
 * \brief   removes a place recursively
 * 
 * \return  true, iff this formula has to be removed by parent
 *          due to this removal, false otherwise.
 */
bool Formula::removePlace(const Place &)
{
  return false;
}

/*!
 * \brief   removes a place recursively
 * 
 * \return  true, iff this formula has to be removed by parent
 *          due to this removal, false otherwise.
 */
bool Operator::removePlace(const Place & p)
{
  set<const Formula *> children = children_;
  PNAPI_FOREACH(f, children)
  {
    if(const_cast<Formula *>(*f)->removePlace(p))
    {
      children_.erase(*f);
      delete (*f);
    }
  }
  
  return children_.empty();
}

/*!
 * \brief   removes a place recursively
 * 
 * \return  true, iff this formula has to be removed by parent
 *          due to this removal, false otherwise.
 */
bool Proposition::removePlace(const Place & p)
{
  return (&p == &place_);
}


/**************************************************************************
 ***** output() implementation
 **************************************************************************/

/*!
 * \brief output the formula
 */
std::ostream & Negation::output(std::ostream & os) const
{
  return os << *this;
}

/*!
 * \brief output the formula
 */
std::ostream & Conjunction::output(std::ostream & os) const
{
  return os << *this;
}

/*!
 * \brief output the formula
 */
std::ostream & Disjunction::output(std::ostream & os) const
{
  return os << *this;
}

/*!
 * \brief output the formula
 */
std::ostream & FormulaTrue::output(std::ostream & os) const
{
  return os << *this;
}

/*!
 * \brief output the formula
 */
std::ostream & FormulaFalse::output(std::ostream & os) const
{
  return os << *this;
}

/*!
 * \brief output the formula
 */
std::ostream & FormulaEqual::output(std::ostream & os) const
{
  return os << *this;
}

/*!
 * \brief output the formula
 */
std::ostream & FormulaNotEqual::output(std::ostream & os) const
{
  return os << *this;
}

/*!
 * \brief output the formula
 */
std::ostream & FormulaGreater::output(std::ostream & os) const
{
  return os << *this;
}

/*!
 * \brief output the formula
 */
std::ostream & FormulaGreaterEqual::output(std::ostream & os) const
{
  return os << *this;
}

/*!
 * \brief output the formula
 */
std::ostream & FormulaLess::output(std::ostream & os) const
{
  return os << *this;
}

/*!
 * \brief output the formula
 */
std::ostream & FormulaLessEqual::output(std::ostream & os) const
{
  return os << *this;
}


/**************************************************************************
 ***** accessor implementation
 **************************************************************************/

/*!
 * \brief child formulae
 */
const std::set<const Formula *> & Operator::getChildren() const
{
  return children_;
}

/*!
 * \brief get the place concerned by this proposition
 */
const Place & Proposition::getPlace() const
{
  return place_;
}

/*!
 * \brief get the tokens mentioned by this proposition
 */
unsigned int Proposition::getTokens() const
{
  return tokens_;
}


/**************************************************************************
 ***** concerning/empty places implementation
 **************************************************************************/

/*!
 * \brief set of concerning places
 */
std::set<const Place *> Formula::getPlaces() const
{
  return set<const Place *>();
}

/*!
 * \brief set of concerning places
 */
std::set<const Place *> Operator::getPlaces() const
{
  set<const Place *> places;
  PNAPI_FOREACH(it, children_)
  {
    set<const Place *> childCps = (*it)->getPlaces();
    places.insert(childCps.begin(), childCps.end());
  }
  return places;
}

/*!
 * \brief set of concerning places
 */
std::set<const Place *> Proposition::getPlaces() const
{
  set<const Place *> places;
  places.insert(&place_);
  return places;
}

/*!
 * \brief set of places implied to be empty
 */
std::set<const Place *> Formula::getEmptyPlaces() const
{
  return set<const Place *>();
}

/*!
 * \brief set of places implied to be empty
 */
std::set<const Place *> Conjunction::getEmptyPlaces() const
{
  set<const Place *> result;
  PNAPI_FOREACH(f, children_)
  {
    result = util::setUnion(result, (*f)->getEmptyPlaces());
  }
  
  return result;
}

/*!
 * \brief set of places implied to be empty
 */
std::set<const Place *> Disjunction::getEmptyPlaces() const
{
  if(children_.empty())
  {
    return set<const Place *>();
  }
    
  set<const Place *> result = (*children_.begin())->getEmptyPlaces();
  for(set<const Formula *>::iterator f = ++(children_.begin());
       f != children_.end(); ++f)
  {
    result = util::setIntersection(result, (*f)->getEmptyPlaces());
  }
  
  return result;
}

/*!
 * \brief set of places implied to be empty
 */
std::set<const Place *> FormulaEqual::getEmptyPlaces() const
{
  set<const Place *> result;
  
  if(tokens_ == 0)
    result.insert(&place_);
  
  return result;
}

/*!
 * \brief set of places implied to be empty
 */
std::set<const Place *> FormulaLessEqual::getEmptyPlaces() const
{
  set<const Place *> result;
  
  if(tokens_ == 0)
    result.insert(&place_);
  
  return result;
}

/*!
 * \brief set of places implied to be empty
 */
std::set<const Place *> FormulaLess::getEmptyPlaces() const
{
  set<const Place *> result;
  
  if(tokens_ <= 1)
    result.insert(&place_);
  
  return result;
}


/**************************************************************************
 ***** simplify children implementation
 **************************************************************************/

/*!
 * \brief simplify child formulae
 */
void Negation::simplifyChildren()
{
}

/*!
 * \brief simplify child formulae
 */
void Conjunction::simplifyChildren()
{
  set<const Formula *> children = children_;
  PNAPI_FOREACH(it, children)
  {
    if ((*it)->getType() == F_TRUE)
    {
      children_.erase(*it);
      delete (*it);
    }
    else
    {
      const Operator * o = dynamic_cast<const Conjunction *> (*it);
      if (o != NULL)
      {
        PNAPI_FOREACH(it, o->getChildren())
        {
          children_.insert((*it)->clone());
        }
        children_.erase(o);
        delete o;
      }
    }
  }
}

/*!
 * \brief simplify child formulae
 */
void Disjunction::simplifyChildren()
{
  set<const Formula *> children = children_;
  PNAPI_FOREACH(it, children)
  {
    if ((*it)->getType() == F_FALSE)
    {
      children_.erase(*it);
      delete *it;
    }
    else
    {
      const Operator * o = dynamic_cast<const Disjunction *> (*it);
      if (o != NULL)
      {
        PNAPI_FOREACH(it, o->getChildren())
        {
          children_.insert((*it)->clone());
        }
        children_.erase(o);
        delete o;
      }
    }
  }
}


/**************************************************************************
 ***** get type implementation
 **************************************************************************/

/*!
 * \brief returns formula type
 */
Formula::Type Negation::getType() const
{
  return F_NEGATION;
}

/*!
 * \brief returns formula type
 */
Formula::Type Conjunction::getType() const
{
  return F_CONJUNCTION;
}

/*!
 * \brief returns formula type
 */
Formula::Type Disjunction::getType() const
{
  return F_DISJUNCTION;
}

/*!
 * \brief returns formula type
 */
Formula::Type FormulaTrue::getType() const
{
  return F_TRUE;
}

/*!
 * \brief returns formula type
 */
Formula::Type FormulaFalse::getType() const
{
  return F_FALSE;
}

/*!
 * \brief returns formula type
 */
Formula::Type FormulaEqual::getType() const
{
  return F_EQUAL;
}

/*!
 * \brief returns formula type
 */
Formula::Type FormulaNotEqual::getType() const
{
  return F_NOT_EQUAL;
}

/*!
 * \brief returns formula type
 */
Formula::Type FormulaGreater::getType() const
{
  return F_GREATER;
}

/*!
 * \brief returns formula type
 */
Formula::Type FormulaGreaterEqual::getType() const
{
  return F_GREATER_EQUAL;
}

/*!
 * \brief returns formula type
 */
Formula::Type FormulaLess::getType() const
{
  return F_LESS;
}

/*!
 * \brief returns formula type
 */
Formula::Type FormulaLessEqual::getType() const
{
  return F_LESS_EQUAL;
}


/**************************************************************************
 ***** DNF implementation
 **************************************************************************/

/*!
 * \brief removes negation
 */
Formula * Formula::removeNegation() const
{
  return clone();
}

/*!
 * \brief removes negation
 */
Formula * Negation::removeNegation() const
{
  return (*children_.begin())->negate();
}

/*!
 * \brief removes negation
 */
Formula * Conjunction::removeNegation() const
{
  Conjunction * result = new Conjunction();
  
  PNAPI_FOREACH(f, children_)
  {
    result->children_.insert((*f)->removeNegation());
  }
  
  return static_cast<Formula*>(result);
}

/*!
 * \brief removes negation
 */
Formula * Disjunction::removeNegation() const
{
  set<const Formula*> children;
    
  PNAPI_FOREACH(f, children_)
  {
    children.insert((*f)->removeNegation());
  }
  
  Disjunction * result = new Disjunction(children);
  
  PNAPI_FOREACH(f, children)
  {
    delete (*f);
  }
  
  return static_cast<Formula*>(result);
}

/*!
 * \brief removes negation
 */
Formula * FormulaNotEqual::removeNegation() const
{
  return ((place_ < tokens_) || (place_ > tokens_)).clone();
}

/*!
 * \brief negates the formula
 */
Formula * Negation::negate() const
{
  return (*children_.begin())->clone();
}

/*!
 * \brief negates the formula
 */
Formula * Conjunction::negate() const
{
  set<const Formula *> children;
  
  PNAPI_FOREACH(f, children_)
  {
    children.insert((*f)->negate());
  }
  
  Formula * result = static_cast<Formula *>(new Disjunction(children));
  
  PNAPI_FOREACH(f, children)
  {
    delete (*f);
  }
  
  return result;
}

/*!
 * \brief negates the formula
 */
Formula * Disjunction::negate() const
{
  set<const Formula *> children;
    
  PNAPI_FOREACH(f, children_)
  {
    children.insert((*f)->negate());
  }
  
  Formula * result = static_cast<Formula *>(new Conjunction(children));
  
  PNAPI_FOREACH(f, children)
  {
    delete (*f);
  }
  
  return result;
}

/*!
 * \brief negates the formula
 */
Formula * FormulaTrue::negate() const
{
  return static_cast<Formula *>(new FormulaFalse());
}

/*!
 * \brief negates the formula
 */
Formula * FormulaFalse::negate() const
{
  return static_cast<Formula *>(new FormulaTrue());
}

/*!
 * \brief negates the formula
 */
Formula * FormulaEqual::negate() const
{
  return static_cast<Formula *>(((place_ < tokens_) || (place_ > tokens_)).clone());
}

/*!
 * \brief negates the formula
 */
Formula * FormulaNotEqual::negate() const
{
  return static_cast<Formula *>(new FormulaEqual(place_, tokens_));
}

/*!
 * \brief negates the formula
 */
Formula * FormulaGreater::negate() const
{
  return static_cast<Formula *>(new FormulaLessEqual(place_, tokens_));
}

/*!
 * \brief negates the formula
 */
Formula * FormulaGreaterEqual::negate() const
{
  return static_cast<Formula *>(new FormulaLess(place_, tokens_));
}

/*!
 * \brief negates the formula
 */
Formula * FormulaLess::negate() const
{
  return static_cast<Formula *>(new FormulaGreaterEqual(place_, tokens_));
}

/*!
 * \brief negates the formula
 */
Formula * FormulaLessEqual::negate() const
{
  return static_cast<Formula *>(new FormulaGreater(place_, tokens_));
}

/*! 
 * AND(a_1,...,a_n,OR(b_{1,1},...,b_{1,m_1}),...,OR(b_{l,1},...,b_{l,m_l}))
 * -> OR(AND(a_1,...,a_n,b_{1,1},...,b_{l,1}), ...,
 *       AND(a_1,...,a_n,b_{1,1},...,b_{l,m_l}), ...,
 *       AND(a_1,...,a_n,b_{1,m_1},...,b_{l,m_l}))
 */

/*!
 * \brief forms formula in disjunctive normal form
 */
Formula * Formula::dnf() const
{
  return clone();
}

/*!
 * \brief forms formula in disjunctive normal form
 */
Formula * Conjunction::dnf() const
{
  set<const Formula *> atomics;
  set<const Formula *> disjunctions;
  
  /// split children
  PNAPI_FOREACH(f, children_)
  {
    if((*f)->getType() == F_DISJUNCTION)
    {
      /// convert child in dnf
      disjunctions.insert((*f)->dnf());
    }
    else
    {
      atomics.insert(*f);
    }
  }
  
  if(disjunctions.empty()) // just atomics
    return clone();
  
  set<set<const Formula *> > disChildren;
  /// calculate cartesian product
  {
    set<set<const Formula *> > * result = new set<set<const Formula *> >();
    bool first = true;
    
    PNAPI_FOREACH(f, disjunctions)
    {
      set<set<const Formula *> > * newResult = new set<set<const Formula *> >();
      const Disjunction * d = static_cast<const Disjunction *>(*f);
      
      if(first)
      {
        first = false;
        PNAPI_FOREACH(ff, d->getChildren())
        {
          set<const Formula *> tmpset;
          tmpset.insert(*ff);
          newResult->insert(tmpset);
        }
      }
      else
      {
        PNAPI_FOREACH(ff, d->getChildren())
        {
          PNAPI_FOREACH(fff, (*result))
          {
            set<const Formula *> tmpset = *fff;
            tmpset.insert(*ff);
            newResult->insert(tmpset);
          }
        }
      }
      
      delete result;
      result = newResult;
    }
    
    disChildren = *result;
    delete result;
  }
  
  set<const Formula *> conjunctions;
  /// create conjunctions
  PNAPI_FOREACH(f, disChildren)
  {
    set<const Formula *> tmpset = util::setUnion(atomics, *f);
    conjunctions.insert(static_cast<Formula *>(new Conjunction(tmpset)));
  }
  
  Formula * result = static_cast<Formula *>(new Disjunction(conjunctions));
  
  // cleanup
  PNAPI_FOREACH(f, conjunctions)
  {
    delete (*f);
  }
  PNAPI_FOREACH(f, disjunctions)
  {
    delete (*f);
  }
  
  return result;
}

/*!
 * \brief forms formula in disjunctive normal form
 */
Formula * Disjunction::dnf() const
{
  set<const Formula *> children;
  
  PNAPI_FOREACH(f, children_)
  {
    children.insert((*f)->dnf());
  }
  
  Formula * result = static_cast<Formula *>(new Disjunction(children));
  
  PNAPI_FOREACH(f, children)
  {
    delete (*f);
  }
  
  return result;
}


/**************************************************************************
 ***** evaluate Place implementation
 **************************************************************************/

/*!
 * \brief evaluates the formula partially and replaces propositions by constants
 */
Formula * Formula::evaluatePlace(const Place &) const
{
  return clone();
}

/*!
 * \brief evaluates the formula partially and replaces propositions by constants
 */
Formula * Negation::evaluatePlace(const Place & p) const
{
  Formula * tmp = (*(children_.begin()))->evaluatePlace(p);
  Negation * result = new Negation(*tmp);
  delete tmp;
  return result;
}

/*!
 * \brief evaluates the formula partially and replaces propositions by constants
 */
Formula * Conjunction::evaluatePlace(const Place & p) const
{
  Conjunction * result = new Conjunction();
  
  PNAPI_FOREACH(f, children_)
  {
    result->children_.insert((*f)->evaluatePlace(p));
  }
  
  return result;
}

/*!
 * \brief evaluates the formula partially and replaces propositions by constants
 */
Formula * Disjunction::evaluatePlace(const Place & p) const
{
  Disjunction * result = new Disjunction();
  
  PNAPI_FOREACH(f, children_)
  {
    result->children_.insert((*f)->evaluatePlace(p));
  }
  
  return result;
}

/*!
 * \brief evaluates the formula partially and replaces propositions by constants
 */
Formula * FormulaEqual::evaluatePlace(const Place & p) const
{
  if(&place_ != &p)
  {
    return clone();
  }
  
  if(p.getTokenCount() == tokens_)
  {
    return new FormulaTrue();
  }
  else
  {
    return new FormulaFalse();
  }
}

/*!
 * \brief evaluates the formula partially and replaces propositions by constants
 */
Formula * FormulaNotEqual::evaluatePlace(const Place & p) const
{
  if(&place_ != &p)
  {
    return clone();
  }
  
  if(p.getTokenCount() != tokens_)
  {
    return new FormulaTrue();
  }
  else
  {
    return new FormulaFalse();
  }
}

/*!
 * \brief evaluates the formula partially and replaces propositions by constants
 */
Formula * FormulaGreater::evaluatePlace(const Place & p) const
{
  if(&place_ != &p)
  {
    return clone();
  }
  
  if(p.getTokenCount() > tokens_)
  {
    return new FormulaTrue();
  }
  else
  {
    return new FormulaFalse();
  }
}

/*!
 * \brief evaluates the formula partially and replaces propositions by constants
 */
Formula * FormulaGreaterEqual::evaluatePlace(const Place & p) const
{
  if(&place_ != &p)
  {
    return clone();
  }
  
  if(p.getTokenCount() >= tokens_)
  {
    return new FormulaTrue();
  }
  else
  {
    return new FormulaFalse();
  }
}

/*!
 * \brief evaluates the formula partially and replaces propositions by constants
 */
Formula * FormulaLess::evaluatePlace(const Place & p) const
{
  if(&place_ != &p)
  {
    return clone();
  }
  
  if(p.getTokenCount() < tokens_)
  {
    return new FormulaTrue();
  }
  else
  {
    return new FormulaFalse();
  }
}

/*!
 * \brief evaluates the formula partially and replaces propositions by constants
 */
Formula * FormulaLessEqual::evaluatePlace(const Place & p) const
{
  if(&place_ != &p)
  {
    return clone();
  }
  
  if(p.getTokenCount() <= tokens_)
  {
    return new FormulaTrue();
  }
  else
  {
    return new FormulaFalse();
  }
}


/**************************************************************************
 ***** replacePlace implementation
 **************************************************************************/

/*!
 * \brief replace place references
 */
Formula * Formula::replacePlace(const Place &, const Place &) const
{
  return clone();
}

/*!
 * \brief replace place references
 */
Formula * Negation::replacePlace(const Place & p1, const Place & p2) const
{
  Formula * tmp = (*(children_.begin()))->replacePlace(p1, p2);
  Negation * result = new Negation(*tmp);
  delete tmp;
  return result;
}

/*!
 * \brief replace place references
 */
Formula * Conjunction::replacePlace(const Place & p1, const Place & p2) const
{
  Conjunction * result = new Conjunction();
  PNAPI_FOREACH(f, children_)
  {
    result->children_.insert((*f)->replacePlace(p1, p2));
  }
  return result;
}

/*!
 * \brief replace place references
 */
Formula * Disjunction::replacePlace(const Place & p1, const Place & p2) const
{
  Disjunction * result = new Disjunction();
  PNAPI_FOREACH(f, children_)
  {
    result->children_.insert((*f)->replacePlace(p1, p2));
  }
  return result;
}

/*!
 * \brief replace place references
 */
Formula * FormulaEqual::replacePlace(const Place & p1, const Place & p2) const
{
  if(&place_ == &p1)
  {
    return new FormulaEqual(p2, tokens_);
  }
  
  return clone();
}

/*!
 * \brief replace place references
 */
Formula * FormulaNotEqual::replacePlace(const Place & p1, const Place & p2) const
{
  if(&place_ == &p1)
  {
    return new FormulaNotEqual(p2, tokens_);
  }
  
  return clone();
}

/*!
 * \brief replace place references
 */
Formula * FormulaGreater::replacePlace(const Place & p1, const Place & p2) const
{
  if(&place_ == &p1)
  {
    return new FormulaGreater(p2, tokens_);
  }
  
  return clone();
}

/*!
 * \brief replace place references
 */
Formula * FormulaGreaterEqual::replacePlace(const Place & p1, const Place & p2) const
{
  if(&place_ == &p1)
  {
    return new FormulaGreaterEqual(p2, tokens_);
  }
  
  return clone();
}

/*!
 * \brief replace place references
 */
Formula * FormulaLess::replacePlace(const Place & p1, const Place & p2) const
{
  if(&place_ == &p1)
  {
    return new FormulaLess(p2, tokens_);
  }
  
  return clone();
}

/*!
 * \brief replace place references
 */
Formula * FormulaLessEqual::replacePlace(const Place & p1, const Place & p2) const
{
  if(&place_ == &p1)
  {
    return new FormulaLessEqual(p2, tokens_);
  }
  
  return clone();
}


/**************************************************************************
 ***** getPlaceInterval implementation
 **************************************************************************/

/*!
 * \brief get the valid interval of a place
 */
Interval Negation::getPlaceInterval(const Place & p) const
{
  Formula * tmp = (*(children_.begin()))->negate();
  Interval result = tmp->getPlaceInterval(p);
  delete tmp;
  return result;
}

/*!
 * \brief get the valid interval of a place
 */
Interval Conjunction::getPlaceInterval(const Place & p) const
{
  Interval result(0, -2);
  
  PNAPI_FOREACH(f, children_)
  {
    result = result && (*f)->getPlaceInterval(p);
  }
  
  return result;
}

/*!
 * \brief get the valid interval of a place
 */
Interval Disjunction::getPlaceInterval(const Place & p) const
{
  Interval result(0, -1);
  PNAPI_FOREACH(f, children_)
  {
    result = result || (*f)->getPlaceInterval(p);
  }
  
  return result;
}

/*!
 * \brief get the valid interval of a place
 */
Interval FormulaTrue::getPlaceInterval(const Place &) const
{
  return Interval();
}

/*!
 * \brief get the valid interval of a place
 */
Interval FormulaFalse::getPlaceInterval(const Place &) const
{
  return Interval(0, -1);
}

/*!
 * \brief get the valid interval of a place
 */
Interval FormulaEqual::getPlaceInterval(const Place & p) const
{
  if(&p == &place_)
  {
    return Interval(tokens_, tokens_);
  }
  
  return Interval();
}

/*!
 * \brief get the valid interval of a place
 */
Interval FormulaNotEqual::getPlaceInterval(const Place & p) const
{
  if(&p == &place_)
  {
    Interval result;
    result.exclude(tokens_, tokens_);
    return result;
  }
  
  return Interval();
}

/*!
 * \brief get the valid interval of a place
 */
Interval FormulaGreater::getPlaceInterval(const Place & p) const
{
  if(&p == &place_)
  {
    return Interval(tokens_ + 1, -2);
  }
  
  return Interval();
}

/*!
 * \brief get the valid interval of a place
 */
Interval FormulaGreaterEqual::getPlaceInterval(const Place & p) const
{
  if(&p == &place_)
  {
    return Interval(tokens_, -2);
  }
  
  return Interval();
}

/*!
 * \brief get the valid interval of a place
 */
Interval FormulaLess::getPlaceInterval(const Place & p) const
{
  if(&p == &place_)
  {
    return Interval(0, tokens_ - 1);
  }
  
  return Interval();
}

/*!
 * \brief get the valid interval of a place
 */
Interval FormulaLessEqual::getPlaceInterval(const Place & p) const
{
  if(&p == &place_)
  {
    return Interval(0, tokens_);
  }
  
  return Interval();
}


/**************************************************************************
 ***** Interval implementation
 **************************************************************************/

/*!
 * \brief constructor
 */
Interval::Interval(int lower, int upper) :
  lower_(lower), upper_(upper)
{
}

/*!
 * \brief exclude an interval
 */
void Interval::exclude(int lower, int upper)
{
  if( ((upper_ != -2) && (upper >= upper_)) ||
      (upper == -2))
  {
    upper_ = (((upper_ != -2) && (upper_ < lower - 1)) ? upper_ : (lower - 1));
    
    if((upper_ != -2) && (upper_ < lower_))
    {
      upper_ = -1;
      lower_ = 0;
      excludes_.clear();
    }
    else
    {
      map<int, int> backup = excludes_;
      excludes_.clear();
      PNAPI_FOREACH(e, backup)
      {
        exclude(e->first, e->second);
      }
    }
    return;
  }
  
  if(lower <= lower_)
  {
    lower_ = ((lower_ > upper + 1) ? lower_ : (upper + 1));
    map<int, int> backup = excludes_;
    excludes_.clear();
    PNAPI_FOREACH(e, backup)
    {
      exclude(e->first, e->second);
    }
    return;
  }
  
  bool excluded = false;
  
  PNAPI_FOREACH(e, excludes_)
  {
    if(e->second < lower - 1)
    {
      continue;
    }
    
    if(e->first > upper + 1)
    {
      break;
    }
    
    int l = ((lower < e->first) ? lower : (e->first));
    int u = ((upper > e->second) ? upper : (e->second));
    
    set<int> del;
    
    for(map<int, int>::iterator e2 = e; e2 != excludes_.end(); ++e2)
    {
      if(e2->second <= u)
      {
        del.insert(e2->first);
        continue;
      }
      
      if(e2->first <= u + 1)
      {
        u = e2->second;
        del.insert(e2->first);
        break;
      }
    }

    PNAPI_FOREACH(d, del)
    {
      excludes_.erase(*d);
    }

    excludes_[l] = u;
    excluded = true;
    break;
  }
  
  if(!excluded)
  {
    excludes_[lower] = upper;
  }
}

/*!
 * \brief union of two intervals
 */
Interval & Interval::operator||(const Interval & other)
{
  int l1, l2, u1, u2;
  map<int, int> ex1, ex2;
  if(lower_ < other.lower_)
  {
    l1 = lower_;
    ex1 = excludes_;
    u1 = upper_;
    l2 = other.lower_;
    u2 = other.upper_;
    ex2 = other.excludes_;
  }
  else
  {
    l1 = other.lower_;
    ex1 = other.excludes_;
    u1 = other.upper_;
    l2 = lower_;
    u2 = upper_;
    ex2 = excludes_;
  }
  
  excludes_.clear();
  lower_ = l1;
  if((u1 == -2) || (u2 == -2))
  {
    upper_ = -2;
  }
  else
  {
    upper_ = ((u1 > u2) ? u1 : u2);
  }
  
  if((u1 != -2) && (u1 < l2 - 1))
  {
    exclude(u1 + 1, l2 - 1);
  }
  
  map<int, int>::iterator it1 = ex1.begin();
  map<int, int>::iterator it2 = ex2.begin();
  
  while((it1 != ex1.end()) && (it1->second < l2))
  {
    exclude(it1->first, it1->second);
    ++it1;
  }
  if((it1 != ex1.end()) && (it1->first < l2))
  {
    exclude(it1->first, l2 - 1);
  }
  
  while((it1 != ex1.end()) && (it2 != ex2.end()))
  {
    map<int, int>::iterator * itP1 = ((it1->first < it2->first) ? &it1 : &it2);
    map<int, int>::iterator * itP2 = ((it1->first < it2->first) ? &it2 : &it1);
    
    if((*itP1)->second < (*itP2)->first)
    {
      ++(*itP1);
      continue;
    }
    
    int tmp;
    exclude((*itP2)->first, (((*itP1)->second < (*itP2)->second)
                             ? (tmp = (*itP1)->second, ++(*itP1), tmp)
                             : (tmp = (*itP2)->second, ++(*itP2), tmp)));
  }
  
  if( ((it1 != ex1.end()) && (u2 != -2)) ||
      ((it2 != ex2.end()) && (u1 != -2)) )
  {
    int u = ((it1 == ex1.end()) ? u1 : u2);
    map<int, int> & ex((it1 == ex1.end()) ? ex2 : ex1);
    map<int, int>::iterator & it((it1 == ex1.end()) ? it2 : it1);
    
    while((it != ex.end()) && (it->second < u))
    {
      ++it;
    }
    if(it != ex.end())
    {
      exclude(u + 1, it->second);
      ++it;
    }
    
    while(it != ex.end())
    {
      exclude(it->first, it->second);
      ++it;
    }
  }
  
  return (*this);
}

/*!
 * \brief intersection of two intervals
 */
Interval & Interval::operator&&(const Interval & other)
{ 
  map<int, int> backup = excludes_;
  excludes_.clear();
  
  if(other.lower_ != 0)
  {
    exclude(0, other.lower_ - 1);
  }
  
  if(other.upper_ != -2)
  {
    exclude(other.upper_ + 1, -2);
  }
  
  PNAPI_FOREACH(e, backup)
  {
    exclude(e->first, e->second);
  }
  
  PNAPI_FOREACH(e, other.excludes_)
  {
    exclude(e->first, e->second);
  }
  
  return (*this);
}

/*!
 * \brief whether inteval contains the given value
 */
bool Interval::isIn(int v) const
{
  if( (v < lower_) ||
      ((upper_ != -2) && (v > upper_)))
  {
    return false;
  }
  
  PNAPI_FOREACH(e, excludes_)
  {
    if(e->second < v)
    {
      continue;
    }
    
    return (e->first > v);
  }
  
  return true;
}

/*!
 * \brief upper bound
 */
int Interval::getUpper() const
{
  return upper_;
}

/*!
 * \brief lower bound
 */
int Interval::getLower() const
{
  return lower_;
}

} /* namespace formula */

} /* namespace pnapi */
