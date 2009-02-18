#ifndef NDEBUG
#include <iostream>
#include "io.h"
using std::cout;
using std::endl;
#endif

#include <cassert>

#include "marking.h"
#include "formula.h"
#include "io.h"
#include "util.h"

using std::map;
using std::vector;
using std::ostream;

using pnapi::io::util::operator<<;

namespace pnapi
{

  namespace formula
  {

    /**************************************************************************
     ***** Constructor() implementation
     **************************************************************************/

    Operator::Operator(const Formula & f)
    {
      children_.push_back(f.clone());
    }

    Operator::Operator(const Formula & l, const Formula & r)
    {
      children_.push_back(l.clone());
      children_.push_back(r.clone());
    }

    Operator::Operator(const vector<const Formula *> & children,
		       const map<const Place *, const Place *> * places)
    {
      for (vector<const Formula *>::const_iterator it = children.begin();
	   it != children.end(); ++it)
	children_.push_back((*it)->clone(places));
    }

    Negation::Negation(const Formula & f) :
      Operator(f)
    {
    }

    Negation::Negation(const vector<const Formula *> & children,
		       const map<const Place *, const Place *> * places) :
      Operator(children, places)
    {
    }

    Negation::Negation(const Negation & n) :
      Operator(n.children_)
    {
    }

    Conjunction::Conjunction(const Formula & l, const Formula & r) :
      Operator(l, r)
    {
    }

    Conjunction::Conjunction(const vector<const Formula *> & children,
			     const map<const Place *, const Place *> * places) :
      Operator(children, places)
    {
    }

    Conjunction::Conjunction(const Conjunction & c) :
      Operator(c.children_)
    {
    }

    Disjunction::Disjunction(const Formula & l, const Formula & r) :
      Operator(l, r)
    {
    }

    Disjunction::Disjunction(const vector<const Formula *> & children,
			     const map<const Place *, const Place *> * places) :
      Operator(children, places)
    {
    }

    Disjunction::Disjunction(const Disjunction & d) :
      Operator(children_)
    {
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
      for (vector<const Formula *>::iterator it = children_.begin();
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
      for (vector<const Formula *>::const_iterator f = children_.begin();
	   f != children_.end(); f++)
	if (!(*f)->isSatisfied(m))
	  return false;
      return true;
    }

    bool Disjunction::isSatisfied(const Marking & m) const
    {
      for (vector<const Formula *>::const_iterator f = children_.begin();
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

    const vector<const Formula *> & Operator::children() const
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

/*    std::set<Place *> & Operator::concerningPlaces()
    {
      std::set<Place *> &cp = *new std::set<Place *>();
      cp.clear();

      for (int i = 0; i < children_.size(); i++)
        util::setUnion(cp, children_[i].concerningPlaces());

      return cp;
    }


    std::set<Place *> & Proposition::concerningPlaces()
    {
      std::set<Place *> &cp = *new std::set<Place *>();
      cp.clear();

      cp.insert(&place_);

      return cp;
    }


    std::set<Place *> & FormulaEqual::concerningPlaces()
    {
      std::set<Place *> &cp = *new std::set<Place *>();
      cp.clear();

      // implementation of "all other places empty"
      if (tokens_ == 0)
        cp.insert(&place_);

      return cp;
    }


    std::set<Place *> & FormulaTrue::concerningPlaces()
    {
      std::set<Place *> &cp = *new std::set<Place *>();
      cp.clear();

      return cp;
    }


    std::set<Place *> & FormulaFalse::concerningPlaces()
    {
      std::set<Place *> &cp = *new std::set<Place *>();
      cp.clear();

      return cp;
    }*/

  }

}
