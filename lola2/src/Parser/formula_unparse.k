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


%uview internal;

%{ KC_UNPARSE
#include <map>
#include "Formula/StatePredicate.h"
#include "Formula/AtomicStatePredicate.h"
extern StatePredicate *sp;

std::map<int, int> current;
%}

StatePredicateFormula(x) -> [internal: x ];
ConstDisjunction_list(h, l) -> [internal: h l ];
ConstConjunction_list(h, l) -> [internal: h l ];

LessEqualAtomicProposition(x, Number(y)) -> [internal: x {
    int factors_positive = 0;
    int factors_negative = 0;

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

    sp = new AtomicStatePredicate(factors_positive, factors_negative, y->value);
    int position_pos = 0;
    int position_neg = 0;

    for (std::map<int, int>::const_iterator it = current.begin(); it != current.end(); ++it)
    {
        if (it->second == 0)
        {
            continue;
        }
        if (it->second > 0)
        {
            ((AtomicStatePredicate*)sp)->addPos(position_pos++, it->first, it->second);
        }
        else
        {
            ((AtomicStatePredicate*)sp)->addNeg(position_neg++, it->first, it->second);
        }
    }
} ];

Product(x, Node(p)) -> [internal: { current[p->value] += x->value; } ];
Node(p) -> [internal: { current[p->value] += 1; } ];
