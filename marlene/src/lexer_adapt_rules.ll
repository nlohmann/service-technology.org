/*****************************************************************************\
 Copyright (c) 2009  Christian Gierds

 This file is part of Marlene, see <http://service-technology.org/marlene>.

#warning "LICENSE"

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

#include "parser_adapt_rules.h"            // list of all tokens used

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
{name}	{ adapt_rules_yylval.str = strdup(adapt_rules_yytext); return NAME; }
"->"    { return ARROW; }
","     { return COMMA; }
";"     { return SEMICOLON; }


 /* whitespaces */
[ \t\n\r]         { break; /* skip */ }

.             { adapt_rules_yyerror("lexical error"); }

