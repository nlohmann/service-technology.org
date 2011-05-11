%option noyywrap
%option yylineno
%option nodefault

%{
#include <cstring>
#include "form-syntax.h"

void yyerror(const char *);
%}


name      [a-zA-Z_][a-zA-Z0-9_]*
number    [0-9][0-9]*

%%

","          { return COMMA; }
"="	{ return EQSIGN; }
"("	{ return POPEN; }
")"	{ return PCLOSE; }
"["	{ return BROPEN; }
"]"	{ return BRCLOSE; }
"{"	{ return BOPEN; }
"}"	{ return BCLOSE; }
"*"	{ return STARSIGN; }
"."	{ return DOTSIGN; }
"reachable"	{ return REACHABLE; }
"invariant"	{ return INVARIANT; }
"equals"	{ return EQUALS; }
"contains"	{ return CONTAINS; }
"not"	{ return NOTSIGN; }
"and"	{ return ANDSIGN; }
"or"	{ return ORSIGN; }
"NOT"	{ return NOTSIGN; }
"AND"	{ return ANDSIGN; }
"OR"	{ return ORSIGN; }

{number}     { yylval.val = atoi(yytext); return NUMBER; }

{name}       { yylval.str = strdup(yytext); return NAME; }

[ \t\r]*   { /* skip */ }
\n	{ return EOL; }
<<EOF>>      { return EOF; }
.            { yyerror("lexical error"); }

%%

__attribute__((noreturn)) void yyerror(const char *msg) {
  fprintf(stderr, "%d: error near '%s': %s\n", yylineno, yytext, msg);
  exit(1);
}

