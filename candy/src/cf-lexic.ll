%option outfile="lex.yy.c"
%option prefix="cf_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
// RICH
//#include <cstdio>

// c++ streams library, neccessary for std:cerr
#include <iostream>
// from configure, neccessary for PACKAGE
#include "config.h"

// from parser
#include "cf-syntax.h"

extern int cf_yyerror(char const *msg);
%}

%s COMMENT

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+


%%


"{"                                     { BEGIN(COMMENT);              }
<COMMENT>"}"                            { BEGIN(INITIAL);              }
<COMMENT>[^}]*                          { /* do nothing */             }

";"                                     { return SEMICOLON;            }

{number}       { cf_yylval.value = atoi(cf_yytext); return NUMBER;     }
{identifier}   { cf_yylval.str = strdup(cf_yytext); return IDENT;      }

{whitespace}                            { /* do nothing */             }

.                                       { cf_yyerror("lexical error"); }

%%

int cf_yyerror(char const *msg) {
    /*fprintf(stderr, "%d: error near '%s': %s\n", cf_yylineno, cf_yytext,
    ** msg);*/
    std::cerr << PACKAGE << ": " << cf_yylineno
              << ": ERROR near '" << cf_yytext 
              << "': " << msg << std::endl;

    return EXIT_FAILURE;
}
