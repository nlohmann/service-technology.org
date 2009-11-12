%option outfile="lex.yy.c"
%option prefix="og_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cstdio>

#include "og-syntax.h"

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

"INITIALNODE"                           { return KEY_INITIALNODE;      }
"TRANSITIONS"                           { return KEY_TRANSITIONS;      }

"INTERFACE"                             { return KEY_INTERFACE;        }
"INPUT"                                 { return KEY_INPUT;            }
"OUTPUT"                                { return KEY_OUTPUT;           }
"SYNCHRONOUS"                           { return KEY_SYNCHRONOUS;      }

<BITS>"F"                               { BEGIN(INITIAL); return BIT_F; }
<BITS>"S"                               { BEGIN(INITIAL); return BIT_S; }
"TRUE"                                  { return KEY_TRUE;             }
"FALSE"                                 { return KEY_FALSE;            }
"FINAL"                                 { return KEY_FINAL;            }
"INITIAL"				{ return KEY_INITIAL;	       }
"~"                                     { return OP_NOT;               }
"*"                                     { return OP_AND;               }
"+"                                     { return OP_OR;                }
"("                                     { return LPAR;                 }
")"                                     { return RPAR;                 }

"RED"                                   { return KEY_RED;              }
"BLUE"                                  { return KEY_BLUE;             }
"FINALNODE"                             { return KEY_FINALNODE;        }

":"                                     { return COLON;                }
"::"                                    { BEGIN(BITS); return DOUBLECOLON; }
";"                                     { return SEMICOLON;            }
","                                     { return COMMA;                }
"->"                                    { return ARROW;                }

{number}       { og_yylval.value = atoi(og_yytext); return NUMBER;     }
{identifier}   { og_yylval.str = strdup(og_yytext); return IDENT;              }

{whitespace}                            { /* do nothing */             }

.                                       { og_yyerror("lexical error"); }

%%

int og_yyerror(char const *msg) {
    fprintf(stderr, "%d: error near '%s': %s\n", og_yylineno, og_yytext, msg);
    return EXIT_FAILURE;
}
