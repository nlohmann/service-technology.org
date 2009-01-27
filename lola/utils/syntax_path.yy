%token KW_PATH
%token NAME

%start outputpath

%token_table
%defines
%name-prefix="path_"

%{

#include <string>
#include <list>

#define YYERROR_VERBOSE

using namespace std;

extern int path_lex();
extern int path_error(const char *);

list<string> sequence;

%}

%union {
  char *name;
}

%type <name> NAME

%%

outputpath:
  KW_PATH sequencelist
;

sequencelist:
  /* empty */
| sequencelist NAME
    { sequence.push_back(string($2)); }
;
