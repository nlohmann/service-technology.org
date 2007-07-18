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

static void setlval();
%}


namestart		[A-Za-z\200-\377_]
namechar		[A-Za-z\200-\377_0-9/.:]
number			[0-9]+
placename		"p"{number}
name			{namestart}{namechar}*
transitionname1		"out."{name}
transitionname2		"in."{name}


%%
 /* RULES */

"#"            { BEGIN(COMMENT); }
<COMMENT>[\n\r]   { BEGIN(INITIAL); }
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
"finalize"		{ stg_yylval.str = strdup(stg_yytext);
                          return TRANSITIONNAME; }
{name}			{ return IDENTIFIER; }

[\n\r]        { return NEWLINE; }
[ \t]         { break; }
.             { stg_yyerror("lexical error"); }
