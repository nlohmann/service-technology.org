%{
// map library
#include <map>
// string library
#include <string>

// header from configure
#include "config.h"
// header for graph class
#include "Graph.h"


// from main.cc
extern map< string, unsigned int > ogEdges;


// from flex
extern char* cf_yytext;
extern int cf_yylex();
extern int cf_yyerror(char const *msg);

using std::pair;
using std::string;


%}

%name-prefix="cf_yy"
%error-verbose
%token_table
%defines

%token SEMICOLON IDENT NUMBER

%union {
    char *str;
    unsigned int value;
}

%type <value> NUMBER
%type <str>   IDENT

%start costfile
%%


costfile:
  eventcost
;


eventcost:
  /* empty */
| eventcost IDENT NUMBER SEMICOLON
  {
    // check and create new entry for edge
    if ( ogEdges.find($2) != ogEdges.end() ) {
        cf_yyerror("read an edge label twice");
        return EXIT_FAILURE;
    } else {
        ogEdges[$2] = $3;
    }

    free($2);
  }
;
