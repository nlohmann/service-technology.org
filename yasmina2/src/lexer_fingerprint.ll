/* flex options */
%option outfile="lex.yy.c"
%option prefix="fingerprint_yy"
%option noyywrap
%option yylineno
%option nodefault

%{
#define YY_NO_UNPUT         // We don't need yyunput().
#include <string>
#include "helpers.h"
#include "syntax_fingerprint.h"      // list of all tokens used

extern int fingerprint_yyerror(const char *msg);
%}



%%

"PLACE"                         {return KW_PLACE;}
"INTERNAL"                      {return KW_INTERNAL;}
"INPUT"                         {return KW_INPUT;}
"OUTPUT"                        {return KW_OUTPUT;}
"FINALMARKINGS"                 {return KW_FINALMARKINGS;}
"TERMS"                         {return KW_TERMS;}
"BOUNDS"                        {return KW_BOUNDS;}
"unbounded"                        {return KW_UNBOUNDED;}
[T][0-9][0-9]* { 
            fingerprint_yylval.yt_int = atoi(std::string(yytext).substr(1).c_str());  return TERM_IDENTIFIER; }
[F][0-9][0-9]* { 
            fingerprint_yylval.yt_int = atoi(std::string(yytext).substr(1).c_str()); return FINALMARKING_IDENTIFIER; }


[a-zA-Z][^,\-\+\*;:()\t \n\r\{\}=]* { 
            fingerprint_yylval.yt_string = new std::string(yytext); return IDENT; }
[0-9][0-9]*     		{ fingerprint_yylval.yt_int = atoi(yytext); return VALUE; }
"+"                             { return PLUS; }
"-"   		                      { return MINUS; }
"*"   		                      { return TIMES; }

","                             { return COMMA; }
";"                             { return SEMICOLON; }
":"                             { return COLON; }
"="                             { return EQUALS; }
[\n\r]                          { /* skip */ }
[ \t]                           { /* skip */ }


 /* anything else */
.                               { fingerprint_yyerror("unexpected lexical token in profile file"); }

%%
