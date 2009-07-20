#include "config.h"
#include <cassert>

#ifndef NDEBUG
#include <iostream>
#include "io.h"
#include "component.h"
using std::cout;
using std::endl;
#endif

#include "marking.h"
#include "formula.h"
#include "io.h"
#include "util.h"

using std::map;
using std::set;
using std::ostream;

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

    Conjunction::Conjunction(const Formula & f, const set<const Place *> & wc) :
      Operator(f)
    {
      set<const Place *> cps = places();

      for (set<const Place *>::const_iterator it = wc.begin(); it != wc.end();
	   ++it)
	if (cps.find(*it) == cps.end())
	  children_.insert(new FormulaEqual(**it, 0));

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

    bool Conjunction::isSatisfied(const Marking & m) const
    {
      for (set<const Formula *>::const_iterator f = children_.begin();
	   f != children_.end(); f++)
	if (!(*f)->isSatisfied(m))
	  return false;
      return true;
    }

    bool Disjunction::isSatisfied(const Marking & m) const
    {
      for (set<const Formula *>::const_iterator f = children_.begin();
	   f != children_.end(); f++)
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
     ***** concerning places implementation
     **************************************************************************/

    set<const Place *> Formula::places(bool excludeEmpty) const
    {
      return set<const Place *>();
    }

    set<const Place *> Operator::places(bool excludeEmpty) const
    {
      set<const Place *> places;
      for (set<const Formula *>::const_iterator it = children_.begin();
	   it != children_.end(); ++it)
	{
	  set<const Place *> childCps = (*it)->places(excludeEmpty);
	  places.insert(childCps.begin(), childCps.end());
	}
      return places;
    }

    set<const Place *> Proposition::places(bool excludeEmpty) const
    {
      set<const Place *> places;
      places.insert(&place_);
      return places;
    }

    set<const Place *> FormulaEqual::places(bool excludeEmpty) const
    {
      set<const Place *> places;
      if (!excludeEmpty || tokens_ > 0)
        places.insert(&place_);
      return places;
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
	if (dynamic_cast<const FormulaTrue *>(*it) != NULL)
	  {
	    children_.erase(*it);
	    delete *it;
	  }
	else
	  {
	    const Operator * o = dynamic_cast<const Conjunction *>(*it);
	    if (o != NULL)
	      {
		for (set<const Formula *>::const_iterator it =
		       o->children().begin(); it != o->children().end(); ++it)
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
	if (dynamic_cast<const FormulaFalse *>(*it) != NULL)
	  {
	    children_.erase(*it);
	    delete *it;
	  }
	else
	  {
	    const Operator * o = dynamic_cast<const Disjunction *>(*it);
	    if (o != NULL)
	      {
		for (set<const Formula *>::const_iterator it =
		       o->children().begin(); it != o->children().end(); ++it)
		  children_.insert((*it)->clone());
		children_.erase(o);
		delete o;
	      }
	  }
    }


  /***************************************************************************
   ******** removeProposition
   ***************************************************************************/

    bool Operator::removeProposition(const Place *p)
    {
      for (std::set<const Formula *>::iterator f = children_.begin(); f != children_.end(); f++)
      {
        Formula *ff = const_cast<Formula *>(*f);
        if (ff->removeProposition(p))
        {
          children_.erase(f);
        }
      }

      simplifyChildren();

      return false;
    }


    bool Proposition::removeProposition(const Place *p)
    {
      if (p == &place_)
        return true;
      else
        return false;
    }


    bool FormulaTrue::removeProposition(const Place *p)
    {
      return false;
    }


    bool FormulaFalse::removeProposition(const Place *p)
    {
      return false;
    }

  } /* namespace formula */

} /* namespace pnapi */
