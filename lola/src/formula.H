#ifndef FORMULA_H
#define FORMULA_H

#include "net.H"
#include "unfold.H"
#include "symboltab.H"
#include "dimensions.H"

#include <fstream>

typedef enum
{
eq, neq, geq, leq, lt, gt, conj, disj, neg, ax, ex, au, eu, ag, eg, af, ef, qe, qa, trans, stat
} FType;

class State;
class TrSymbol;
class PlSymbol;

class formula
{
public:
  static unsigned int card; // Laenge eines Labelfeldes fuer alle Teilformeln
  static unsigned int tempcard; // Laenge eines Labelfeldes fuer alle temporalen Teilformeln
  unsigned int index; // Index in Labelfeldern fuer alle Teilformeln
  unsigned int tempindex; // Index in Labelfeldern fuer alle temporalen Teilformeln
  FType type;
  bool value;
  bool atomic;
  virtual formula * posate() = 0; // remove negation in formulae without temporal
  virtual formula * negate() = 0; // operators
  virtual void evaluateatomic(State *) = 0;
  virtual bool initatomic() = 0;
  virtual formula *  copy() = 0;
  virtual formula *  merge() = 0;
  virtual Transition ** spp2(State *) = 0;
  virtual void  setstatic() = 0;
  virtual void  updateatomic() = 0;
  virtual formula *  replacequantifiers() = 0;
  virtual unsigned int  counttype(FType) = 0;
  virtual unsigned int  collectsubs(FType,formula **,unsigned int) = 0;
  virtual bool evaluatetransition(Transition *) = 0;
  formula * parent;
  virtual formula * reduce(int *) = 0;
  unsigned int parentindex;
};

class transitionformula : public formula
{
	public:
	Transition * transition;
	TrSymbol * hltransition;
	fmode * firingmode;
	transitionformula(Transition *);
	formula *  posate(){return (formula *) 0;}
	formula * negate(){return (formula *) 0;}
    virtual Transition ** spp2(State *){return (Transition **)0;}
	transitionformula(TrSymbol *, fmode *);
  void  updateatomic(){}
	formula * copy();
    formula * reduce(int *){return (formula *) 0;}
    void  setstatic(){}
    formula *  merge();
	unsigned int counttype(FType);
    formula *  replacequantifiers();
    bool evaluatetransition(Transition *);
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
	bool initatomic(){return (formula *) 0;}
  void evaluateatomic(State * s){}
};

class atomicformula : public formula
{
public:
  Place * p;     // Stelle, falls Atomformel
  unsigned int k;// Vergleichszahl in Atomformeln
  atomicformula(FType,Place *, unsigned int);
  virtual bool initatomic();
  void  updateatomic();
  formula * posate();
    virtual Transition ** spp2(State *);
  formula * negate();
    void  setstatic();
  virtual formula * copy();
    formula *  merge();
    formula * reduce(int *);
    bool evaluatetransition(Transition * t){return 0;}
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
  virtual formula *  replacequantifiers();
  virtual void evaluateatomic(State *);
    unsigned int  counttype(FType);
};

class unarybooleanformula : public formula
{
public:
  formula * sub; // Teilformeln zum Parsen, AND/OR-Ketten werden
						  // nachher zu **sub komprimiert
  unarybooleanformula(FType,formula *);
  void evaluateatomic(State *);
    bool evaluatetransition(Transition *);
  void  updateatomic();
    formula *  merge();
	formula * posate();
    virtual Transition ** spp2(State *){return (Transition **)0;};
	formula * negate();
  bool initatomic();
    formula * reduce(int *);
    formula *  replacequantifiers();
    void  setstatic();
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
    unsigned int  counttype(FType);
  formula * copy();
};

class binarybooleanformula : public formula
{
public:
  formula * left, * right; // Teilformeln zum Parsen, AND/OR-Ketten werden
						  // nachher zu **sub komprimiert
  binarybooleanformula(FType,formula *, formula *);
  void evaluateatomic(State *);
    formula *  merge();
	formula * posate(){return (formula *) 0;}
	formula * negate(){return (formula *) 0;}
    formula *  replacequantifiers();
    virtual Transition ** spp2(State *){return (Transition **)0;};
    void  setstatic();
    formula * reduce(int *);
    bool evaluatetransition(Transition *);
  void  updateatomic(){}
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
    unsigned int  counttype(FType);
  formula * copy();
  bool initatomic();
};

