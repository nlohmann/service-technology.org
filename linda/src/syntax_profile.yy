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
double* curterm = 0;

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
%token OP_LE OP_EQ OP_GE OP_PLUS OP_MINUS OP_TIMES OP_SLASH OP_AND
%token IDENT
%token VALUE
%token COMMA SEMMELKORN COLON

%token_table

%union 
{
  int yt_int;
  std::string* yt_string;
  ElementalConstraint* yt_constr;
  int* yt_intarray;
}


%type <yt_int> VALUE
%type <yt_string> IDENT
%type <yt_constr> constraint

%%

usecases: 
        usecase usecases
        |
        usecase
        ;

usecase:
       KEY_USECASE
       {
//        std::cerr << "Creating new use case" << std::endl;    
		usecase = new UseCase();
       }
       KEY_CONSTRAINT opt_constraints SEMMELKORN
       KEY_MARKING opt_marking SEMMELKORN
       KEY_COSTS costs SEMMELKORN
       KEY_POLICY policy SEMMELKORN
       {
        // usecase->output();    
		profile->usecases->push_back(usecase);
       }
       ;

opt_constraints:
               constraints
               |
               /* empty */
               ;

constraints:
           constraint OP_AND constraints
	   { usecase->constraint->push_back($1); }
           |
           constraint
	   { usecase->constraint->push_back($1); }
           ;

constraint:
          term OP_LE VALUE
	  { 
		ElementalConstraint* cons = new ElementalConstraint();
		cons->lhs = curterm;
		curterm = 0;
		cons->len = CostAgent::transitions->size();
		cons->sign = 1;
		cons->rhs = $3;	
		$$ = cons;
	  }
          |
          term OP_GE VALUE
	  { 
		ElementalConstraint* cons = new ElementalConstraint();
		cons->lhs = curterm;
		curterm = 0;
		cons->len = CostAgent::transitions->size();
		cons->sign = 2;
		cons->rhs = $3;
		$$ = cons;	
	  }
          |
          term OP_EQ VALUE
	  { 
		ElementalConstraint* cons = new ElementalConstraint();
		cons->lhs = curterm;
		curterm = 0;
		cons->len = CostAgent::transitions->size();
		cons->sign = 3;
		cons->rhs = $3;
		$$ = cons;	
	  }
          ;
term:
    VALUE OP_TIMES IDENT term
    { 
	if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
	curterm[CostAgent::getTransitionID(*$3)] += $1;
    }
    |
    IDENT term
    { 
	if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
	++curterm[CostAgent::getTransitionID(*$1)];
    }
    |
    VALUE OP_TIMES IDENT
    { 
	if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
	curterm[CostAgent::getTransitionID(*$3)] += $1;
    }
    |
    IDENT
    { 
	if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
	++curterm[CostAgent::getTransitionID(*$1)];
    }
    ;

opt_marking:
           marking
           |
           /* empty */
           ;

marking:
       IDENT COLON VALUE COMMA marking
       { usecase->marking[CostAgent::getPlaceID(*$1)] = $3; }
       |
       IDENT COMMA marking
       { usecase->marking[CostAgent::getPlaceID(*$1)] = 1; }
       |
       IDENT COLON VALUE
       { usecase->marking[CostAgent::getPlaceID(*$1)] = $3; }
       |
       IDENT
       { usecase->marking[CostAgent::getPlaceID(*$1)] = 1; }
       ;

costs:
     VALUE OP_TIMES IDENT costs
     {
      	usecase->variableCosts[CostAgent::getTransitionID(*$3)] += $1;
     }
     |
     IDENT costs
     {
      	++usecase->variableCosts[CostAgent::getTransitionID(*$1)];
     }
     |
     VALUE OP_TIMES IDENT
     {
      	usecase->variableCosts[CostAgent::getTransitionID(*$3)] += $1;
     }
     |
     IDENT
     {
      	++usecase->variableCosts[CostAgent::getTransitionID(*$1)];
     }
     |
     VALUE
     {
        usecase->fixCosts += (double) $1;
     }
     ;

policy:
      VALUE COMMA VALUE
      {
      usecase->policyLHS = $1;
	  usecase->policyLB = true;
      usecase->policyRHS = $3;
	  usecase->policyRB = true;
      }
	  | 
      COMMA VALUE
      {
      usecase->policyRHS = $2;
	  usecase->policyRB = true;
      }
	  | 
	  VALUE COMMA
      {
      usecase->policyLHS = $1;
	  usecase->policyLB = true;
      }
	  |      
	  COMMA
      {
		/* nothing do be done */
      }
      ;

