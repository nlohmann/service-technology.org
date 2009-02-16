// -*- C++ -*-

#ifndef PNAPI_FORMULA_H
#define PNAPI_FORMULA_H

#include <map>
#include <vector>
#include <set>
#include <string>

namespace pnapi
{

  // forward declarations
  class PetriNet;
  class Place;
  class Node;
  class Marking;


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
      virtual Formula * clone(std::map<const Place *, const Place *> *
			      = NULL) const =0;

      /// output the formula
      virtual std::ostream & output(std::ostream &) const =0;

      /// set of concerning places
      //virtual const std::set<std::string *> & concerningPlaces() const = 0;

    };



    /**************************************************************************
     ***** Formula Tree Inner Nodes
     **************************************************************************/

    class Operator : public Formula
    {
    public:

      Operator(const Formula &);

      Operator(const Formula &, const Formula &);

      Operator(const std::vector<const Formula *> &,
	       std::map<const Place *, const Place *> * = NULL);

      ~Operator();

      const std::vector<const Formula *> & children() const;

      const std::set<std::string *> & concerningPlaces() const;

    protected:
      std::vector<const Formula *> children_;
    };


    class Negation : public Operator
    {
    public:

      Negation(const Negation &);

      Negation(const Formula &);

      Negation(const std::vector<const Formula *> &,
	       std::map<const Place *, const Place *> * = NULL);

      bool isSatisfied(const Marking &) const;

      Negation * clone(std::map<const Place *, const Place *> * = NULL) const;

      std::ostream & output(std::ostream &) const;
    };


    class Conjunction : public Operator
    {
    public:

      Conjunction(const Conjunction &);

      Conjunction(const Formula &, const Formula &);

      Conjunction(const std::vector<const Formula *> &,
		  std::map<const Place *, const Place *> * = NULL);

      bool isSatisfied(const Marking &) const;

      Conjunction * clone(std::map<const Place *, const Place *> *
			  = NULL) const;

      std::ostream & output(std::ostream &) const;
    };


    class Disjunction : public Operator
    {
    public:

      Disjunction(const Disjunction &);

      Disjunction(const Formula &, const Formula &);

      Disjunction(const std::vector<const Formula *> &,
		  std::map<const Place *, const Place *> * = NULL);

      bool isSatisfied(const Marking &) const;

      Disjunction * clone(std::map<const Place *, const Place *> *
			  = NULL) const;

      std::ostream & output(std::ostream &) const;
    };



    /**************************************************************************
     ***** Formula Tree Leaves
     **************************************************************************/

    class Proposition : public Formula
    {
    public:

      Proposition(const Place &, unsigned int,
		  std::map<const Place *, const Place *> *);

      const Place & place() const;

      unsigned int tokens() const;

      const std::set<std::string *> & concerningPlaces() const;

    protected:
      const Place & place_;
      const unsigned int tokens_;
    };


    class FormulaTrue : public Formula
    {
    public:

      bool isSatisfied(const Marking &) const;

      FormulaTrue * clone(std::map<const Place *, const Place *> *
			  = NULL) const;

      std::ostream & output(std::ostream &) const;

      const std::set<std::string *> & concerningPlaces() const;
    };


    class FormulaFalse : public Formula
    {
    public:
      bool isSatisfied(const Marking &) const;

      FormulaFalse * clone(std::map<const Place *, const Place *> *
			      = NULL) const;

      std::ostream & output(std::ostream &) const;

      const std::set<std::string *> & concerningPlaces() const;
    };


    class FormulaEqual : public Proposition
    {
    public:

      FormulaEqual(const Place &, unsigned int,
		   std::map<const Place *, const Place *> * = NULL);

      bool isSatisfied(const Marking &) const;

      FormulaEqual * clone(std::map<const Place *, const Place *> *
			   = NULL) const;

      std::ostream & output(std::ostream &) const;

      const std::set<std::string *> & concerningPlaces() const;
    };


    class FormulaNotEqual : public Proposition
    {
    public:

      FormulaNotEqual(const Place &, unsigned int,
		      std::map<const Place *, const Place *> * = NULL);

      bool isSatisfied(const Marking &) const;

      FormulaNotEqual * clone(std::map<const Place *, const Place *> *
			      = NULL) const;

      std::ostream & output(std::ostream &) const;
    };


    class FormulaGreater : public Proposition
    {
    public:
      FormulaGreater(const Place &, unsigned int,
		     std::map<const Place *, const Place *> * = NULL);

      bool isSatisfied(const Marking &) const;

      FormulaGreater * clone(std::map<const Place *, const Place *> *
			     = NULL) const;

      std::ostream & output(std::ostream &) const;
    };


    class FormulaGreaterEqual : public Proposition
    {
    public:
      FormulaGreaterEqual(const Place &, unsigned int,
			  std::map<const Place *, const Place *> * = NULL);

      bool isSatisfied(const Marking &) const;

      FormulaGreaterEqual * clone(std::map<const Place *, const Place *> *
				  = NULL) const;

      std::ostream & output(std::ostream &) const;
    };


    class FormulaLess : public Proposition
    {
    public:
      FormulaLess(const Place &, unsigned int,
		  std::map<const Place *, const Place *> * = NULL);

      bool isSatisfied(const Marking &) const;

      FormulaLess * clone(std::map<const Place *, const Place *> *
			  = NULL) const;

      std::ostream & output(std::ostream &) const;
    };


    class FormulaLessEqual : public Proposition
    {
    public:
      FormulaLessEqual(const Place &, unsigned int,
		       std::map<const Place *, const Place *> * = NULL);

      bool isSatisfied(const Marking &) const;

      FormulaLessEqual * clone(std::map<const Place *, const Place *> *
			       = NULL) const;

      std::ostream & output(std::ostream &) const;
    };

  }

}

#endif
