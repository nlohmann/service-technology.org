/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="hlowfn_"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno

%{
#include <cstdlib>
#include "syntax_hlowfn.h"
#include "types.h"

extern FILE *pipe_out;
extern int hlowfn_parse();
extern pType parseInterface;
%}

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+

%s PLACES
%s MARKING

%%

 /* INITIAL: copy everything until you read the word "PLACE" */
<INITIAL>"PLACE" { BEGIN(PLACES); fprintf(pipe_out, "PLACE "); }

 /* PLACES: strip oWFN keywords, copy everything until "INITIALMARKING" */
<PLACES>"INTERNAL" { parseInterface = INTERNAL; }
<PLACES>"INPUT" { parseInterface = INPUT; }
<PLACES>"OUTPUT" { parseInterface = OUTPUT; }
<PLACES>"," { return COMMA; }
<PLACES>":" { return COLON; }
<PLACES>";" { return SEMICOLON; }
<PLACES>"INITIALMARKING" { BEGIN(MARKING); fprintf(pipe_out, ";\nMARKING"); }
<PLACES>{identifier} { hlowfn_lval.str = strdup(hlowfn_text);  return NAME; }


%%

int hlowfn_error(const char *s) {
	fprintf(stderr, "%s: %s", hlowfn_text, s);
	exit(EXIT_FAILURE);
}
