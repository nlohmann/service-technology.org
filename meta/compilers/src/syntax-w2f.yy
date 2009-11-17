%error-verbose
%token_table
%defines

%{
#include "config.h"
#include <iostream>
#include <map>
#include <string>

extern char* yytext;
extern int yylex();
extern int yyerror(char const *msg);

extern std::ostream * myOut;

// data structures
std::map<std::string, char> events;
char event;
bool finalNode;
bool emptyNode;
unsigned int nodeID;
std::map<unsigned int, std::map<std::string, unsigned int> > transitions;
unsigned int * initialNode;
std::string delim;


%}


%token KEY_NODES
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COMMA COLON DOUBLECOLON SEMICOLON IDENT ARROW NUMBER
%token KEY_TRUE KEY_FALSE KEY_FINAL BIT_F BIT_S BIT_T
%token LPAR RPAR

%union {
    char *str;
    unsigned int value;
    std::string * phi;
}

%type <value> NUMBER
%type <str>   IDENT
%type <phi>   formula annotation

%left OP_OR
%left OP_AND
%left OP_NOT

%start og


%%


og:
  { 
    (*myOut) << "INTERFACE\n  INPUT\n    ";
    event = '?';
    emptyNode = false;
    initialNode = NULL;
  }
  KEY_INTERFACE input
  { 
    (*myOut) << ";\n  OUTPUT\n    "; 
    event = '!';
  }
  output synchronous KEY_NODES 
  {
    (*myOut) << ";\n\nNODES";
    delim = "";
  }
  nodes
  {
    (*myOut) << ";\n\nINITIALNODE\n  "
             << (*initialNode) << ";\n\nTRANSITIONS";
    delete initialNode;
    delim = "";
    for(std::map<unsigned int, std::map<std::string, unsigned int> >::iterator it = transitions.begin();
         it != transitions.end(); ++it)
    {
      for(std::map<std::string, unsigned int>::iterator t = it->second.begin();
           t != it->second.end(); ++t)
      {
        if(t->second != 0)
        {
          (*myOut) << delim << "\n  " << it->first << " -> " << t->second << " : "
                   << events[t->first] << t->first;
          delim = ",";
        }
      }
    }
    (*myOut) << ";\n" << std::flush;
  }
;


input:
  /* empty */
| KEY_INPUT identlist SEMICOLON
;


output:
  /* empty */
| KEY_OUTPUT identlist SEMICOLON
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS 
  { yyerror("cannot convert to Fiona OG format since Fiona does not support synchronous communication"); }
  identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
  { 
    events[$1] = event;
    (*myOut) << $1;
    free($1);
  }
| identlist COMMA IDENT
  { 
    events[$3] = event;
    (*myOut) << ", " << $3;
    free($3);
  }
;


nodes:
  node
| nodes node
;


node:
  NUMBER
  { 
    if (initialNode == NULL)
      initialNode = new unsigned int($1);

    nodeID = $1;
    finalNode = false;
  }
  annotation 
  {
    if(nodeID == 0)
    {
      emptyNode = true;
    }
    else
    {
      (*myOut) << delim << "\n  " << nodeID << " : " << (*$3) << " : blue";
      delim = ",";

      if(finalNode)
        (*myOut) << " : finalnode";
    }
    delete $3;
  }
  successors
;


annotation:
  /* empty */       { yyerror("missing formula. can only convert formula OGs"); }
| COLON formula     { $$ = $2; }
| DOUBLECOLON BIT_S { yyerror("can only convert formula OGs"); }
| DOUBLECOLON BIT_F { yyerror("can only convert formula OGs"); }
| DOUBLECOLON BIT_T { yyerror("can only convert formula OGs"); }
;


formula:
  LPAR formula RPAR
  { 
    (*$2) = "(" + (*$2) + ")";
    $$ = $2;
  }
| formula OP_AND formula
  { 
    (*$1) += " * " + (*$3);
    free($3);
    $$ = $1;
  }
| formula OP_OR formula
  { 
    (*$1) += " + " + (*$3);
    free($3);
    $$ = $1;
  }
| OP_NOT formula
  {
    (*$2) = "~" + (*$2);
    $$ = $2;
  }
| KEY_FINAL
  {
    finalNode = true; 
    $$ = new std::string("final");
  }
| KEY_TRUE
  { $$ = new std::string("true"); }
| KEY_FALSE
  { $$ = new std::string("false"); }
| IDENT
  {
    $$ = new std::string();
    (*$$) = events[$1];
    (*$$) += $1;
    free($1);
  }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    if(!emptyNode)
    {
      transitions[nodeID][$2] = $4;
    }
    free($2);
  }
;


%%


