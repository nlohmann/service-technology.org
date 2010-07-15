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

(?i:nodes)                              { return KEY_NODES;            }

(?i:initialnode)                        { return KEY_INITIALNODE;      }
(?i:transitions)                        { return KEY_TRANSITIONS;      }

(?i:interface)                          { return KEY_INTERFACE;        }
(?i:input)                              { return KEY_INPUT;            }
(?i:output)                             { return KEY_OUTPUT;           }
(?i:synchronous)                        { return KEY_SYNCHRONOUS;      }

<BITS>(?i:f)                            { BEGIN(INITIAL); return BIT_F; }
<BITS>(?i:s)                            { BEGIN(INITIAL); return BIT_S; }
<BITS>(?i:t)                            { BEGIN(INITIAL); return BIT_T; }
(?i:true)                               { return KEY_TRUE;             }
(?i:false)                              { return KEY_FALSE;            }
(?i:final)                              { return KEY_FINAL;            }
(?i:initial)				{ return KEY_INITIAL;	       }
"~"                                     { return OP_NOT;               }
"*"                                     { return OP_AND;               }
"+"                                     { return OP_OR;                }
"("                                     { return LPAR;                 }
")"                                     { return RPAR;                 }

(?i:red)                                { return KEY_RED;              }
(?i:blue)                               { return KEY_BLUE;             }
(?i:finalnode)                          { return KEY_FINALNODE;        }

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
