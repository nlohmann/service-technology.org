/* flex options */
%option outfile="lex.yy.c"
%option prefix="profile_yy"
%option noyywrap
%option yylineno
%option nodefault

%{
#define YY_NO_UNPUT         // We don't need yyunput().
#include <string>
#include "costs.h"
#include "syntax_profile.hh"      // list of all tokens used

extern int profile_yyerror(const char *msg);
%}



%%


"USECASE"                       { return KEY_USECASE; }
"CONSTRAINT"                    { return KEY_CONSTRAINT; }
"MARKING"                       { return KEY_MARKING; }
"COSTS"                         { return KEY_COSTS; }
"POLICY"                        { return KEY_POLICY; }
"AND"                           { return OP_AND; }

[a-zA-Z][^,\-\+\*;:()\t \n\r\{\}]* { 
            profile_yylval.yt_string = new std::string(yytext); return IDENT; }
[0-9][0-9]*     		{ profile_yylval.yt_int = atoi(yytext); return VALUE; }
"+"[0-9][0-9]*     		{ profile_yylval.yt_int = atoi(yytext); return VALUE; }
"-"[0-9][0-9]*     		{ profile_yylval.yt_int = atoi(yytext); return VALUE; }

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
.                               { profile_yyerror("unexpected lexical token in profile file"); }

%%
