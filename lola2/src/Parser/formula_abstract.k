%{ KC_TYPES_HEADER
#include <cstring>
#include <map>
#include <string>
#include <Core/Dimensions.h>
#include <Formula/StatePredicate.h>

// required, because the abstract grammar does not allow pointer types
typedef StatePredicate* StatePredicate_p;
%}

tFormula:
  StatePredicateFormula(tStatePredicate)
{
    formula_t type;
    StatePredicate_p formula = NULL;
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
| NextState(tStatePredicate)
| Until(tStatePredicate tStatePredicate)
| Release(tStatePredicate tStatePredicate)
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
| Initial()
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
