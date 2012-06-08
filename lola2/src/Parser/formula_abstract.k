%{ KC_TYPES_HEADER
#include <cstring>
#include <map>
#include <string>
%}

tFormula:
  StatePredicateFormula(tStatePredicate)
{
    int formulaType;
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
| AllPath(tStatePredicate)
| ExPath(tStatePredicate)
| Always(tStatePredicate)
| Eventually(tStatePredicate)
{
    bool containsTemporal = false;
}
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
