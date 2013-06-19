%{ KC_TYPES_HEADER
#include <cstring>
%}

property:
  Property(casestring formula)
| ReachabilityProperty(casestring formula)
| Negation(property)
| DeadlockInitial(casestring)
| DeadlockNet(casestring)
| TrueProperty(casestring)
| CTLProperty(casestring formula)
| LTLProperty(casestring formula)
    {
        bool isCTL = false;
        bool isLTL = false;
    }
;

formula:
  True()                        /* fliegt raus: simplify/problem */
| False()                       /* fliegt raus: simplify/problem */
| Not(formula)
| I(formula)                    /* fliegt raus: simplify/problem */
| N(formula)                    /* fliegt raus: simplify/problem */
| A(formula)
| E(formula)
| X(formula)
| G(formula)
| F(formula)
| Deadlock()                    /* fliegt raus: problem */
| Fireable(casestring)
| Live0(casestring)
| Live1(casestring)
| Live2(casestring)
| Live3(casestring)
| Live4(casestring)
| EQ(expression expression)    /* fliegt raus: simplify */
| NE(expression expression)    /* fliegt raus: simplify */
| GT(expression expression)    /* fliegt raus: simplify */
| GE(expression expression)    /* fliegt raus: simplify */
| LT(expression expression)    /* fliegt raus: simplify */
| LE(expression expression)
| And(formulas)
| Or(formulas)
| Xor(formulas)                /* fliegt raus: arrows */
| Imp(formula formula)         /* fliegt raus: arrows */
| Iff(formula formula)         /* fliegt raus: arrows */
| W(formula formula)
| U(formula formula)
      {
          bool isTemporal = false;
      }
;

formulas:
  list formula
;

expression:
  Integer(integer)
| Bound(casestring)
| Card(expression)
| Marking(casestring)
| Add(expression expression)
| Mult(expression expression)
| Minus(expression expression)  /* fliegt raus */
| Div(expression expression)
;
