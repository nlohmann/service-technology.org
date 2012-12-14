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

// (x XOR y)   ->   ((x AND !y) OR (!x AND y))
ExclusiveDisjunction(x, y) -> <arrows: Disjunction(Conjunction(x, Negation(y)), Conjunction(Negation(x), y)) >;


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


// Temporal Tautologies

// NOT A   <->   E NOT
Negation(AllPath(x)) -> <neg: ExPath(Negation(x))>;

// NOT E   <->   A NOT
Negation(ExPath(x)) -> <neg: AllPath(Negation(x))>;

// NOT F   <->   G NOT
Negation(Eventually(x)) -> <neg: Always(Negation(x))>;

// NOT G   <->   F NOT
Negation(Always(x)) -> <neg: Eventually(Negation(x))>;

// NOT X   <->   X NOT
Negation(NextState(x)) -> <neg: NextState(Negation(x))>;

// FF a        <->  Fa
Eventually(a=Eventually(x)) -> <neg: a>;

// F (a OR b)  <->  (Fa OR Fb)
Disjunction(Eventually(x), Eventually(y)) -> <neg: Eventually(Disjunction(x,y))>;

// G (a AND b) <->  (Ga AND Gb)
Conjunction(Always(x), Always(y)) -> <neg: Always(Conjunction(x,y))>;

// X (a AND b) <->  (Xa AND Xb)
Conjunction(NextState(x), NextState(y)) -> <neg: NextState(Conjunction(x,y))>;

// X (a OR b) <->  (Xa OR Xb)
Disjunction(NextState(x), NextState(y)) -> <neg: NextState(Disjunction(x,y))>;

// FGF x       <->  GF x
Eventually(a=Always(Eventually(x))) -> <neg: a>;

// GFG x       <->  FG x
Always(a=Eventually(Always(x))) -> <neg: a>;

// AGAG x   <-> AG x
AllPath(Always(a=AllPath(Always(x)))) -> <neg: a>;

// AFAF x   <-> AF x
AllPath(Eventually(a=AllPath(Eventually(x)))) -> <neg: a>;

// EFEF x   <-> EF x
ExPath(Eventually(a=ExPath(Eventually(x)))) -> <neg: a>;

// AGEFAGEF x <-> AGEF x
AllPath(AllPath(Always(ExPath(Eventually(a=AllPath(Always(ExPath(Eventually(x))))))))) -> <neg: a>;

// EFAGEFAG x <-> EFAG x
ExPath(Eventually(AllPath(Always(a=ExPath(Eventually(AllPath(Always(x)))))))) -> <neg: a>;

// TRUE U x <-> F X
Until(AtomicProposition(True()), x) -> <neg: Eventually(x) >;

// FALSE R x <-> G X
Release(AtomicProposition(False()), x) -> <neg: Always(x) >;

// NOT (x U y) -> (NOT x R NOT y) 
Negation(Until(x,y)) -> <neg: Release(Negation(x), Negation(y)) >;

// NOT (x R y) -> (NOT x U NOT y) 
Negation(Release(x,y)) -> <neg: Until(Negation(x), Negation(y)) >;

// X (x U y) <-> (Xx U Xy)
Until(NextState(x),NextState(y)) -> <neg: NextState(Until(x,y)) >;

// (x U (x U y)) -> (x U y)
Until(x, a=Until(x,y)) -> <neg: a >;


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
LessAtomicProposition(x,y) -> <leq: LessEqualAtomicProposition(Sum(x, Number(mkinteger(1))), y) >;


/****************************************************
 * LEFT SIDE: SUM OF PRODUCTS, RIGHT SIDE: 1 NUMBER *
 ****************************************************/

%rview sides;


// take right side to left
LessEqualAtomicProposition(x, s=Sum(y,z))        -> <sides: LessEqualAtomicProposition(Difference(x, s), Number(mkinteger(0))) >;
LessEqualAtomicProposition(x, p=Product(y,z))    -> <sides: LessEqualAtomicProposition(Difference(x, p), Number(mkinteger(0))) >;
LessEqualAtomicProposition(x, n=Node(y))         -> <sides: LessEqualAtomicProposition(Difference(x, n), Number(mkinteger(0))) >;

// take numbers back to right
LessEqualAtomicProposition(Sum(Number(x), y), Number(z) )    -> <sides: LessEqualAtomicProposition(y, Number(minus(z,x))) >;

// any node should be multiplied by 1 to have products everywhere
LessEqualAtomicProposition(Node(x), Number(z)) -> <sides: LessEqualAtomicProposition(Product(mkinteger(1), Node(x)), Number(z)) >;
Sum(n=Node(x),y) -> <sides: Sum(Product(mkinteger(1),n),y)>;
Sum(x,n=Node(y)) -> <sides: Sum(x,Product(mkinteger(1),n))>;

