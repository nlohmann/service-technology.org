#ifndef PNAPI_FORMULA_H
#define PNAPI_FORMULA_H

#include <list>
#include <map>
#include <string>

using std::list;
using std::map;
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
    // destructor
    virtual ~Formula() {}

    // evaluating the formula
    virtual bool evaluate(Marking &m) const = 0;

    // output method for formulas
    virtual const string toString() const = 0;

    virtual Formula * flatCopy() const = 0;

    //virtual Formula * deepCopy(const map<Place *, Place *> &newP) const = 0;

  };




  /***********************************************
   *               Atomic Formulas               *
   ***********************************************/
  class AtomicFormula : public Formula
  {
  public:
    AtomicFormula(Place &p, unsigned int k);

    virtual ~AtomicFormula() {}

    virtual bool evaluate(Marking &m) const = 0;

    Place & getPlace();

    unsigned int getNumber();

    virtual const string toString() const = 0;

    virtual AtomicFormula * flatCopy() const = 0;

    //virtual AtomicFormula * deepCopy(const map<Place *, Place *> &newP) const = 0;

  protected:
    /// copy constructor
    AtomicFormula(const AtomicFormula &f);

    Place &place_;

    unsigned int number_;
  };

  class FormulaEqual : public AtomicFormula
  {
  public:
    FormulaEqual(Place &p, unsigned int k);

    /// copy constructor
    //FormulaEqual(const FormulaEqual &f);

    virtual ~FormulaEqual() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    FormulaEqual * flatCopy() const;

    //FormulaEqual * deepCopy(const map<Place *, Place *> &newP) const;
  };

  class FormulaNotEqual : public AtomicFormula
  {
  public:
    FormulaNotEqual(Place &p, unsigned int k);

    /// copy constructor
    //FormulaNotEqual(const FormulaNotEqual &f);

    virtual ~FormulaNotEqual() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    FormulaNotEqual * flatCopy() const;
    //FormulaNotEqual * deepCopy(const map<Place *, Place *> &newP) const;
  };

  class FormulaGreater : public AtomicFormula
  {
  public:
    FormulaGreater(Place &p, unsigned int k);

    /// copy constructor
    //FormulaGreater(const FormulaGreater &f);

    virtual ~FormulaGreater() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    FormulaGreater * flatCopy() const;
    //FormulaGreater * deepCopy(const map<Place *, Place *> &newP) const;
  };

  class FormulaGreaterEqual : public AtomicFormula
  {
  public:
    FormulaGreaterEqual(Place &p, unsigned int k);

    /// copy constructor
    //FormulaGreaterEqual(const FormulaGreaterEqual &f);

    virtual ~FormulaGreaterEqual() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    FormulaGreaterEqual * flatCopy() const;
    //FormulaGreaterEqual * deepCopy(const map<Place *, Place *> &newP) const;
  };

  class FormulaLess : public AtomicFormula
  {
  public:
    FormulaLess(Place &p, unsigned int k);

    ///copy constructor
    //FormulaLess(const FormulaLess &f);

    virtual ~FormulaLess() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    FormulaLess * flatCopy() const;
    //FormulaLess * deepCopy(const map<Place *, Place *> &newP) const;
  };

  class FormulaLessEqual : public AtomicFormula
  {
  public:
    FormulaLessEqual(Place &p, unsigned int k);

    /// copy constructor
    //FormulaLessEqual(const FormulaLessEqual &f);

    virtual ~FormulaLessEqual() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    FormulaLessEqual * flatCopy() const;
    //FormulaLessEqual * deepCopy(const map<Place *, Place *> &newP) const;
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

    virtual bool evaluate(Marking &m) const = 0;

    virtual const string toString() const = 0;

    virtual UnaryBooleanFormula * flatCopy() const = 0;
    //virtual UnaryBooleanFormula * deepCopy(const map<Place *, Place *> &newP) const = 0;

  protected:
    Formula *sub_;
  };

  class FormulaNot : public UnaryBooleanFormula
  {
  public:
    FormulaNot(Formula *f);

    /// copy constructor
    //FormulaNot(const FormulaNot &f);

    virtual ~FormulaNot() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    FormulaNot * flatCopy() const;

    //FormulaNot * deepCopy(const map<Place *, Place *> &newP) const;
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

    virtual bool evaluate(Marking &m) const = 0;

    virtual const string toString() const = 0;

    void addSubFormula(Formula *s);

    virtual NaryBooleanFormula * flatCopy() const = 0;

    //virtual NaryBooleanFormula * deepCopy(const map<Place *, Place *> &newP) const = 0;

  protected:
    list<Formula *> subs_;

    /// copy constructor
    NaryBooleanFormula(const NaryBooleanFormula &f);
  };

  class FormulaAnd : public NaryBooleanFormula
  {
  public:
    FormulaAnd(Formula *l, Formula *r);

    FormulaAnd(list<Formula *> &flst);

    virtual ~FormulaAnd() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    FormulaAnd * flatCopy() const;

    //FormulaAnd * deepCopy(const map<Place *, Place *> &newP) const;
  };

  class FormulaOr : public NaryBooleanFormula
  {
  public:
    FormulaOr(Formula *l, Formula *r);

    FormulaOr(list<Formula *> &flst);

    virtual ~FormulaOr() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    FormulaOr * flatCopy() const;

    //FormulaOr * deepCopy(const map<Place *, Place *> &newP) const;
  };




  /************************************************
   *                Empty Formula                 *
   ************************************************/
  class True : public Formula
  {
  public:
    virtual ~True() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    True * flatCopy() const;

    True * deepCopy(const map<Place *, Place *> &newP) const;
  };

  class False : public Formula
  {
  public:
    virtual ~False() {}

    bool evaluate(Marking &m) const;

    const string toString() const;

    False * flatCopy() const;

    False * deepCopy(const map<Place *, Place *> &newP) const;
  };

  } /* namespace formula */

} /* namespace pnapi */

#endif /* FORMULA_H */
