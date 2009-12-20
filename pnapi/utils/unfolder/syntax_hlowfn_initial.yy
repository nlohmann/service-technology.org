%token NAME COMMA COLON SEMICOLON

%defines
%name-prefix="hlowfn_initial_"

%{
#include <cstdio>
#include <string>
#include <map>
#include "types.h"

extern int hlowfn_initial_lex();
extern int hlowfn_initial_error(const char *);
extern FILE *pipe_out_initial;

pType parseInterface;
std::map<std::string, pType> placeTypes;
%}

%union {
  char* str;
}

%type <str> NAME

%%

placelist:
  places SEMICOLON { fprintf(pipe_out_initial, ", "); } placelist
| places SEMICOLON
;

places:
  place
| places COMMA { fprintf(pipe_out_initial, ", "); } place
;

place:
  NAME
    { placeTypes[$1] = parseInterface;
      fprintf(pipe_out_initial, "%s", $1); free($1); }
| NAME COLON NAME
    { placeTypes[$1] = parseInterface;
      fprintf(pipe_out_initial, "%s : %s", $1, $3); free($1); free($3); }
;
