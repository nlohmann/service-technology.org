%option outfile="lex.yy.c"
%option prefix="graph_yy"
%option noyywrap
%option yylineno
%option nodefault
%option nounput

%{
#include <cstring>
#include <string>
#include "graph-syntax.h"
#include "verbose.h"

int graph_yyerror(const char *);
%}

name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    "-"?[0-9][0-9]*

%%

{number}" Places"               { /* skip */ }
{number}" Transitions"          { /* skip */ }
{number}" significant places"   { /* skip */ }
">>>>> "{number}" States, "{number}" Edges, "{number}" Hash table entries" { /* skip */ }

"STATE"      { return KW_STATE; }
"Lowlink:"   { return KW_LOWLINK; }
"SCC:"       { return KW_SCC; }
":"          { return COLON; }
","          { return COMMA; }
"->"         { return ARROW; }

{number}     { graph_yylval.value = atoi(graph_yytext); return NUMBER; }
{name}       { graph_yylval.str = graph_yytext; return NAME; }

[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { graph_yyerror("lexical error"); }

%%

int graph_yyerror(char const *msg) {
		status("%d: error near '%s': %s", graph_yylineno, graph_yytext, msg);
  	abort(6, "error while parsing the reachability graph");
}
