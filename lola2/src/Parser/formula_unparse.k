%uview out;

StatePredicateFormula(x) -> [out: x];

AtomicProposition(x) -> [out: x];

Conjunction(x, y) -> [out: "(" x " AND " y ")"];

Disjunction(x, y) -> [out: "(" x " OR " y ")"];

Negation(x) -> [out: "NOT (" x ")"];

EqualsAtomicProposition(p, n) -> [out: p " = " n];
NotEqualsAtomicProposition(p, n) -> [out: p " != " n];

/*
tConjunction_list(h, NiltConjunction_list()) -> [out: h];
tConjunction_list(h, l) -> [out: h " AND " l];
*/
