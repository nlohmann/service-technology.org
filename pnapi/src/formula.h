#ifndef PNAPI_FORMULA_H
#define PNAPI_FORMULA_H

namespace pnapi
{

/// forward declaration needed by
class Place;
class Node;

  /*!
   * \brief   Final Condition Formulas
   */
  namespace formula {

  typedef enum { EQ, NEQ, G, GEQ, L, LEQ, NOT, AND, OR, NONE } ftype;

  class Formula
  {
  public:
    Formula();
    Formula(Formula *f);
    virtual ~Formula();

    virtual bool evaluate(Marking &m);

    virtual ftype getType();

  private:
    Formula *sub;
  };




  class AllOtherPlacesEmpty : public Formula
  {
  public:
    AllOtherPlacesEmpty();
    virtual ~AllOtherPlacesEmpty();

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

    virtual ftype getType();

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

    ftype getType();
  };

  class FormulaNotEqual : public AtomicFormula
  {
  public:
    FormulaNotEqual();
    FormulaNotEqual(Place *p, unsigned int k);
    virtual ~FormulaNotEqual();

    bool evaluate(Marking &m);

    ftype getType();
  };

  class FormulaGreater : public AtomicFormula
  {
  public:
    FormulaGreater();
    FormulaGreater(Place *p, unsigned int k);
    virtual ~FormulaGreater();

    bool evaluate(Marking &m);

    ftype getType();
  };

  class FormulaGreaterEqual : public AtomicFormula
  {
  public:
    FormulaGreaterEqual();
    FormulaGreaterEqual(Place *p, unsigned int k);
    virtual ~FormulaGreaterEqual();

    bool evaluate(Marking &m);

    ftype getType();
  };

  class FormulaLess : public AtomicFormula
  {
  public:
    FormulaLess();
    FormulaLess(Place *p, unsigned int k);
    virtual ~FormulaLess();

    bool evaluate(Marking &m);

    ftype getType();
  };

  class FormulaLessEqual : public AtomicFormula
  {
  public:
    FormulaLessEqual();
    FormulaLessEqual(Place *p, unsigned int k);
    virtual ~FormulaLessEqual();

    bool evaluate(Marking &m);

    ftype getType();
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

    void setSubFormula(Formula *s);

    Formula* getSubFormula();

    virtual ftype getType() = 0;

  protected:
    Formula *sub;
  };

  class FormulaNot : public UnaryBooleanFormula
  {
  public:
    FormulaNot();
    FormulaNot(Formula *f);

    bool evaluate(Marking &m);

    ftype getType();
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

    void setLeft(Formula *l);
    void setRight(Formula *r);

    virtual ftype getType() = 0;

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

    ftype getType();
  };

  class FormulaOr : public BinaryBooleanFormula
  {
  public:
    FormulaOr();
    FormulaOr(Formula *l, Formula *r);
    virtual ~FormulaOr();

    bool evaluate(Marking &m);

    ftype getType();
  };

  } /* namespace formula */

} /* namespace pnapi */

#endif /* FORMULA_H */
