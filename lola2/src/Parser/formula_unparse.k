//////////////////////////////////////////////////////////////////////////
%uview out;

StatePredicateFormula(x) -> [out: x "\n"];

AtomicProposition(x) -> [out: x];

AllPath(x) -> [out: "A" x ];
ExPath(x)  -> [out: "E" x ];

Eventually(x) -> [out: "F (" x ")" ];
Always(x)     -> [out: "G (" x ")" ];

Negation(x) -> [out: "! (" x ")"];
Conjunction(x, y) -> [out: "(" x " && " y ")"];
Disjunction(x, y) -> [out: "(" x " || " y ")"];
Implication(x, y) -> [out: "(" x " => " y ")"];
Equivalence(x, y) -> [out: "(" x " <=> " y ")"];

EqualsAtomicProposition(x, y) -> [out: x " = " y];
NotEqualsAtomicProposition(x, y) -> [out: x " != " y];
GreaterAtomicProposition(x, y) -> [out: x " > " y];
GreaterEqualAtomicProposition(x, y) -> [out: x " >= " y];
LessAtomicProposition(x, y) -> [out: x " < " y];
LessEqualAtomicProposition(x, y) -> [out: x " <= " y];

Sum(x,y) -> [out: x " + " y ];
Difference(x,y) -> [out: x " - " y " SNH"];
Product(x,y) -> [out: x "*" y];

ConstProduct_list(h, NiltProduct_list()) -> [out: h];
ConstProduct_list(h, l) -> [out: h "+" l];

Node(x) -> [out: "n" x ];

//////////////////////////////////////////////////////////////////////////
%uview temporal;

f=StatePredicateFormula(x) -> [temporal: x
{
    with(x) {
        ExPath(Eventually(y)) provided (not y->containsTemporal) :
            { f->type = FORMULA_REACHABLE; }
        AllPath(Always(y)) provided (not y->containsTemporal) :
            { f->type = FORMULA_INVARIANT; }
        AllPath(Always(Negation(y))) provided (not y->containsTemporal) :
            { f->type = FORMULA_IMPOSSIBLE; }
        AllPath(Always(ExPath(Eventually(y)))) provided (not y->containsTemporal) :
            { f->type = FORMULA_LIVENESS; }
        Always(Eventually(y)) provided (not y->containsTemporal) :
            { f->type = FORMULA_FAIRNESS; }
        Eventually(Always(y)) provided (not y->containsTemporal) :
            { f->type = FORMULA_STABILIZATION; }
        Eventually(y) provided (not y->containsTemporal) :
            { f->type = FORMULA_EVENTUALLY; }
        default provided (not x->containsTemporal) :
            { f->type = FORMULA_INITIAL; }
        default :
            { f->type = FORMULA_MODELCHECKING; }
    }
}
];

f=AtomicProposition(x) -> [temporal:     { f->containsTemporal = false; } ];
f=Negation(x)          -> [temporal: x   { f->containsTemporal = x->containsTemporal; } ];
f=Conjunction(x,y)     -> [temporal: x y { f->containsTemporal = (x->containsTemporal || y->containsTemporal); } ];
f=Disjunction(x,y)     -> [temporal: x y { f->containsTemporal = (x->containsTemporal || y->containsTemporal); } ];
f=AllPath(x)           -> [temporal: x   { f->containsTemporal = true; assert(x->containsTemporal); } ];
f=ExPath(x)            -> [temporal: x   { f->containsTemporal = true; assert(x->containsTemporal); } ];
f=Always(x)            -> [temporal: x   { f->containsTemporal = true; } ];
f=Eventually(x)        -> [temporal: x   { f->containsTemporal = true; } ];

//////////////////////////////////////////////////////////////////////////
%uview internal;

%{ KC_UNPARSE
#include <map>
#include <vector>
#include <stack>
#include <Core/Dimensions.h>
#include <Net/Net.h>
#include <Formula/StatePredicate.h>
#include <Formula/AtomicStatePredicate.h>
#include <Formula/ConjunctionStatePredicate.h>
#include <Formula/DisjunctionStatePredicate.h>
#include <Formula/TruePredicate.h>
#include <Formula/FalsePredicate.h>

// mapping from node ids to factors
std::map<int, int> current;

// mapping from tree node ids to list of intermediate formulas
std::map<int, std::vector<StatePredicate *> > formulas;

// invariant: top is always my parent
std::stack<size_t> id_stack;

// next free id
size_t nextId = 0;
%}

f=StatePredicateFormula(x) -> [internal:
    {
        //printf("I am a state predicate. I am %d\n", nextId);
        id_stack.push(nextId++);
    }
    x
    {
        nextId = id_stack.top();
        id_stack.pop();
        // return the only entry for the root
        assert(nextId == 0);
        assert(formulas[0].size() == 1);
        f->formula = formulas[0][0];
        //printf("I am a state predicate. Done!\n");
    }
];


