/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="hlowfn_initial_"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno

%{
#include <cstdlib>
#include <string>
#include "syntax_hlowfn_initial.h"
#include "types.h"

extern FILE *pipe_out_initial;
extern int hlowfn_parse_initial();
extern pType parseInterface;

%}

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+

%s PLACES
%s INITIALMARKING
%s FINALMARKING

%%

 /* INITIAL: copy everything until you read the word "PLACE" */
<INITIAL>"PLACE" { BEGIN(PLACES); fprintf(pipe_out_initial, "PLACE "); }

 /* PLACES: strip oWFN keywords, copy everything until "INITIALMARKING" */
<PLACES>"INTERNAL" { parseInterface = INTERNAL; }
<PLACES>"INPUT" { parseInterface = INPUT; }
<PLACES>"OUTPUT" { parseInterface = OUTPUT; }
<PLACES>"," { return COMMA; }
<PLACES>":" { return COLON; }
<PLACES>";" { return SEMICOLON; }
<PLACES>"INITIALMARKING" { BEGIN(INITIALMARKING); fprintf(pipe_out_initial, ";\nMARKING"); }
<PLACES>{identifier} { hlowfn_initial_lval.str = strdup(hlowfn_initial_text);  return NAME; }

<INITIALMARKING>"FINALMARKING" { BEGIN(FINALMARKING); }
<FINALMARKING>";" { BEGIN(INITIAL); }
<FINALMARKING>.	  { /*skip*/ }

	
%%

__attribute__((noreturn)) int hlowfn_initial_error(const char *s) {
    fprintf(stderr, "hl-net:%d: %s near '%s'\n", hlowfn_initial_lineno, s, hlowfn_initial_text);
    exit(EXIT_FAILURE);
}
