/*****************************************************************************\
 ProductOG -- Computing product OGs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 ProductOG is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 ProductOG is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with ProductOG.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

#include "config.h"
#include "formula.h"
#include "util.h"
#include <string>
#include <set>
#include <iostream>

using std::string;
using std::set;
using std::cerr;
using std::endl;


/*!
 * \brief comparison operator
 */
bool Formula::operator==(bool) const
{
  return false;
}

/*!
 * \brief create a conjunction
 */
Formula * Formula::operator&(const Formula & other) const
{
  return static_cast<Formula *>(new Conjunction(clone(), other.clone()));
}

/*!
 * \brief create a disjunction
 */
Formula * Formula::operator|(const Formula & other) const
{
  return static_cast<Formula *>(new Disjunction(clone(), other.clone()));
}

/*!
 * \brief clear children without deleting them
 */
void Operator::clear()
{
  children_.clear();
}

/*!
 * \brief whether operatore contains a given literal
 * 
 * \pre dnf() has been called before
 */
bool Operator::isIn(const Literal & l) const
{
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    if((**f) == l)
    {
      return true;
    }
  }
  
  return false;
}

/*!
 * \brief constructor
 * 
 * \note  if left or right child is also a conjunction,
 *        it will be merged with this node.
 */
Conjunction::Conjunction(Formula * left, Formula * right)
{
  Conjunction * l = dynamic_cast<Conjunction *>(left);
  if(l == NULL)
  {
    children_.insert(left);
  }
  else
  {
    // if left child is also a conjunction, we can merge it with us
    children_ = l->children_;
    l->clear();
    delete l;
  }
  
  Conjunction * r = dynamic_cast<Conjunction *>(right);
  if(r == NULL)
  {
    children_.insert(right);
  }
  else
  {
    // if right child is also a conjunction, we can merge it with us
    for(set<Formula *>::iterator f = r->children_.begin(); f != r->children_.end(); ++f)
    {
      children_.insert(*f);
    }
    r->clear();
    delete r;
  }
}

/*!
 * \brief copyconstructor
 */
Conjunction::Conjunction(const Conjunction & cc)
{
  for(set<Formula*>::iterator f = cc.children_.begin(); 
       f != cc.children_.end(); ++f)
  {
    children_.insert((*f)->clone());
  }
}

/*!
 * \brief yet another constructor
 */
Conjunction::Conjunction(const set<Formula *> & formulae)
{
  for(set<Formula *>::iterator f = formulae.begin();
       f != formulae.end(); ++f)
  {
    Formula * ff = (*f)->clone();
    Conjunction * c = dynamic_cast<Conjunction *>(ff);
    
    if(c != NULL)
    {
      for(set<Formula*>::iterator fff = c->children_.begin();
           fff != c->children_.end(); ++fff)
      {
        children_.insert(*fff);
      }
      c->clear();
      delete c;
    }
    else
    {
      children_.insert(ff);
    }
  }
}

/*!
 * \brief destructor
 * 
 * \note  children will also be deleted.
 */
Conjunction::~Conjunction()
{
  for(set<Formula *>::iterator f = children_.begin(); f != children_.end(); ++f)
  {
    delete (*f);
  }
}

// clone the formula
Formula * Conjunction::clone() const
{
  return static_cast<Formula *>(new Conjunction(*this));
}

/*!
 * \brief forms formula in disjunctive normal form
 */
