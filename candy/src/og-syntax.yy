%{
// map library
#include <map>
// string library
#include <string>
#include <iostream>

// header from configure
#include "config.h"
// header for graph class
#include "Graph.h"

using std::pair;
using std::string;
using std::cerr;
using std::endl;


// from main.cc
extern Graph* parsedOG;

// from flex
extern char* og_yytext;
extern int og_yylex();
extern int og_yyerror(char const *msg);

// for parser
bool foundRootNode;
Node* currentNode;
Node* currentSuccessor;
Formula* currentFormula;
EventType currentType;
%}

%name-prefix="og_yy"
%error-verbose
%token_table
%defines

%token KEY_NODES
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COMMA COLON DOUBLECOLON SEMICOLON IDENT ARROW NUMBER
%token KEY_TRUE KEY_FALSE KEY_FINAL BIT_F BIT_S
%token LPAR RPAR

%union {
    char *str;
    unsigned int value;
    Formula* form;
}

%type <value> NUMBER
%type <str>   IDENT
%type <form> formula

%left OP_OR
%left OP_AND
%left OP_NOT

%start og
%%


og:
  KEY_INTERFACE
  {
    currentType = T_INPUT;
  }
  input
  {
    currentType = T_OUTPUT;
  }
  output
  {
    currentType = T_SYNC;
  }
  synchronous KEY_NODES
  {
    foundRootNode = false;
    currentNode = NULL;
    currentSuccessor = NULL;
  }
  nodes
  {
    for ( map< unsigned int, Node* >::const_iterator i = parsedOG->nodes.begin();
          i != parsedOG->nodes.end(); ++i ) {

        // if a successor node was created but not declared
        // it has no formula thanks to the node's constructor
        if ( (i->second)->formula == NULL ) {
            og_yyerror("read a successor which is not declared");
            exit(EXIT_FAILURE);
        }
    }
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
| KEY_SYNCHRONOUS identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
  {
    Event* newEvent = NULL;
    if ( currentType == T_INPUT ) {
        newEvent = new Event($1, 0, T_INPUT, false);
    } else if ( currentType == T_OUTPUT ) {
        newEvent = new Event($1, 0, T_OUTPUT, false);
    } else if ( currentType == T_SYNC ) {
        newEvent = new Event($1, 0, T_SYNC, false);
    } else {
        og_yyerror("read an event of unknown type");
        exit(EXIT_FAILURE);
    }
    
    if ( parsedOG->events.find($1) == parsedOG->events.end() ) {
        parsedOG->events[$1] = newEvent;
    } else {
        og_yyerror("read an event twice");
        exit(EXIT_FAILURE);
    }
    
    free($1);
  }
| identlist COMMA IDENT
  {
    Event* newEvent = NULL;
    if ( currentType == T_INPUT ) {
        newEvent = new Event($3, 0, T_INPUT, false);
    } else if ( currentType == T_OUTPUT ) {
        newEvent = new Event($3, 0, T_OUTPUT, false);
    } else if ( currentType == T_SYNC ) {
        newEvent = new Event($3, 0, T_SYNC, false);
    } else {
        og_yyerror("read an event of unknown type");
        exit(EXIT_FAILURE);
    }
    
    if ( parsedOG->events.find($3) == parsedOG->events.end() ) {
        parsedOG->events[$3] = newEvent;
    } else {
        og_yyerror("read an event twice");
        exit(EXIT_FAILURE);
    }
    
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
    // find known current node or create it
    map< unsigned int, Node* >::const_iterator i = parsedOG->nodes.find($1);
    if ( i != parsedOG->nodes.end() ) {
        currentNode = i->second;
    } else {
        //cerr << "PARSER: read new node '" << $1 << "' as new node" << endl;
        currentNode = new Node($1);
        parsedOG->nodes[$1] = currentNode;
    }
    
    // first found node is per definition the root node
    if ( not foundRootNode ) {
        foundRootNode = true;
        parsedOG->root = currentNode;
    }
  }
  annotation
  {
    // add parsed formula
    currentNode->formula = currentFormula;
  }
  successors
;


annotation:
  /* empty */
| COLON formula
  {
    currentFormula = $2;
  }
| DOUBLECOLON BIT_S
  {
    // parsing 2-bit OGs is not supported
    og_yyerror("read a 2-bit annotation; only formulae are supported");
    exit(EXIT_FAILURE);
  }
| DOUBLECOLON BIT_F
  {
    // parsing 2-bit OGs is not supported
    og_yyerror("read a 2-bit annotation; only formulae are supported");
    exit(EXIT_FAILURE);
  }
;


formula:
  LPAR formula RPAR
  {
    $$ = $2;
  }
| formula OP_AND formula
  {
    $$ = new FormulaMultiaryAnd($1, $3);
  }
| formula OP_OR formula
  {
    $$ = new FormulaMultiaryOr($1, $3);
  }
| OP_NOT formula
  {
    //$$ = new pnapi::formula::Negation(*$2);
    // TODO implement negation
    $$ = NULL;
  }
| KEY_FINAL
  {
    // TODO remove
    currentNode->final = true;
    $$ = new FormulaFinal();
    //$$ = NULL;
  }
| KEY_TRUE
  {
    $$ = new FormulaTrue();
  }
| KEY_FALSE
  {
    $$ = new FormulaFalse();
  }
| IDENT
  { 
    $$ = new FormulaLiteral($1);

    // since lexer uses strdup() to copy the ident it has to be freed
    free($1); 
  }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    // find known successor node or create it
    map< unsigned int, Node* >::const_iterator i = parsedOG->nodes.find($4);
    if ( i != parsedOG->nodes.end() ) {
        currentSuccessor = i->second;
    } else {
        // we create the successor as new node with formula NULL
        //cerr << "PARSER: read new node '" << $4 << "' as successor from '" << currentNode->getID() << "'" << endl;
        currentSuccessor = new Node($4);
        parsedOG->nodes[$4] = currentSuccessor;
    }

    // register successor node as successor
    // we allow more than one equal successors with different events, just
    // remember the empty node ...
    //if ( currentNode->successors.find(currentSuccessor) == currentNode->successors.end() ) {
        map< string, Event* >::const_iterator j = parsedOG->events.find($2);
        if ( j != parsedOG->events.end() ) {
            Event* successorEvent= j->second;
            currentNode->successors[currentSuccessor].push_back(successorEvent);
        } else {
            og_yyerror("read a successor with unknown event");
            exit(EXIT_FAILURE);
        }
    //} else {
    //    og_yyerror("read a successor node twice");
    //    return EXIT_FAILURE;
    //}
    
    free($2);
  }
;
