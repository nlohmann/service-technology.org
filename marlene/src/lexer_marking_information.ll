/*****************************************************************************\
 Marlene -- synthesizing behavioral adapters

 Copyright (C) 2009  Christian Gierds <gierds@informatik.hu-berlin.de>

 Marlene is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Marlene is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Marlene.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/

/* flex options */
%option outfile="lex.yy.c"
%option prefix="marking_information_yy"
%option nounput
%option noyywrap
%option yylineno
%option nodefault

/* a start condition to skip comments */
%s COMMENT

%{
#include <cstring>
#include <string>
#include <list>

#include "parser_marking_information.h"            // list of all tokens used

using std::string;

extern int marking_information_yyerror(const char*);
%}


namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9/.]
name			{namestart}{namechar}*

valuestart              [1-9]
valueproceed            [0-9]

%%

 /* no keywords */

 /* comments */
"#"                             { BEGIN(COMMENT); }
<COMMENT>[\n\r]                 { BEGIN(INITIAL); }
<COMMENT>[^\n\r]*               { /* skip */ }

 /* identifiers and other characters */
{name}	{ marking_information_yylval.str = strdup(marking_information_yytext); return NAME; }

"0"                             { marking_information_yylval.ival = atoi(marking_information_yytext); return VALUE; }
{valuestart}                    { marking_information_yylval.ival = atoi(marking_information_yytext); return VALUE; }
{valuestart}{valueproceed}*     { marking_information_yylval.ival = atoi(marking_information_yytext); return VALUE; }

","     { return COMMA; }
":"     { return COLON; }


 /* whitespaces */
[ \t\n\r]         { break; /* skip */ }

.             { marking_information_yyerror("lexical error"); }

