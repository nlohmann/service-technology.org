/* flex options */
%option outfile="lex.yy.c"
%option prefix="adapt_rules_yy"
%option noyywrap
%option yylineno
%option nodefault

%{
#include <cstring>
#include <string>
#include "syntax_adapt_rules.h"            // list of all tokens used

using std::string;

extern int adapt_rules_yyerror(const char*);
%}


namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9/.:]
name			{namestart}{namechar}*


%%
 /* RULES */

{name}	{ adapt_rules_yylval.str = strdup(adapt_rules_yytext);
            return NAME; }
"->"    { return ARROW; }
","     { return COMMA; }
";"     { return SEMICOLON; }

[ \t\n\r]         { break; /* ignore whitespaces */ }

.             { adapt_rules_yyerror("lexical error"); }

