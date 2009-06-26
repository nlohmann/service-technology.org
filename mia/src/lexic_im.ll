%option noyywrap
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="im_"

%{
#include <cstring>
#include <string>
#include "syntax_im.h"
#include "cmdline.h"

extern std::string NAME_token;
extern gengetopt_args_info args_info;
extern void status(const char* format, ...);
extern void abort(unsigned int code, const char* format, ...);

void im_error(const char *);
%}

name      [^,;:()\t \n\{\}\[\]][^,;:()\t \n\{\}\[\]]*
number    [0-9][0-9]*


%%

"INTERFACE"  { return KW_INTERFACE; }
","          { return COMMA; }
"["          { return LBRACKET; }
"]"          { return RBRACKET; }

{number}     { im_lval.val = atoi(im_text); return NUMBER; }
{name}       { NAME_token = im_text; return NAME; }

[ \t\r\n]*   { /* skip */ }
<<EOF>>      { return EOF; }
.            { im_error("lexical error"); }

%%

void im_error(const char *msg) {
  status("%d: error near '%s': %s", im_lineno, im_text, msg);
  abort(2, "error while parsing the migration information");
}
