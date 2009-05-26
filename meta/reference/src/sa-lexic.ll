%option outfile="lex.yy.c"
%option prefix="sa_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cstdio>

#include "sa-syntax.h"

extern int sa_yyerror(char const *msg);
%}

%s COMMENT

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+


%%


"{"                                     { BEGIN(COMMENT);              }
<COMMENT>"}"                            { BEGIN(INITIAL);              }
<COMMENT>[^}]*                          { /* do nothing */             }

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

{number}       { sa_yylval.value = atoi(sa_yytext); return NUMBER;     }
{identifier}   { sa_yylval.str = sa_yytext; return IDENT;              }

{whitespace}                            { /* do nothing */             }

.                                       { sa_yyerror("lexical error"); }

%%

int sa_yyerror(char const *msg) {
    fprintf(stderr, "%d: error near '%s': %s\n", sa_yylineno, sa_yytext, msg);
    return EXIT_FAILURE;
}
