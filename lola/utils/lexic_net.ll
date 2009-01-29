%option noyywrap
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="net_"

%{

#include <cstring>
#include "syntax_net.h"

void net_error(const char *msg);

%}


name			[^,;:\-()\t \n\{\}][^,;:\-()\t \n\{\}]*
number			[0-9][0-9]*


%%


"PLACE"			{ return KW_PLACE; }
"TRANSITION"		{ return KW_TRANSITION; }
"MARKING"		{ return KW_MARKING; }
"CONSUME"		{ return KW_CONSUME; }
"PRODUCE"		{ return KW_PRODUCE; }

";"			{ return SEMICOLON; }
":"			{ return COLON; }
","			{ return COMMA; }

"{"[^\n]*"}"     	{ break; }

{number}		{ net_lval.val = atoi(yytext); return NUMBER; }
{name} 			{ char *temp = (char *)malloc(strlen(yytext) * sizeof(char));
                          strcpy(temp, yytext);
                          net_lval.name = temp; return NAME; }


[ \t\r\n]*		{ /* skip */ }
<<EOF>>			{ return EOF; }
.			{ net_error("lexical error"); }

%%

void net_error(const char *msg)
{
  fprintf(stderr, "net %d: %s\n", net_lineno, msg);
}
