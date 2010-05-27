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

// #include "eventTerm.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include "costs.h"

// from flex
extern char* profile_yytext;
extern int profile_yylineno;
extern int profile_yylex();

CostProfile * profile = new CostProfile();
UseCase * usecase = NULL;

int profile_yyerror(const char* msg)
{
  std::cerr << msg << std::endl;
  std::cerr << "error in profile file in line " << profile_yylineno << ": token last read: `" << profile_yytext << "'" << std::endl;
  exit(1);
}


%}


// Bison options
%name-prefix="profile_yy"
%defines

%token KEY_USECASE KEY_CONSTRAINT KEY_MARKING KEY_COSTS KEY_POLICY
%token OP_LE OP_EQ OP_GE OP_PLUS OP_MINUS OP_TIMES OP_SLASH
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

usecases: 
        usecase usecases
        |
        usecase
        ;

usecase:
       KEY_USECASE
       {
            usecase = new UseCase();
       }
       KEY_CONSTRAINT opt_constraints SEMMELKORN
       KEY_MARKING opt_marking SEMMELKORN
       KEY_COSTS costs SEMMELKORN
       KEY_POLICY policy SEMMELKORN
       {
            profile->usecases.push_back(usecase);
       }
       ;

opt_constraints:
               constraints
               |
               /* empty */
               ;

constraints:
           constraint COMMA constraints
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
    VALUE OP_TIMES IDENT OP_PLUS term
    |
    IDENT OP_PLUS term
    |
    VALUE
    ;

opt_marking:
           marking
           |
           /* empty */
           ;

marking:
       IDENT COLON VALUE COMMA marking
       |
       IDENT COMMA marking
       |
       IDENT COLON VALUE
       |
       IDENT
       ;

costs:
     VALUE OP_TIMES IDENT OP_PLUS costs
     |
     IDENT OP_PLUS costs
     |
     VALUE
     {
        usecase->fixCosts += $1;
     }
     ;

policy:
      VALUE COMMA VALUE
      {
      usecase->policyLHS = $1;
      usecase->policyRHS = $3;
      }
      ;

