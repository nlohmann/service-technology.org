%{
/* Prologue: Syntax and usage of the prologue.
 * Bison Declarations: Syntax and usage of the Bison declarations section.
 * Grammar Rules: Syntax and usage of the grammar rules section.
 * Epilogue: Syntax and usage of the epilogue.  */

// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages

// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000

// from flex
extern char* et_yytext;
extern int et_yylineno;
extern int et_yylex();

#include "eventTerm.h"
#include <iostream>
#include <string>
#include <vector>

extern std::vector<EventTerm*>* term_vec;
extern bool stop_interaction;

int et_yyerror(const char* msg)
{
  std::cerr << msg << std::endl;
  std::cerr << "error in line " << et_yylineno << ": token last read: `" << et_yytext << "'" << std::endl;
  exit(1);
}


%}


// Bison options
%name-prefix="et_yy"
%defines

%token LPAR RPAR NUMBER NEW_TERM ADD MULT IDENT QUIT

%token_table

%union 
{
  int yt_int;
  std::string* yt_string;
  EventTerm* yt_term;
}


%type <yt_int> NUMBER
%type <yt_string> IDENT
%type <yt_term> term_def

%%

terms: QUIT {stop_interaction = true; return 0;}
| term terms
| ;

term: term_def NEW_TERM {term_vec->push_back($1);};

term_def: IDENT {$$ = new BasicTerm($1);}
		  | LPAR term_def LPAR {$$ = $2;}
		  | LPAR term_def ADD term_def RPAR {$$ = new AddTerm($2,$4);}
		  | LPAR term_def MULT NUMBER RPAR {$$ = new MultiplyTerm($2,$4);}
		  | LPAR NUMBER MULT term_def RPAR {$$ = new MultiplyTerm($4,$2);}
		  ;