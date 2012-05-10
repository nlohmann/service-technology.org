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
#include <string>
#include <map>
#include "Formula/StatePredicate.h"
#include "Formula/AtomicStatePredicate.h"
#include "Parser/ParserPTNet.h"
#include "Parser/Symbol.h"
extern StatePredicate *sp;
extern ParserPTNet* symbolTables;

std::map<std::string, int> current;
%}

StatePredicateFormula(x) -> [internal: x ];
ConstDisjunction_list(h, l) -> [internal: h l ];
ConstConjunction_list(h, l) -> [internal: h l ];

LessEqualAtomicProposition(x, Number(y)) -> [internal: x {
    int factors_positive = 0;
    int factors_negative = 0;

    for (std::map<std::string, int>::const_iterator it = current.begin(); it != current.end(); ++it)
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

    for (std::map<std::string, int>::const_iterator it = current.begin(); it != current.end(); ++it)
    {
        Symbol *p = symbolTables->PlaceTable->lookup(it->first.c_str());
        if (p == NULL)
        {
            fprintf(stderr, "place %s not found\n", it->first.c_str());
            assert(p);
        }

        if (it->second == 0)
        {
            continue;
        }
        if (it->second > 0)
        {
            ((AtomicStatePredicate*)sp)->addPos(position_pos++, p->getIndex(), it->second);
        }
        else
        {
            ((AtomicStatePredicate*)sp)->addNeg(position_neg++, p->getIndex(), it->second);
        }
    }
} ];

Product(x, Node(p)) -> [internal: { current[p->name] += x->value; } ];
