%rview neg;

// NOT(NOT(x))  ->  x
Negation(Negation(x)) -> < neg: x >;

// NOT (x AND y)  ->    (NOT x OR NOT y)
Negation(Conjunction(x, y)) -> < neg: Disjunction(Negation(x), Negation(y)) >;

// NOT (x OR y)   ->    (NOT x AND NOT y)
Negation(Disjunction(x, y)) -> < neg: Conjunction(Negation(x), Negation(y)) >;

// NOT (p == n)   ->    p != n
Negation(AtomicProposition(EqualsAtomicProposition(p,n))) -> < neg: AtomicProposition(NotEqualsAtomicProposition(p, n)) >;

// NOT (p != n)   ->    p == n
Negation(AtomicProposition(NotEqualsAtomicProposition(p,n))) -> < neg: AtomicProposition(EqualsAtomicProposition(p, n)) >;


%rview lists;

Conjunction(x,y) -> < lists: ConjunctionList(ConstConjunction_list(x, ConstConjunction_list(y, NiltConjunction_list()))) >;
