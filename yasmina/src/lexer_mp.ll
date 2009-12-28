/* flex options */
%option outfile="lex.yy.c"
%option prefix="mp_yy"
%option nounput
%option noyywrap
%option yylineno
%option nodefault

/* a start condition to skip comments */
%s COMMENT

%{
#define YY_NO_UNPUT         // We don't need yyunput().
#include "eventTerm.h"
#include "syntax_mp.h"      // list of all tokens used
#include "config.h"
#include "limits.h"

using std::string;
extern int mp_yyerror(const char* msg);
%}





%%

 


"FINALMARKINGS"                    { return KEY_FINALMARKING; }
"PLACE"                           { return KEY_PLACE; }
"INTERNAL"                        { return KEY_INTERNAL; }
"INPUT"                           { return KEY_INPUT; }
"OUTPUT"                          { return KEY_OUTPUT; }
"SYNCHRONOUS"			{return KEY_SYNCHRONOUS;}
"TERMS"					{ return KEY_TERMS; }
"BOUNDS"			{ return KEY_BOUNDS; }
"unbounded"			{ return MUNBOUNDED;}


"-"                             { return MINUS; }
\(                              { return LPAR; }
\)                              { return RPAR; }
[0-9][0-9]*			{ mp_yylval.yt_int = atoi(yytext); return NUMBER; }
[\n\r]                          { /* skip */ }
[ \t]                           { /* skip */ }
\;								{ return SEMICOLON; }
\+								{ return ADD;}
\*								{ return MULT;}
":"								{ return COLON; }
","								{ return COMMA; }
"="								{ return MEQ; }

[F][0-9][0-9]*			{mp_yylval.yt_string = new std::string(yytext); return FIDENT; }
[T][0-9][0-9]*			{mp_yylval.yt_string = new std::string(yytext); return CIDENT; }

[^,\-\*\+;:=()\t \n\r\{\}][^,\-\+\*;:=()\t \n\r\{\}]* { 
            mp_yylval.yt_string = new std::string(yytext); return IDENT; }

 /* anything else */
.                               { mp_yyerror("unexpected lexical token"); }

%%