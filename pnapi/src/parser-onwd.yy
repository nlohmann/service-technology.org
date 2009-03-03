 // -*- C++ -*-

 /*****************************************************************************
  * bison options 
  ****************************************************************************/

/* plain c parser: the prefix is our "namespace" */
%name-prefix="pnapi_onwd_"

/* write tokens to parser-onwd.h for use by scanner */
%defines

/* more verbose error reporting */
%error-verbose


 /*****************************************************************************
  * C declarations
  ****************************************************************************/
%{

#include <string>
#include "parser.h"

#undef yylex
#undef yyparse
#undef yyerror

#define yyerror pnapi::parser::error

#define yylex pnapi::parser::onwd::lex
#define yyparse pnapi::parser::onwd::parse

%}


 /*****************************************************************************
  * types, tokens, start symbol
  ****************************************************************************/

%union {
  int                         yt_int;
  std::string *               yt_string;
  pnapi::parser::onwd::Node * yt_node;
}

%token KEY_INSTANCE KEY_WIRING
%token SYM_COMMA SYM_COLON SYM_SEMICOLON SYM_DOT SYM_SINGLE_ARR SYM_DOUBLE_ARR
%token SYM_BRACKET_LEFT SYM_BRACKET_RIGHT

%token <yt_int> CL_NUMBER
%token <yt_string> CL_IDENTIFIER CL_STRING

%type <yt_node> instance instances wiring wirings place

%start onwd


 /*****************************************************************************
  * grammar rules 
  ****************************************************************************/
%%

onwd: KEY_INSTANCE instances SYM_SEMICOLON 
      KEY_WIRING   wirings   SYM_SEMICOLON 
      { node = new Node($2, $5); };

instances: instance                     { $$ = new Node($1);     }
         | instances SYM_COMMA instance { $$ = $1->addChild($3); }
         ;

instance: CL_IDENTIFIER SYM_COLON CL_STRING 
                              { $$ = new Node(INSTANCE, $1, $3, 1); }
        | CL_IDENTIFIER SYM_BRACKET_LEFT CL_NUMBER SYM_BRACKET_RIGHT 
  	  SYM_COLON CL_STRING { $$ = new Node(INSTANCE, $1, $6, $3); }
        ;

wirings: wiring                   { $$ = new Node($1);     }
       | wirings SYM_COMMA wiring { $$ = $1->addChild($3); }
       ;

wiring: place SYM_SINGLE_ARR place { $$ = new Node(ANY_WIRING, $1, $3); }
      | place SYM_DOUBLE_ARR place { $$ = new Node(ALL_WIRING, $1, $3); }
      ;

place: CL_IDENTIFIER SYM_DOT CL_IDENTIFIER { $$ = new Node(PLACE, $1, $3); };
