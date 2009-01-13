#ifndef PNAPI_FORMULA_H
#define PNAPI_FORMULA_H

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



  /*
   *    All other places empty
   */
  class AllOtherPlacesEmpty : public Formula
  {
  public:
    AllOtherPlacesEmpty(Formula *p);
    virtual ~AllOtherPlacesEmpty();

    virtual bool evaluate(Marking &m);

    set<Place *> concerningPlaces();

  protected:
    set<Place *> others;
    Formula *parent;
  };




  class AllOtherExternalPlacesEmpty : public AllOtherPlacesEmpty
  {
    AllOtherExternalPlacesEmpty(Formula *p);
    virtual ~AllOtherExternalPlacesEmpty();

    bool evaluate(Marking &m);
  };




  class AllOtherInternalPlacesEmpty : public AllOtherPlacesEmpty
  {
    AllOtherInternalPlacesEmpty(Formula *p);
    virtual ~AllOtherInternalPlacesEmpty();

    bool evaluate(Marking &m);
  };




  /***********************************************
   *               Atomic Formulas               *
   ***********************************************/
  class AtomicFormula : public Formula
  {
  public:
    AtomicFormula();
    AtomicFormula(Place *p, unsigned int k);
    virtual ~AtomicFormula();

    virtual bool evaluate(Marking &m);

    void setPlace(Place *p);
    void setNumber(unsigned int k);

    Place* getPlace();
    unsigned int getNumber();

  protected:
    Place *place;
    unsigned int number;
  };

  class FormulaEqual : public AtomicFormula
  {
  public:
    FormulaEqual();
    FormulaEqual(Place *p, unsigned int k);
    virtual ~FormulaEqual();

    bool evaluate(Marking &m);
  };

  class FormulaNotEqual : public AtomicFormula
  {
  public:
    FormulaNotEqual();
    FormulaNotEqual(Place *p, unsigned int k);
    virtual ~FormulaNotEqual();

    bool evaluate(Marking &m);
  };

  class FormulaGreater : public AtomicFormula
  {
  public:
    FormulaGreater();
    FormulaGreater(Place *p, unsigned int k);
    virtual ~FormulaGreater();

    bool evaluate(Marking &m);
  };

  class FormulaGreaterEqual : public AtomicFormula
  {
  public:
    FormulaGreaterEqual();
    FormulaGreaterEqual(Place *p, unsigned int k);
    virtual ~FormulaGreaterEqual();

    bool evaluate(Marking &m);
  };

  class FormulaLess : public AtomicFormula
  {
  public:
    FormulaLess();
    FormulaLess(Place *p, unsigned int k);
    virtual ~FormulaLess();

    bool evaluate(Marking &m);
  };

  class FormulaLessEqual : public AtomicFormula
  {
  public:
    FormulaLessEqual();
    FormulaLessEqual(Place *p, unsigned int k);
    virtual ~FormulaLessEqual();

    bool evaluate(Marking &m);
  };




  /************************************************
   *          Unary Boolean Formulas              *
   ************************************************/
  class UnaryBooleanFormula : public Formula
  {
  public:
    UnaryBooleanFormula();
    UnaryBooleanFormula(Formula *f);
    virtual ~UnaryBooleanFormula();

    virtual bool evaluate(Marking &m) = 0;

  protected:
    Formula *sub;
  };

  class FormulaNot : public UnaryBooleanFormula
  {
  public:
    FormulaNot();
    FormulaNot(Formula *f);

    bool evaluate(Marking &m);
  };




  /************************************************
   *           Binary Boolean Formulas            *
   ************************************************/
  class BinaryBooleanFormula : public Formula
  {
  public:
    BinaryBooleanFormula();
    BinaryBooleanFormula(Formula *l, Formula *r);
    virtual ~BinaryBooleanFormula();

    virtual bool evaluate(Marking &m) = 0;

  protected:
    Formula *left;
    Formula *right;
  };

  class FormulaAnd : public BinaryBooleanFormula
  {
  public:
    FormulaAnd();
    FormulaAnd(Formula *l, Formula *r);
    virtual ~FormulaAnd();

    bool evaluate(Marking &m);
  };

  class FormulaOr : public BinaryBooleanFormula
  {
  public:
    FormulaOr();
    FormulaOr(Formula *l, Formula *r);
    virtual ~FormulaOr();

    bool evaluate(Marking &m);
  };

  } /* namespace formula */

} /* namespace pnapi */

#endif /* FORMULA_H */
