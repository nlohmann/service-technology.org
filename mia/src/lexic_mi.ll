%option noyywrap
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="mi_"

%{
#include <cstring>
#include <string>
#include "syntax_mi.h"
#include "cmdline.h"

extern std::string NAME_token;
extern gengetopt_args_info args_info;
extern void abort(unsigned int code, const char* format, ...);

void mi_error(const char *);
%}

name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number    "-"?[0-9][0-9]*


%%

":"          { return COLON; }
","          { return COMMA; }

{number}     { mi_lval.val = atoi(mi_text); return NUMBER; }

{name}       { NAME_token = mi_text; return NAME; }

[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { mi_error("lexical error"); }

%%

void mi_error(const char *msg) {
  if (args_info.verbose_flag) {
      fprintf(stderr, "%s:%d: error near '%s': %s\n", PACKAGE, mi_lineno, mi_text, msg);
  }
  abort(10, "error while parsing the marking information");
}
