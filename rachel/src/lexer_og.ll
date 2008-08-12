/*****************************************************************************
 * Copyright 2008       Niels Lohmann                                        *
 * Copyright 2005, 2006 Jan Bretschneider                                    *
 *                                                                           *
 * This file is part of Fiona.                                               *
 *                                                                           *
 * Fiona is free software; you can redistribute it and/or modify it          *
 * under the terms of the GNU General Public License as published by the     *
 * Free Software Foundation; either version 2 of the License, or (at your    *
 * option) any later version.                                                *
 *                                                                           *
 * Fiona is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
 * more details.                                                             *
 *                                                                           *
 * You should have received a copy of the GNU General Public License along   *
 * with Fiona; if not, write to the Free Software Foundation, Inc., 51       *
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.                      *
 *****************************************************************************/

/* flex options */
%option outfile="lex.yy.c"
%option prefix="og_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cassert>
#include <cstdio>
#include <string>

/* make this class visible for flex as flex uses it in og_yylval. */
class Formula;

#include "syntax_og.h"

extern char* G_filename;
extern int og_yyerror(char *msg);
%}

%s COMMENT

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+


%%


"{"                                     { BEGIN(COMMENT);         }
<COMMENT>"}"                            { BEGIN(INITIAL);         }
<COMMENT>[^}]*                          { /* do nothing */        }

NODES                                   { return key_nodes;       }
INITIALNODE                             { return key_initialnode; }
TRANSITIONS                             { return key_transitions; }

[Tt][Rr][Uu][Ee]                        { return key_true;        }
[Ff][Aa][Ll][Ss][Ee]                    { return key_false;       }
[Ff][Ii][Nn][Aa][Ll]                    { return key_final;       }
"*"                                     { return op_and;          }
"+"                                     { return op_or;           }
"("                                     { return lpar;            }
")"                                     { return rpar;            }

[Rr][Ee][Dd]                            { return key_red;         }
[Bb][Ll][Uu][Ee]                        { return key_blue;        }
[Ff][Ii][Nn][Aa][Ll][Nn][Oo][Dd][Ee]    { return key_finalnode;   }

":"                                     { return colon;           }
";"                                     { return semicolon;       }
","                                     { return comma;           }
"->"                                    { return arrow;           }

{number}       { og_yylval.value = atoi(og_yytext); return number; }
{identifier}   { og_yylval.str = og_yytext; return ident;  }

{whitespace}                            { break; }

.                                       { og_yyerror("lexical error"); }


%%


int og_yyerror(char *msg) {
    assert(msg != NULL);
    assert(og_yytext != NULL);
    assert(G_filename != NULL);

    fprintf(stderr, "parse error in %s:%d: %s - token last read '%s'\n",
            G_filename, og_yylineno, msg, og_yytext);
    
    exit(EXIT_FAILURE);
}
