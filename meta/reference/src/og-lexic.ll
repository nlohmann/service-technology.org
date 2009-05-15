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

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+


%%


"{"                                     { BEGIN(COMMENT);              }
<COMMENT>"}"                            { BEGIN(INITIAL);              }
<COMMENT>[^}]*                          { /* do nothing */             }

"NODES"                                 { return key_nodes;            }
"INITIALNODE"                           { return key_initialnode;      }
"TRANSITIONS"                           { return key_transitions;      }

"INTERFACE"                             { return key_interface;        }
"INPUT"                                 { return key_input;            }
"OUTPUT"                                { return key_output;           }

"TRUE"                                  { return key_true;             }
"FALSE"                                 { return key_false;            }
"FINAL"                                 { return key_final;            }
"*"                                     { return op_and;               }
"+"                                     { return op_or;                }
"("                                     { return lpar;                 }
")"                                     { return rpar;                 }

"RED"                                   { return key_red;              }
"BLUE"                                  { return key_blue;             }
"FINALNODE"                             { return key_finalnode;        }

":"                                     { return colon;                }
";"                                     { return semicolon;            }
","                                     { return comma;                }
"->"                                    { return arrow;                }

{number}       { og_yylval.value = atoi(og_yytext); return number;     }
{identifier}   { og_yylval.str = og_yytext; return ident;              }

{whitespace}                            { /* do nothing */             }

.                                       { og_yyerror("lexical error"); }

%%

int og_yyerror(char const *msg) {
    fprintf(stderr, "%d: error near '%s': %s\n", og_yylineno, og_yytext, msg);
    return EXIT_FAILURE;
}
