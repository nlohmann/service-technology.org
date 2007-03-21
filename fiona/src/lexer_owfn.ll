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

%s COMMENT

%{
// c-code (wird übernommen)

// We don't need yyunput().
#define YY_NO_UNPUT

#include "syntax_owfn_wrap.h"			// list of all tokens used
#include <string>

using namespace std;

extern int owfn_yyerror(const char *msg);
static void setlval();

%}


%%
 /* RULES */

"{$"		{ return lcontrol; }
"$}"		{ return rcontrol; }

"{"            { BEGIN(COMMENT); }
<COMMENT>"}"   { BEGIN(INITIAL); }
<COMMENT>[^}]* {}

MAX_UNIQUE_EVENTS		{ return key_max_unique_events; }
ON_LOOP				{ return key_on_loop; }
MAX_OCCURRENCES			{ return key_max_occurrences; }
TRUE				{ return key_true; }
FALSE				{ return key_false; }

SAFE             						{ return key_safe;}
PLACE            						{ return key_place; }
INTERNAL		 						{ return key_internal; }
INPUT			 						{ return key_input; }
OUTPUT			 						{ return key_output; }
TRANSITION       						{ return key_transition; }
INITIALMARKING          				{ return key_marking; }
FINALMARKING          					{ return key_finalmarking; }
FINALCONDITION     						{ return key_finalcondition; }
CONSUME          						{ return key_consume; }
PRODUCE          						{ return key_produce; }
ALL_OTHER_PLACES_EMPTY           { return key_all_other_places_empty; }
ALL_OTHER_INTERNAL_PLACES_EMPTY  { return key_all_other_internal_places_empty; }
ALL_OTHER_EXTERNAL_PLACES_EMPTY  { return key_all_other_external_places_empty; }
AND		 								{ return op_and;}
OR		 								{ return op_or;}
NOT		 								{ return op_not;}
\>		 								{ return op_gt;}
\<		 								{ return op_lt;}
\>=		 								{ return op_ge;}
\<=		 								{ return op_le;}
=		 								{ return op_eq;}
\<\>									{ return op_ne;}
\#		 								{ return op_ne;}
\:              						{ return colon; }
\;              						{ return semicolon; }
,               						{ return comma; }
\(		 								{ return lpar;}
\)		 								{ return rpar;}
[0-9][0-9]*     						{ setlval(); return number; }
"-"[0-9][0-9]*     						{ setlval(); return negative_number; }
[^,;:()\t \n\r\{\}=][^,;:()\t \n\r\{\}=]*		{ setlval(); return ident; }
[\n\r]            						{ break; }
[ \t]           						{ break; }
.										{ owfn_yyerror("lexical error"); }


%%
// USER CODE

// pass token string as attribute to bison
static void setlval() {
  owfn_yylval.str = strdup(owfn_yytext);
}




