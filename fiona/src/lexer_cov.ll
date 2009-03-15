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
 * \file    lexer_cov.ll
 *
 * \brief   the lexer for the file format specifying the set of open net nodes
 *          to be covered
 *          (needed for operating guideline with global constraint)
 *
 * \author  responsible: Robert Danitz <danitz@informatik.hu-berlin.de>
 *
 */

/* DEFINITIONS */

/* flex options */
%option outfile="lex.yy.c"
%option prefix="cov_yy"
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
#include "syntax_cov.h"            // list of all tokens used
#include <cstring>
using namespace std;

static void setlval();
%}


%%
 /* RULES */

"{"            { BEGIN(COMMENT); }
<COMMENT>"}"   { BEGIN(INITIAL); }
<COMMENT>[^}]* {                 }

PLACES        { return key_places;      }
TRANSITIONS   { return key_transitions; }

\;            { return semicolon; }
,             { return comma;     }

[^,;:()\t \n\r\{\}=]+  { setlval(); return ident;  }

[\n\r]        { break; }
[ \t]         { break; }
.             { cov_yyerror("lexical error"); }


%%
// USER CODE

// pass token string as attribute to bison
static void setlval() {
  cov_yylval.str = strdup(cov_yytext);
}




