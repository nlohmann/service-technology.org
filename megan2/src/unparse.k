%{ KC_UNPARSE_HEADER
#include <config.h>
#include "Task.h"
#include "profile.h"

void dummy_printer(const char *s, kc::uview v);
extern FILE *outfile;
%}

%{ KC_UNPARSE
/// a file that is used in the printer unparser
FILE *outfile = NULL;

void printer(const char *s, kc::uview v) {
    fprintf(outfile, "%s", s);
}

void dummy_printer(const char *s, kc::uview v) {
}

bool temp_isCTL = true;
bool temp_isLTL = true;
bool temp_inQuantor = false;
%}

%uview task;

/* when a task object is created, it is automatically added to Task::queue */

Negation(CTLProperty(name, f)) ->          [task: { new CTLTask(name->name, true); } ];
Negation(LTLProperty(name, f)) ->          [task: { new LTLTask(name->name, true); } ];
Negation(ReachabilityProperty(name, f)) -> [task: { new ReachabilityTask(name->name, true); } ];
Negation(DeadlockNet(name)) ->             [task: { new DeadlockTask(name->name, true); } ];
Negation(DeadlockInitial(name)) ->         [task: { new DeadlockInitialTask(name->name, true); } ];

CTLProperty(name, f) ->          [task: { new CTLTask(name->name); } ];
LTLProperty(name, f) ->          [task: { new LTLTask(name->name); } ];
ReachabilityProperty(name, f) -> [task: { Task *t = new ReachabilityTask(name->name); } ];
DeadlockNet(name) ->             [task: { Task *t = new DeadlockTask(name->name); } ];
DeadlockInitial(name) ->         [task: { Task *t = new DeadlockInitialTask(name->name); } ];
Property(name, f) ->             [task: { Task *t = new UnknownTask(name->name); }];



%uview lola;

// some assertions
Xor(*) -> [lola: { assert(false); } ];
Imp(*,*) -> [lola: { assert(false); } ];
Iff(*,*) -> [lola: { assert(false); } ];
And(Consformulas(*,Nilformulas())) -> [lola: { assert(false); } ];
Or(Consformulas(*,Nilformulas())) -> [lola: { assert(false); } ];
I(*) -> [lola: { assert(false); } ];
N(*) -> [lola: { assert(false); } ];

//Negation(x) -> [lola: "NEGATION: " x ];
//Property(name, f) -> [lola: name ":\n" f "\n\n" ];
ReachabilityProperty(name, f) -> [lola: f ];
//DeadlockNet(name) -> [lola: name " (deadlock check)\n\n"];
//DeadlockInitial(name) -> [lola: name " (initial state deadlocks)\n\n"];

True()  -> [lola: "TRUE"];
False() -> [lola: "FALSE"];

EQ(a,b) -> [lola: "(" a " = " b ")"];
NE(a,b) -> [lola: "(" a " != " b ")"];
GT(a,b) -> [lola: "(" a " > " b ")"];
GE(a,b) -> [lola: "(" a " >= " b ")"];
LT(a,b) -> [lola: "(" a " < " b ")"];
LE(a,b) -> [lola: "(" a " <= " b ")"];

Not(a) -> [lola: "NOT(" a ")" ];

And(x) -> [lola:
"("
{
    auto comma = false;
    foreach( a; formulas x ) {
        if (comma) {
            mkcasestring("\nAND ")->unparse(kc_printer, kc_current_view);
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
    auto comma = false;
    foreach( a; formulas x ) {
        if (comma) {
            mkcasestring("\nOR ")->unparse(kc_printer, kc_current_view);
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




%uview sara;

ReachabilityProperty(name, f) -> [sara: f];
LE(a,b) -> [sara: "(" a " <= " b ")"];

And(x) -> [sara:
"("
{
    auto comma = false;
    foreach( a; formulas x ) {
        if (comma) {
            mkcasestring("\nAND ")->unparse(kc_printer, kc_current_view);
        }
        comma = true;
        a->unparse(kc_printer, kc_current_view);
    }
}
")"
];

Or(x) -> [sara:
"("
{
    auto comma = false;
    foreach( a; formulas x ) {
        if (comma) {
            mkcasestring("\nOR ")->unparse(kc_printer, kc_current_view);
        }
        comma = true;
        a->unparse(kc_printer, kc_current_view);
    }
}
")"
];

Add(a,b) -> [sara: a " " b];
Mult(Integer(a),b) provided (a->value > 1) -> [sara: b ":" a];
Mult(Integer(a),b) provided (a->value == 1) -> [sara: b];
Mult(Integer(a),b) provided (a->value < 0) -> [sara: "- " b ":"  { mkinteger(- (a->value))->unparse(kc_printer, kc_current_view); } ];

Marking(x) -> [sara: x];
Card(x) -> [sara: x];



/*
Traverse the formula and check if it belongs to the LTL or CTL subset of CTL*.
*/

%uview ctl;

P=Property(n,f) -> [ctl: { isCTL = true; temp_isLTL = true; } f { P->isCTL = temp_isCTL; P->isLTL = temp_isLTL; } ];
A(x) ->   [ctl: { if  (temp_inQuantor) temp_isCTL = false; temp_inQuantor = true; temp_isLTL = false; } x ];
E(x) ->   [ctl: { if  (temp_inQuantor) temp_isCTL = false; temp_inQuantor = true; temp_isLTL = false; } x ];
F(x) ->   [ctl: { if (!temp_inQuantor) temp_isCTL = false; temp_inQuantor = false; } x ];
G(x) ->   [ctl: { if (!temp_inQuantor) temp_isCTL = false; temp_inQuantor = false; } x ];
X(x) ->   [ctl: { if (!temp_inQuantor) temp_isCTL = false; temp_inQuantor = false; } x ];
U(x,y) -> [ctl: { if (!temp_inQuantor) temp_isCTL = false; temp_inQuantor = false; } x ];
W(x,y) -> [ctl: { if (!temp_inQuantor) temp_isCTL = false; temp_inQuantor = false; } x ];
