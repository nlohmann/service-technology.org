%{ KC_UNPARSE_HEADER
#include <config.h>
%}

%uview lola;

// some assertions
Xor(*) -> [lola: { assert(false); } ];
Imp(*,*) -> [lola: { assert(false); } ];
Iff(*,*) -> [lola: { assert(false); } ];
And(Consformulas(*,Nilformulas())) -> [lola: { assert(false); } ];
Or(Consformulas(*,Nilformulas())) -> [lola: { assert(false); } ];
I(*) -> [lola: { assert(false); } ];
N(*) -> [lola: { assert(false); } ];

Negation(x) -> [lola: "NEGATION: " x ];
Property(name, f) -> [lola: name ":\n" f "\n\n" ];
ReachabilityProperty(name, f) -> [lola: name " (reachability):\nFORMULA REACHABLE (" f ");\n\n" ];
DeadlockNet(name) -> [lola: name " (deadlock check)\n\n"];
DeadlockInitial(name) -> [lola: name " (initial state deadlocks)\n\n"];

True()  -> [lola: "TRUE"];
False() -> [lola: "FALSE"];

EQ(a,b) -> [lola: "(" a " = " b ")"];
NE(a,b) -> [lola: "(" a " != " b ")"];
GT(a,b) -> [lola: "(" a " > " b ")"];
GE(a,b) -> [lola: "(" a " >= " b ")"];
LT(a,b) -> [lola: "(" a " < " b ")"];
LE(a,b) -> [lola: "(" a " <= " b ")"];

Not(a) -> [lola: "!(" a ")" ];

And(x) -> [lola:
"("
{
    bool comma = false;
    foreach( a; formulas x ) {
        if (comma) {
            mkcasestring(" AND ")->unparse(kc_printer, kc_current_view);
        }
        comma = true;
        a->unparse(kc_printer, kc_current_view);
    }
}
")"
];

Or(x) -> [lola:
"("
{
    bool comma = false;
    foreach( a; formulas x ) {
        if (comma) {
            mkcasestring(" OR ")->unparse(kc_printer, kc_current_view);
        }
        comma = true;
        a->unparse(kc_printer, kc_current_view);
    }
}
")"
];

Consformulas(x,Nilformulas()) -> [lola: x];
Consformulas(x,y) -> [lola: x ", " y];

Add(a,b) -> [lola: "(" a " + " b ")"];
Minus(a,b) -> [lola: "(" a " - " b ")"];
Mult(a,b) -> [lola: "(" a " * " b ")"];
Div(a,b) -> [lola: "(" a " / " b ")"];

Marking(x) -> [lola: x];
Card(x) -> [lola: x];
Fireable(x) -> [lola: "FIREABLE(" x ")"];

A(x) -> [lola: "A" x];
E(x) -> [lola: "E" x];
X(x) -> [lola: "X" x];
G(x) -> [lola: "G" x];
F(x) -> [lola: "F" x];
W(x,y) -> [lola: "(" x " W " y ")"];
U(x,y) -> [lola: "(" x " U " y ")"];
