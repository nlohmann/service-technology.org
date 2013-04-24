%{
#include "ast-system-k.h"
#include "ast-system-yystype.h"
#include "scanner.h"
int yyerror(const char *msg);
extern int yylineno;
extern kc::property_set root;

using namespace kc;
%}

%output "parser.cc"
%defines "parser.h"

%token _REACH _STRUCT _CTL _LTL _BOUND _MARKING _DEADLOCK
%token _TRUE _FALSE _TILDE
%token _QUESTIONMARK _LIVENESS0 _LIVENESS1 _LIVENESS2 _LIVENESS3 _LIVENESS4
%token _COMMA _COLON _LPAREN _RPAREN _EOL

%left _OR _XOR
%left _AND
%left _IMPLY
%left _EQUIV
%left _EQ _NEQ
%left _LE _LEQ _GE _GEQ
%left _PLUS _MINUS
%left _MULT _DIV
%right _NEGATION
%right _ALWAYS _EVENTUALLY _NEXTSTATE
%left _SUNTIL _WUNTIL
%right _ALLPATH _EXPATH
%right _INVARIANT _IMPOSSIBILITY

%token <yt_integer> _INTEGER
%token <yt_casestring> _IDENTIFIER
%token <yt_casestring> _QUOTED

%type <yt_property_set> propertyset
%type <yt_property> property
%type <yt_formula> simple_formula
%type <yt_formula> complex_formula
%type <yt_formula> AND_sequence
%type <yt_formula> OR_sequence
%type <yt_formula> XOR_sequence
%type <yt_expression> simple_expression
%type <yt_expression> complex_expression
%type <yt_casestring> identifier

%%

start:
  propertyset { root = $1; }
;

propertyset:
  property              { $$ = Consproperty_set($1, Nilproperty_set()); }
| property propertyset  { $$ = Consproperty_set($1, $2); }
;

property:
  identifier _COLON complex_formula _EOL         { $$ = Property($1, $3); }
| _REACH identifier _COLON complex_formula _EOL  { $$ = Property($2, $4); }
| _STRUCT identifier _COLON complex_formula _EOL { $$ = Property($2, $4); }
| _CTL identifier _COLON complex_formula _EOL    { $$ = Property($2, $4); }
| _LTL identifier _COLON complex_formula _EOL    { $$ = Property($2, $4); }
;

simple_formula:
  _TRUE                                        { $$ = True(); }
| _FALSE                                       { $$ = False(); }
| _LPAREN complex_formula _RPAREN              { $$ = $2; }
| _NEGATION simple_formula                     { $$ = Not($2); }
| _INVARIANT simple_formula                    { $$ = I($2); }
| _IMPOSSIBILITY simple_formula                { $$ = N($2); }
| _ALLPATH simple_formula                      { $$ = A($2); }
| _EXPATH simple_formula                       { $$ = E($2); }
| _NEXTSTATE simple_formula                    { $$ = X($2); }
| _NEXTSTATE _INTEGER simple_formula           { $$ = X($3); }
| _NEXTSTATE _TILDE simple_formula             { $$ = X($3); }
| _NEXTSTATE _INTEGER _TILDE simple_formula    { $$ = X($4); }
| _ALWAYS simple_formula                       { $$ = G($2); }
| _EVENTUALLY simple_formula                   { $$ = F($2); }
| _DEADLOCK                                    { $$ = Deadlock(); }
| identifier _QUESTIONMARK                     { $$ = Fireable($1); }
| identifier _QUESTIONMARK _LIVENESS0          { $$ = Live0($1); }
| identifier _QUESTIONMARK _LIVENESS1          { $$ = Live1($1); }
| identifier _QUESTIONMARK _LIVENESS2          { $$ = Live2($1); }
| identifier _QUESTIONMARK _LIVENESS3          { $$ = Live3($1); }
| identifier _QUESTIONMARK _LIVENESS4          { $$ = Live4($1); }
| complex_expression _EQ complex_expression    { $$ = EQ($1, $3); }
| complex_expression _NEQ complex_expression   { $$ = NE($1, $3); }
| complex_expression _LE complex_expression    { $$ = LT($1, $3); }
| complex_expression _LEQ complex_expression   { $$ = LE($1, $3); }
| complex_expression _GE complex_expression    { $$ = GT($1, $3); }
| complex_expression _GEQ complex_expression   { $$ = GE($1, $3); }
;

complex_formula:
  simple_formula                         { $$ = $1; }
| simple_formula _AND AND_sequence       { $$ = And($1, $3); }
| simple_formula _OR OR_sequence         { $$ = Or($1, $3); }
| simple_formula _XOR XOR_sequence       { $$ = Xor($1, $3); }
| simple_formula _IMPLY simple_formula   { $$ = Imp($1, $3); }
| simple_formula _EQUIV simple_formula   { $$ = Iff($1, $3); }
| simple_formula _SUNTIL simple_formula  { $$ = U($1, $3); }
| simple_formula _WUNTIL simple_formula  { $$ = W($1, $3); }
;

AND_sequence:
  simple_formula                    { $$ = $1; }
| simple_formula _AND AND_sequence  { $$ = And($1, $3); }
;

OR_sequence:
  simple_formula                  { $$ = $1; }
| simple_formula _OR OR_sequence  { $$ = Or($1, $3); }
;

XOR_sequence:
  simple_formula                    { $$ = $1; }
| simple_formula _XOR XOR_sequence  { $$ = Xor($1, $3); }
;

complex_expression:
  simple_expression                            { $$ = $1; }
| _LPAREN complex_expression _RPAREN           { $$ = $2; }
| complex_expression _PLUS complex_expression  { $$ = Add($1, $3); }
| complex_expression _MULT complex_expression  { $$ = Mult($1, $3); }
| complex_expression _MINUS complex_expression { $$ = Minus($1, $3); }
| complex_expression _DIV complex_expression   { $$ = Div($1, $3); }
;

simple_expression:
  _INTEGER                             { $$ = Integer($1); }
| _BOUND _LPAREN identifier _RPAREN    { $$ = Bound($3); }
| _OR complex_expression _OR           { $$ = Card($2); }
| _MARKING _LPAREN identifier _RPAREN  { $$ = Marking($3); }
;

identifier:
  _IDENTIFIER { $$ = $1; }
| _QUOTED     { $$ = $1; }
;

%%

int yyerror(const char *msg) {
    fprintf(stderr, "parse error in line %d: %s\n", yylineno, msg);
    fprintf(stderr, "last token: %s\n", yytext);
    exit(1);
}
