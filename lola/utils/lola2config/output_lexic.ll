%option noyywrap
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="output_"

%{
#include <cstring>
#include "output_syntax.h"

void output_error(const char *);
%}

comment   "{"[^\}]*"}"
name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    "-"?[0-9][0-9]*


%%

{comment}               { /* skip */ }

"computing symmetries..." { return KW_COMPUTING_SYMM; }
"Places"                { return KW_PLACES; }
"Transitions"           { return KW_TRANSITIONS; }
"significant places"    { return KW_SIGNIFICATPLACES; }

"generators in "        { return KW_GENERATORS; }
"groups for "           { return KW_GROUPS; }
"symmetries found."     { return KW_SYMMETRIES; }
"dead branches entered during calculation." { return KW_DEADBRANCHES; }


"net does not have deadlocks!" { return MSG_NODEADLOCK; }
"dead state found!"     { return MSG_DEADLOCK; }
"net is bounded!"       { return MSG_BOUNDED; }
"net is unbounded!"     { return MSG_UNBOUNDED; }
    
">>>>>"                 { return MARKER; }

"States"                { return KW_STATES;}
"Edges"                 { return KW_EDGES; }
"Hash table entries"    { return KW_HASHTABLEENTRIES; }

"GENERATOR"             { return KW_GENERATOR; }
"PATH EXPRESSION"       { return KW_PATH_EXPRESSION; }
"PATH"                  { return KW_PATH; }
"STATE"                 { return KW_STATE; }
"Prog:"                 { return KW_PROG; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"!"                     { return BANG; }
"*"                     { return STAR; }
"?"                     { return QUESTION; }
"#"                     { return HASH; }
":"                     { return COLON; }
","                     { return COMMA; }
"."                     { return DOT; }
"->"                    { return ARROW; }
"=>"                    { return PATHARROW; }

"oo"      { output_lval.val = -1; return NUMBER; }

{number}  { char *temp = (char *)malloc((strlen(output_text)+1) * sizeof(char));
            strcpy(temp, output_text);
            output_lval.val = atoi(temp); return NUMBER; }

{name}    { char *temp = (char *)malloc((strlen(output_text)+1) * sizeof(char));
            strcpy(temp, output_text);
            output_lval.name = temp; return NAME; }


[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { output_error("lexical error"); }

%%

void output_error(const char *msg) {
  fprintf(stderr, "%d: error near '%s': %s\n", output_lineno, output_text, msg);
}
