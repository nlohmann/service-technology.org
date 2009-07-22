%option noyywrap
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="chor_"

%{
#include <cstring>
#include <string>
#include "syntax_chor.h"
#include "verbose.h"

extern std::string NAME_token;

void chor_error(const char *);
%}

name      [^!?#,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    "-"?[0-9][0-9]*


%%

"PEER"       { return KEY_PEER; }
"NODES"      { return KEY_NODES; }
"INITIAL"    { return KEY_INITIAL; }
"FINAL"      { return KEY_FINAL; }
"IN"         { return KEY_IN; }
"OUT"        { return KEY_OUT; }

";"          { return SEMICOLON; }
","          { return COMMA; }
":"          { return COLON; }
"->"         { return ARROW; }

{number}     { chor_lval.val = atoi(chor_text); return NUMBER; }
"#"{name}    { NAME_token = chor_text; return SYNCLABEL; }
"!"{name}    { NAME_token = chor_text; return SENDLABEL; }
"?"{name}    { NAME_token = chor_text; return RCVLABEL; }
{name}       { NAME_token = chor_text; return NAME; }

[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { chor_error("lexical error"); }

%%

void chor_error(const char *msg) {
  status("%d: error near '%s': %s", chor_lineno, chor_text, msg);
  abort(3, "error while parsing the choreography");
}
