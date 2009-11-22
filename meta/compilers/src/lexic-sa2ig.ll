%option outfile="lex.yy.c"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{

#include <cstring>
#include <iostream>

#include "syntax-sa2ig.h"
#include "config.h"

int yyerror(char const *msg);

extern std::ostream * myOut;

%}

%s COMMENT

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+


%%


"{"                                     { (*myOut) << "{"; BEGIN(COMMENT);      }
<COMMENT>"}"                            { (*myOut) << "}\n\n"; BEGIN(INITIAL);      }
<COMMENT>[^}]*                          { (*myOut) << yytext; /* copy comment */  }

"NODES"                                 { return KEY_NODES;            }
"INITIAL"                               { return KEY_INITIAL;          }
"FINAL"                                 { return KEY_FINAL;            }

"INTERFACE"                             { return KEY_INTERFACE;        }
"INPUT"                                 { return KEY_INPUT;            }
"OUTPUT"                                { return KEY_OUTPUT;           }
"SYNCHRONOUS"                           { return KEY_SYNCHRONOUS;      }

":"                                     { return COLON;                }
";"                                     { return SEMICOLON;            }
","                                     { return COMMA;                }
"->"                                    { return ARROW;                }

{number}       { yylval.value = atoi(yytext); return NUMBER;     }
{identifier}   { yylval.str = yytext; return IDENT;              }

{whitespace}                            { /* do nothing */             }

.                                       { yyerror("lexical error"); }

%%

int yyerror(char const *msg) {
    std::cerr << PACKAGE << ": " << yylineno
              << ": ERROR near '" << yytext 
              << "': " << msg << std::endl;
    exit(EXIT_FAILURE);
}

