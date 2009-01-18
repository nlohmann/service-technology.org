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
  };




  /***********************************************
   *               Atomic Formulas               *
   ***********************************************/
  class AtomicFormula : public Formula
  {
  public:
    AtomicFormula(Place *p, unsigned int k);
    virtual ~AtomicFormula() {}

    virtual bool evaluate(Marking &m);

    Place* getPlace();
    unsigned int getNumber();

    virtual const string toString() const = 0;

  protected:
    Place *place;
    unsigned int number;
  };

  class FormulaEqual : public AtomicFormula
  {
  public:
    FormulaEqual(Place *p, unsigned int k);
    virtual ~FormulaEqual() {}

    bool evaluate(Marking &m);

    const string toString() const;
  };

  class FormulaNotEqual : public AtomicFormula
  {
  public:
    FormulaNotEqual(Place *p, unsigned int k);
    virtual ~FormulaNotEqual() {}

    bool evaluate(Marking &m);

    const string toString() const;
  };

  class FormulaGreater : public AtomicFormula
  {
  public:
    FormulaGreater(Place *p, unsigned int k);
    virtual ~FormulaGreater() {}

    bool evaluate(Marking &m);

    const string toString() const;
  };

  class FormulaGreaterEqual : public AtomicFormula
  {
  public:
    FormulaGreaterEqual(Place *p, unsigned int k);
    virtual ~FormulaGreaterEqual() {}

    bool evaluate(Marking &m);

    const string toString() const;
  };

  class FormulaLess : public AtomicFormula
  {
  public:
    FormulaLess(Place *p, unsigned int k);
    virtual ~FormulaLess() {}

    bool evaluate(Marking &m);

    const string toString() const;
  };

  class FormulaLessEqual : public AtomicFormula
  {
  public:
    FormulaLessEqual(Place *p, unsigned int k);
    virtual ~FormulaLessEqual() {}

    bool evaluate(Marking &m);

    const string toString() const;
  };




  /************************************************
   *          Unary Boolean Formulas              *
   ************************************************/
  class UnaryBooleanFormula : public Formula
  {
  public:
    UnaryBooleanFormula(Formula *f);
    virtual ~UnaryBooleanFormula() {}

    virtual bool evaluate(Marking &m) = 0;

    virtual const string toString() const = 0;

  protected:
    Formula *sub;
  };

  class FormulaNot : public UnaryBooleanFormula
  {
  public:
    FormulaNot(Formula *f);
    virtual ~FormulaNot() {}

    bool evaluate(Marking &m);

    const string toString() const;
  };




  /************************************************
   *           n ary Boolean Formulas             *
   ************************************************/
  class NaryBooleanFormula : public Formula
  {
  public:
    NaryBooleanFormula(Formula *l, Formula *r);
    NaryBooleanFormula(list<Formula *> &flst);
    virtual ~NaryBooleanFormula() {}

    virtual bool evaluate(Marking &m) = 0;

    virtual const string toString() const = 0;

  protected:
    list<Formula *> subs;
  };

  class FormulaAnd : public NaryBooleanFormula
  {
  public:
    FormulaAnd(Formula *l, Formula *r);
    FormulaAnd(list<Formula *> &flst);
    virtual ~FormulaAnd() {}

    bool evaluate(Marking &m);

    const string toString() const;
  };

  class FormulaOr : public NaryBooleanFormula
  {
  public:
    FormulaOr(Formula *l, Formula *r);
    FormulaOr(list<Formula *> &flst);
    virtual ~FormulaOr() {}

    bool evaluate(Marking &m);

    const string toString() const;
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
  };

  } /* namespace formula */

} /* namespace pnapi */

#endif /* FORMULA_H */
