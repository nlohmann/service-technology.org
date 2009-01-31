%option noyywrap
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="graph_"

%{
#include <cstring>
#include "syntax_graph.h"

void graph_error(const char *);
%}

name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    "-"?[0-9][0-9]*


%%

{number}" Places"               { /* skip */ }
{number}" Transitions"          { /* skip */ }
{number}" significant places"   { /* skip */ }
"dead state found!"             { /* skip */ }
">>>>> "{number}" States, "{number}" Edges, "{number}" Hash table entries" { /* skip */ }

"STATE"                 { return KW_STATE; }
"Prog:"                 { return KW_PROG; }
"!"                     { return BANG; }
"*"                     { return STAR; }
"?"                     { return QUESTION; }
":"                     { return COLON; }
","                     { return COMMA; }
"->"                    { return ARROW; }
"=>"                    { return PATHARROW; }

{number}  { char *temp = (char *)malloc((strlen(graph_text)+1) * sizeof(char));
            strcpy(temp, graph_text);
            graph_lval.val = atoi(temp); return NUMBER; }

{name}    { char *temp = (char *)malloc((strlen(graph_text)+1) * sizeof(char));
            strcpy(temp, graph_text);
            graph_lval.name = temp; return NAME; }


[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { graph_error("lexical error"); }

%%

void graph_error(const char *msg) {
  fprintf(stderr, "%d: %s\n", graph_lineno, msg);
}
