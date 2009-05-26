%option outfile="lex.yy.c"
%option prefix="ognew_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cstdio>

#include "ognew-syntax.h"

extern int ognew_yyerror(char const *msg);
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

{number}       { ognew_yylval.value = atoi(ognew_yytext); return NUMBER;     }
{identifier}   { ognew_yylval.str = ognew_yytext; return IDENT;              }

{whitespace}                            { /* do nothing */             }

.                                       { ognew_yyerror("lexical error"); }

%%

int ognew_yyerror(char const *msg) {
    fprintf(stderr, "%d: error near '%s': %s\n", ognew_yylineno, ognew_yytext, msg);
    return EXIT_FAILURE;
}