Formula * Conjunction::dnf() const
{
  set<Formula *> literals;
  set<Formula *> disjunctions;
  
  /// split children
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    if(dynamic_cast<Disjunction *>(*f) != NULL)
    {
      /// convert child in dnf
      disjunctions.insert((*f)->dnf());
    }
    else
    {
      literals.insert(*f);
    }
  }
  
  if(disjunctions.empty()) // just literals
    return clone();
  
  set<set<Formula *> > disChildren;
  /// calculate cartesian product
  {
    set<set<Formula *> > * result = new set<set<Formula *> >();
    bool first = true;
    
    for(set<Formula *>::iterator f = disjunctions.begin();
         f != disjunctions.end(); ++f)
    {
      set<set<Formula *> > * newResult = new set<set<Formula *> >();
      Disjunction * d = static_cast<Disjunction *>(*f);
      
      if(first)
      {
        first = false;
        for(set<Formula *>::iterator ff = d->children_.begin();
             ff != d->children_.end(); ++ff)
        {
          set<Formula *> tmpset;
          tmpset.insert(*ff);
          newResult->insert(tmpset);
        }
      }
      else
      {
        for(set<Formula *>::iterator ff = d->children_.begin();
             ff != d->children_.end(); ++ff)
        {
          for(set<set<Formula *> >::iterator fff = result->begin();
               fff != result->end(); ++fff)
          {
            set<Formula *> tmpset = *fff;
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
  
  set<Formula *> conjunctions;
  /// create conjunctions
  for(set<set<Formula *> >::iterator f = disChildren.begin();
       f != disChildren.end(); ++f)
  {
    set<Formula *> tmpset = util::setUnion(literals, *f);
    conjunctions.insert(new Conjunction(tmpset));
  }
  
  Formula * result = static_cast<Formula *>(new Disjunction(conjunctions));
  
  // cleanup
  for(set<Formula *>::iterator f = conjunctions.begin();
       f != conjunctions.end(); ++f)
  {
    delete (*f);
  }
  for(set<Formula *>::iterator f = disjunctions.begin();
       f != disjunctions.end(); ++f)
  {
    delete (*f);
  }
  
  return result;
}

/*!
 * \brief whether formula is satisfied under a given set of labels
 */
bool Conjunction::sat(const set<std::string> & labels) const
{
  bool result = true;
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    result = result && (*f)->sat(labels);
  }
  
  return result;
}

/*!
 * \brief simplify formula
 * 
 * \pre   dnf() has be called before, i.e. children are now literals only
 */
Formula * Conjunction::simplify(const std::set<std::string> & labels) const
{
  if(sat(labels))
  {
    return killLiterals();
  }
  
  return static_cast<Formula *>(new Literal(false));
}

/*!
 * \brief remove "and true" and "x and x"
 */
Formula * Conjunction::killLiterals() const
{
  Conjunction * result = static_cast<Conjunction *>(clone());
  set<Formula *> newChildren;
  for(set<Formula *>::iterator f = result->children_.begin();
       f != result->children_.end(); ++f)
  {
    if((**f) == true)
    {
      delete (*f);
    }
    else
    {
      bool alreadyIn = false;
      set<Formula*> obsolete;
      for(set<Formula *>::iterator ff = newChildren.begin();
           ff != newChildren.end(); ++ff)
      {
        if((**ff) <= (**f))
        {
          alreadyIn = true;
          break;
        }
        if((**f) <= (**ff))
        {
          obsolete.insert(*ff);
        }
      }
      
      for(set<Formula*>::iterator ff = obsolete.begin();
           ff != obsolete.end(); ++ff)
      {
        assert(!alreadyIn);
        newChildren.erase(*ff);
        delete (*ff);
      }
      
      if(alreadyIn)
      {
        delete (*f);
      }
      else
      {
        newChildren.insert(*f);
      }
    }
  }
  
  if(newChildren.size() == 1)
  {
    result->clear();
    delete result;
    return *(newChildren.begin());
  }
  
  result->children_ = newChildren;
  
  if(newChildren.empty())
  {
    delete result;
    return static_cast<Formula *>(new Literal(true));
  }
  
  return static_cast<Formula *>(result);
}

/*!
 * \brief comparison operator
 * 
 * \pre   dnf() has been called before
 */
bool Conjunction::operator==(const Formula & other) const
{
  const Conjunction * o = dynamic_cast<const Conjunction *>(&other);
  if(o == NULL)
    return false;
  
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    if(!o->isIn(*static_cast<Literal *>(*f)))
      return false;
  }
  for(set<Formula *>::iterator f = o->children_.begin();
       f != o->children_.end(); ++f)
  {
    if(!isIn(*static_cast<Literal *>(*f)))
      return false;
  }
  
  return true;
}

/*!
 * \brief whether (NOT this) implies (NOT the other formula)
 * 
 * (NOT this) is TRUE, if at least one child equals to FALSE,
 * so (NOT other) then also has to be TRUE. I.e. for all
 * children f has to apply, that (NOT f) implies (NOT other),
 * i.e. f <= other.  
 */
bool Conjunction::operator<=(const Formula & other) const
{
  bool result = true;
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    result = result && ((**f) <= other);
  }
  
  return result;
}

/*!
 * \brief write to stream
 */
std::ostream & Conjunction::out(std::ostream & os)
{
  os << "(";
  string delim = "";
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    os << delim << (*f);
    delim = " * ";
  }
  os << ")";
  return os;
}



/*!
 * \brief constructor
 * 
 * \note  if left or right child is also a disjunction,
 *        it will be merged with this node.
 */
