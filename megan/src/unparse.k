%uview lola;

Property(name, f) -> [lola: name ":\n" f "\n\n" ];
ReachabilityProperty(name, f) -> [lola: name " (reachability):\n" f "\n\n" ];

True()  -> [lola: "true"];
False() -> [lola: "false"];

EQ(a,b) -> [lola: "(" a " = " b ")"];
NE(a,b) -> [lola: "(" a " != " b ")"];
GT(a,b) -> [lola: "(" a " > " b ")"];
GE(a,b) -> [lola: "(" a " >= " b ")"];
LT(a,b) -> [lola: "(" a " < " b ")"];
LE(a,b) -> [lola: "(" a " <= " b ")"];

Not(a) -> [lola: "!(" a ")" ];
And(a,b) -> [lola: "(" a " AND " b ")"];
Or(a,b) -> [lola: "(" a " Or " b ")"];

Add(a,b) -> [lola: "(" a " + " b ")"];
Minus(a,b) -> [lola: "(" a " - " b ")"];
Mult(a,b) -> [lola: "(" a " * " b ")"];
Div(a,b) -> [lola: "(" a " / " b ")"];

Marking(x) -> [lola: x];
Card(x) -> [lola: x];

A(x) -> [lola: "A" x];
E(x) -> [lola: "E" x];
X(x) -> [lola: "X" x];
G(x) -> [lola: "G" x];
F(x) -> [lola: "F" x];
W(x,y) -> [lola: "(" x " W " y ")"];
U(x,y) -> [lola: "(" x " U " y ")"];
