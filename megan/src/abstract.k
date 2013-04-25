property_set:
  list property;

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
| EQ(expression expression)
| NE(expression expression)
| GT(expression expression)
| GE(expression expression)
| LT(expression expression)
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
| Minus(expression expression)
| Div(expression expression)
;
