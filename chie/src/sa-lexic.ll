%option outfile="lex.yy.c"
%option prefix="sa_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cstdio>
#include <string>

#include "sa-syntax.h"
#include "verbose.h"

extern int sa_yyerror(char const *msg);
extern std::string sa_lastIdent;
%}

%s COMMENT

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+


%%


"{"                                     { BEGIN(COMMENT);              }
<COMMENT>"}"                            { BEGIN(INITIAL);              }
<COMMENT>[^}]*                          { /* do nothing */             }

"NODES"                                 { return NODES;            }
"INITIAL"                               { return INITIAL_;          }
"FINAL"                                 { return FINAL;            }

"INTERFACE"                             { return INTERFACE;        }
"INPUT"                                 { return INPUT;            }
"OUTPUT"                                { return OUTPUT;           }
"SYNCHRONOUS"                           { return SYNCHRONOUS;      }

":"                                     { return COLON;                }
";"                                     { return SEMICOLON;            }
","                                     { return COMMA;                }
"->"                                    { return ARROW;                }

{number}       { sa_yylval.value = atoi(sa_yytext); return NUMBER;     }
{identifier}   { sa_lastIdent = sa_yytext; return IDENT;              }

{whitespace}                            { /* do nothing */             }

.                                       { sa_yyerror("lexical error"); }

%%

int sa_yyerror(char const *msg) {
    abort(0, "%d: error near '%s': %s\n", sa_yylineno, sa_yytext, msg);
}

