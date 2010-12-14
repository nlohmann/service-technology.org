%option noyywrap
%option nounput
%option full
%option outfile="lex.yy.c"
%option prefix="rg_yy"

%{
#include <cstring>
#include "rg-syntax.h"
#include "verbose.h"

void rg_yyerror(const char*);
%}

name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    [0-9][0-9]*

%%

"STATE"      { return KW_STATE; }
"Lowlink:"   { return KW_LOWLINK; }
"SCC:"       { return KW_SCC; }
":"          { return COLON; }
","          { return COMMA; }
"->"         { return ARROW; }

{number}     { rg_yylval.val = atoi(rg_yytext); return NUMBER; }
{name}       { rg_yylval.str = strdup(rg_yytext); return NAME; }

[ \t\r\n]*   { /* skip */ }

%%

__attribute__((noreturn)) void rg_yyerror(const char* msg) {
  status("error near '%s': %s", rg_yytext, msg);
  abort(6, "error while parsing the reachability graph");
}
