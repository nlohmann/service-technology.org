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
%option prefix="adapt_rules_yy"
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

typedef std::list< unsigned int > uilist;

#include "parser_adapt_rulesmarlene.h"            // list of all tokens used

using std::string;

extern int adapt_rules_yyerror(const char*);
%}


namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9/.:]
name			{namestart}{namechar}*

valuestart              [1-9]
valueproceed            [0-9]

%%

 /* keywords */
HIDDEN       { return RULE_HIDDEN;       }
OBSERVABLE   { return RULE_OBSERVABLE;   }
CONTROLLABLE { return RULE_CONTROLLABLE; }
 /* rules for total communication flow don't need an extra keyword because */
 /* they are the standard rules */
 /* TOTAL        { return RULE_TOTAL;        } */

 /* comments */
"#"                             { BEGIN(COMMENT); }
<COMMENT>[\n\r]                 { BEGIN(INITIAL); }
<COMMENT>[^\n\r]*               { /* skip */ }

 /* identifiers and other characters */
{name}	{ adapt_rules_yylval.str = strdup(adapt_rules_yytext); return NAME; }

{valuestart}                                    { adapt_rules_yylval.ival = atoi(adapt_rules_yytext); return VALUE; }
{valuestart}\.{valueproceed}{valueproceed}*     { adapt_rules_yylval.ival = atoi(adapt_rules_yytext); return VALUE; }
{valuestart}{valueproceed}*                                     { adapt_rules_yylval.ival = atoi(adapt_rules_yytext); return VALUE; }
{valuestart}{valueproceed}*\.{valueproceed}{valueproceed}*      { adapt_rules_yylval.ival = atoi(adapt_rules_yytext); return VALUE; }

"->"    { return ARROW; }
"--"    { return DMINUS; }
","     { return COMMA; }
";"     { return SEMICOLON; }

"("     { return LBRACE; }
")"     { return RBRACE; }


 /* whitespaces */
[ \t\n\r]         { break; /* skip */ }

.             { adapt_rules_yyerror("lexical error"); }