// replace differences by sums
Difference(x,y) -> <sides: Sum(x, Product(mkinteger(-1), y)) >;

// move numbers to left
Sum(x, n=Number(y)) -> <sides: Sum(n, x) >;
Sum(x, Sum(n=Number(y), z)) -> <sides: Sum(n, Sum(x,z)) >;

// calculate values if possible
Sum(Number(x), Number(y)) -> <sides: Number(plus(x,y)) >;
Sum(Number(x), Sum(Number(y), z)) -> <sides: Sum(Number(plus(x,y)), z) >;
Sum(Number(x), y) provided (x->value == 0) -> <sides: y >;
Product(x, y) provided (x->value == 0) -> <sides: Number(0) >;
Product(x, y=Sum(u,v)) provided (x->value == 1) -> <sides: y >;
Product(x, Number(y)) -> <sides: Number(mult(x,y)) >;

// move parentheses
Sum(Sum(x,y), z) -> <sides: Sum(x, Sum(y, z)) >;
//Sum(Sum(u,v), w) -> <sides: Sum(u, Sum(v, w)) >;

// calculate nested multiplications
Product(x, Product(y, z)) -> <sides: Product(mult(x,y), z) >;
Product(x, Sum(Number(y), z)) -> <sides: Sum(Number(mult(x,y)), Product(x, z)) >;
Product(x, Sum(y,z)) -> <sides: Sum(Product(x,y), Product(x,z)) >;

// sort atomic propositions to back
Disjunction(x=AtomicProposition(z),y=Disjunction(u,v)) -> <sides: Disjunction(y,x) >;
Conjunction(x=AtomicProposition(z),y=Disjunction(u,v)) -> <sides: Conjunction(y,x) >;
Disjunction(x=AtomicProposition(z),y=Conjunction(u,v)) -> <sides: Disjunction(y,x) >;
Conjunction(x=AtomicProposition(z),y=Conjunction(u,v)) -> <sides: Conjunction(y,x) >;


// NOT TRUE  ->  FALSE
Negation(AtomicProposition(True())) -> <sides: AtomicProposition(False()) >;

// NOT FALSE  ->  TRUE
Negation(AtomicProposition(False())) -> <sides: AtomicProposition(True()) >;

// AND + TRUE/FALSE
Conjunction(a, AtomicProposition(True())) -> <sides: a >;
Conjunction(AtomicProposition(True()), a) -> <sides: a >;
Conjunction(a, f = AtomicProposition(False())) -> <sides: f >;
Conjunction(f = AtomicProposition(False()), a) -> <sides: f >;

// OR + TRUE/FALSE
Disjunction(a, AtomicProposition(False())) -> <sides: a >;
Disjunction(AtomicProposition(False()), a) -> <sides: a >;
Disjunction(a, t = AtomicProposition(True())) -> <sides: t >;
Disjunction(t = AtomicProposition(True()), a) -> <sides: t >;

// if only numbers are left, we can immediately calculate the formula's value
LessEqualAtomicProposition(Number(x), Number(y)) provided (x->value >= y->value) -> <sides: True() >;
LessEqualAtomicProposition(Number(x), Number(y)) provided (x->value < y->value) -> <sides: False() >;


/**********************************************************
 * TRANSFORM IMPOSSIBILITY AND INVARIANTS TO REACHABILITY *
 **********************************************************/

%rview reachability;

// EF f -> f
StatePredicateFormula(ExPath(Eventually(y))) -> <reachability: StatePredicateFormula(y)>;

// AG f -> NOT EF NOT f -> NOT f   (the outmost negation is handled by LoLA)
StatePredicateFormula(AllPath(Always(y))) -> <reachability: StatePredicateFormula(Negation(y))>;

// AG NOT f -> NOT EF f -> f   (the outmost negation is handled by LoLA)
StatePredicateFormula(AllPath(Always(Negation(y)))) -> <reachability: StatePredicateFormula(y)>;



/****************************
 * COPY EVERYTHING TO LISTS *
 ****************************/

%rview lists;

// collect sums of products in a product list
Sum(x,y) -> <lists: ProductList(ConstProduct_list(x, ConstProduct_list(y, NiltProduct_list())))>;


%rview rbuechi;

// expands all transition rules to triples (from,formula,to)
BuechiRule(from,TransitionRules(lrules,rrules)) -> <rbuechi: BuechiRules(BuechiRule(from,lrules),BuechiRule(from,rrules))>;
BuechiRule(from,TransitionRule(formula,to)) -> <rbuechi: ExpandedBuechiRule(from,formula,to)>;
BuechiRule(from,EmptyTransitionRules()) -> <rbuechi: EmptyBuechiRules()>;