Disjunction::Disjunction(Formula * left, Formula * right)
{
  Disjunction * l = dynamic_cast<Disjunction *>(left);
  if(l == NULL)
  {
    children_.insert(left);
  }
  else
  {
    // if left child is also a disjunction, we can merge it with us
    children_ = l->children_;
    l->clear();
    delete l;
  }
  
  Disjunction * r = dynamic_cast<Disjunction *>(right);
  if(r == NULL)
  {
    children_.insert(right);
  }
  else
  {
    // if right child is also a disjunction, we can merge it with us
    for(set<Formula *>::iterator f = r->children_.begin(); f != r->children_.end(); ++f)
    {
      children_.insert(*f);
    }
    r->clear();
    delete r;
  }
}

/*!
 * \brief copyconstructor
 */
Disjunction::Disjunction(const Disjunction & cc)
{
  for(set<Formula*>::iterator f = cc.children_.begin(); 
       f != cc.children_.end(); ++f)
  {
    children_.insert((*f)->clone());
  }
}

/*!
 * \brief yet another constructor
 */
Disjunction::Disjunction(const std::set<Formula *> & formulae)
{
  for(set<Formula *>::iterator f = formulae.begin();
       f != formulae.end(); ++f)
  {
    children_.insert((*f)->clone());
  }
}

/*!
 * \brief destructor
 */
Disjunction::~Disjunction()
{
  for(set<Formula *>::iterator f = children_.begin(); f != children_.end(); ++f)
  {
    delete (*f);
  }
}

/*!
 * \brief clone the formula
 */
Formula * Disjunction::clone() const
{
  return static_cast<Formula *>(new Disjunction(*this));
}

/*!
 * \brief forms formula in disjunctive normal form
 */
Formula * Disjunction::dnf() const
{
  set<Formula *> children;
  
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    children.insert((*f)->dnf());
  }
  
  Formula * result = static_cast<Formula*>(new Disjunction(children));
  
  for(set<Formula *>::iterator f = children.begin();
       f != children.end(); ++f)
  {
    delete (*f);
  }
  
  return result;
}

/*!
 * \brief whether formula is satisfied
 */
bool Disjunction::sat(const std::set<std::string> & labels) const
{
  bool result = false;
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    result = result || (*f)->sat(labels);
  }
  
  return result;
}

/*!
 * \brief simplifies the formula
 */
Formula * Disjunction::simplify(const std::set<std::string> & labels) const
{
  if(sat(labels))
  {
    return killChildren(labels);
  }
  
  return static_cast<Formula *>(new Literal(false));
}

/*!
 * \brief comparison operator
 */
bool Disjunction::operator==(const Formula & other) const
{
  const Disjunction * o = dynamic_cast<const Disjunction *>(&other);
  if(o == NULL)
    return false;
  
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    if(!o->isIn(*static_cast<Literal *>(*f)))
      return false;
  }
  for(set<Formula *>::iterator f = o->children_.begin();
       f != o->children_.end(); ++f)
  {
    if(!isIn(*static_cast<Literal *>(*f)))
      return false;
  }
  
  return true;
}

/*!
 * \brief whether (NOT this) implies (NOT the other formula)
 * 
 * This is a disjunction, so the formula has the form
 * (a_1 + a_2 + a_3 + ... + a_n) with child formulae a_i.
 * 
 *         ~(a_1 + a_2 + a_3 + ... + a_n) -> ~other
 * equals  (a_1 + a_2 + a_3 + ... + a_n) + ~other
 * equals  (a_1 + a_2 + a_3 + ... + a_n) + (~other + ~other + ~other + ... ~other)
 * equals  (a_1 + ~other) + (a_2 + ~other) + (a_3 + ~other) + ... + (a_n + ~other)
 * equals  (~a_1 -> ~other) + (~a_2 -> ~other) + (~a_3 -> ~other) + ... + (~a_n + ~other)
 * 
 * I.e. this operator returns true, if for at least one child 
 * formula f applies: f <= other.
 */
bool Disjunction::operator<=(const Formula & other) const
{
  bool result = false;
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    result = result || ((**f) <= other);
  }
  
  return result;
}

/*!
 * \brief remove "or false" and "x or x"
 * 
 * \pre   dnf() has been called before
 */
