#ifndef FORMULA_H
#define FORMULA_H
#include"net.H"
#include"unfold.H"
#include"symboltab.H"
#include<fstream>

#include"dimensions.H"

typedef enum
{
eq, neq, geq, leq, lt, gt, conj, disj, neg, ax, ex, au, eu, ag, eg, af, ef, qe, qa, trans, stat
} FType;

class State;

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
	formula *  posate(){}
	formula * negate(){}
    virtual Transition ** spp2(State *){}
	transitionformula(TrSymbol *, fmode *);
  void  updateatomic(){}
	formula * copy();
    formula * reduce(int *){}
    void  setstatic(){}
    formula *  merge();
	unsigned int counttype(FType);
    formula *  replacequantifiers();
    bool evaluatetransition(Transition *);
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
	bool initatomic(){}
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
    bool evaluatetransition(Transition * t){}
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
    virtual Transition ** spp2(State *){}
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
	formula * posate(){}
	formula * negate(){}
    formula *  replacequantifiers();
    virtual Transition ** spp2(State *){}
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
    virtual Transition ** spp2(State *){}
	formula * negate(){}
    formula *  replacequantifiers();
    void  setstatic();
    bool evaluatetransition(Transition *){}
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
  formula * negate(){}
    virtual Transition ** spp2(State *){}
  formula * copy();
    void  setstatic();
  untilformula(FType,formula *, formula *, formula *);
  void evaluateatomic(State *);
    unsigned int  counttype(FType);
    bool evaluatetransition(Transition *){}
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
  bool initatomic();
};

class hlatomicformula : public formula
{
public:
  Place * p;
  UExpression *  k;
    formula *  merge(){}
  UExpression * color; // Instanzbeschreibung einer HL-Stelle
    formula * reduce(int *){}
  PlSymbol * ps; // -"-
    void  setstatic(){}
	formula * posate(){}
    virtual Transition ** spp2(State *){}
	formula * negate(){}
  formula * copy();
  void  updateatomic(){}
    bool evaluatetransition(Transition *){}
    virtual unsigned int  collectsubs(FType,formula **,unsigned int){}
  formula *  replacequantifiers();
  hlatomicformula(FType,PlSymbol *, UExpression *);
    unsigned int  counttype(FType){}
  void evaluateatomic(State *s){}
  bool initatomic(){}
};


class quantifiedformula : public formula
{
public:
  formula * sub; 
    formula *  merge(){}
  UVar * var; // quantifizierte Variable; kommt nur beim Parsen vor und wird zu
    void  setstatic(){}
			  // Konjunktion  bzw. Disjunktion entfaltet
  void  updateatomic(){}
    virtual Transition ** spp2(State *){}
  formula * posate(){}
  formula * negate(){}
    formula * reduce(int *){}
  quantifiedformula(FType, UVar *, formula *);
    virtual unsigned int  collectsubs(FType,formula **,unsigned int){}
  formula * copy();
    bool evaluatetransition(Transition *){}
    formula *  replacequantifiers();
    unsigned int  counttype(FType){}
  void evaluateatomic(State * s){}
  bool initatomic(){}
};

class UExpression;

class staticformula : public formula
{
public:
	UExpression * exp;
    formula *  merge();
    void  setstatic(){}
  void  updateatomic(){}
    virtual Transition ** spp2(State *){}
  formula * posate(){}
  formula * negate(){}
    formula * reduce(int *);
  staticformula(UExpression *);
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
  formula * copy();
    bool evaluatetransition(Transition *){}
    formula *  replacequantifiers();
    unsigned int  counttype(FType);
  void evaluateatomic(State * s){}
  bool initatomic(){}
};

extern formula * F;
		
extern unsigned int TemporalIndex;
extern bool * DeadStatePathRestriction;
void update_formula(Transition *);
extern unsigned int xoperators;

#endif

