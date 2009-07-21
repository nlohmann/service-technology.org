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
extern char* etc_yytext;
extern int etc_yylineno;
extern int etc_yylex();

#include "eventTerm.h"
#include <iostream>
#include <string>
#include <vector>

extern std::vector<EventTermConstraint*>* constraint_vec;
extern bool stop_interaction;

int etc_yyerror(const char* msg)
{
  std::cerr << msg << std::endl;
  std::cerr << "error in line " << etc_yylineno << ": token last read: `" << etc_yytext << "'" << std::endl;
  exit(1);
}


%}


// Bison options
%name-prefix="etc_yy"
%defines

%token LPAR RPAR NUMBER NEW_TERM ADD MULT IDENT QUIT MINUS EQUALS GT LT

%token_table

%union 
{
  int yt_int;
  std::string* yt_string;
  EventTerm* yt_term;
  EventTermConstraint* yt_constraint;
  EventTermBound* yt_bounds;
}


%type <yt_int> NUMBER
%type <yt_int> val
%type <yt_string> IDENT
%type <yt_term> term
%type <yt_constraint> constraint_def
%type <yt_bounds> bounds
%type <yt_int> vorzeichen
%type <yt_term> summe
%type <yt_term> produkt
%%

constraints: QUIT {stop_interaction = true; return 0;}
| constraint constraints
| ;

constraint: constraint_def NEW_TERM {constraint_vec->push_back($1);}
    ;

constraint_def: term bounds { $$ = new EventTermConstraint($1,$2); }
            ;

bounds: val NEW_TERM val {$$ = new EventTermBound(true,true,$1,$3); }
        ; 

val:    vorzeichen NUMBER {$$ = $1*$2;}
        | NUMBER {$$ = $1;}
        ;

term: summe NEW_TERM {$$ = $1;};

summe: produkt {$$ = $1;}
       | produkt summe {$$ = new AddTerm($1,$2);} 
;

produkt: IDENT {$$ = new BasicTerm($1);}
         | NUMBER LPAR summe RPAR {$$ = new MultiplyTerm($3,$1);}
         | vorzeichen NUMBER LPAR summe RPAR {$$ = new MultiplyTerm($4,$1*$2);}
         | NUMBER MULT IDENT {$$ = new MultiplyTerm(new BasicTerm($3),$1);}
         | vorzeichen NUMBER MULT IDENT {$$ = new MultiplyTerm(new BasicTerm($4),$1*$2);}
         | vorzeichen IDENT {$$ = new MultiplyTerm(new BasicTerm($2),$1);}
;

vorzeichen: MINUS {$$ = -1;}
            | ADD {$$ = 1;}
;