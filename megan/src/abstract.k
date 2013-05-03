%{ KC_TYPES_HEADER
#include <cstring>
%}

property:
  Property(casestring formula)
| ReachabilityProperty(casestring formula)
| Negation(property)
| DeadlockInitial(casestring)
| DeadlockNet(casestring)
;

formula:
  True()
| False()
| Not(formula)
| I(formula)                    /* fliegt raus */
| N(formula)                    /* fliegt raus */
| A(formula)
| E(formula)
| X(formula)
| G(formula)
| F(formula)
| Deadlock()
| Fireable(casestring)
| Live0(casestring)
| Live1(casestring)
| Live2(casestring)
| Live3(casestring)
| Live4(casestring)
| EQ(expression expression)    /* fliegt raus */
| NE(expression expression)    /* fliegt raus */
| GT(expression expression)    /* fliegt raus */
| GE(expression expression)    /* fliegt raus */
| LT(expression expression)    /* fliegt raus */
| LE(expression expression)
| And(formulas)
| Or(formulas)
| Xor(formulas)                /* fliegt raus */
| Imp(formula formula)         /* fliegt raus */
| Iff(formula formula)         /* fliegt raus */
| W(formula formula)
| U(formula formula)
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
