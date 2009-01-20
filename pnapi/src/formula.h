#ifndef PNAPI_FORMULA_H
#define PNAPI_FORMULA_H

#include <list>
#include <string>

using std::list;
using std::string;

namespace pnapi
{

/// forward declaration needed by Formula
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
    // evaluating the formula
    virtual bool evaluate(Marking &m) = 0;

    // output method for formulas
    virtual const string toString() const = 0;

    virtual Formula * clone() const = 0;
  };




  /***********************************************
   *               Atomic Formulas               *
   ***********************************************/
  class AtomicFormula : public Formula
  {
  public:
    AtomicFormula(Place *p, unsigned int k);

    /// copy constructor
    AtomicFormula(const AtomicFormula &f, Place &p);

    virtual ~AtomicFormula() {}

    virtual bool evaluate(Marking &m);

    Place* getPlace();
    unsigned int getNumber();

    virtual const string toString() const = 0;

    virtual AtomicFormula * clone() const = 0;

  protected:
    Place *place_;
    unsigned int number_;
  };

  class FormulaEqual : public AtomicFormula
  {
  public:
    FormulaEqual(Place *p, unsigned int k);

    /// copy constructor
    FormulaEqual(const FormulaEqual &f);

    virtual ~FormulaEqual() {}

    bool evaluate(Marking &m);

    const string toString() const;

    FormulaEqual * clone() const;
  };

  class FormulaNotEqual : public AtomicFormula
  {
  public:
    FormulaNotEqual(Place *p, unsigned int k);

    /// copy constructor
    FormulaNotEqual(const FormulaNotEqual &f);

    virtual ~FormulaNotEqual() {}

    bool evaluate(Marking &m);

    const string toString() const;

    FormulaNotEqual * clone() const;
  };

  class FormulaGreater : public AtomicFormula
  {
  public:
    FormulaGreater(Place *p, unsigned int k);

    FormulaGreater(const FormulaGreater &f);

    virtual ~FormulaGreater() {}

    bool evaluate(Marking &m);

    const string toString() const;

    FormulaGreater * clone() const;
  };

  class FormulaGreaterEqual : public AtomicFormula
  {
  public:
    FormulaGreaterEqual(Place *p, unsigned int k);

    /// copy constructor
    FormulaGreaterEqual(const FormulaGreaterEqual &f);

    virtual ~FormulaGreaterEqual() {}

    bool evaluate(Marking &m);

    const string toString() const;

    FormulaGreaterEqual * clone() const;
  };

  class FormulaLess : public AtomicFormula
  {
  public:
    FormulaLess(Place *p, unsigned int k);

    FormulaLess(const FormulaLess &f);

    virtual ~FormulaLess() {}

    bool evaluate(Marking &m);

    const string toString() const;

    FormulaLess * clone() const;
  };

  class FormulaLessEqual : public AtomicFormula
  {
  public:
    FormulaLessEqual(Place *p, unsigned int k);

    /// copy constructor
    FormulaLessEqual(const FormulaLessEqual &f);

    virtual ~FormulaLessEqual() {}

    bool evaluate(Marking &m);

    const string toString() const;

    FormulaLessEqual * clone() const;
  };




  /************************************************
   *          Unary Boolean Formulas              *
   ************************************************/
  class UnaryBooleanFormula : public Formula
  {
  public:
    UnaryBooleanFormula(Formula *f);

    UnaryBooleanFormula(const UnaryBooleanFormula &f);

    virtual ~UnaryBooleanFormula() {}

    virtual bool evaluate(Marking &m) = 0;

    virtual const string toString() const = 0;

    virtual UnaryBooleanFormula * clone() const;

  protected:
    Formula *sub_;
  };

  class FormulaNot : public UnaryBooleanFormula
  {
  public:
    FormulaNot(Formula *f);

    /// copy constructor
    FormulaNot(const FormulaNot &f);

    virtual ~FormulaNot() {}

    bool evaluate(Marking &m);

    const string toString() const;

    FormulaNot * clone() const;
  };




  /************************************************
   *           n ary Boolean Formulas             *
   ************************************************/
  class NaryBooleanFormula : public Formula
  {
  public:
    NaryBooleanFormula(Formula *l, Formula *r);
    NaryBooleanFormula(list<Formula *> &flst);

    /// copy constructor
    NaryBooleanFormula(const NaryBooleanFormula &f);

    virtual ~NaryBooleanFormula() {}

    virtual bool evaluate(Marking &m) = 0;

    virtual const string toString() const = 0;

    NaryBooleanFormula * clone() const;

  protected:
    list<Formula *> subs_;
  };

  class FormulaAnd : public NaryBooleanFormula
  {
  public:
    FormulaAnd(Formula *l, Formula *r);
    FormulaAnd(list<Formula *> &flst);

    /// copy constructor
    FormulaAnd(const FormulaAnd &f);

    virtual ~FormulaAnd() {}

    bool evaluate(Marking &m);

    const string toString() const;

    FormulaAnd * clone() const;
  };

  class FormulaOr : public NaryBooleanFormula
  {
  public:
    FormulaOr(Formula *l, Formula *r);
    FormulaOr(list<Formula *> &flst);

    /// copy constructor
    FormulaOr(const FormulaOr &f);

    virtual ~FormulaOr() {}

    bool evaluate(Marking &m);

    const string toString() const;

    FormulaOr * clone() const;
  };




  /************************************************
   *                Empty Formula                 *
   ************************************************/
  class True : public Formula
  {
  public:
    virtual ~True() {}

    bool evaluate(Marking &m);

    const string toString() const;

    True * clone() const;
  };

  } /* namespace formula */

} /* namespace pnapi */

#endif /* FORMULA_H */
