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


#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include "helpers.h"

// from flex
extern char* fingerprint_yytext;
extern int fingerprint_yylineno;
extern int fingerprint_yylex();

int fingerprint_yyerror(const char* msg)
{
  std::cerr << msg << std::endl;
  std::cerr << "error in fingerprint file in line " << fingerprint_yylineno << ": token last read: `" << fingerprint_yytext << "'" << std::endl;
  exit(1);
}


// Global parser variables

extern ServiceModel* parsedModel; 
std::vector<std::string> stringVector;
std::vector<std::pair<std::vector<int>*,std::vector<int>* > > terms;
int nr_of_finalmarkings = 0;
int nr_of_terms = 0;
std::vector<int>* currentIDVector;
std::vector<int>* currentTermDom;
std::vector<int>* currentTermVals;
int currentFM, currentT;
%}


// Bison options
%name-prefix="fingerprint_yy"
%defines

%token KW_PLACE KW_INTERNAL KW_INPUT KW_OUTPUT KW_FINALMARKINGS KW_TERMS KW_BOUNDS
%token KW_UNBOUNDED TERM_IDENTIFIER FINALMARKING_IDENTIFIER
%token IDENT
%token VALUE
%token PLUS MINUS TIMES
%token COMMA SEMICOLON COLON EQUALS

%token_table

%union 
{
  int yt_int;
  std::string* yt_string;
  int* yt_intarray;
}

%type <yt_int> FINALMARKING_IDENTIFIER
%type <yt_int> TERM_IDENTIFIER
%type <yt_int> VALUE
%type <yt_int> value
%type <yt_string> IDENT

%%

fingerprint :{parsedModel = new ServiceModel(); terms = std::vector<std::pair<std::vector<int>*,std::vector<int>* > >();}  places KW_FINALMARKINGS finalmarkings KW_TERMS terms KW_BOUNDS bounds ;
places : KW_PLACE internals inputs    outputs ;
internals : KW_INTERNAL internal_identifierlist ;
inputs : {currentIDVector = &(parsedModel->inputs); } KW_INPUT  identifierlist ;
outputs : {currentIDVector = &(parsedModel->outputs);} KW_OUTPUT  identifierlist ;
finalmarkings : FINALMARKING_IDENTIFIER COLON finalmarking {++nr_of_finalmarkings; parsedModel->constraints.push_back(std::vector<Constraint*>());} 
              | FINALMARKING_IDENTIFIER COLON finalmarking finalmarkings {++nr_of_finalmarkings; parsedModel->constraints.push_back(std::vector<Constraint*>());};
terms : TERM_IDENTIFIER COLON term terms 
      |  ;

bounds : FINALMARKING_IDENTIFIER COMMA TERM_IDENTIFIER COLON {currentFM = $1-1; currentT = $3-1;} bound SEMICOLON bounds ;
      | ;

term : {currentTermDom = new std::vector<int>; currentTermVals = new std::vector<int>;} valueidentifierlist {terms.push_back(std::pair<std::vector<int>*,std::vector<int>* >(currentTermDom,currentTermVals));  ++nr_of_terms;} ;

bound : value COMMA value  {  parsedModel->constraints[currentFM].push_back(new BoundedConstraint(terms[currentT].first, terms[currentT].second, $1, $3)); } 
        | KW_UNBOUNDED COMMA value {  parsedModel->constraints[currentFM].push_back(new RightBoundedConstraint(terms[currentT].first, terms[currentT].second, $3)); } 
        | value COMMA KW_UNBOUNDED {  parsedModel->constraints[currentFM].push_back(new LeftBoundedConstraint(terms[currentT].first, terms[currentT].second, $1)); }   
        | KW_UNBOUNDED COMMA KW_UNBOUNDED {  parsedModel->constraints[currentFM].push_back(new UnboundedConstraint(terms[currentT].first, terms[currentT].second)); } ;
 
valueidentifierlist : valueidentifierpair valueidentifierlist
                     |  SEMICOLON ;
                     
valueidentifierpair : value TIMES IDENT { 
          currentTermDom->push_back(Universe::getNumID($3)); currentTermVals->push_back($1);} 
          
          
          | IDENT {  
          currentTermDom->push_back(Universe::getNumID($1)); 
          currentTermVals->push_back(1);};
value : PLUS VALUE {$$ = $2;} | MINUS VALUE {$$ = -1*$2;} | VALUE {$$ = $1;} ;

identifierlist : IDENT { currentIDVector->push_back(Universe::addIdentifier($1)); } COMMA identifierlist |
                 IDENT { currentIDVector->push_back(Universe::addIdentifier($1)); } SEMICOLON ;
internal_identifierlist : IDENT COMMA internal_identifierlist |
                 IDENT SEMICOLON ;

finalmarking : keyvalpairlist ;
 
keyvalpairlist : keyvalpair SEMICOLON | keyvalpair COMMA keyvalpairlist ;
keyvalpair : IDENT EQUALS value ;
