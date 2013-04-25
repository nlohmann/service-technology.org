%{ KC_REWRITE /* code redirection */
integer add( integer a, integer b ) {
    return mkinteger( a -> value + b -> value );
}

integer minus( integer a, integer b ) {
    return mkinteger( a -> value - b -> value );
}

integer mult( integer a, integer b ) {
    return mkinteger( a -> value * b -> value );
}

integer div( integer a, integer b ) {
    return mkinteger( a -> value * b -> value );
}

impl_formulas* _(formula a, formula b) {
    return Consformulas(a, Consformulas(b, Nilformulas()));
}

impl_formulas* _(formula a, formula b, impl_formulas* c) {
    return Consformulas(a, Consformulas(b, c));
}

%}


/******************************************************
 * REPLACE COMPLEX BOOLEAN CONNECTORS BY SIMPLER ONES *
 ******************************************************/

%rview arrows;

// remove XOR once we are done
Xor(Consformulas(a, Nilformulas())) -> <arrows: a >;
// (a XOR b) == ((a AND !b) OR (!a AND b))
Xor(Consformulas(a, Consformulas(b, c))) -> <arrows:
Xor(Consformulas(Or(Consformulas( And(_(a, Not(b))),
                    Consformulas(And(_(Not(a),b)), Nilformulas()) ) ), c) )
>;

// (a => b) == (!a OR b)
Imp(a,b) -> <arrows: Or( _(Not(a), b) ) >;

// (a <=> b) == ((a => b) AND (b => a)
Iff(a,b) -> <arrows: And( _(Imp(a,b), Imp(b,a)) ) >;


%rview simplify;

/*****************
 * PROCESS LISTS *
 *****************/

// sort formula lists: constants first
Consformulas(x, Consformulas(True(), y)) -> <simplify: Consformulas(True(), Consformulas(x,y)) >;
Consformulas(x, Consformulas(False(), y)) -> <simplify: Consformulas(False(), Consformulas(x,y)) >;

// deal with singletons
And(Consformulas(x, Nilformulas())) -> <simplify: x>;
Or(Consformulas(x, Nilformulas())) -> <simplify: x>;

// flatten nested lists (avoid AND in AND or OR in OR)
And(Consformulas(And(x), y)) -> <simplify: And(concat(x,y)) >;
Or(Consformulas(Or(x), y)) -> <simplify: Or(concat(x,y)) >;


/******************
 * TRUE AND FALSE *
 ******************/

// !TRUE == FALSE
Not(True()) -> <simplify: False() >;
// !FALSE == TRUE
Not(False()) -> <simplify: True() >;

// TRUE AND x == x
And(Consformulas(True(), x)) -> <simplify: And(x) >;
// FALSE AND x == FALSE
And(Consformulas(False(), x)) -> <simplify: False() >;

// TRUE OR x == TRUE
Or(Consformulas(True(), x)) -> <simplify: True() >;
// FALSE OR x == x
Or(Consformulas(False(), x)) -> <simplify: Or(x) >;


/*******************
 * SIMPLIFIED MATH *
 *******************/

// 1*x == x
Mult(Integer(a), x) provided (a->value == 1) -> <simplify: x >;
// x*1 == x
Mult(x, Integer(a)) provided (a->value == 1) -> <simplify: x >;
// 1*0 == 0
Mult(Integer(a), x) provided (a->value == 0) -> <simplify: Integer(mkinteger(0)) >;
// x*0 == 0
Mult(x, Integer(a)) provided (a->value == 0) -> <simplify: Integer(mkinteger(0)) >;
// integer multiplication
Mult(Integer(a), Integer(b)) -> <simplify: Integer(mult(a,b)) >;

// x/1 == x
Div(x, Integer(a)) provided (a->value == 1) -> <simplify: x >;
// 0/x == 0
Div(Integer(a), x) provided (a->value == 0) -> <simplify: Integer(mkinteger(0)) >;
// integer division
Div(Integer(a), Integer(b)) -> <simplify: Integer(div(a,b)) >;

// x+0 == x
Add(Integer(a), x) provided (a->value == 0) -> <simplify: x >;
// 0+x == x
Add(x, Integer(a)) provided (a->value == 0) -> <simplify: x >;
// integer addition
Add(Integer(a), Integer(b)) -> <simplify: Integer(add(a,b)) >;

