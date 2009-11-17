%option outfile="lex.yy.c"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{

#include <cstring>
#include <iostream>
#include "syntax-wf2b.h"
#include "config.h"

int yyerror(char const *msg);

extern std::ostream * myOut;

%}

%s COMMENT
%s BITS

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=~]+
number         [0-9]+


%%


"{"                                     { (*myOut) << "\n{"; BEGIN(COMMENT);      }
<COMMENT>"}"                            { (*myOut) << "}\n"; BEGIN(INITIAL);      }
<COMMENT>[^}]*                          { (*myOut) << yytext; /* copy comment */  }

"NODES"                                 { return KEY_NODES;            }

"INTERFACE"                             { return KEY_INTERFACE;        }
"INPUT"                                 { return KEY_INPUT;            }
"OUTPUT"                                { return KEY_OUTPUT;           }
"SYNCHRONOUS"                           { return KEY_SYNCHRONOUS;      }

<BITS>"F"                               { BEGIN(INITIAL); return BIT_F; }
<BITS>"S"                               { BEGIN(INITIAL); return BIT_S; }
<BITS>"T"                               { BEGIN(INITIAL); return BIT_T; }
"TRUE"                                  { return KEY_TRUE;             }
"true"                                  { return KEY_TRUE;             }
"FALSE"                                 { return KEY_FALSE;            }
"false"                                 { return KEY_FALSE;            }
"FINAL"                                 { return KEY_FINAL;            }
"final"                                 { return KEY_FINAL;            }
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

{number}       { yylval.value = atoi(yytext); return NUMBER;     }
{identifier}   { yylval.str = strdup(yytext); return IDENT;              }

{whitespace}                            { /* do nothing */             }

.                                       { yyerror("lexical error"); }

%%

int yyerror(char const *msg) {
    std::cerr << PACKAGE << ": " << yylineno
              << ": ERROR near '" << yytext 
              << "': " << msg << std::endl;
    exit(EXIT_FAILURE);
}