Conjunction(a,b) -> [internal:
    {
        //printf("I am %d (a conjunction). My parent is %d\n", nextId, id_stack.top());
        id_stack.push(nextId++);
    }
    a
    b
    {
        // after returning from my childs, I am the top of the stack
        const index_t myId = id_stack.top();
        id_stack.pop();

        StatePredicate *f = new ConjunctionStatePredicate(formulas[myId].size());
        for (size_t i = 0; i < formulas[myId].size(); ++i)
        {
            //printf("conjunction %d: added a subformula to index %d\n", myId, i);
            assert(formulas[myId][i]);
            ((ConjunctionStatePredicate*)f)->addSub(i, formulas[myId][i]);
        }

        //printf("told my parent %d about me %d\n", id_stack.top(), myId);
        formulas[id_stack.top()].push_back(f);
    }
];

Disjunction(a,b) -> [internal:
    {
        //printf("I am %d (a disjunction). My parent is %d\n", nextId, id_stack.top());
        id_stack.push(nextId++);
    }
    a
    b
    {
        // after returning from my childs, I am the top of the stack
        const index_t  myId = id_stack.top();
        id_stack.pop();

        StatePredicate *f = new DisjunctionStatePredicate(formulas[myId].size());
        for (size_t i = 0; i < formulas[myId].size(); ++i)
        {
            //printf("disjunction %d: added a subformula to index %d\n", myId, i);
            assert(formulas[myId][i]);
            ((DisjunctionStatePredicate*)f)->addSub(i, formulas[myId][i]);
        }

        //printf("told my parent %d about me %d\n", id_stack.top(), myId);
        formulas[id_stack.top()].push_back(f);
    }
];

AtomicProposition(a) -> [internal: a ];

LessEqualAtomicProposition(x, Number(y)) -> [internal: x
{
    //printf("I am an atomic proposition. My parent is %d\n", id_stack.top());

    index_t factors_positive = 0;
    index_t factors_negative = 0;

    // count positive and negative entries
    for (std::map<int, int>::const_iterator it = current.begin(); it != current.end(); ++it)
    {
        if (it->second > 0)
        {
            factors_positive++;
        }
        else if (it->second < 0)
        {
            factors_negative++;
        }
    }

    // create formula object
    StatePredicate *f = new AtomicStatePredicate(factors_positive, factors_negative, y->value);
    index_t position_pos = 0;
    index_t position_neg = 0;

    // add positive and negative entries
    for (std::map<int, int>::const_iterator it = current.begin(); it != current.end(); ++it)
    {
        if (it->second > 0)
        {
            ((AtomicStatePredicate*)f)->addPos(position_pos++, it->first, it->second);
        }
        else if (it->second < 0)
        {
            ((AtomicStatePredicate*)f)->addNeg(position_neg++, it->first, -it->second);
        }
    }

    // reset current
    current.clear();

    // register formula at parent
    //printf("told my parent %d about me; %p\n", id_stack.top(), f);
    formulas[id_stack.top()].push_back(f);
}
];

True() -> [internal:
{
    StatePredicate *f = new TruePredicate();
    // register formula at parent
    formulas[id_stack.top()].push_back(f);    
}
];

False() -> [internal:
{
    StatePredicate *f = new FalsePredicate();
    // register formula at parent
    formulas[id_stack.top()].push_back(f);    
}
];

Fireable(x) -> [internal:
{
    // create a conjunction with #preset elements
    StatePredicate *f = new ConjunctionStatePredicate(Net::CardArcs[TR][PRE][x->value]);
    //printf("%p=ConjunctionStatePredicate(Net::CardArcs[TR][PRE][x->value]=%d)\n", f, Net::CardArcs[TR][PRE][x->value]);

    // iterate preset
    for (index_t p = 0; p < Net::CardArcs[TR][PRE][x->value]; ++p)
    {
        // create atomic state predicate with 1 positive factor and the arc weight
        StatePredicate *a = new AtomicStatePredicate(0, 1, - Net::Mult[TR][PRE][x->value][p]);
        //printf("%p=AtomicStatePredicate(0, 1, - Net::Mult[TR][PRE][x->value][p]=%d)\n", a, - Net::Mult[TR][PRE][x->value][p]);

        // add place p to index 0 with factor 1
        ((AtomicStatePredicate*)a)->addNeg(0, Net::Arc[TR][PRE][x->value][p], 1);
        //printf("%p->addNeg(0, Net::Arc[TR][PRE][x->value][p]=%d, 1)\n", a, Net::Arc[TR][PRE][x->value][p]);

        // add atomic state predicate to conjunction
        ((ConjunctionStatePredicate*)f)->addSub(p, a);
        //printf("%p->addSub(p=%d, a=%p)\n", f, p, a);
    }

    formulas[id_stack.top()].push_back(f);
}
];

Initial() -> [internal:
{
    StatePredicate *f = new ConjunctionStatePredicate(Net::Card[PL]);
    // iterate places
    for (index_t p = 0; p < Net::Card[PL]; ++p)
    {
        /* TODO */
    }

    formulas[id_stack.top()].push_back(f);
}
];


Product(x, Node(p)) -> [internal: { current[p->value] += x->value; } ];
Node(p) -> [internal: { assert(false /* sollte nicht mehr vorkommen */); current[p->value] += 1; } ];
Number(x) -> [internal: /* do not print values */ ];
