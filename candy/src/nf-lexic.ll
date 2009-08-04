%option outfile="lex.yy.c"
%option prefix="nf_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cstdio>
#include <iostream>
#include "config.h"
#include "nf-syntax.h"

extern int nf_yyerror(char const *msg);
%}


 /* a start condition to skip comments */
%s COMMENT

/* 
 * The state "IDENT2" is used for transition identifier.
 * Unlike place identifier these can contain the char "=",
 */
%s IDENT2

%%

 /* control comments */ 
"{$"                            { return LCONTROL; }
"$}"                            { return RCONTROL; }

 /* comments */
"{"                             { BEGIN(COMMENT); }
<COMMENT>"}"                    { BEGIN(INITIAL); }
<COMMENT>[^}]*                  { /* skip */ }

 /* control keywords */
MAX_UNIQUE_EVENTS               { return KEY_MAX_UNIQUE_EVENTS; }
ON_LOOP                         { return KEY_ON_LOOP; }
MAX_OCCURRENCES                 { return KEY_MAX_OCCURRENCES; }
TRUE                            { return KEY_TRUE; }
FALSE                           { return KEY_FALSE; }

 /* keywords */
SAFE                            { return KEY_SAFE; }
PLACE                           { return KEY_PLACE; }
INTERFACE                       { return KEY_INTERFACE; }
INTERNAL                        { return KEY_INTERNAL; }
INPUT                           { return KEY_INPUT; }
OUTPUT                          { return KEY_OUTPUT; }

TRANSITION                      { BEGIN(IDENT2); return KEY_TRANSITION; }
<IDENT2>[ \n\r\t]               { /* skip whitespaces */ }
<IDENT2>[^,;:()\t \n\r\{\}]+    { BEGIN(INITIAL); nf_yylval.str = strdup(yytext); return IDENT; }
<IDENT2>.                       { nf_yyerror("Unexpected symbol at transition identifier"); }

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

 /* other characters */
\:                              { return COLON; }
\;                              { return SEMICOLON; }
,                               { return COMMA; }

 /* identifiers */
[0-9]+                          { nf_yylval.value = atoi(yytext); return NUMBER; }
"-"[0-9]+                       { return NEGATIVE_NUMBER; }
[^,;:()\t \n\r\{\}=]+           { nf_yylval.str = strdup(yytext); return IDENT; }

 /* whitespace */
[ \n\r\t]                       { /* skip */ }

 /* anything else */
.                               { nf_yyerror("lexical error"); }

%%

int nf_yyerror(char const *msg) {
    /*fprintf(stderr, "%d: error near '%s': %s\n", cf_yylineno, cf_yytext,
    ** msg);*/
    std::cerr << PACKAGE << ": " << nf_yylineno
              << ": ERROR near '" << nf_yytext 
              << "': " << msg << std::endl;

    return EXIT_FAILURE;
}
