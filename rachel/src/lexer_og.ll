/*****************************************************************************\
 Rachel -- Reparing Service Choreographies

 Copyright (c) 2008, 2009 Niels Lohmann

 Rachel is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Rachel.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/


/* flex options */
%option outfile="lex.yy.c"
%option prefix="og_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput
%option caseless

%{
#include <config.h>
#include <cstring>
#include "verbose.h"
#include "types.h"

/* make this class visible for flex as flex uses it in og_yylval. */
class Formula;

#include "syntax_og.hh"

extern char* G_filename;
extern int og_yyerror(char const *msg);

Node last_parsed;
%}

%s COMMENT

whitespace     [\n\r\t ]
identifier     [^\n\r\t ,;:()\{\}=+*~<>]+
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
"SYNCHRONOUS"                           { return key_synchronous;      }

(?i:true)                               { return key_true;             }
(?i:false)                              { return key_false;            }
(?i:final)                              { return key_final;            }
"*"                                     { return op_and;               }
"+"                                     { return op_or;                }
"("                                     { return lpar;                 }
")"                                     { return rpar;                 }
"INITIAL"                               { return key_initial;          }

"RED"                                   { return key_red;              }
"BLUE"                                  { return key_blue;             }
"FINALNODE"                             { return key_finalnode;        }

":"                                     { return colon;                }
";"                                     { return semicolon;            }
","                                     { return comma;                }
"->"                                    { return arrow;                }

{number}       { last_parsed = og_yylval.value = atoi(og_yytext); return number;     }
{identifier}   { og_yylval.str = strdup(og_yytext); return ident;      }

{whitespace}                            { /* do nothing */             }

.                                       { og_yyerror("lexical error"); }


%%


int og_yyerror(char const *msg) {
    assert(msg != NULL);
    assert(og_yytext != NULL);
    assert(G_filename != NULL);

    abort(7, "%s:%d: error near '%s': %s", G_filename, og_yylineno, og_yytext, msg);
}