// x-0 == x
Minus(x, Integer(a)) provided (a->value == 0) -> <simplify: x >;
// x-x == 0
Minus(x,x) -> <simplify: Integer(mkinteger(0)) >;
// integer subtraction
Minus(Integer(a), Integer(b)) -> <simplify: Integer(minus(a,b)) >;


/*************************
 * MOVE NEGATIONS INSIDE *
 ************************/
// !!x == x
Not(Not(x)) -> <simplify: x >;
/*
// !(x AND y) == (!a OR !b)
Not(And(x,y)) -> <simplify: Or(Not(x), Not(y)) >;
// !(x OR y) == (!a AND !b)
Not(Or(x,y)) -> <simplify: And(Not(x), Not(y)) >;
*/

// !(x == y) == (x != y)
Not(EQ(x,y)) -> <simplify: NE(x,y) >;
// !(x != y) == (x == y)
Not(NE(x,y)) -> <simplify: EQ(x,y) >;
// !(x > y) == (x <= y)
Not(GT(x,y)) -> <simplify: LE(x,y) >;
// !(x >= y) == (x < y)
Not(GE(x,y)) -> <simplify: LT(x,y) >;
// !(x < y) == (x >= y)
Not(LT(x,y)) -> <simplify: GE(x,y) >;
// !(x <= y) == (x > y)
Not(LE(x,y)) -> <simplify: GT(x,y) >;

// !(Ax) == E(!x)
Not(A(x)) -> <simplify: E(Not(x)) >;
// !(Ex) == A(!x)
Not(E(x)) -> <simplify: A(Not(x)) >;
// !(Fx) == G(!x)
Not(F(x)) -> <simplify: G(Not(x)) >;
// !(Gx) == F(!x)
Not(G(x)) -> <simplify: F(Not(x)) >;
// !(Xx) == X(!x)
Not(X(x)) -> <simplify: X(Not(x)) >;


/****************
 * SIMPLIFY CTL *
 ****************/

// FFx == Fx
F(a=F(x)) -> <simplify: a >;
// AAx == Ax
A(a=A(x)) -> <simplify: a >;
// AEx == Ex
A(a=E(x)) -> <simplify: a >;
// EAx == Ax
E(a=A(x)) -> <simplify: a >;
// EEx == Ex
E(a=E(x)) -> <simplify: a >;
// FGFx == GFx
F(a=G(F(x))) -> <simplify: a>;
// GFGx == FGx
G(a=F(G(x))) -> <simplify: a>;

// (Fx OR Fy) == F(x OR y)
//Or(F(x),F(y)) -> <simplify: F(Or(x,y)) >;
// (Gx AND Gy) == G(x AND y)
//And(G(x), G(y)) -> <simplify: G(And(x,y)) >;
// (Xx AND Xy) == X(x AND y)
//And(X(x), X(y)) -> <simplify: X(And(x,y)) >;
// (Xx OR Xy) == X(x OR y)
//Or(X(x), X(y)) -> <simplify: X(Or(x,y)) >;

// AGAGx == AGx
A(G(a=A(G(x)))) -> <simplify: a>;
// AFAFx == AFx
A(F(a=A(F(x)))) -> <simplify: a>;
// EFEFx == EFx
E(F(a=E(F(x)))) -> <simplify: a>;
// AGEFAGEFx == AGEFx
A(G(E(F(a=A(G(E(F(x)))))))) -> <simplify: a>;
// EFAGEFAGx == EFAGx
E(F(A(G(a=E(F(A(G(x)))))))) -> <simplify: a>;

// not A(phi W psi) = E ( not psi U not phi & not psi)
A(W(x,y)) -> <simplify: Not(E(U(Not(y),And(Not(x),Not(y)))))>


/******************
 * PROBLEM FINDER *
 ******************/

// invariant == AG
Property(name, I(x)) -> <simplify: Property(name, A(G(x))) >;
// impossibility == !EF
Property(name, N(x)) -> <simplify: Property(name, Not(E(F(x)))) >;

Property(name, E(F(x))) -> <simplify: ReachabilityProperty(name, x) >;
Property(name, A(G(x))) -> <simplify: Negation(ReachabilityProperty(name, x)) >;
