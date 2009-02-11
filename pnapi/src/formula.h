// -*- C++ -*-

#ifndef PNAPI_FORMULA_H
#define PNAPI_FORMULA_H

#include <map>
#include <list>
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

    // forward declarations
    class Formula;
    class FormulaEqual;
    class FormulaNotEqual;
    class FormulaGreater;
    class FormulaGreaterEqual;
    class FormulaLess;
    class FormulaLessEqual;
    class FormulaAnd;
    class FormulaOr;
    class FormulaNot;


    /// formula construction operator
    FormulaEqual operator==(const Place &, unsigned int);

    /// formula construction operator
    FormulaNotEqual operator!=(const Place &, unsigned int);

    /// formula construction operator
    FormulaGreater operator>(const Place &, unsigned int);

    /// formula construction operator
    FormulaGreaterEqual operator>=(const Place &, unsigned int);

    /// formula construction operator
    FormulaLess operator<(const Place &, unsigned int);

    /// formula construction operator
    FormulaLessEqual operator<=(const Place &, unsigned int);

    /// formula construction operator
    FormulaAnd operator&&(const Formula &, const Formula &);

    /// formula construction operator
    FormulaOr operator||(const Formula &, const Formula &);

    /// formula construction operator
    FormulaNot operator!(const Formula &);


    /*!
     */
    class Formula
    {
    public:
      
      // destructor
      virtual ~Formula();

      // evaluating the formula under the given marking
      virtual bool evaluate(const Marking &) const =0;

      // create a deep copy of the formula
      virtual Formula & clone() const =0;


      virtual const std::string toString() const =0;

      virtual Formula * flatCopy() const =0;

      virtual Formula * deepCopy(std::map<const Place *, Place *> &) const =0;

    };




  /***********************************************
   *               Atomic Formulas               *
   ***********************************************/
  class AtomicFormula : public Formula
  {
  public:

    AtomicFormula(const Place &, unsigned int);

    virtual ~AtomicFormula() {}

    virtual bool evaluate(const Marking &m) const = 0;

    const Place & getPlace();

    unsigned int getNumber();

    virtual const std::string toString() const = 0;

    virtual AtomicFormula * flatCopy() const = 0;

    virtual AtomicFormula * deepCopy(std::map<const Place *, Place *> &newP) const = 0;

  protected:
    const Place & place_;
    const unsigned int number_;
  };

  class FormulaEqual : public AtomicFormula
  {
  public:

    FormulaEqual(const Place &, unsigned int);

    virtual ~FormulaEqual() {}

    bool evaluate(const Marking &m) const;

    virtual FormulaEqual & clone() const;

    const std::string toString() const;

    FormulaEqual * flatCopy() const;

    FormulaEqual * deepCopy(std::map<const Place *, Place *> &newP) const;
  };

  class FormulaNotEqual : public AtomicFormula
  {
  public:

    FormulaNotEqual(const Place &, unsigned int k);

    virtual ~FormulaNotEqual() {}

    bool evaluate(const Marking &m) const;

    virtual FormulaNotEqual & clone() const;

    const std::string toString() const;

    FormulaNotEqual * flatCopy() const;

    FormulaNotEqual * deepCopy(std::map<const Place *, Place *> &newP) const;
  };

  class FormulaGreater : public AtomicFormula
  {
  public:
    /// standard constructor
    FormulaGreater(const Place &p, unsigned int k);

    /// copy constructor
    //FormulaGreater(const FormulaGreater &f);

    virtual ~FormulaGreater() {}

    bool evaluate(const Marking &m) const;

    virtual FormulaGreater & clone() const;

    const std::string toString() const;

    FormulaGreater * flatCopy() const;

    FormulaGreater * deepCopy(std::map<const Place *, Place *> &newP) const;
  };

  class FormulaGreaterEqual : public AtomicFormula
  {
  public:
    /// standard constructor
    FormulaGreaterEqual(const Place &p, unsigned int k);

    /// copy constructor
    //FormulaGreaterEqual(const FormulaGreaterEqual &f);

    virtual ~FormulaGreaterEqual() {}

    bool evaluate(const Marking &m) const;

    virtual FormulaGreaterEqual & clone() const;

    const std::string toString() const;

    FormulaGreaterEqual * flatCopy() const;

    FormulaGreaterEqual * deepCopy(std::map<const Place *, Place *> &newP) const;
  };

  class FormulaLess : public AtomicFormula
  {
  public:
    /// standard constructor
    FormulaLess(const Place &p, unsigned int k);

    ///copy constructor
    //FormulaLess(const FormulaLess &f);

    virtual ~FormulaLess() {}

    bool evaluate(const Marking &m) const;

    virtual FormulaLess & clone() const;

    const std::string toString() const;

    FormulaLess * flatCopy() const;

    FormulaLess * deepCopy(std::map<const Place *, Place *> &newP) const;
  };

  class FormulaLessEqual : public AtomicFormula
  {
  public:
    /// standard constructor
    FormulaLessEqual(const Place &p, unsigned int k);

    /// copy constructor
    //FormulaLessEqual(const FormulaLessEqual &f);

    virtual ~FormulaLessEqual() {}

    bool evaluate(const Marking &m) const;

    virtual FormulaLessEqual & clone() const;

    const std::string toString() const;

    FormulaLessEqual * flatCopy() const;

    FormulaLessEqual * deepCopy(std::map<const Place *, Place *> &newP) const;
  };




  /************************************************
   *          Unary Boolean Formulas              *
   ************************************************/
  class UnaryBooleanFormula : public Formula
  {
  public:
    /// constructor
    UnaryBooleanFormula(const Formula &);

    virtual ~UnaryBooleanFormula() {}

    virtual bool evaluate(const Marking &m) const = 0;

    virtual const std::string toString() const = 0;

    virtual UnaryBooleanFormula * flatCopy() const = 0;

    virtual UnaryBooleanFormula * deepCopy(std::map<const Place *, Place *> &newP) const = 0;

  protected:
    const Formula & sub_;
  };

  class FormulaNot : public UnaryBooleanFormula
  {
  public:

    /// constructor
    FormulaNot(const Formula &);

    virtual ~FormulaNot() {}

    bool evaluate(const Marking &m) const;

    virtual FormulaNot & clone() const;

    const std::string toString() const;

    FormulaNot * flatCopy() const;

    FormulaNot * deepCopy(std::map<const Place *, Place *> &newP) const;
  };




  /************************************************
   *           n ary Boolean Formulas             *
   ************************************************/
  class NaryBooleanFormula : public Formula
  {
  public:

    NaryBooleanFormula(const Formula &, const Formula &);

    NaryBooleanFormula(const std::list<const Formula *> &);

    virtual ~NaryBooleanFormula() {}

    virtual bool evaluate(const Marking &m) const = 0;

    virtual const std::string toString() const = 0;

    void addSubFormula(Formula *s);

    virtual NaryBooleanFormula * flatCopy() const = 0;

    virtual NaryBooleanFormula * deepCopy(std::map<const Place *, Place *> &) const =0;

  protected:

    std::list<const Formula *> subs_;

    std::list<const Formula *> cloneChildren() const;

  };

  class FormulaAnd : public NaryBooleanFormula
  {
  public:

    FormulaAnd(const Formula &l, const Formula &r);

    FormulaAnd(const std::list<const Formula *> &);

    virtual ~FormulaAnd() {}

    bool evaluate(const Marking &m) const;

    virtual FormulaAnd & clone() const;

    const std::string toString() const;

    FormulaAnd * flatCopy() const;

    FormulaAnd * deepCopy(std::map<const Place *, Place *> &newP) const;
  };

  class FormulaOr : public NaryBooleanFormula
  {
  public:

    FormulaOr(const Formula &, const Formula &);

    FormulaOr(const std::list<const Formula *> &);

    virtual ~FormulaOr() {}

    bool evaluate(const Marking &) const;

    virtual FormulaOr & clone() const;

    const std::string toString() const;

    FormulaOr * flatCopy() const;

    FormulaOr * deepCopy(std::map<const Place *, Place *> &newP) const;
  };




  /************************************************
   *                Empty Formula                 *
   ************************************************/
  class True : public Formula
  {
  public:
    virtual ~True() {}

    bool evaluate(const Marking &m) const;

    virtual True & clone() const;

    const std::string toString() const;

    True * flatCopy() const;

    True * deepCopy(std::map<const Place *, Place *> &newP) const;
  };

  class False : public Formula
  {
  public:
    virtual ~False() {}

    bool evaluate(const Marking &m) const;

    virtual False & clone() const;

    const std::string toString() const;

    False * flatCopy() const;

    False * deepCopy(std::map<const Place *, Place *> &newP) const;
  };

  }

}

#endif
