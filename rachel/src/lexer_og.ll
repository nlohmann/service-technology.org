/*****************************************************************************\
 Rachel -- Repairing Automata for Choreographies by Editing Labels
 
 Copyright (C) 2008, 2009  Niels Lohmann <niels.lohmann@uni-rostock.de>
 Copyright (C) 2005, 2006  Jan Bretschneider (Fiona parser)
 
 Rachel is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Rachel is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Rachel (see file COPYING); if not, see http://www.gnu.org/licenses or write to
 the Free Software Foundation,Inc., 51 Franklin Street, Fifth
 Floor, Boston, MA 02110-1301  USA.
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
#include <cassert>
#include <cstdio>
#include <string>

#include "config.h"

/* make this class visible for flex as flex uses it in og_yylval. */
class Formula;

#include "syntax_og.h"

extern char* G_filename;
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
    assert(msg != NULL);
    assert(og_yytext != NULL);
    assert(G_filename != NULL);

    fprintf(stderr, "%s: %s:%d: %s - token last read '%s'\n",
            PACKAGE, G_filename, og_yylineno, msg, og_yytext);
    
    exit(EXIT_FAILURE);
}
