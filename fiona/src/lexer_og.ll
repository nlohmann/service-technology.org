/*****************************************************************************\
 Copyright (c) 2008  Robert Danitz, Christian Gierds, Niels Lohmann,
                     Peter Massuthe, Richard Mu"ller, Janine Ott,
                     Jan Su"rmeli, Daniela Weinberg, Karsten Wolf,
                     Martin Znamirowski

 Copyright (c) 2007  Jan Bretschneider, Robert Danitz, Christian Gierds,
                     Kathrin Kaschner, Leonard Kern, Niels Lohmann,
                     Peter Massuthe, Daniela Weinberg, Karsten Wolf

 Copyright (c) 2006  Kathrin Kaschner, Peter Massuthe, Daniela Weinberg,
                     Karsten Wolf

 This file is part of Fiona, see <http://service-technology.org/fiona>.

 Fiona is free software; you can redistribute it and/or modify it under the
 terms of the GNU General Public License as published by the Free Software
 Foundation; either version 3 of the License, or (at your option) any later
 version.
 
 Fiona is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with
 Fiona (see file COPYING). If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

/*!
 * \file    lexer_og.cc
 *
 * \brief   the lexer for the OG file format
 *
 * \author  responsible: Jan Bretschneider <bretschn@informatik.hu-berlin.de>
 */

/* DEFINITIONS */

/* flex options */
%option outfile="lex.yy.c"
%option prefix="og_yy"
%option noyywrap
%option yylineno
%option nodefault
%option debug

%s COMMENT

%{
// c-code (wird übernommen)

// We don't need yyunput().
#define YY_NO_UNPUT

//#include "debug.h"
#include "AnnotatedGraph.h"
#include "syntax_og.h"            // list of all tokens used
#include <cstring>

static void setlval();
%}


%%
 /* RULES */

"{"            { BEGIN(COMMENT); }
<COMMENT>"}"   { BEGIN(INITIAL); }
<COMMENT>[^}]* {                 }

NODES         { return key_nodes;       }
INITIALNODE   { return key_initialnode; }
TRANSITIONS   { return key_transitions; }

INTERFACE     { return key_interface;   }
INPUT         { return key_input;       }
OUTPUT        { return key_output;      }

 /* We want to match 'true' and 'false' case insensitively, but at least my
  * flex version did not accept the rule
  *     (?i:TRUE)  { return key_true; }
  * for case insensitive matching. So we have to do it by hand.
  */
[Tt][Rr][Uu][Ee]      { return key_true;  }
[Ff][Aa][Ll][Ss][Ee]  { return key_false; }
\*            { return op_and;    }
\+            { return op_or;     }
\(            { return lpar;      }
\)            { return rpar;      }

[Rr][Ee][Dd]      { return key_red;  }
[Bb][Ll][Uu][Ee]  { return key_blue; }

[Ff][Ii][Nn][Aa][Ll][Nn][Oo][Dd][Ee]  { return key_finalnode; }

\:            { return colon;     }
\;            { return semicolon; }
,             { return comma;     }
->            { return arrow;     }

[^,;:()\t \n\r\{\}=]+  { setlval(); return ident;  }

[\n\r]        { break; }
[ \t]         { break; }
.             { og_yyerror("lexical error"); }


%%
// USER CODE

// pass token string as attribute to bison
static void setlval() {
  og_yylval.str = strdup(og_yytext);
}




