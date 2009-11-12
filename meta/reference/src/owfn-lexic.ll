%option outfile="lex.yy.c"
%option prefix="owfn_yy"
%option noyywrap
%option nounput
%option yylineno
%option nodefault

%{

#include "owfn-syntax.h"

int owfn_yyerror(const char *);

%}

%s COMMENT IDENT2

%%

"{$"                            { return LCONTROL; }
"$}"                            { return RCONTROL; }

"{"                             { BEGIN(COMMENT); }
<COMMENT>"}"                    { BEGIN(INITIAL); }
<COMMENT>[^}]*                  { /* skip */ }

MAX_UNIQUE_EVENTS               { return KEY_MAX_UNIQUE_EVENTS; }
ON_LOOP                         { return KEY_ON_LOOP; }
MAX_OCCURRENCES                 { return KEY_MAX_OCCURRENCES; }
TRUE                            { return KEY_TRUE; }
FALSE                           { return KEY_FALSE; }

SAFE                            { return KEY_SAFE; }
PLACE                           { return KEY_PLACE; }
INTERFACE                       { return KEY_INTERFACE; }
INTERNAL                        { return KEY_INTERNAL; }
INPUT                           { return KEY_INPUT; }
OUTPUT                          { return KEY_OUTPUT; }

TRANSITION                      { BEGIN(IDENT2); 
                                  return KEY_TRANSITION; }
<IDENT2>[ \n\r\t]               { /* skip */ }
<IDENT2>[^,;:()\t \n\r\{\}]+    { BEGIN(INITIAL); 
                                  owfn_yylval.yt_str = strdup(yytext); 
                                  return IDENT; }
<IDENT2>.                       { owfn_yyerror("Unexpected symbol at transition identifier"); }

INITIALMARKING                  { return KEY_INITIALMARKING; }
FINALMARKING                    { return KEY_FINALMARKING; }
NOFINALMARKING                  { return KEY_NOFINALMARKING; }
FINALCONDITION                  { return KEY_FINALCONDITION; }
COST                            { return KEY_COST; }
CONSUME                         { return KEY_CONSUME; }
PRODUCE                         { return KEY_PRODUCE; }
PORT                            { return KEY_PORT; }
PORTS                           { return KEY_PORTS; }
SYNCHRONOUS                     { return KEY_SYNCHRONOUS; }
SYNCHRONIZE                     { return KEY_SYNCHRONIZE; }
CONSTRAIN                       { return KEY_CONSTRAIN; }

 /* keywords for final conditions */
ALL_PLACES_EMPTY                { return KEY_ALL_PLACES_EMPTY; }
ALL_OTHER_PLACES_EMPTY          { return KEY_ALL_OTHER_PLACES_EMPTY; }
ALL_OTHER_INTERNAL_PLACES_EMPTY { return KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY; }
ALL_OTHER_EXTERNAL_PLACES_EMPTY { return KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY; }
AND                             { return OP_AND; }
OR                              { return OP_OR; }
NOT                             { return OP_NOT; }
\>                              { return OP_GT; }
\<                              { return OP_LT; }
\>=                             { return OP_GE; }
\<=                             { return OP_LE; }
=                               { return OP_EQ; }
\<\>                            { return OP_NE; }
\#                              { return OP_NE; }
\(                              { return LPAR; }
\)                              { return RPAR; }

\:                              { return COLON; }
\;                              { return SEMICOLON; }
,                               { return COMMA; }

[0-9]+                          { owfn_yylval.yt_int = atoi(yytext); 
                                  return NUMBER; }
"-"[0-9]+                       { owfn_yylval.yt_int = atoi(yytext); 
                                  return NEGATIVE_NUMBER; }
[^,;:()\t \n\r\{\}=]+           { owfn_yylval.yt_str = strdup(yytext); 
                                  return IDENT; }

[ \n\r\t]                       { /* skip */ }

.                               { owfn_yyerror("unexpected lexical token"); }

%%

int owfn_yyerror(const char *msg)
{
  fprintf(stderr, "%d: error near '%s': %s\n", yylineno, yytext, msg);
  return EXIT_FAILURE;
}
