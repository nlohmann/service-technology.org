// -*- C++ -*-

#ifndef PNAPI_FORMULA_H
#define PNAPI_FORMULA_H

#include <map>
#include <set>
#include <string>
#include "marking.h"

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
      virtual std::set<const Place *> places(bool excludeEmpty = false) const;

      /// removes all propositions with Place p
      virtual bool removeProposition(const Place *) =0;

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

      std::set<const Place *> places(bool excludeEmpty = false) const;

      bool removeProposition(const Place *);

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

    protected:
      void simplifyChildren();
    };


    class Conjunction : public Operator
    {
    public:

      Conjunction(const Conjunction &);

      Conjunction();

      Conjunction(const Formula &);

      Conjunction(const Formula &, const Formula &);

      Conjunction(const Formula &, const std::set<const Place *> &);

      Conjunction(const std::set<const Formula *> &,
		  const std::map<const Place *, const Place *> * = NULL);

      bool isSatisfied(const Marking &) const;

      Conjunction * clone(const std::map<const Place *, const Place *> *
			  = NULL) const;

      std::ostream & output(std::ostream &) const;

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

      std::set<const Place *> places(bool excludeEmpty = false) const;

      bool removeProposition(const Place *);

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

      bool removeProposition(const Place *);

    };


    class FormulaFalse : public Formula
    {
    public:
      bool isSatisfied(const Marking &) const;

      FormulaFalse * clone(const std::map<const Place *, const Place *> *
			      = NULL) const;

      std::ostream & output(std::ostream &) const;

      bool removeProposition(const Place *);
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

      std::set<const Place *> places(bool excludeEmpty = false) const;
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
    };

  }

}

#endif
