%{ KC_REWRITE_HEADER
#include <config.h>
#include <pnapi/pnapi.h>
#include "Task.h"
#include "verbose.h"
%}

%{ KC_REWRITE /* code redirection */

// functions to immediately process constants
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
    return mkinteger( a -> value / b -> value );
}

// helpers for lists
formulas _(formula a, formula b) {
    //return Consformulas(a, Consformulas(b, Nilformulas()));
    return new impl_formulas(a, new impl_formulas(b, new impl_formulas()));
}
formulas _(formula a, formula b, formulas c) {
    //return Consformulas(a, Consformulas(b, c));
    return new impl_formulas(a, new impl_formulas(b,c));
}

// helper function for the map-function in negateList
formula negateElement(formula f) {
    return new impl_formula_Not(f);
}
formulas negateList(formulas l) {
    return l->map(*negateElement);
}

// unfold Fireable predicate
formula unfoldFireability(casestring transition_name) {
    pnapi::PetriNet *net = Task::getNet();

    // get Petri net transition
    pnapi::Transition *t = net->findTransition(transition_name->name);
    if (!t) {
        abort(12, "could not find transition %s", transition_name->name);
    }

    // a list to collect expressions
    formulas l = new impl_formulas();

    // traverse preset of t
    PNAPI_FOREACH(a, t->getPresetArcs()) {
        const casestring place_name = mkcasestring((*a)->getSourceNode().getName().c_str());
        const integer arc_weight = mkinteger((*a)->getWeight());
        const formula f = new impl_formula_GE(new impl_expression_Marking(place_name), new impl_expression_Integer(arc_weight));
        l->append(f);
    }

    if (l->is_nil()) {
        formula res = new impl_formula_True();
        return res;
    } else {
        formula res = new impl_formula_And(l);
        return res;
    }
}

%}



/****************************************************************************
 * REPLACE COMPLEX BOOLEAN CONNECTORS BY SIMPLER ONES *
 ****************************************************************************/

%rview arrows;

/////////////////////////////////////////////////////////
// postcondition: Xor, Imp, Iff will disappear
/////////////////////////////////////////////////////////

// remove XOR once we are done
Xor(Consformulas(a, Nilformulas())) -> <arrows: a >;
// (a XOR b) == ((a AND !b) OR (!a AND b))
Xor(Consformulas(a, Consformulas(b, c))) -> <arrows:
Xor(Consformulas(Or(Consformulas( And(_(a, Not(b))),
                    Consformulas(And(_(Not(a),b)), Nilformulas()) ) ), c) ) >;

// (a => b) == (!a OR b)
Imp(a,b) -> <arrows: Or( _(Not(a), b) ) >;

// (a <=> b) == ((a => b) AND (b => a)
Iff(a,b) -> <arrows: And( _(Imp(a,b), Imp(b,a)) ) >;




/****************************************************************************
 * FORMULA SIMPLIFICATION
 ****************************************************************************/

%rview simplify;

/////////////////////////////////////////////////////////
// postcondition: I and N will will disappear
// postcondition: True and False will disappear
// postcondition: EQ, NE, GT, GE, LT will disappear
// postcondition: Not only occurs in front of Fireable
/////////////////////////////////////////////////////////


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

// if only numbers are left, we can immediately calculate the formula's value
EQ(Integer(x), Integer(y)) provided (x->value == y->value) -> <simplify: True() >;
EQ(Integer(x), Integer(y)) provided (x->value != y->value) -> <simplify: False() >;
NE(Integer(x), Integer(y)) provided (x->value != y->value) -> <simplify: True() >;
NE(Integer(x), Integer(y)) provided (x->value == y->value) -> <simplify: False() >;
GT(Integer(x), Integer(y)) provided (x->value  > y->value) -> <simplify: True() >;
GT(Integer(x), Integer(y)) provided (x->value <= y->value) -> <simplify: False() >;
GE(Integer(x), Integer(y)) provided (x->value >= y->value) -> <simplify: True() >;
GE(Integer(x), Integer(y)) provided (x->value  < y->value) -> <simplify: False() >;
LT(Integer(x), Integer(y)) provided (x->value  < y->value) -> <simplify: True() >;
LT(Integer(x), Integer(y)) provided (x->value >= y->value) -> <simplify: False() >;
LE(Integer(x), Integer(y)) provided (x->value <= y->value) -> <simplify: True() >;
LE(Integer(x), Integer(y)) provided (x->value  > y->value) -> <simplify: False() >;


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
Mult(Integer(a), Mult(Integer(b), c)) -> <simplify: Mult(Integer(mult(a,b)), c) >;
// x*(y+z) -> (x*y)+(x*z)
Mult(x,Add(y,z)) -> <simplify: Add(Mult(x,y),Mult(x,z)) >;

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


/********************
 * MOVE PARENTHESES *
 ********************/

// (x+y)+z  ==  x+(y+z)
Add(Add(x,y),z) -> <simplify: Add(x,Add(y,z)) >;


/**************************
 * MOVE INTEGERS TO FRONT *
 **************************/

// x+n == n+x
Add(x,n=Integer(*)) -> <simplify: Add(n,x) >;
// n1+(n2+x) == (n1+n2)+x
Add(n1=Integer(*),Add(n2=Integer(*),x)) -> <simplify: Add(Add(n1,n2),x) >;
// x*n == n*x
Mult(x,n=Integer(*)) -> <simplify: Mult(n,x) >;
// x+(n+y) == n+(x+y)
Add(x,Add(n=Integer(*),y)) -> <simplify: Add(n,Add(x,y)) >;
// n1*(n2+x) == (n1*n2)+(n1*x)
Mult(n1=Integer(*),Add(n2=Integer(*),x)) -> <simplify: Add(Mult(n1,n2),Mult(n1,x)) >;


