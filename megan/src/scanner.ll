%{
#include <cstring>     /* for atoi */
#include "ast-system-k.h"         /* for kc namespace */
#include "ast-system-yystype.h"   /* for flex/bison bridge */
#include "parser.h"    /* for token names */

extern int yyerror(const char *);
%}

%option noyywrap
%option outfile="scanner.cc" header-file="scanner.h"

%%

"reach"             { return _REACH; }
"struct"            { return _STRUCT; }
"ctl"               { return _CTL; }
"ltl"               { return _LTL; }
"bound"             { return _BOUND; }
"marking"           { return _MARKING; }
"deadlock"          { return _DEADLOCK; }

"true"              { return _TRUE; }
"false"             { return _FALSE; }
"I"                 { return _INVARIANT; }
"N"                 { return _IMPOSSIBILITY; }
"A"                 { return _ALLPATH; }
"E"                 { return _EXPATH; }
"X"                 { return _NEXTSTATE; }
"G"                 { return _ALWAYS; }
"F"                 { return _EVENTUALLY; }

"?"                 { return _QUESTIONMARK; }
"l0"                { return _LIVENESS0; }
"l1"                { return _LIVENESS1; }
"l2"                { return _LIVENESS2; }
"l3"                { return _LIVENESS3; }
"l4"                { return _LIVENESS4; }

"~"                 { return _TILDE; }

"="                 { return _EQ; }
"!="                { return _NEQ; }
"<"                 { return _LE; }
"<="                { return _LEQ; }
">"                 { return _GE; }
">="                { return _GEQ; }

"!"                 { return _NEGATION; }
"&"                 { return _AND; }
"|"                 { return _OR; }
"xor"               { return _XOR; }
"=>"                { return _IMPLY; }
"<=>"               { return _EQUIV; }
"U"                 { return _SUNTIL; }
"W"                 { return _WUNTIL; }

"+"                 { return _PLUS; }
"*"                 { return _MULT; }
"-"                 { return _MINUS; }
"/"                 { return _DIV; }

","                 { return _COMMA; }
":"                 { return _COLON; }
"("                 { return _LPAREN; }
")"                 { return _RPAREN; }

[0-9]+              { yylval.yt_integer = kc::mkinteger(atoi(yytext)); return _INTEGER; }
[a-zA-Z0-9_]+       { yylval.yt_casestring = kc::mkcasestring(yytext); return _IDENTIFIER; }
"\""[^"]*"\""       { yylval.yt_casestring = kc::mkcasestring(yytext); return _QUOTED; }

[\r\n]+             { return _EOL; }

[\t ]               { /* skip whitespace */ }
.                   { yyerror("lexical error"); }
