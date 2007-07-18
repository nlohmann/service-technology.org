/*****************************************************************************
 * Copyright 2005, 2006 Peter Massuthe, Daniela Weinberg, Karsten Wolf,      *
 *                      Jan Bretschneider, Christian Gierds                  *
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

/*!
 * \file    lexer_owfn.cc
 *
 * \brief   the lexer
 *
 * \author  responsible: Daniela Weinberg <weinberg@informatik.hu-berlin.de>
 *
 * \note    This file is part of the tool Fiona and was created during the
 *          project "Tools4BPEL" at the Humboldt-Universität zu Berlin. See
 *          http://www.informatik.hu-berlin.de/top/tools4bpel for details.
 *
 */

/* DEFINITIONS */

/* flex options */
%option outfile="lex.yy.c"
%option prefix="owfn_yy"
%option noyywrap
%option yylineno
%option nodefault
%option debug
%option always-interactive


%s COMMENT

%{
// c-code (wird übernommen)

// We don't need yyunput().
#define YY_NO_UNPUT

#include "ast-config.h"		// all you need from Kimwitu++
#include "syntax_owfn.h"			// list of all tokens used
#include <debug.h>
#include <string>

using namespace std;

extern int owfn_yyerror(const char *msg);
static void setlval();

 static void setlval() {
  owfn_yylval.yt_casestring = kc::mkcasestring(owfn_yytext);
}


%}


%%
 /* RULES */

"{$"		{ return LCONTROL; }
"$}"		{ return RCONTROL; }

"{"            { BEGIN(COMMENT); }
<COMMENT>"}"   { BEGIN(INITIAL); }
<COMMENT>[^}]* {}

MAX_UNIQUE_EVENTS		{ return KEY_MAX_UNIQUE_EVENTS; }
ON_LOOP				{ return KEY_ON_LOOP; }
MAX_OCCURRENCES			{ return KEY_MAX_OCCURRENCES; }
TRUE				{ return KEY_TRUE; }
FALSE				{ return KEY_FALSE; }

SAFE             						{ return KEY_SAFE;}
PLACE            						{ return KEY_PLACE; }
INTERNAL		 						{ return KEY_INTERNAL; }
INPUT			 						{ return KEY_INPUT; }
OUTPUT			 						{ return KEY_OUTPUT; }
TRANSITION       						{ return KEY_TRANSITION; }
INITIALMARKING          				{ return KEY_MARKING; }
FINALMARKING          					{ return KEY_FINALMARKING; }
FINALCONDITION     						{ return KEY_FINALCONDITION; }
CONSUME          						{ return KEY_CONSUME; }
PRODUCE          						{ return KEY_PRODUCE; }
ALL_OTHER_PLACES_EMPTY           { return KEY_ALL_OTHER_PLACES_EMPTY; }
ALL_OTHER_INTERNAL_PLACES_EMPTY  { return KEY_ALL_OTHER_INTERNAL_PLACES_EMPTY; }
ALL_OTHER_EXTERNAL_PLACES_EMPTY  { return KEY_ALL_OTHER_EXTERNAL_PLACES_EMPTY; }
AND		 								{ return OP_AND;}
OR		 								{ return OP_OR;}
NOT		 								{ return OP_NOT;}
\>		 								{ return OP_GT;}
\<		 								{ return OP_LT;}
\>=		 								{ return OP_GE;}
\<=		 								{ return OP_LE;}
=		 								{ return OP_EQ;}
\<\>									{ return OP_NE;}
\#		 								{ return OP_NE;}
\:              						{ return COLON; }
\;              						{ return SEMICOLON; }
,               						{ return COMMA; }
\(		 								{ return LPAR;}
\)		 								{ return RPAR;}
[0-9][0-9]*     						{ setlval(); return NUMBER; }
"-"[0-9][0-9]*     						{ setlval(); return NEGATIVE_NUMBER; }
[^,;:()\t \n\r\{\}=][^,;:()\t \n\r\{\}=]*		{ setlval(); return IDENT; }
[\n\r]            						{ break; }
[ \t]           						{ break; }
.										{ owfn_yyerror("lexical error"); }


%%
// USER CODE

// pass token string as attribute to bison




