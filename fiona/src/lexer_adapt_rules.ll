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
%option prefix="adapt_rules_yy"
%option noyywrap
%option yylineno
%option nodefault

/* a start condition to skip comments */
%s COMMENT

%{
#include <cstring>
#include <string>
#include "syntax_adapt_rules.hh"            // list of all tokens used

using std::string;

extern int adapt_rules_yyerror(const char*);
%}


namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9/.:]
name			{namestart}{namechar}*


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
{name}	     { adapt_rules_yylval.str = strdup(adapt_rules_yytext); return NAME; }
[0-9][0-9]*  { adapt_rules_yylval.str = strdup(adapt_rules_yytext); return NUMBER; }
"--"         { return ARROW_BEGIN; }
"->"         { return ARROW_END; }
","          { return COMMA; }
";"          { return SEMICOLON; }


 /* whitespaces */
[ \t\n\r]         { break; /* skip */ }

.             { adapt_rules_yyerror("lexical error"); }

