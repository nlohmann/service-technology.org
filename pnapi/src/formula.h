#ifndef PNAPI_FORMULA_H
#define PNAPI_FORMULA_H

#include <list>

using std::list;

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
  };

  class FormulaNotEqual : public AtomicFormula
  {
  public:
    FormulaNotEqual(Place *p, unsigned int k);
    virtual ~FormulaNotEqual() {}

    bool evaluate(Marking &m);
  };

  class FormulaGreater : public AtomicFormula
  {
  public:
    FormulaGreater(Place *p, unsigned int k);
    virtual ~FormulaGreater() {}

    bool evaluate(Marking &m);
  };

  class FormulaGreaterEqual : public AtomicFormula
  {
  public:
    FormulaGreaterEqual(Place *p, unsigned int k);
    virtual ~FormulaGreaterEqual() {}

    bool evaluate(Marking &m);
  };

  class FormulaLess : public AtomicFormula
  {
  public:
    FormulaLess(Place *p, unsigned int k);
    virtual ~FormulaLess() {}

    bool evaluate(Marking &m);
  };

  class FormulaLessEqual : public AtomicFormula
  {
  public:
    FormulaLessEqual(Place *p, unsigned int k);
    virtual ~FormulaLessEqual() {}

    bool evaluate(Marking &m);
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

  protected:
    Formula *sub;
  };

  class FormulaNot : public UnaryBooleanFormula
  {
  public:
    FormulaNot(Formula *f);
    virtual ~FormulaNot() {}

    bool evaluate(Marking &m);
  };




  /************************************************
   *           Binary Boolean Formulas            *
   ************************************************/
  class NaryBooleanFormula : public Formula
  {
  public:
    NaryBooleanFormula(Formula *l, Formula *r);
    NaryBooleanFormula(list<Formula *> &flst);
    virtual ~NaryBooleanFormula() {}

    virtual bool evaluate(Marking &m) = 0;

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
  };

  class FormulaOr : public NaryBooleanFormula
  {
  public:
    FormulaOr(Formula *l, Formula *r);
    FormulaOr(list<Formula *> &flst);
    virtual ~FormulaOr() {}

    bool evaluate(Marking &m);
  };




  /************************************************
   *                Empty Formula                 *
   ************************************************/
  class True : public Formula
  {
  public:
    virtual ~True() {}

    bool evaluate(Marking &m);
  };

  } /* namespace formula */

} /* namespace pnapi */

#endif /* FORMULA_H */
