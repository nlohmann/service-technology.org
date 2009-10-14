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


/***************************************************************************** 
 * flex options 
 ****************************************************************************/

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="pnapi_pn_yy"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno


/***************************************************************************** 
  * C declarations 
  ****************************************************************************/
%{

#include "parser.h"
#include "parser-pn.h"

#include <cstring>

#define yystream pnapi::parser::stream
#define yylineno pnapi::parser::line
#define yytext   pnapi::parser::token
#define yyerror  pnapi::parser::error

#define yylex    pnapi::parser::pn::lex
#define yylex_destroy pnapi::parser::pn::lex_destroy

/* hack to read input from a C++ stream */
#define YY_INPUT(buf,result,max_size)		\
   yystream->read(buf, max_size); \
   if (yystream->bad()) \
     YY_FATAL_ERROR("input in flex scanner failed"); \
   result = yystream->gcount();

/* hack to overwrite YY_FATAL_ERROR behavior */
#define fprintf(file,fmt,msg) \
   yyerror(msg);

%}

%s COMMENT
%s CAPACITY
%s ARCWEIGHT
%s G_OUTPUT

namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9/.:]
number			[0-9]+
placename		"p"{number}
name			{namestart}{namechar}*
transitionname1		"t"{number}([_/]{number})?
transitionname2		"out."{name}
transitionname3		"in."{name}

%%

 /* RULES */

"#"                            { BEGIN(COMMENT); }
<COMMENT>[\n\r]                { BEGIN(INITIAL); }
<COMMENT>[^\n]*                { /* ignore */    }

".capacity"                    { BEGIN(CAPACITY); }
<CAPACITY>[\n\r]               { BEGIN(INITIAL); }
<CAPACITY>[^\n]*               { /* ignore */    }

".model"                       { return K_MODEL; }
".dummy"                       { return K_DUMMY; }
".graph"                       { return K_GRAPH; }
".marking"                     { return K_MARKING; }
".end"                         { return K_END; }

".outputs"                     { BEGIN(G_OUTPUT); return K_OUTPUTS; }
<G_OUTPUT>{transitionname1}    { /* ignore */ }
<G_OUTPUT>[\n\r]               { BEGIN(INITIAL); return NEWLINE; }

"{"                            { return OPENBRACE; }
"}"                            { return CLOSEBRACE; }

"("                            { BEGIN(ARCWEIGHT); return LPAR; }
<ARCWEIGHT>{number}            { BEGIN(INITIAL); pnapi_pn_yylval.yt_uInt = atoi(pnapi_pn_yytext); return WEIGHT; }
")"                            { return RPAR; }

{placename}	               { pnapi_pn_yylval.yt_str = strdup(pnapi_pn_yytext); return PLACENAME; }

{transitionname1}	       { pnapi_pn_yylval.yt_str = strdup(pnapi_pn_yytext); return TRANSITIONNAME; }

{transitionname2}	       { pnapi_pn_yylval.yt_str = strdup(pnapi_pn_yytext); return TRANSITIONNAME; }

{transitionname3}	       { pnapi_pn_yylval.yt_str = strdup(pnapi_pn_yytext); return TRANSITIONNAME; }

"finalize"		       { pnapi_pn_yylval.yt_str = strdup(pnapi_pn_yytext); return TRANSITIONNAME; }

{name}		               { return IDENTIFIER; }

[\n\r]                         { return NEWLINE; }

[ \t]                          { /* ingore */ }

.                              { yyerror("unexpected lexical token"); }
