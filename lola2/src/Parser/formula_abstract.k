%{ KC_TYPES_HEADER
#include <cstring>
#include <map>
#include <string>
%}

tFormula:
  StatePredicateFormula(tStatePredicate)
  {
      int atomicPropositions = 0;
  }
;

tStatePredicate:
  AtomicProposition(tAtomicProposition)
| Negation(tStatePredicate)
| Conjunction(tStatePredicate tStatePredicate)
| Disjunction(tStatePredicate tStatePredicate)
| ExclusiveDisjunction(tStatePredicate tStatePredicate)
| Implication(tStatePredicate tStatePredicate)
| Equivalence(tStatePredicate tStatePredicate)
| ConjunctionList(tConjunction_list)
| DisjunctionList(tDisjunction_list)
;

tConjunction_list:
  list tStatePredicate
;

tDisjunction_list:
  list tStatePredicate
;

tAtomicProposition:
  EqualsAtomicProposition(tTerm tTerm)
| NotEqualsAtomicProposition(tTerm tTerm)
| GreaterAtomicProposition(tTerm tTerm)
| GreaterEqualAtomicProposition(tTerm tTerm)
| LessAtomicProposition(tTerm tTerm)
| LessEqualAtomicProposition(tTerm tTerm)
| True()
| False()
| aDeadlock()
| Fireable(integer)
;

tTerm:
  Node(integer)
| Number(integer)
| Sum(tTerm tTerm)
| Difference(tTerm tTerm)
| Product(integer tTerm)
| ProductList(tProduct_list)
;

tProduct_list:
  list tTerm
;
