#include "config.h"
#include <cassert>
#include <iostream>
#include <vector>

#ifndef NDEBUG
#include "myio.h"
#include "component.h"
using std::cout;
using std::endl;
#endif

#include "marking.h"
#include "formula.h"
#include "myio.h"
#include "petrinet.h"
#include "util.h"

using std::map;
using std::set;
using std::ostream;
using std::vector;

using pnapi::io::util::operator<<;

namespace pnapi
{

namespace formula
{

/**************************************************************************
 ***** Constructor() implementation
 **************************************************************************/

Operator::Operator()
{
}

Operator::Operator(const Formula & f)
{
  children_.insert(f.clone());
}

Operator::Operator(const Formula & l, const Formula & r)
{
  children_.insert(l.clone());
  children_.insert(r.clone());


#ifndef NDEBUG
  // consistency check
  const Proposition * child1 = NULL;
  for (set<const Formula *>::iterator it = children_.begin(); 
  it != children_.end(); ++it)
  {
    const Proposition * child2 = dynamic_cast<const Proposition *>(*it);
    if (child2 != NULL)
    {
      if (child1 != NULL)
        assert(&child1->place().getPetriNet() == 
          &child2->place().getPetriNet());
      child1 = child2;
    }
  }
#endif
}

Operator::Operator(const set<const Formula *> & children,
    const map<const Place *, const Place *> * places)
{
  for (set<const Formula *>::const_iterator it = children.begin();
  it != children.end(); ++it)
    children_.insert((*it)->clone(places));

#ifndef NDEBUG
  // consistency check
  const Proposition * child1 = NULL;
  for (set<const Formula *>::iterator it = children_.begin(); 
  it != children_.end(); ++it)
  {
    const Proposition * child2 = dynamic_cast<const Proposition *>(*it);
    if (child2 != NULL)
    {
      if (child1 != NULL)
        assert(&child1->place().getPetriNet() == 
          &child2->place().getPetriNet());
      child1 = child2;
    }
  }
#endif
}

Negation::Negation(const Formula & f) :
  Operator(f)
{
}

Negation::Negation(const set<const Formula *> & children,
    const map<const Place *, const Place *> * places) :
  Operator(children, places)
{
}

Negation::Negation(const Negation & n) :
  Operator(n.children_)
{
}

Conjunction::Conjunction() :
  Operator()
{
}

Conjunction::Conjunction(const Formula & f) :
  Operator(f)
{
  simplifyChildren();
}

Conjunction::Conjunction(const Formula & l, const Formula & r) :
  Operator(l, r)
{
  simplifyChildren();
}

Conjunction::Conjunction(const set<const Formula *> & children,
    const map<const Place *, const Place *> * places) :
      Operator(children, places)
{
  simplifyChildren();
}

Conjunction::Conjunction(const Conjunction & c) :
  Operator(c.children_)
{
  simplifyChildren();
}

Disjunction::Disjunction(const Formula & l, const Formula & r) :
  Operator(l, r)
{
  simplifyChildren();
}

Disjunction::Disjunction(const set<const Formula *> & children,
    const map<const Place *, const Place *> * places) :
      Operator(children, places)
{
  simplifyChildren();
}

Disjunction::Disjunction(const Disjunction & d) :
  Operator(children_)
{
  simplifyChildren();
}

Proposition::Proposition(const Place & p, unsigned int k,
    const map<const Place *, const Place *> * places) :
      place_(places == NULL ? p : *places->find(&p)->second), tokens_(k)
{
  assert(places == NULL || places->find(&p)->second != NULL);
}

FormulaEqual::FormulaEqual(const Place & p, unsigned int k,
    const map<const Place *, const Place *> * places) :
      Proposition(p, k, places)
{
}

FormulaNotEqual::FormulaNotEqual(const Place & p, unsigned int k,
    const map<const Place *, const Place *> * places) :
      Proposition(p, k, places)
{
}

FormulaGreater::FormulaGreater(const Place & p, unsigned int k,
    const map<const Place *, const Place *> * places) :
      Proposition(p, k, places)
{
}

FormulaGreaterEqual::FormulaGreaterEqual(const Place & p, unsigned int k,
    const map<const Place *, const Place *> * places) :
      Proposition(p, k, places)
{
}

FormulaLess::FormulaLess(const Place & p, unsigned int k,
    const map<const Place *, const Place *> * places) :
      Proposition(p, k, places)
{
}

FormulaLessEqual::FormulaLessEqual(const Place & p, unsigned int k,
    const map<const Place *, const Place *> * places) :
      Proposition(p, k, places)
{
}



/**************************************************************************
 ***** ~Destructor() implementation
 **************************************************************************/

Formula::~Formula()
{
}

Operator::~Operator()
{
  for (set<const Formula *>::iterator it = children_.begin();
  it != children_.end(); ++it)
    delete *it;
}

/**************************************************************************
 ***** clone() implementation
 **************************************************************************/

Negation * Negation::clone(const map<const Place *, const Place *> * places) const
{
  return new Negation(children_, places);
}

Conjunction * Conjunction::clone(const map<const Place *,
    const Place *> * places) const
{
  return new Conjunction(children_, places);
}

Disjunction * Disjunction::clone(const map<const Place *,
    const Place *> * places) const
{
  return new Disjunction(children_, places);
}

FormulaTrue * FormulaTrue::clone(const map<const Place *,
    const Place *> *) const
{
  return new FormulaTrue();
}

FormulaFalse * FormulaFalse::clone(const map<const Place *,
    const Place *> *) const
{
  return new FormulaFalse();
}

FormulaEqual * FormulaEqual::clone(const map<const Place *,
    const Place *> * places) const
{
  return new FormulaEqual(place_, tokens_, places);
}

FormulaNotEqual * FormulaNotEqual::clone(const map<const Place *,
    const Place *> * places) const
{
  return new FormulaNotEqual(place_, tokens_, places);
}

FormulaGreater * FormulaGreater::clone(const map<const Place *,
    const Place *> * places) const
{
  return new FormulaGreater(place_, tokens_, places);
}

FormulaGreaterEqual * FormulaGreaterEqual::clone(const map<const Place *,
    const Place *> * places) const
{
  return new FormulaGreaterEqual(place_, tokens_, places);
}

FormulaLess * FormulaLess::clone(const map<const Place *,
    const Place *> * places) const
{
  return new FormulaLess(place_, tokens_, places);
}

FormulaLessEqual * FormulaLessEqual::clone(const map<const Place *,
    const Place *> * places) const
{
  return new FormulaLessEqual(place_, tokens_, places);
}



/**************************************************************************
 ***** isSatisfied() implementation
 **************************************************************************/

bool Negation::isSatisfied(const Marking & m) const
{
  return !(*children_.begin())->isSatisfied(m);
}

/*!
 * \brief checks, whether this conjunction is satisfied by a given marking
 * \pre   the formel is unfolded
 */
bool Conjunction::isSatisfied(const Marking & m) const
{
  for (set<const Formula *>::const_iterator f = children_.begin();
  f != children_.end(); ++f)
    if (!(*f)->isSatisfied(m))
      return false;

  return true;
}

bool Disjunction::isSatisfied(const Marking & m) const
{
  for (set<const Formula *>::const_iterator f = children_.begin();
  f != children_.end(); ++f)
    if ((*f)->isSatisfied(m))
      return true;
  return false;
}

bool FormulaTrue::isSatisfied(const Marking &) const
{
  return true;
}

bool FormulaFalse::isSatisfied(const Marking &) const
{
  return false;
}

bool FormulaEqual::isSatisfied(const Marking & m) const
{
  return m[place_] == tokens_;
}

bool FormulaNotEqual::isSatisfied(const Marking & m) const
{
  return m[place_] != tokens_;
}

bool FormulaGreater::isSatisfied(const Marking & m) const
{
  return m[place_] > tokens_;
}

bool FormulaGreaterEqual::isSatisfied(const Marking & m) const
{
  return m[place_] >= tokens_;
}

bool FormulaLess::isSatisfied(const Marking & m) const
{
  return m[place_] < tokens_;
}

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
 * \return  true, if this formula has to be removed by parent
 *          due to this removal, false otherwise.
 */
bool Formula::removePlace(const Place & p)
{
  return false;
}

/*!
 * \brief   removes a place recursively
 * 
 * \return  true, if this formula has to be removed by parent
 *          due to this removal, false otherwise.
 */
bool Operator::removePlace(const Place & p)
{
  set<const Formula*> children = children_;
  for(set<const Formula*>::iterator f = children.begin();
       f != children.end(); ++f)
  {
    if(const_cast<Formula*>(*f)->removePlace(p))
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
 * \return  true, if this formula has to be removed by parent
 *          due to this removal, false otherwise.
 */
bool Proposition::removePlace(const Place & p)
{
  return (&p == &place_);
}


/**************************************************************************
 ***** output() implementation
 **************************************************************************/

ostream & Negation::output(ostream & os) const
{
  return os << *this;
}

ostream & Conjunction::output(ostream & os) const
{
  return os << *this;
}

ostream & Disjunction::output(ostream & os) const
{
  return os << *this;
}

ostream & FormulaTrue::output(ostream & os) const
{
  return os << *this;
}

ostream & FormulaFalse::output(ostream & os) const
{
  return os << *this;
}

ostream & FormulaEqual::output(ostream & os) const
{
  return os << *this;
}

ostream & FormulaNotEqual::output(ostream & os) const
{
  return os << *this;
}

ostream & FormulaGreater::output(ostream & os) const
{
  return os << *this;
}

ostream & FormulaGreaterEqual::output(ostream & os) const
{
  return os << *this;
}

ostream & FormulaLess::output(ostream & os) const
{
  return os << *this;
}

ostream & FormulaLessEqual::output(ostream & os) const
{
  return os << *this;
}



/**************************************************************************
 ***** accessor implementation
 **************************************************************************/

const set<const Formula *> & Operator::children() const
{
  return children_;
}

const Place & Proposition::place() const
{
  return place_;
}

unsigned int Proposition::tokens() const
{
  return tokens_;
}


/**************************************************************************
 ***** concerning/empty places implementation
 **************************************************************************/

set<const Place *> Formula::places() const
{
  return set<const Place *>();
}

set<const Place *> Operator::places() const
{
  set<const Place *> places;
  for (set<const Formula *>::const_iterator it = children_.begin();
  it != children_.end(); ++it)
  {
    set<const Place *> childCps = (*it)->places();
    places.insert(childCps.begin(), childCps.end());
  }
  return places;
}

set<const Place *> Proposition::places() const
{
  set<const Place *> places;
  places.insert(&place_);
  return places;
}

/*!
 * \brief set of places implied to be empty
 */
std::set<const Place *> Formula::emptyPlaces() const
{
  return set<const Place*>();
}

/*!
 * \brief set of places implied to be empty
 */
std::set<const Place *> Conjunction::emptyPlaces() const
{
  set<const Place*> result;
  for(set<const Formula*>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    result = util::setUnion(result, (*f)->emptyPlaces());
  }
  
  return result;
}

/*!
 * \brief set of places implied to be empty
 */
std::set<const Place *> Disjunction::emptyPlaces() const
{
  if(children_.empty())
    return set<const Place*>();
    
  set<const Place*> result = (*children_.begin())->emptyPlaces();
  for(set<const Formula*>::iterator f = ++(children_.begin());
       f != children_.end(); ++f)
  {
    result = util::setIntersection(result, (*f)->emptyPlaces());
  }
  
  return result;
}

/*!
 * \brief set of places implied to be empty
 */
std::set<const Place *> FormulaEqual::emptyPlaces() const
{
  set<const Place*> result;
  
  if(tokens_ == 0)
    result.insert(&place_);
  
  return result;
}


/**************************************************************************
 ***** simplify children implementation
 **************************************************************************/

void Negation::simplifyChildren()
{
}

void Conjunction::simplifyChildren()
{
  set<const Formula *> children = children_;
  for (set<const Formula *>::iterator it = children.begin(); 
        it != children.end(); ++it)
    if ((*it)->getType() == F_TRUE)
    {
      children_.erase(*it);
      delete *it;
    }
    else
    {
      const Operator * o = dynamic_cast<const Conjunction *> (*it);
      if (o != NULL)
      {
        for (set<const Formula *>::const_iterator it = o->children().begin(); 
              it != o->children().end(); ++it)
          children_.insert((*it)->clone());
        children_.erase(o);
        delete o;
      }
    }
}

void Disjunction::simplifyChildren()
{
  set<const Formula *> children = children_;
  for (set<const Formula *>::iterator it = children.begin(); 
        it != children.end(); ++it)
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
        for (set<const Formula *>::const_iterator it = o->children().begin(); 
              it != o->children().end(); ++it)
          children_.insert((*it)->clone());
        children_.erase(o);
        delete o;
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
  
  for(set<const Formula*>::iterator f = children_.begin();
       f != children_.end(); ++f)
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
    
  for(set<const Formula*>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    children.insert((*f)->removeNegation());
  }
  
  Disjunction * result = new Disjunction(children);
  
  for(set<const Formula*>::iterator f = children.begin();
       f != children.end(); ++f)
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
  set<const Formula*> children;
  
  for(set<const Formula*>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    children.insert((*f)->negate());
  }
  
  Formula * result = static_cast<Formula*>(new Disjunction(children));
  
  for(set<const Formula*>::iterator f = children.begin();
         f != children.end(); ++f)
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
  set<const Formula*> children;
    
  for(set<const Formula*>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    children.insert((*f)->negate());
  }
  
  Formula * result = static_cast<Formula*>(new Conjunction(children));
  
  for(set<const Formula*>::iterator f = children.begin();
         f != children.end(); ++f)
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
  return static_cast<Formula*>(new FormulaFalse());
}

/*!
 * \brief negates the formula
 */
Formula * FormulaFalse::negate() const
{
  return static_cast<Formula*>(new FormulaTrue());
}

/*!
 * \brief negates the formula
 */
Formula * FormulaEqual::negate() const
{
  return static_cast<Formula*>(((place_ < tokens_) || (place_ > tokens_)).clone());
}

/*!
 * \brief negates the formula
 */
Formula * FormulaNotEqual::negate() const
{
  return static_cast<Formula*>(new FormulaEqual(place_, tokens_));
}

/*!
 * \brief negates the formula
 */
Formula * FormulaGreater::negate() const
{
  return static_cast<Formula*>(new FormulaLessEqual(place_, tokens_));
}

/*!
 * \brief negates the formula
 */
Formula * FormulaGreaterEqual::negate() const
{
  return static_cast<Formula*>(new FormulaLess(place_, tokens_));
}

/*!
 * \brief negates the formula
 */
Formula * FormulaLess::negate() const
{
  return static_cast<Formula*>(new FormulaGreaterEqual(place_, tokens_));
}

/*!
 * \brief negates the formula
 */
Formula * FormulaLessEqual::negate() const
{
  return static_cast<Formula*>(new FormulaGreater(place_, tokens_));
}

/*! 
 * AND(a1,...,an,OR(b11,...,b1m1),...,OR(bl1,...,blml))
 * -> OR(AND(a1,...,an,b11,...,bl1), ...,
 *       AND(a1,...,an,b11,...,blml), ...,
 *       AND(a1,...,an,b1m1,...,blml))
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
  set<const Formula*> atomics;
  set<const Formula*> disjunctions;
  
  /// split children
  for(set<const Formula*>::iterator f = children_.begin();
       f != children_.end(); ++f)
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
  
  set<set<const Formula*> > disChildren;
  /// calculate cartesian product
  {
    set<set<const Formula*> > * result = new set<set<const Formula*> >();
    bool first = true;
    
    for(set<const Formula*>::iterator f = disjunctions.begin();
         f != disjunctions.end(); ++f)
    {
      set<set<const Formula*> > * newResult = new set<set<const Formula*> >();
      const Disjunction * d = static_cast<const Disjunction*>(*f);
      
      if(first)
      {
        first = false;
        for(set<const Formula*>::iterator ff = d->children().begin();
             ff != d->children().end(); ++ff)
        {
          set<const Formula*> tmpset;
          tmpset.insert(*ff);
          newResult->insert(tmpset);
        }
      }
      else
      {
        for(set<const Formula*>::iterator ff = d->children().begin();
             ff != d->children().end(); ++ff)
        {
          for(set<set<const Formula*> >::iterator fff = result->begin();
               fff != result->end(); ++fff)
          {
            set<const Formula*> tmpset = *fff;
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
  
  set<const Formula*> conjunctions;
  /// create conjunctions
  for(set<set<const Formula*> >::iterator f = disChildren.begin();
       f != disChildren.end(); ++f)
  {
    set<const Formula*> tmpset = util::setUnion(atomics, *f);
    conjunctions.insert(static_cast<Formula*>(new Conjunction(tmpset)));
  }
  
  Formula * result = static_cast<Formula*>(new Disjunction(conjunctions));
  
  // cleanup
  for(set<const Formula*>::iterator f = conjunctions.begin();
       f != conjunctions.end(); ++f)
  {
    delete (*f);
  }
  for(set<const Formula*>::iterator f = disjunctions.begin();
       f != disjunctions.end(); ++f)
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
  set<const Formula*> children;
  
  for(set<const Formula*>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    children.insert((*f)->dnf());
  }
  
  Formula * result = static_cast<Formula*>(new Disjunction(children));
  
  for(set<const Formula*>::iterator f = children.begin();
       f != children.end(); ++f)
  {
    delete (*f);
  }
  
  return result;
}

} /* namespace formula */

} /* namespace pnapi */
