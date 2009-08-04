%{
// map library
#include <map>
// string library
#include <string>

// header from configure
#include "config.h"
// header for graph class
#include "Graph.h"

using std::string;


// from main.cc
extern Graph* parsedOG;

// from flex
extern char* cf_yytext;
extern int cf_yylex();
extern int cf_yyerror(char const *msg);

// for parser
map< string, bool > parsedEventsCF;
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
  {
    if ( parsedEventsCF.size() != parsedOG->events.size() ) {
        cf_yyerror("given costfile does not include all events from given OG");
        exit(EXIT_FAILURE);
    }
  }
;


eventcost:
  /* empty */
| eventcost IDENT NUMBER SEMICOLON
  {
    // check and set cost entry for current event
    if ( parsedEventsCF.find($2) == parsedEventsCF.end() ) {
        map< string, Event* >::iterator iter = parsedOG->events.find($2);
        if ( iter != parsedOG->events.end() ) {
            (iter->second)->cost = $3;
            parsedEventsCF[$2] = true;
        } else {
            cf_yyerror("given costfile includes events which are not used in given OG");
            exit(EXIT_FAILURE);
        }
    } else {
        cf_yyerror("given costfile includes events several times");
        exit(EXIT_FAILURE);
    }

    free($2);
  }
;
