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


/******************************************************
 * REPLACE COMPLEX BOOLEAN CONNECTORS BY SIMPLER ONES *
 ******************************************************/

%rview arrows;

// (x <=> y)    ->   (x => y) && (y => x)
Equivalence(x, y) -> <arrows: Conjunction(Implication(x,y), Implication(y,x))>;

// (x => y)    ->   (!x || y)
Implication(x, y) -> <arrows: Disjunction(Negation(x), y)>;


/*************************
 * MOVE NEGATIONS INSIDE *
 ************************/

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


/****************************
 * EVERYTHING SHOULD BE <= *
 ***************************/

%rview leq;

// transform everything to <=

// x == y   ->   x <= y && y <= x 
AtomicProposition(EqualsAtomicProposition(x,y)) -> <leq: Conjunction(AtomicProposition(LessEqualAtomicProposition(x,y)), AtomicProposition(LessEqualAtomicProposition(y,x))) >;
// x != y   ->   x+1 <= y || y+1 <= x
AtomicProposition(NotEqualsAtomicProposition(x,y)) -> <leq: Disjunction(AtomicProposition(LessEqualAtomicProposition(Sum(x, Number(mkinteger(1))),y)), AtomicProposition(LessEqualAtomicProposition(Sum(y, Number(mkinteger(1))),x))) >;
// x >= y   ->   y <= x
GreaterEqualAtomicProposition(x,y) -> <leq: LessEqualAtomicProposition(y,x) >;
// x > y    ->   y+1 <= x
GreaterAtomicProposition(x,y) -> <leq: LessEqualAtomicProposition(Sum(y, Number(mkinteger(1))), x) >;
// x < y    ->   x+1 <= y
LessAtomicProposition(x,y) -> <leq: LessAtomicProposition(Sum(x, Number(mkinteger(1))), y) >;


/****************************************************
 * LEFT SIDE: SUM OF PRODUCTS, RIGHT SIDE: 1 NUMBER *
 ****************************************************/

%rview sides;


// take right side to left
LessEqualAtomicProposition(x, Sum(y,z))        -> <sides: LessEqualAtomicProposition(Difference(x, Sum(y,z)), Number(mkinteger(0))) >;
LessEqualAtomicProposition(x, Product(y,z))    -> <sides: LessEqualAtomicProposition(Difference(x, Product(y,z)), Number(mkinteger(0))) >;

// take number back to right
LessEqualAtomicProposition(Sum(Number(x), y), Number(z) )    provided (z->value == 0) -> <sides: LessEqualAtomicProposition(y, Number(mult(mkinteger(-1),x))) >;



// x <= z    ->   1*x <= z
LessEqualAtomicProposition(Node(x), Number(z)) -> <sides: LessEqualAtomicProposition(Product(mkinteger(1), Node(x)), Number(z)) >;

// replace differences by sums
Difference(x,y) -> <sides: Sum(x, Product(mkinteger(-1), y)) >;



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
Product(x, Sum(y,z)) -> <sides: Sum(Product(x,y), Product(x,z)) >;



/****************************
 * COPY EVERYTHING TO LISTS *
 ****************************/

%rview lists;

// collect sums of products in a product list
Sum(x,y) -> <lists: ProductList(ConstProduct_list(x, ConstProduct_list(y, NiltProduct_list())))>;

// collect conjunctions in a conjunction list
Conjunction(x,y) -> <lists: ConjunctionList(ConstConjunction_list(x, ConstConjunction_list(y, NiltConjunction_list())))>;

// collect disjunctions in a disjunction list
Disjunction(x,y) -> <lists: DisjunctionList(ConstDisjunction_list(x, ConstDisjunction_list(y, NiltDisjunction_list())))>;
