#ifndef FORMULA_H
#define FORMULA_H
#include"owfnPlace.h"
#include"symboltab.h"
#include<fstream>

#include"dimensions.h"

#ifdef LOG_NEW
#include "mynew.h"
#endif

using namespace std;

typedef enum
{
eq, neq, geq, leq, lt, gt, conj, disj, neg
} FType;

class formula
{
public:
  FType type;
  bool value;   // value of formula w.r.t. CurrentMarking
  virtual formula * posate() = 0; // remove negation in formulae without temporal
  virtual formula * negate() = 0; // operators
  virtual bool init(unsigned int *) = 0; // compute value of whole formula 
  virtual formula *  copy() = 0;
  virtual formula *  merge() = 0; // compress chains of AND or OR to single n-ary AND/OR
  virtual void  setstatic() = 0; // set links to parents and from/to mentioned places
  virtual void  update(unsigned int) = 0; // incremental re-calculation of partial formula
  virtual unsigned int  counttype(FType) = 0; // explore size of AND or OR chain
  virtual unsigned int  collectsubs(FType,formula **,unsigned int) = 0; //collect elements of AND or OR chain
  formula * parent;
  unsigned int parentindex; //position of this in parent's array of subformulas
};

class atomicformula : public formula
{
public:
  owfnPlace * p;     // Stelle
  unsigned int k;// Vergleichszahl
  atomicformula(FType,owfnPlace *, unsigned int);
  virtual bool init(unsigned int *);
  void  update(unsigned int);
  formula * posate();
  formula * negate();
    void  setstatic();
  virtual formula * copy();
    formula *  merge();
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
    unsigned int  counttype(FType);
};

class unarybooleanformula : public formula
{
public:
  formula * sub; // Teilformeln zum Parsen, AND/OR-Ketten werden
						  // nachher zu **sub komprimiert
  unarybooleanformula(FType,formula *);
  void  update(unsigned int);
    formula *  merge();
	formula * posate();
	formula * negate();
  bool init(unsigned int *);
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
    formula *  merge();
	formula * posate(){return (formula *) 0;}
	formula * negate(){return (formula *) 0;}
    void  setstatic();
  void  update(unsigned int){}
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
    unsigned int  counttype(FType);
  formula * copy();
  bool init(unsigned int *);
};

class booleanformula : public formula
{
public:
  formula ** sub; // Teilformeln 
    formula *  merge();
	formula * posate();
	formula * negate();
    virtual unsigned int  collectsubs(FType,formula **,unsigned int);
  void  update(unsigned int);
    unsigned int  counttype(FType);
    void  setstatic();
  formula * copy();
   unsigned int cardsub;
  bool init(unsigned int *);
  unsigned int firstvalid; // Teilformeln werden manchmal nach Gueltigkeit sortiert
						   // (links die ungueltigen, rechts die gueltigen)
 booleanformula(){}
};


#endif

