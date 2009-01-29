%option noyywrap
%option yylineno
%option nodefault
%option outfile="lex.yy.c"
%option prefix="path_"

%{

#include <cstring>
#include "syntax_path.h"

void path_error(const char *msg);

%}


name      [^,;:()\t \n\{\}][^,;:()\t \n\{\}]*
number	  [0-9][0-9]*


%%


"PATH"			{ return KW_PATH; }

{name} 			{ char *temp = (char *)malloc((strlen(yytext)+1) * sizeof(char));
                          strcpy(temp, yytext);
                          path_lval.name = temp; return NAME; }


[ \t\r\n]*		{ /* skip */ }
<<EOF>>			{ return EOF; }
.			{ path_error("lexical error"); }

%%

void path_error(const char *msg)
{
  fprintf(stderr, "path %d: %s\n", path_lineno, msg);
}
