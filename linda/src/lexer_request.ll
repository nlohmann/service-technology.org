/* flex options */
%option outfile="lex.yy.c"
%option prefix="request_yy"
%option noyywrap
%option yylineno
%option nodefault

%{
#define YY_NO_UNPUT         // We don't need yyunput().
#include <string>
#include "costs.h"
#include "syntax_request.hh"      // list of all tokens used

extern int request_yyerror(const char *msg);
%}



%%


"ASSERTIONS"                    { return KEY_ASSERTIONS; }
"CONSTRAINT"                    { return KEY_CONSTRAINT; }
"GRANT"                         { return KEY_GRANT; }
"PAYMENT"                       { return KEY_PAYMENT; }

"OR"                            { return OP_OR; }
"AND"                           { return OP_AND; }

[a-zA-Z][^,\-\+\*;:()\t \n\r\{\}]* { 
            request_yylval.yt_string = new std::string(yytext); return IDENT; }
[0-9][0-9]*     		{ request_yylval.yt_int = atoi(yytext); return VALUE; }
"+"[1-9][0-9]*     		{ request_yylval.yt_int = atoi(yytext); return VALUE; }
"-"[1-9][0-9]*     		{ request_yylval.yt_int = atoi(yytext); return VALUE; }

"<="                            { return OP_LE; }
">="                            { return OP_GE; }
"="                             { return OP_EQ; }
"+"                             { return OP_PLUS; }
"-"                             { return OP_MINUS; }
"*"                             { return OP_TIMES; }
"/"                             { return OP_SLASH; }

","                             { return COMMA; }
";"                             { return SEMMELKORN; }
":"                             { return COLON; }

[\n\r]                          { /* skip */ }
[ \t]                           { /* skip */ }


 /* anything else */
.                               { request_yyerror("unexpected lexical token in request file"); }

%%
