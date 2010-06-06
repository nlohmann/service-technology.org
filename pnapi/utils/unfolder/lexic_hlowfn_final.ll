/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="hlowfn_final_"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno

%{
#include <cstdlib>
#include <string>
#include "syntax_hlowfn_final.h"
#include "types.h"

extern FILE *pipe_out_final;
extern int hlowfn_final_parse();
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
<INITIAL>"PLACE" { BEGIN(PLACES); fprintf(pipe_out_final, "PLACE "); }

 /* PLACES: strip oWFN keywords, copy everything until "INITIALMARKING" */
<PLACES>"INTERNAL" { parseInterface = INTERNAL; }
<PLACES>"INPUT" { parseInterface = INPUT; }
<PLACES>"OUTPUT" { parseInterface = OUTPUT; }
<PLACES>"," { return COMMA; }
<PLACES>":" { return COLON; }
<PLACES>";" { return SEMICOLON; }
<PLACES>"INITIALMARKING" { BEGIN(INITIALMARKING); }
<PLACES>{identifier} { hlowfn_final_lval.str = strdup(hlowfn_final_text);  return NAME; }

<INITIALMARKING>"FINALMARKING" { BEGIN(INITIAL); fprintf(pipe_out_final, ";\nMARKING");}
<INITIALMARKING>. { /*skip*/ }

	
%%

__attribute__((noreturn)) int hlowfn_final_error(const char *s) {
    fprintf(stderr, "hl-net:%d: %s near '%s'\n", hlowfn_final_lineno, s, hlowfn_final_text);
    exit(EXIT_FAILURE);
}