class booleanformula : public formula
{
public:
  formula ** sub; // Teilformeln 
  void evaluateatomic(State *);
    bool evaluatetransition(Transition *);
    formula *  merge();
    formula *  replacequantifiers();
	formula * posate();
	formula * negate();
    virtual Transition ** spp2(State *);
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
    formula * reduce(int *);
  void  updateatomic();
    unsigned int  counttype(FType);
    void  setstatic();
  formula * copy();
   unsigned int cardsub;
  bool initatomic();
  unsigned int firstvalid; // Teilformeln werden manchmal nach Gueltigkeit sortiert
						   // (links die ungueltigen, rechts die gueltigen)
 booleanformula(){}
};

class unarytemporalformula : public formula
{
public:
  formula * element;
  formula * tformula;
    formula *  merge();
    unsigned int  counttype(FType);
  void  updateatomic(){}
    formula * reduce(int *);
	formula * posate();
    virtual Transition ** spp2(State *){return (Transition **)0;}
	formula * negate(){return (formula *) 0;}
    formula *  replacequantifiers();
    void  setstatic();
    bool evaluatetransition(Transition *){return 0;}
  unarytemporalformula(FType, formula *, formula *);
  void evaluateatomic(State *);
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
  formula * copy();
  bool initatomic();
};

class untilformula : public formula
{
public:
  formula * hold, * goal; 
  formula * tformula;
    formula *  merge();
    formula *  replacequantifiers();
    formula * reduce(int *);
  void  updateatomic(){}
  formula * posate();
  formula * negate(){return (formula *) 0;}
    virtual Transition ** spp2(State *){return (Transition **)0;}
  formula * copy();
    void  setstatic();
  untilformula(FType,formula *, formula *, formula *);
  void evaluateatomic(State *);
    unsigned int  counttype(FType);
    bool evaluatetransition(Transition *){return 0;}
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
  bool initatomic();
};

class hlatomicformula : public formula
{
public:
  Place * p;
  UExpression *  k;
    formula *  merge(){return (formula *) 0;}
  UExpression * color; // Instanzbeschreibung einer HL-Stelle
    formula * reduce(int *){return (formula *) 0;}
  PlSymbol * ps; // -"-
    void  setstatic(){}
	formula * posate(){return (formula *) 0;}
    virtual Transition ** spp2(State *){return (Transition **)0;}
	formula * negate(){return (formula *) 0;}
  formula * copy();
  void  updateatomic(){}
    bool evaluatetransition(Transition *){return 0;}
    virtual unsigned int  collectsubs(FType,formula **,unsigned int){return 0;}
  formula *  replacequantifiers();
  hlatomicformula(FType,PlSymbol *, UExpression *);
    unsigned int  counttype(FType){return 0;}
  void evaluateatomic(State *s){}
  bool initatomic(){return 0;}
};


class quantifiedformula : public formula
{
public:
  formula * sub; 
    formula *  merge(){return (formula *) 0;}
  UVar * var; // quantifizierte Variable; kommt nur beim Parsen vor und wird zu
    void  setstatic(){}
			  // Konjunktion  bzw. Disjunktion entfaltet
  void  updateatomic(){}
    virtual Transition ** spp2(State *){return (Transition **)0;}
  formula * posate(){return (formula *) 0;}
  formula * negate(){return (formula *) 0;}
    formula * reduce(int *){return (formula *) 0;}
  quantifiedformula(FType, UVar *, formula *);
    virtual unsigned int  collectsubs(FType,formula **,unsigned int){return 0;}
  formula * copy();
    bool evaluatetransition(Transition *){return 0;}
    formula *  replacequantifiers();
    unsigned int  counttype(FType){return 0;}
  void evaluateatomic(State * s){}
  bool initatomic(){return 0;}
};

class UExpression;

class staticformula : public formula
{
public:
	UExpression * exp;
    formula *  merge();
    void  setstatic(){}
  void  updateatomic(){}
    virtual Transition ** spp2(State *){return (Transition **)0;}
  formula * posate(){return (formula *) 0;}
  formula * negate(){return (formula *) 0;}
    formula * reduce(int *);
  staticformula(UExpression *);
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
  formula * copy();
    bool evaluatetransition(Transition *){return 0;}
    formula *  replacequantifiers();
    unsigned int  counttype(FType);
  void evaluateatomic(State * s){}
  bool initatomic(){return 0;}
};

extern formula * F;
		
extern unsigned int TemporalIndex;
extern bool * DeadStatePathRestriction;
void update_formula(Transition *);
extern unsigned int xoperators;

#endif

