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
 * \file    lexer_covog.cc
 *
 * \brief   the lexer for the file format of an OG with global constraint
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de>
 *
 */

/* DEFINITIONS */

/* flex options */
%option outfile="lex.yy.c"
%option prefix="covog_yy"
%option noyywrap
%option yylineno
%option nodefault
%option debug

%s COMMENT

%{
// c-code (wird übernommen)

// We don't need yyunput().
#define YY_NO_UNPUT

#include "debug.h"
#include "AnnotatedGraph.h"
#include "syntax_covog.h"            // list of all tokens used
#include <cstring>
using namespace std;

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
COVER         { return key_cover; }

"Places to cover"        { return key_placestocover; }
"Transitions to cover"   { return key_transitionstocover; } 
"Global Constraint"      { return key_globalconstraint; }
none                     { return key_none; }

[Tt][Rr][Uu][Ee]      { return key_true;  }
[Ff][Aa][Ll][Ss][Ee]  { return key_false; }

\*            { return op_and;    }
\+            { return op_or;     }
\(            { return lpar;      }
\)            { return rpar;      }

[Rr][Ee][Dd]      { return key_red;  }
[Bb][Ll][Uu][Ee]  { return key_blue; }

\:            { return colon;     }
\;            { return semicolon; }
,             { return comma;     }
->            { return arrow;     }

[^,;:()\t \n\r\{\}=]+  { setlval(); return ident;  }

[\n\r]        { break; }
[ \t]         { break; }
.             { covog_yyerror("lexical error"); }


%%
// USER CODE

// pass token string as attribute to bison
static void setlval() {
  covog_yylval.str = strdup(covog_yytext);
}