/*************************
 * MOVE NEGATIONS INSIDE *
 ************************/

// !!x == x
Not(Not(x)) -> <simplify: x >;

// !(x AND y) == (!a OR !b)
Not(And(x)) -> <simplify: Or(negateList(x)) >;

// !(x OR y) == (!a AND !b)
Not(Or(x)) -> <simplify: And(negateList(x)) >;

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


/***************************
 * EVERYTHING SHOULD BE <= *
 ***************************/

/////////////////////////////////////////////////////////
// postcondition: EQ, NE, GT, GE, LT will disappear
/////////////////////////////////////////////////////////

// x == y  -->  x <= y AND y <= x
EQ(x,y) -> <simplify: And(Consformulas(LE(x,y), Consformulas(LE(y,x), Nilformulas()))) >;
// x != y  -->  x < y OR y < x
NE(x,y) -> <simplify: Or(Consformulas(LT(x,y), Consformulas(LT(y,x), Nilformulas()))) >;
// x > y --> x >= y+1
GT(x,y) -> <simplify: GE(x, Add(y, Integer(mkinteger(1)))) >;
// x < y --> x <= y-1
LT(x,y) -> <simplify: LE(x, Minus(y, Integer(mkinteger(1)))) >;
// x >= y  -->  y <= x
GE(x,y) -> <simplify: LE(y,x) >;


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
A(W(x,y)) -> <simplify: Not(E(U(Not(y),And(_(Not(x),Not(y))))))>;

// from http://i12www.ira.uka.de/~pschmitt/FM210/Folien/CTL.pdf

// A(true U phi) == AF phi
A(U(True(), phi)) -> <simplify: A(F(phi)) >;
// E(true U phi) == EF phi
E(U(True(), phi)) -> <simplify: E(F(phi)) >;
// !A(true U !phi) == EG phi
Not(A(U(True(), Not(phi)))) -> <simplify: E(G(phi)) >;
// !E(true U !phi) == AG phi
Not(E(U(True(), Not(phi)))) -> <simplify: A(G(phi)) >;

/******************
 * PROBLEM FINDER *
 ******************/

%rview problem;

/////////////////////////////////////////////////////////
// postcondition: I and N will will disappear
// postcondition: True and False will disappear
// postcondition: Deadlock will disappear
/////////////////////////////////////////////////////////

// special care for deadlocks
Property(name, I(Deadlock())) -> <problem: DeadlockInitial(name) >;
Property(name, N(Deadlock())) -> <problem: Negation(DeadlockNet(name)) >;

// replace top operators by known problem
Property(name, E(F(x))) provided (!x->isTemporal) -> <problem: ReachabilityProperty(name, x) >;
Property(name, A(G(x))) provided (!x->isTemporal) -> <problem: Negation(ReachabilityProperty(name, x)) >;

// CTL / LTL
P=Property(name, x) provided (P->isCTL) -> <problem: CTLProperty(name, x) >;
P=Property(name, x) provided (P->isLTL) -> <problem: LTLProperty(name, x) >;

// replace top constant by dedicated Task
Property(name, True()) -> <problem: TrueProperty(name) >;
Property(name, False()) -> <problem: Negation(TrueProperty(name)) >;


// invariant == AG
Property(name, I(x)) -> <simplify: Property(name, A(G(x))) >;
// impossibility == !EF
Property(name, N(x)) -> <simplify: Property(name, Not(E(F(x)))) >;

// remove double negations
Negation(Negation(x)) -> <simplify: x>;


/*******************************
 * TAKE RIGHT SIDE TO THE LEFT *
 *******************************/

%rview sides;

// x <= y+z  ->  x-(y+z) <= 0
LE(x, y=Add(*,*)) -> <sides: LE(Minus(x,y), Integer(mkinteger(0))) >;
// x <= y*z  ->  x-(y*z) <= 0
LE(x, y=Mult(*,*)) -> <sides: LE(Minus(x,y), Integer(mkinteger(0))) >;
// x <= y-z  ->  x-(y-z) <= 0
LE(x, y=Minus(*,*)) -> <sides: LE(Minus(x,y), Integer(mkinteger(0))) >;
// x <= y/z  ->  x-(y/z) <= 0
LE(x, y=Div(*,*)) -> <sides: LE(Minus(x,y), Integer(mkinteger(0))) >;
// x <= p  ->  x-p <= 0
LE(x, y=Marking(*)) -> <sides: LE(Minus(x,y), Integer(mkinteger(0))) >;
// x <= |p|  ->  x-|p| <= 0
LE(x, y=Bound(*)) -> <sides: LE(Minus(x,y), Integer(mkinteger(0))) >;
// x <= |p|  ->  x-|p| <= 0
LE(x, y=Card(*)) -> <sides: LE(Minus(x,y), Integer(mkinteger(0))) >;

// x - y  ->  x + (-1)*y
Minus(x,y) -> <sides: Add(x, Mult(Integer(mkinteger(-1)), y)) >;

// n1 + x <= n2  ->  x <= (n2 - n1)
LE(Add(Integer(n1),x), Integer(n2)) -> <simplify: LE(x, Integer(minus(n2,n1)))>;




//%rview lola_prep;

// x + ((-y) * z)   ->  x - (y * z)
//Add(x, Mult(Integer(y), z)) provided (y->value < 0) -> <simplify: Minus(x, Mult(Integer(minus(mkinteger(0),y)),z)) >;




/****************************************************************************
 * UNFOLD FIREABLE PREDICATES *
 ***************************************************************************/

%rview sara_unfold;

/////////////////////////////////////////////////////////
// postcondition: Fireable will disappear
/////////////////////////////////////////////////////////

Fireable(t) -> <sara_unfold: unfoldFireability(t) >;

