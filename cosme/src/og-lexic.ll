/*****************************************************************************\
 Cosme -- Checking Simulation, Matching, and Equivalence

 Copyright (c) 2010 Andreas Lehmann

 Cosme is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Cosme is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Cosme.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

%option outfile="lex.yy.c"
%option prefix="og_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cstdio>

#include "og-syntax.hh"
#include "verbose.h"

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
<BITS>"T"                               { BEGIN(INITIAL); return BIT_T; }
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
{identifier}   { og_yylval.str = og_yytext; return IDENT;              }

{whitespace}                            { /* do nothing */             }

.                                       { og_yyerror("lexical error"); }

%%

int og_yyerror(char const *msg) {
		status("%d: error near '%s': %s", og_yylineno, og_yytext, msg);
  	abort(6, "error while parsing the operating guideline");
}
