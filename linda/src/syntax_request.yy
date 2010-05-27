%{
/* Prologue: Syntax and usage of the prologue.
 * Bison Declarations: Syntax and usage of the Bison declarations section.
 * Grammar Rules: Syntax and usage of the grammar rules section.
 * Epilogue: Syntax and usage of the epilogue.  */

// options for Bison
#define YYDEBUG 0
#define YYERROR_VERBOSE 1  // for verbose error messages

// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000

// from flex
extern char* request_yytext;
extern int request_yylineno;
extern int request_yylex();

// #include "eventTerm.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

extern std::vector<int*>* term_vec;
extern bool stop_interaction;

int request_yyerror(const char* msg)
{
  std::cerr << msg << std::endl;
  std::cerr << "error in request file in line " << request_yylineno << ": token last read: `" << request_yytext << "'" << std::endl;
  exit(1);
}


%}


// Bison options
%name-prefix="request_yy"
%defines

%token KEY_ASSERTIONS KEY_CONSTRAINT KEY_GRANT KEY_PAYMENT
%token OP_LE OP_EQ OP_GE OP_PLUS OP_MINUS OP_TIMES OP_SLASH OP_OR OP_AND
%token IDENT
%token VALUE
%token COMMA SEMMELKORN COLON

%token_table

%union 
{
  int yt_int;
  std::string* yt_string;
}


%type <yt_int> VALUE
%type <yt_string> IDENT

%%

requests: 
        request requests 
        |
        request
        ;

request:
       KEY_ASSERTIONS opt_assertion 
       KEY_GRANT grant
       ;

opt_assertion:
             assertion
             |
             /* empty */
             ;

assertion:
         clause OP_OR assertion
         |
         clause
         ;

clause:
      constraint OP_AND clause
      |
      constraint
      ;

constraint:
          term OP_LE VALUE
          |
          term OP_GE VALUE
          |
          term OP_EQ VALUE
          ;
term:
    VALUE OP_TIMES IDENT term
    |
    IDENT term
    |
    VALUE OP_TIMES IDENT
    |
    IDENT
    |
    VALUE
    ;

grant:
     KEY_CONSTRAINT clause SEMMELKORN
     KEY_PAYMENT payment SEMMELKORN
     ;

payment:
       VALUE OP_TIMES IDENT payment
       |
       IDENT payment
       |
       VALUE OP_TIMES IDENT
       |
       IDENT
       |
       VALUE
       ;



