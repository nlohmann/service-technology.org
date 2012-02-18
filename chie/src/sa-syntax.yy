%{
#include <map>
#include <string>
#include <vector>
#include "serviceAutomaton.h"

// generic bison/flex stuff
extern char * sa_yytext;
extern int sa_yylex();
extern int sa_yyerror(char const *msg);

// pointer to service automata
extern ServiceAutomaton * sa_result;
extern ServiceAutomaton * sa_specification;

// whether we are handling input events
bool inputEvents = true;

// wheter node has successors
bool hasSuccessors = false;

// number of current node
unsigned int currentNode;

// last read ident
std::string sa_lastIdent;

// vector of receive events
std::vector<std::string> sa_receiving;

// vector of sending events
std::vector<std::string> sa_sending;

%}

%name-prefix="sa_yy"
%error-verbose
%token_table
%defines

%token NODES INITIAL_ FINAL
%token INTERFACE INPUT OUTPUT SYNCHRONOUS
%token COLON COMMA SEMICOLON IDENT ARROW NUMBER

%union {
    unsigned int value;
}

%type <value> NUMBER

%start sa
%%


sa:
  INTERFACE
  {
    // clear vectors
    sa_receiving.clear();
    sa_sending.clear();
  }
  input output synchronous NODES
  {
    sa_result = new ServiceAutomaton(sa_sending, sa_receiving, !sa_specification);
  }
  nodes
;


input:
  /* empty */
| INPUT
  { inputEvents = true; }
  identlist SEMICOLON
;


output:
  /* empty */
| OUTPUT
  { inputEvents = false; }
  identlist SEMICOLON
;


synchronous:
  /* empty */
| SYNCHRONOUS
  { sa_yyerror("synchronous communication not supported"); }
  identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
  {
    if (inputEvents)
    {
      sa_receiving.push_back(sa_lastIdent);
    }
    else
    {
      sa_sending.push_back(sa_lastIdent);
    }
  }
| identlist COMMA IDENT
  {
    if (inputEvents)
    {
      sa_receiving.push_back(sa_lastIdent);
    }
    else
    {
      sa_sending.push_back(sa_lastIdent);
    }
  }
;


nodes:
  node
| nodes node
;


node:
  NUMBER
  {
    currentNode = $1;
    hasSuccessors = false;
  }
  annotation successors
  {
    if(!hasSuccessors)
    {
      sa_result->setNoSuccessorState($1);
    }
  }
;


annotation:
  /* empty */
| COLON INITIAL_
  { sa_result->setInitialState(currentNode); }
| COLON FINAL
  { sa_result->setFinalState(currentNode); }
| COLON INITIAL_ COMMA FINAL
  {
    sa_result->setFinalState(currentNode);
    sa_result->setInitialState(currentNode);
  }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    sa_result->addSuccessor(currentNode, sa_lastIdent, $4);
    hasSuccessors = true;
  }
;


%%

