%name-prefix="pnapi_petrify_"
%defines

%{
// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


#include <string>
#include <iostream>
#include <set>
#include <map>
#include <cstdlib>

#include "parser.h"


using std::cerr;
using std::endl;
using std::string;
using std::set;
using std::map;

using PNapi::parser::petrify::Node;

extern int yylex();
extern char *yytext;
extern int yylineno;


int yyerror(const char *);

// global variable for parse result (root node of AST)
Node * pnapi_petrify_ast;
%}

%union {
  char *str; 
  PNapi::parser::petrify::Node * yt_node;
}

/* the types of the non-terminal symbols */
%type <str> TRANSITIONNAME
%type <str> PLACENAME

%type <yt_node> stg
%type <yt_node> transition_list
%type <yt_node> tp_list
%type <yt_node> pt_list
%type <yt_node> place_list

%token PLACENAME TRANSITIONNAME IDENTIFIER
%token K_MODEL K_DUMMY K_GRAPH K_MARKING K_END NEWLINE
%token OPENBRACE CLOSEBRACE

%start stg

%%

stg:
  K_MODEL IDENTIFIER newline
  K_DUMMY transition_list newline
  K_GRAPH newline
  tp_list pt_list
  K_MARKING OPENBRACE place_list CLOSEBRACE newline
  K_END newline { pnapi_petrify_ast = new Node(Node::STG, $5, $9, $10, $13); }
;

transition_list:
  TRANSITIONNAME transition_list
    { 
      $$ = new Node(Node::TLIST, string($1), $2);
      free($1);
    }
| /* empty */ {$$ = NULL;}
;

place_list:
  PLACENAME place_list
    { 
      $$ = new Node(Node::PLIST, string($1), $2);
      free($1);
    }
| /* empty */ {$$ = NULL;}
;

tp_list:
  TRANSITIONNAME place_list newline tp_list
  {
    $$ = new Node(Node::TP, string($1), $2, $4);
    free($1);
  }
| /* empty */ {$$ = NULL;}
;

pt_list:
  PLACENAME transition_list newline pt_list
  {
    $$ = new Node(Node::PT, string($1), $2, $4);
    free($1);
  }
| /* empty */ {$$ = NULL;}
;

newline:
  NEWLINE
| NEWLINE NEWLINE
;

%%

int yyerror(const char * msg)
{
  throw msg;
}