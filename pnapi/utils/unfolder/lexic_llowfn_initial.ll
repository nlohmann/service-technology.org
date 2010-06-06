/* created lexer should be called "lex.yy.c" to make the ylwrap script work */
%option outfile="lex.yy.c"

/* plain c scanner: the prefix is our "namespace" */
%option prefix="llowfn_initial_"

/* we read only one file */
%option noyywrap

/* yyunput not needed (fix compiler warning) */
%option nounput

/* maintain line number for error reporting */
%option yylineno

%{
#include <cstdlib>
#include <string>
#include "syntax_llowfn_initial.h"
#include "types.h"

extern int llowfn_initial_parse();
extern int llowfn_initial_lineno;

extern std::string markingType;
%}

whitespace     [\n\r\t ]
identifier     [^,;:()\t \n\r\{\}=]+
number         [0-9]+

%s PLACES
%s MARKING

%%
<INITIAL>"PLACE" { BEGIN(PLACES); fprintf(llowfn_initial_out, "PLACE\n"); } 

<PLACES>"," { return COMMA; }
<PLACES>":" { return COLON; }
<PLACES>";" { return SEMICOLON; }
<PLACES>"MARKING" { BEGIN(MARKING); fprintf(llowfn_initial_out, "INITIALMARKING"); }
<PLACES>{identifier} { llowfn_initial_lval.str = strdup(llowfn_initial_text);  return NAME; }
<PLACES>{whitespace} {/*skip whitespace*/}


%%

__attribute__((noreturn)) int llowfn_initial_error(const char *s) {
    fprintf(stderr, "ll-net:%d: %s near '%s'\n", llowfn_initial_lineno, s, llowfn_initial_text);
    exit(EXIT_FAILURE);
}

