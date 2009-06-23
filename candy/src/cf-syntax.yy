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
extern Graph* parsedOG;


// from flex
extern char* cf_yytext;
extern int cf_yylex();
extern int cf_yyerror(char const *msg);

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
    // check and set cost entry for current event
    map< string, Event* >::iterator iter = parsedOG->events.find($2);
    if ( iter != parsedOG->events.end() ) {
        (iter->second)->cost = $3;
    } else {
        cf_yyerror("read an event twice");
        return EXIT_FAILURE;
    }

    free($2);
  }
;
