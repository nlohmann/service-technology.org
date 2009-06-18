/* flex options */
%option outfile="lex.yy.c"
%option prefix="et_yy"
%option noyywrap
%option yylineno
%option nodefault
%option debug

%{
#define YY_NO_UNPUT         // We don't need yyunput().
#include "eventTerm.h"
#include <string>
#include "syntax_et.h"      // list of all tokens used

extern int et_yyerror(const char *msg);
%}



%%



\(                              { return LPAR; }
\)                              { return RPAR; }
"-"?[0-9][0-9]*					{ et_yylval.yt_int = atoi(yytext); return NUMBER; }
[\n\r]                          { /* skip */ }
[ \t]                           { /* skip */ }
\;								{ return NEW_TERM; }
\+								{ return ADD;}
\*								{ return MULT;}
:q								{ return QUIT;}
[^,\-\*\+;:()\t \n\r\{\}=][^,\-\+\*;:()\t \n\r\{\}=]* { 
            et_yylval.yt_string = new std::string(yytext); return IDENT; }

 /* anything else */
.                               { et_yyerror("unexpected lexical token"); }

%%