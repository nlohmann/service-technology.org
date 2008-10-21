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

/* flex options */
%option outfile="lex.yy.c"
%option prefix="stg_yy"
%option noyywrap
%option yylineno
%option nodefault

%s COMMENT


%{
#include <cstring>
#include <string>
#include "syntax_stg.h"            // list of all tokens used

using std::string;

extern int stg_yyerror(const char*);
%}


namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9/.:]
number			[0-9]+
placename		"p"{number}
name			{namestart}{namechar}*
transitionname1		"t"{number}("/"{number})?
transitionname2		"out."{name}
transitionname3		"in."{name}


%%
 /* RULES */

"#"             { BEGIN(COMMENT); }
<COMMENT>[\n\r] { BEGIN(INITIAL); }
<COMMENT>[^\n]* {                 }


".model"	{ return K_MODEL; }
".dummy"	{ return K_DUMMY; }
".graph"	{ return K_GRAPH; }
".marking"	{ return K_MARKING; }
".end"		{ return K_END; }

"{"		{ return OPENBRACE; }
"}"		{ return CLOSEBRACE; }

{placename}		{ stg_yylval.str = strdup(stg_yytext);
                          return PLACENAME; }

{transitionname1}	{ stg_yylval.str = strdup(stg_yytext);
                          return TRANSITIONNAME; }

{transitionname2}	{ stg_yylval.str = strdup(stg_yytext);
                          return TRANSITIONNAME; }

{transitionname3}	{ stg_yylval.str = strdup(stg_yytext);
                          return TRANSITIONNAME; }

"finalize"		{ stg_yylval.str = strdup(stg_yytext);
                          return TRANSITIONNAME; }

{name}			{ return IDENTIFIER; }

[\n\r]        { return NEWLINE; }

[ \t]         { break; }

.             { stg_yyerror("lexical error"); }
