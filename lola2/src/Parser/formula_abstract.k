%{ KC_TYPES_HEADER
#include <cstring>
#include <map>
#include <string>
#include <Core/Dimensions.h>
#include <Formula/StatePredicate.h>
#include <Formula/BuechiAutomata.h>
#include <Formula/CTL/CTLFormula.h>

// required, because the abstract grammar does not allow pointer types
typedef StatePredicate* StatePredicate_p;
typedef BuechiAutomata* BuechiAutomata_p;
typedef CTLFormula* CTLFormula_p;

// don't use hash sets, since they are deprecated and unordered sets are not yet supported by Kimwitu++
#define DONT_USE_HASHSET

%}

tFormula:
  StatePredicateFormula(tStatePredicate)
{
    formula_t type;
    StatePredicate_p formula = NULL;
    CTLFormula_p ctl_formula = NULL;
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
    bool validCTLPathFormula = false;
    bool validCTLStateFormula = false;
    bool validLTLFormula = false;
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

tBuechiAutomata:
  BuechiAutomaton(tBuechiRules tAcceptingSet)
| BuechiNull()
{
	BuechiAutomata_p automata;
}
;

tBuechiRules:
  EmptyBuechiRules()
| BuechiRule(integer tTransitionRules)
| ExpandedBuechiRule(integer tFormula integer)
| BuechiRules(tBuechiRules tBuechiRules)
;

tTransitionRules:
  EmptyTransitionRules()
| TransitionRule(tFormula integer)
| TransitionRules(tTransitionRules tTransitionRules)
;

tAcceptingSet:
  EmptyAcceptingSet()
| AcceptingState(integer)
| AcceptingSet(tAcceptingSet tAcceptingSet)
;
