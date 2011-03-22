%option noyywrap
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="reducedgraph_"

%{
#include <cstring>
#include "syntax_reducedgraph.h"

void reducedgraph_error(const char *);
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
"SCC:"                  { return KW_SCC; }
"Prog:"                 { return KW_PROG; }
"Lowlink:"              { return KW_LOWLINK; }
"!"                     { /* skip */ }
"*"                     { /* skip */ }
"?"                     { return QUESTION; }
":"                     { return COLON; }
","                     { return COMMA; }
"->"                    { return ARROW; }
"=>"                    { return PATHARROW; }

{number}  { //char *temp = (char *)malloc((strlen(graph_text)+1) * sizeof(char));
            //strcpy(temp, graph_text);
            reducedgraph_lval.val = atoi(reducedgraph_text); return NUMBER; }

{name}    { char *temp = (char *)malloc((strlen(reducedgraph_text)+1) * sizeof(char));
            strcpy(temp, reducedgraph_text);
            reducedgraph_lval.name = temp; return NAME; }


[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { reducedgraph_error("lexical error"); }

%%

void reducedgraph_error(const char *msg) {
  fprintf(stderr, "%d: %s - last token '%s'\n", reducedgraph_lineno, msg, reducedgraph_text);
}
