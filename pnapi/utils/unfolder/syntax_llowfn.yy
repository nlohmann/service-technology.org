%token NAME COMMA COLON SEMICOLON

%defines
%name-prefix="llowfn_"

%{
#include <string>
#include <map>
#include "types.h"

extern int llowfn_lex();
extern int llowfn_error(const char *);

extern std::map<std::string, pType> placeTypes;
%}

%union {
  char* str;
}

%type <str> NAME

%%

placelist:
  places SEMICOLON placelist
| places SEMICOLON
;

places:
  place
| places COMMA place
;

place:
  NAME
| NAME COLON NAME
;
