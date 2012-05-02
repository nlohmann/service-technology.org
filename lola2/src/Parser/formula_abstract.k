tFormula:
  StatePredicateFormula(tStatePredicate)
;

tStatePredicate:
  AtomicProposition(tAtomicProposition)
| Conjunction(tStatePredicate tStatePredicate)
| Disjunction(tStatePredicate tStatePredicate)
| Negation(tStatePredicate)
| ConjunctionList(tConjunction_list)
;

tConjunction_list:
  list tStatePredicate
;

tAtomicProposition:
  EqualsAtomicProposition(casestring integer)
| NotEqualsAtomicProposition(casestring integer)
;
