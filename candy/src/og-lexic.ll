%option outfile="lex.yy.c"
%option prefix="og_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
//#include <cstdio>

// RICH
// c++ streams library, neccessary for std:cerr
#include <iostream>
// from configure, neccessary for PACKAGE
#include "config.h"

// hack: for Formula in parser union
#include "Formula.h"
// from parser
#include "og-syntax.h"

extern int og_yyerror(char const *msg);
%}

%s COMMENT
%s BITS

whitespace     [\n\r\t ]
identifier     [^\n\r\t ,;:()\{\}=+*~<>]+
number         [0-9]+


%%


"{"                                     { BEGIN(COMMENT);              }
<COMMENT>"}"                            { BEGIN(INITIAL);              }
<COMMENT>[^}]*                          { /* do nothing */             }

"NODES"                                 { return KEY_NODES;            }
"INTERFACE"                             { return KEY_INTERFACE;        }
"INPUT"                                 { return KEY_INPUT;            }
"OUTPUT"                                { return KEY_OUTPUT;           }
"SYNCHRONOUS"                           { return KEY_SYNCHRONOUS;      }

<BITS>"F"                               { BEGIN(INITIAL); return BIT_F; }
<BITS>"S"                               { BEGIN(INITIAL); return BIT_S; }
(?i:true)                               { return KEY_TRUE;             }
(?i:false)                              { return KEY_FALSE;            }
(?i:final)                              { return KEY_FINAL;            }
"~"                                     { return OP_NOT;               }
"*"                                     { return OP_AND;               }
"+"                                     { return OP_OR;                }
"("                                     { return LPAR;                 }
")"                                     { return RPAR;                 }

":"                                     { return COLON;                }
"::"                                    { BEGIN(BITS); return DOUBLECOLON; }
";"                                     { return SEMICOLON;            }
","                                     { return COMMA;                }
"->"                                    { return ARROW;                }

{number}       { og_yylval.value = atoi(og_yytext); return NUMBER;     }
{identifier}   { og_yylval.str = strdup(og_yytext); return IDENT;      }

{whitespace}                            { /* do nothing */             }

.                                       { og_yyerror("lexical error"); }

%%

int og_yyerror(char const *msg) {
    /*fprintf(stderr, "%d: error near '%s': %s\n", og_yylineno, og_yytext,
    ** msg);*/
    std::cerr << PACKAGE << ": " << og_yylineno
              << ": ERROR near '" << og_yytext 
              << "': " << msg << std::endl;

    return EXIT_FAILURE;
}
