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

/* create a c++ lexer */
%option c++

/* we provide out own class */
%option yyclass="pnapi::parser::pn::yy::Lexer"

/* we need to prefix its base class */
%option prefix="Pn"

/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno

/* get rid of isatty */
%option never-interactive

/***************************************************************************** 
  * C declarations 
  ****************************************************************************/
%{

#include "config.h"

#include "parser-pn-wrapper.h"

#include <iostream>

/* tokens are defined in a struct in a class */
typedef pnapi::parser::pn::yy::BisonParser::token tt;

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

".model"                       { return tt::K_MODEL; }
".dummy"                       { return tt::K_DUMMY; }
".graph"                       { return tt::K_GRAPH; }
".marking"                     { return tt::K_MARKING; }
".end"                         { return tt::K_END; }

".outputs"                     { BEGIN(G_OUTPUT); return tt::K_OUTPUTS; }
<G_OUTPUT>{transitionname1}    { /* ignore */ }
<G_OUTPUT>[\n\r]               { BEGIN(INITIAL); return tt::NEWLINE; }

"{"                            { return tt::OPENBRACE; }
"}"                            { return tt::CLOSEBRACE; }

"("                            { BEGIN(ARCWEIGHT); return tt::LPAR; }
<ARCWEIGHT>{number}            { BEGIN(INITIAL); yylval->yt_uInt = atoi(yytext); return tt::WEIGHT; }
")"                            { return tt::RPAR; }

{placename}	               { yylval->yt_str = strdup(yytext); return tt::PLACENAME; }

{transitionname1}	       { yylval->yt_str = strdup(yytext); return tt::TRANSITIONNAME; }

{transitionname2}	       { yylval->yt_str = strdup(yytext); return tt::TRANSITIONNAME; }

{transitionname3}	       { yylval->yt_str = strdup(yytext); return tt::TRANSITIONNAME; }

"finalize"		       { yylval->yt_str = strdup(yytext); return tt::TRANSITIONNAME; }

{name}		               { return tt::IDENTIFIER; }

[\n\r]                         { return tt::NEWLINE; }

[ \t]                          { /* ingore */ }

.                              { LexerError("unexpected lexical token"); }
