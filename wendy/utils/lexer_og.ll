/* flex options */
%option outfile="lex.yy.c"
%option prefix="og_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput
%option caseless

%{
#include <cassert>
#include <cstdio>
#include <string>

#include "config.h"

/* make these classes visible for flex as flex uses it in og_yylval. */
class Node;
class Formula;

#include "syntax_og.h"

extern int og_yyerror(char const *msg);
%}

%s COMMENT
%s BITS

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
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
<BITS>"T"                               { BEGIN(INITIAL); return BIT_T; }
"TRUE"                                  { return KEY_TRUE;             }
"FALSE"                                 { return KEY_FALSE;            }
"FINAL"                                 { return KEY_FINAL;            }
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
    assert(msg);
    assert(og_yytext);

    fprintf(stderr, "%s: %d: error near '%s': %s\n", PACKAGE, 
        og_yylineno, og_yytext, msg);
    exit(EXIT_FAILURE);
}
