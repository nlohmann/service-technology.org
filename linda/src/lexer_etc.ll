/* flex options */
%option outfile="lex.yy.c"
%option prefix="etc_yy"
%option noyywrap
%option yylineno
%option nodefault
%option debug

%{
#define YY_NO_UNPUT         // We don't need yyunput().
#include "eventTerm.h"
#include <string>
#include "syntax_etc.h"      // list of all tokens used

extern int etc_yyerror(const char *msg);
%}



%%


"-"                             { return MINUS; }
\(                              { return LPAR; }
\)                              { return RPAR; }
[0-9][0-9]*					{ etc_yylval.yt_int = atoi(yytext); return NUMBER; }
[\n\r]                          { /* skip */ }
[ \t]                           { /* skip */ }
\;								{ return NEW_TERM; }
\+								{ return ADD;}
\*								{ return MULT;}
:q								{ return QUIT;}
\>                              { return GT;}
\<                              { return LT;}
=                              { return EQUALS;}

[^,\-\*\+;:()\t \n\r\{\}][^,\-\+\*;:()\t \n\r\{\}]* { 
            etc_yylval.yt_string = new std::string(yytext); return IDENT; }

 /* anything else */
.                               { etc_yyerror("unexpected lexical token"); }

%%