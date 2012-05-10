//////////////////////////////////////////////////////////////////////////
%uview out;

StatePredicateFormula(x) -> [out: x "\n"];

AtomicProposition(x) -> [out: x];

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


ConstConjunction_list(h, NiltConjunction_list()) -> [out: h];
ConstConjunction_list(h, l) -> [out: h " AND " l];

ConstDisjunction_list(h, NiltDisjunction_list()) -> [out: h];
ConstDisjunction_list(h, l) -> [out: h " OR " l];

//////////////////////////////////////////////////////////////////////////
%uview internal;

%{ KC_UNPARSE
#include <map>
#include <vector>
#include "Formula/StatePredicate.h"
#include "Formula/AtomicStatePredicate.h"
#include "Formula/ConjunctionStatePredicate.h"
#include "Formula/DisjunctionStatePredicate.h"
extern StatePredicate *sp;

std::map<int, int> current;
std::map<int, std::vector<StatePredicate *> > formulas;

int currentId = 0;
%}

StatePredicateFormula(x) -> [internal:
    {
        // printf("I am a state predicate. I am %d\n", currentId);
    }
    x
    {
        // return the only entry for the root
        assert(currentId == 0);
        assert(formulas[0].size() == 1);
        sp = formulas[0][0];
    }
];


ConjunctionList(l) -> [internal:
    {
        // printf("I am %d (a conjunction list). My parent is %d\n", currentId+1, currentId);
        currentId++;
    }
    l
    {
        StatePredicate *f = new ConjunctionStatePredicate(formulas[currentId].size());
        for (size_t i = 0; i < formulas[currentId].size(); ++i)
        {
            // printf("added a subformula to index %d\n", i);
            assert(formulas[currentId][i]);
            ((ConjunctionStatePredicate*)f)->addSub(i, formulas[currentId][i]);
        }
        currentId--;
        formulas[currentId].push_back(f);
    }
];

DisjunctionList(l) -> [internal:
    {
        // printf("I am %d (a disjunction list). My parent is %d\n", currentId+1, currentId);
        currentId++;
    }
    l
    {
        StatePredicate *f = new DisjunctionStatePredicate(formulas[currentId].size());
        for (size_t i = 0; i < formulas[currentId].size(); ++i)
        {
            // printf("added a subformula to index %d\n", i);
            assert(formulas[currentId][i]);
            ((DisjunctionStatePredicate*)f)->addSub(i, formulas[currentId][i]);
        }
        currentId--;
        formulas[currentId].push_back(f);
    }
];


LessEqualAtomicProposition(x, Number(y)) -> [internal: x {
    // printf("I am an atomic proposition. My parent is %d\n", currentId);
    
    int factors_positive = 0;
    int factors_negative = 0;

    // count positive and negative entries
    for (std::map<int, int>::const_iterator it = current.begin(); it != current.end(); ++it)
    {
        if (it->second == 0)
        {
            continue;
        }
        if (it->second > 0)
        {
            factors_positive++;
        }
        else
        {
            factors_negative++;
        }
    }

    // create formula object
    StatePredicate *f = new AtomicStatePredicate(factors_positive, factors_negative, y->value);
    int position_pos = 0;
    int position_neg = 0;

    // add positive and negative entries
    for (std::map<int, int>::const_iterator it = current.begin(); it != current.end(); ++it)
    {
        if (it->second == 0)
        {
            continue;
        }
        if (it->second > 0)
        {
            assert(it->second > 0);
            ((AtomicStatePredicate*)f)->addPos(position_pos++, it->first, it->second);
        }
        else
        {
            assert(-it->second > 0);
            ((AtomicStatePredicate*)f)->addNeg(position_neg++, it->first, -it->second);
        }
    }
    current.clear();
    formulas[currentId].push_back(f);
}
];

Product(x, Node(p)) -> [internal: { current[p->value] += x->value; } ];
Node(p) -> [internal: { current[p->value] += 1; } ];
