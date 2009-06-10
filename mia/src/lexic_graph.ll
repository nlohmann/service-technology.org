%option noyywrap
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="graph_"

%{
#include <cstring>
#include <string>
#include "syntax_graph.h"
#include "cmdline.h"

extern std::string NAME_token;
extern gengetopt_args_info args_info;
extern void abort(unsigned int code, const char* format, ...);

void graph_error(const char *);
%}

name      [^,;:()\t \n\{\}.][^,;:()\t \n\{\}.]*
number    "-"?[0-9][0-9]*


%%

{number}" Places"               { /* skip */ }
{number}" Transitions"          { /* skip */ }
{number}" significant places"   { /* skip */ }
">>>>> "{number}" States, "{number}" Edges, "{number}" Hash table entries" { /* skip */ }

"STATE"      { return KW_STATE; }
"Prog:"      { return KW_PROG; }
":"          { return COLON; }
"."          { return DOT; }
","          { return COMMA; }
"->"         { return ARROW; }

{number}     { graph_lval.val = atoi(graph_text); return NUMBER; }
{name}       { NAME_token = graph_text; return NAME; }

[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { graph_error("lexical error"); }

%%

void graph_error(const char *msg) {
  if (args_info.verbose_flag) {
      fprintf(stderr, "%s:%d: error near '%s': %s\n", PACKAGE, graph_lineno, graph_text, msg);
  }
  abort(3, "error while parsing the reachability graph");
}