Formula * Disjunction::killChildren(const std::set<std::string> & labels) const
{
  
  set<Formula *> newChildren;
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    Formula * child = (*f)->simplify(labels);
    
    if((*child) == false)
    {
      delete (child);
    }
    else
    {
      bool alreadyIn = false;
      set<Formula*> obsolete;
      for(set<Formula *>::iterator ff = newChildren.begin();
           ff != newChildren.end(); ++ff)
      {
        if((**ff) <= (*child))
        {
          alreadyIn = true;
          break;
        }
        if((*child) <= (**ff))
        {
          obsolete.insert(*ff);
        }
      }
      
      for(set<Formula*>::iterator ff = obsolete.begin();
           ff != obsolete.end(); ++ff)
      {
        assert(!alreadyIn);
        newChildren.erase(*ff);
        delete (*ff);
      }
      
      if(alreadyIn)
      {
        delete (child);
      }
      else
      {
        newChildren.insert(child);
      }
    }
  }
  
  
  
  if(newChildren.empty())
  {
    return static_cast<Formula *>(new Literal(false));
  }
  
  for(set<Formula *>::iterator f = newChildren.begin();
        f != newChildren.end(); ++f)
  {
    if((**f) == true)
    {
      for(set<Formula *>::iterator f = newChildren.begin();
            f != newChildren.end(); ++f)
      {
        delete (*f);
      }
      
      return static_cast<Formula *>(new Literal(true));
    }
  }
  
  if(newChildren.size() == 1)
  {
    return *(newChildren.begin());
  }
  
  Disjunction * result = new Disjunction(newChildren);
  
  // cleanup
  for(set<Formula *>::iterator f = newChildren.begin();
          f != newChildren.end(); ++f)
  {
    delete (*f);
  }
  
  return static_cast<Formula *>(result);
}

/*!
 * \brief write to stream
 */
std::ostream & Disjunction::out(std::ostream & os)
{
  os << "(";
  string delim = "";
  for(set<Formula *>::iterator f = children_.begin();
       f != children_.end(); ++f)
  {
    os << delim << (*f);
    delim = " + ";
  }
  os << ")";
  return os;
}



/*!
 * \brief constructor
 */
Literal::Literal(const std::string & label)
{
  literal_ = label;
}

/*!
 * \brief constructor
 */
Literal::Literal(bool constant)
{
  if(constant)
  {
    literal_ = "true";
  }
  else
  {
    literal_ = "false"; 
  }
}

/*!
 * \brief copyconstructor
 */
Literal::Literal(const Literal & cc)
{
  literal_ = cc.literal_;
}

/*!
 * \brief clone the formula
 */
Formula * Literal::clone() const
{
  return static_cast<Formula *>(new Literal(*this));
}

/*!
 * \brief comparison operator
 */
bool Literal::operator==(const std::string & label) const
{
  return (literal_ == label);
}

/*!
 * \brief comparison operator
 */
bool Literal::operator==(const Formula & other) const
{
  const Literal * o = dynamic_cast<const Literal *>(&other);
  if(o == NULL)
    return false;
  
  return (literal_ == o->literal_);
}

// comparison operator
bool Literal::operator==(bool other) const
{
  if(other)
  {
    return (literal_ == "true");
  }
  
  return (literal_ == "false");
}

/*!
 * \brief whether (NOT this) implies (NOT the other formula)
 * 
 * \pre   dnf() has been called before
 * 
 * ~TRUE -> ~other is TRUE (FALSE implies everything)
 * ~FALSE -> ~other is FALSE (TRUE implies nothing)
 * ~a -> ~b is TRUE iff a = b
 * ~a -> ~(x_1 * ... * x_2) is TRUE iff the exists an i with a = x_i. 
 * ~a -> ~(x_1 + ... + x_n) is FALSE (will not be needed, anyway)
 */
bool Literal::operator<=(const Formula & other) const
{
  if(literal_ == "true")
    return true;
    
  if(literal_ == "false")
    return false;
  
  const Literal * l = dynamic_cast<const Literal *>(&other);
  if(l != NULL)
  {
    return (literal_ == l->literal_);
  }
  
  const Conjunction * c = dynamic_cast<const Conjunction *>(&other);
  if(c != NULL)
  {
    return c->isIn(*this);
  }
  
  return false;
}

/*!
 * \brief forms formula in disjunctive normal form
 */
Formula * Literal::dnf() const
{
  return clone();
}

/*!
 * \brief whether formula is satisfied
 */
bool Literal::sat(const std::set<std::string> & labels) const
{
  if((literal_ == "true") || (literal_ == "final"))
    return true;
  if(literal_ == "false")
    return false;
  
  return (labels.count(literal_) > 0);
}

/*!
 * \brief simplifies the formula
 */
Formula * Literal::simplify(const std::set<std::string> & labels) const
{
  if(sat(labels))
    return clone();
  
  return static_cast<Formula *>(new Literal(false));
}

/*!
 * \brief write to stream
 */
std::ostream & Literal::out(std::ostream & os)
{
  os << literal_;
  return os;
}

/*!
 * \brief syntactic sugar
 */
std::ostream & operator<<(std::ostream & os, Formula * f)
{
  return f->out(os);
}
