/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="llowfn_"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno

%{
#include <cstdlib>
#include "syntax_llowfn.h"
#include "types.h"

extern int llowfn_parse();
extern int llowfn_lineno;
%}

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+

%s PLACES
%s MARKING

%%

 /* PLACES: strip oWFN keywords, copy everything until "INITIALMARKING" */
<PLACES>"," { return COMMA; }
<PLACES>":" { return COLON; }
<PLACES>";" { return SEMICOLON; }
<PLACES>{identifier} { llowfn_lval.str = strdup(llowfn_text);  return NAME; }


%%

int llowfn_error(const char *s) {
    fprintf(stderr, "ll-net:%d: %s near '%s'\n", llowfn_lineno, s, llowfn_text);
    exit(EXIT_FAILURE);
}
