%rview arrows;

// (x <=> y)    ->   (x => y) && (y => x)
Equivalence(x, y) -> <arrows: Conjunction(Implication(x,y), Implication(y,x))>;

// (x => y)    ->   (!x || y)
Implication(x, y) -> <arrows: Disjunction(Negation(x), y)>;


%rview neg;

// NOT(NOT(x))  ->  x
Negation(Negation(x)) -> <neg: x>;

// NOT (x AND y)  ->    (NOT x OR NOT y)
Negation(Conjunction(x, y)) -> <neg: Disjunction(Negation(x), Negation(y))>;

// NOT (x OR y)   ->    (NOT x AND NOT y)
Negation(Disjunction(x, y)) -> <neg: Conjunction(Negation(x), Negation(y))>;


// NOT (p == n)   ->    p != n
Negation(AtomicProposition(EqualsAtomicProposition(p,n))) -> <neg: AtomicProposition(NotEqualsAtomicProposition(p, n))>;

// NOT (p != n)   ->    p == n
Negation(AtomicProposition(NotEqualsAtomicProposition(p,n))) -> <neg: AtomicProposition(EqualsAtomicProposition(p, n))>;

// NOT (p > n)   ->    p <= n
Negation(AtomicProposition(GreaterAtomicProposition(p,n))) -> <neg: AtomicProposition(LessEqualAtomicProposition(p, n))>;

// NOT (p >= n)   ->    p < n
Negation(AtomicProposition(GreaterEqualAtomicProposition(p,n))) -> <neg: AtomicProposition(LessAtomicProposition(p, n))>;

// NOT (p < n)   ->    p >= n
Negation(AtomicProposition(LessAtomicProposition(p,n))) -> <neg: AtomicProposition(GreaterEqualAtomicProposition(p, n))>;

// NOT (p <= n)   ->    p > n
Negation(AtomicProposition(LessEqualAtomicProposition(p,n))) -> <neg: AtomicProposition(GreaterAtomicProposition(p, n))>;


%rview sides;

%{ KC_REWRITE /* code redirection */
integer plus( integer a, integer b ) {
    return mkinteger( a -> value + b -> value );
}

integer minus( integer a, integer b ) {
    return mkinteger( a -> value - b -> value );
}

integer mult( integer a, integer b ) {
    return mkinteger( a -> value * b -> value );
}
%}

// replace differences by sums
Difference(x,y) -> <sides: Sum(x, Product(mkinteger(-1), y)) >;

// take right side to left
EqualsAtomicProposition(x, Sum(y,z))           -> <sides: EqualsAtomicProposition(Difference(x, Sum(y,z)), Number(mkinteger(0))) >;
EqualsAtomicProposition(x, Product(y,z))       -> <sides: EqualsAtomicProposition(Difference(x, Product(y,z)), Number(mkinteger(0))) >;
NotEqualsAtomicProposition(x, Sum(y,z))        -> <sides: NotEqualsAtomicProposition(Difference(x, Sum(y,z)), Number(mkinteger(0))) >;
NotEqualsAtomicProposition(x, Product(y,z))    -> <sides: NotEqualsAtomicProposition(Difference(x, Product(y,z)), Number(mkinteger(0))) >;
GreaterAtomicProposition(x, Sum(y,z))          -> <sides: GreaterAtomicProposition(Difference(x, Sum(y,z)), Number(mkinteger(0))) >;
GreaterAtomicProposition(x, Product(y,z))      -> <sides: GreaterAtomicProposition(Difference(x, Product(y,z)), Number(mkinteger(0))) >;
GreaterEqualAtomicProposition(x, Sum(y,z))     -> <sides: GreaterEqualAtomicProposition(Difference(x, Sum(y,z)), Number(mkinteger(0))) >;
GreaterEqualAtomicProposition(x, Product(y,z)) -> <sides: GreaterEqualAtomicProposition(Difference(x, Product(y,z)), Number(mkinteger(0))) >;
LessAtomicProposition(x, Sum(y,z))             -> <sides: LessAtomicProposition(Difference(x, Sum(y,z)), Number(mkinteger(0))) >;
LessAtomicProposition(x, Product(y,z))         -> <sides: LessAtomicProposition(Difference(x, Product(y,z)), Number(mkinteger(0))) >;
LessEqualAtomicProposition(x, Sum(y,z))        -> <sides: LessEqualAtomicProposition(Difference(x, Sum(y,z)), Number(mkinteger(0))) >;
LessEqualAtomicProposition(x, Product(y,z))    -> <sides: LessEqualAtomicProposition(Difference(x, Product(y,z)), Number(mkinteger(0))) >;

//GreaterEqualAtomicProposition(x, Number(y)) provided (y->value == 0) -> <sides: LessAtomicProposition(Product(mkinteger(-1), x), Number(y)) >;

// take number back to right
EqualsAtomicProposition(Sum(Number(x), y), Number(z) )       provided (z->value == 0) -> <sides: EqualsAtomicProposition(y, Number(mult(mkinteger(-1),x))) >;
NotEqualsAtomicProposition(Sum(Number(x), y), Number(z) )    provided (z->value == 0) -> <sides: NotEqualsAtomicProposition(y, Number(mult(mkinteger(-1),x))) >;
GreaterAtomicProposition(Sum(Number(x), y), Number(z) )      provided (z->value == 0) -> <sides: GreaterAtomicProposition(y, Number(mult(mkinteger(-1),x))) >;
GreaterEqualAtomicProposition(Sum(Number(x), y), Number(z) ) provided (z->value == 0) -> <sides: GreaterEqualAtomicProposition(y, Number(mult(mkinteger(-1),x))) >;
LessAtomicProposition(Sum(Number(x), y), Number(z) )         provided (z->value == 0) -> <sides: LessAtomicProposition(y, Number(mult(mkinteger(-1),x))) >;
LessEqualAtomicProposition(Sum(Number(x), y), Number(z) )    provided (z->value == 0) -> <sides: LessEqualAtomicProposition(y, Number(mult(mkinteger(-1),x))) >;

// calculate values if possible
Sum(Number(x), Number(y)) -> <sides: Number(plus(x,y)) >;
Sum(Number(x), Sum(Number(y), z)) -> <sides: Sum(Number(plus(x,y)), z) >;

// move numbers to left
Sum(x, Number(y)) -> <sides: Sum(Number(y), x) >;
Sum(x, Sum(Number(y), z)) -> <sides: Sum(Number(y), Sum(x,z)) >;

// move parentheses
Sum(Sum(u,v), Sum(w,y)) -> <sides: Sum(u, Sum(v, Sum(w,y))) >;

// calculate nested multiplications
Product(x, Product(y, z)) -> <sides: Product(mult(x,y), z) >;
Product(x, Sum(Number(y), z)) -> <sides: Sum(Number(mult(x,y)), Product(x, z)) >;



%rview lists;

Conjunction(x,y) -> <lists: ConjunctionList(ConstConjunction_list(x, ConstConjunction_list(y, NiltConjunction_list())))>;

Disjunction(x,y) -> <lists: DisjunctionList(ConstDisjunction_list(x, ConstDisjunction_list(y, NiltDisjunction_list())))>;
