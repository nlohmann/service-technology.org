%token NAME COMMA COLON SEMICOLON

%defines
%name-prefix="hlowfn_"

%{
#include <cstdio>
#include <string>
#include <map>
#include "types.h"

extern int hlowfn_lex();
extern int hlowfn_error(const char *);
extern FILE *pipe_out;

pType parseInterface;
std::map<std::string, pType> placeTypes;
%}

%union {
  char* str;
}

%type <str> NAME

%%

placelist:
  places SEMICOLON { fprintf(pipe_out, ", "); } placelist
| places SEMICOLON
;

places:
  place
| places COMMA { fprintf(pipe_out, ", "); } place
;

place:
  NAME
    { placeTypes[$1] = parseInterface;
      fprintf(pipe_out, "%s", $1); free($1); }
| NAME COLON NAME
    { placeTypes[$1] = parseInterface;
      fprintf(pipe_out, "%s : %s", $1, $3); free($1); free($3); }
;
