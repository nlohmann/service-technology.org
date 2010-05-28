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

#include "costs.h"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

extern double* curterm;
std::vector<ElementalConstraint*>* curclause = 0;
Grant* curgrant = 0;
Request* request = new Request();


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
  ElementalConstraint* yt_constraint;
}


%type <yt_int> VALUE
%type <yt_string> IDENT
%type <yt_constraint> constraint

%%

requests: 
        request requests 
        |
        request
        ;

request:
       KEY_ASSERTIONS {request->assertions = new std::vector<std::vector<ElementalConstraint*> *>; } opt_assertion 
       KEY_GRANT {request->grants = new std::vector<Grant*>(); } grants 
       ;

opt_assertion:
             assertion SEMMELKORN
             |
             /* empty */
             ;

assertion:
	clause {if (curclause != 0) {request->assertions->push_back(curclause); curclause = 0;}}OP_OR assertion
	|
    clause
    {if (curclause != 0) {request->assertions->push_back(curclause);  curclause = 0;}}
	 ;

clause:
      constraint OP_AND clause
      { 
		if(curclause == 0) {
			curclause = new std::vector<ElementalConstraint*>();
			
		}
		curclause->push_back($1);
      }
      |
      constraint
      {
		if(curclause == 0) {
			curclause = new std::vector<ElementalConstraint*>();
			

			}
		curclause->push_back($1);
     }
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
		CostAgent::increaseAllXLabeledTransitionsInTermByY(curterm, $3, $1);
    	}
    |
   	IDENT term
	{ 
		if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
		CostAgent::increaseAllXLabeledTransitionsInTerm(curterm, $1);

	}
	|
    VALUE OP_TIMES IDENT
	{ 
		if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
		CostAgent::increaseAllXLabeledTransitionsInTermByY(curterm, $3, $1);

	}      
	|
    IDENT
	{ 
		if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
		CostAgent::increaseAllXLabeledTransitionsInTerm(curterm, $1);

	}
    ;

grants:
	grant grants
	|
	grant
	;
	
grant: { curgrant = new Grant(); }
     KEY_CONSTRAINT {curclause = 0; } clause {delete curgrant->constraint; curgrant->constraint = curclause; } SEMMELKORN
     KEY_PAYMENT { curterm = 0; } payment {if (curterm != 0) {delete curgrant->variableCosts; curgrant->variableCosts = curterm;} } SEMMELKORN
	 { request->grants->push_back(curgrant); curclause = 0; curterm = 0;} 
     ;

payment:
       VALUE OP_TIMES IDENT payment
		{ 
		if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
		CostAgent::increaseAllXLabeledTransitionsInTermByY(curterm, $3, $1);
		}
       |
       IDENT payment
		{ 
		if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
		CostAgent::increaseAllXLabeledTransitionsInTerm(curterm, $1);
		}
       |
       VALUE OP_TIMES IDENT
	{ 
		if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
		CostAgent::increaseAllXLabeledTransitionsInTermByY(curterm, $3, $1);
    }      
       |
       IDENT
	{ 
		if (curterm == 0) {curterm = CostAgent::getCleanDoubleArray(CostAgent::transitions->size());}
		CostAgent::increaseAllXLabeledTransitionsInTerm(curterm, $1);
    }
       |
       VALUE
	{ 
		curgrant->fixCosts += $1;
    }
       ;



