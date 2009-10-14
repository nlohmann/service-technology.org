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

%token LPAR RPAR NUMBER NEW_TERM ADD MULT IDENT QUIT MINUS

%token_table

%union 
{
  int yt_int;
  std::string* yt_string;
  EventTerm* yt_term;
}


%type <yt_int> NUMBER
%type <yt_int> vorzeichen
%type <yt_term> summe
%type <yt_term> produkt
%type <yt_string> IDENT

%%

terms:  QUIT {stop_interaction = true; return 0;}
        | term terms
        | 
;

term: summe NEW_TERM {term_vec->push_back($1);};

summe: produkt {$$ = $1;}
       | produkt summe {$$ = new AddTerm($1,$2);} 
;

produkt: IDENT {$$ = new BasicTerm($1);}
         | NUMBER MULT LPAR summe RPAR {$$ = new MultiplyTerm($4,$1);}
         | vorzeichen NUMBER LPAR summe RPAR {$$ = new MultiplyTerm($4,$1*$2);}
         | NUMBER MULT IDENT {$$ = new MultiplyTerm(new BasicTerm($3),$1);}
         | vorzeichen NUMBER MULT IDENT {$$ = new MultiplyTerm(new BasicTerm($4),$1*$2);}
         | vorzeichen IDENT {$$ = new MultiplyTerm(new BasicTerm($2),$1);}
;

vorzeichen: MINUS {$$ = -1;}
            | ADD {$$ = 1;}
;