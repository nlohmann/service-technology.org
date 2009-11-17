/*****************************************************************************\
 Fiona2wendy -- compiling Fiona OGs to Wendy OGs

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 Fiona2wendy is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Fiona2wendy is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Fiona2wendy.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


%{
#include "config.h"
#include "verbose.h"
#include <iostream>
#include <map>
#include <set>
#include <string>

/// output stream - set in main.cc
extern std::ostream* myOut;

/// from flex
extern char* yytext;
extern int yylex();
extern int yyerror(char const *msg);
extern int colonCount;

// data structures
// input events
std::set<std::string> inputs;
// output events
std::set<std::string> outputs;
// whether reading input events
bool inputEvent;
// ID of initial node (unmapped)
unsigned int initialNode;
// mapping of old IDs to new IDs
std::map<unsigned int, unsigned int> IDMapping;
// map node IDs
unsigned int mapNode(unsigned int);
// empty node's ID
unsigned int * emptyNode;
// finds the empty node
void findEmpty();
// create empty node if none present
void createEmpty();
// mapping of nodes to formulae
std::map<unsigned int, std::string> formulae;
// mapping of nodes to successors
std::map<unsigned int, std::map<std::string, unsigned int> > succ;
// print a node
void printNode(unsigned int);


%}

%error-verbose
%token_table
%defines

%token K_INTERFACE K_INPUT K_OUTPUT K_NODES K_INITIALNODE K_TRANSITIONS
%token COMMA SEMICOLON COLON ARROW
%token K_FINAL K_TRUE K_FALSE SEND RECEIVE LPAR RPAR 
%token IDENT NUMBER

%left OP_AND OP_OR

%union {
    char *str;
    unsigned int value;
    std::string *phi;
}

%type <value> NUMBER
%type <str>   IDENT 
%type <phi>   formula

%start og


%%

og: 
  K_INTERFACE
  { 
    colonCount = 0; // initialize for flex
    emptyNode = NULL;
    inputEvent = true;
    (*myOut) << "\nINTERFACE\n  INPUT\n    ";
  }
  K_INPUT identlist SEMICOLON
  { 
    (*myOut) << ";\n  OUTPUT\n    ";
    inputEvent = false;
  }
  K_OUTPUT identlist SEMICOLON
  { (*myOut) << ";\n\nNODES\n"; }
  K_NODES nodes SEMICOLON
  K_INITIALNODE NUMBER SEMICOLON
  { initialNode = $15; }
  K_TRANSITIONS transitions SEMICOLON
  {
    findEmpty();

    // print initial node
    printNode(initialNode);
    for(std::map<unsigned int, std::string>::iterator n = formulae.begin();
         n != formulae.end(); ++n)
    {
      if( (n->first == initialNode) || (n->first == *emptyNode) )
        continue;

      printNode(n->first);
    }
    printNode(*emptyNode);
    delete emptyNode;
    (*myOut) << std::endl << std::flush;
  }
;

identlist:
  /* empty */
| IDENT
  { 
    if(inputEvent)
      inputs.insert($1);
    else
      outputs.insert($1);
    (*myOut) << $1;
    free($1);
  }
| identlist COMMA IDENT
  {
    if(inputEvent)
      inputs.insert($3);
    else
      outputs.insert($3);
    (*myOut) << ", " << $3;
    free($3);
  }
;

nodes:
  node
| nodes COMMA node
;

node:
  NUMBER COLON formula COLON
  {
    formulae[$1] = *$3;
    delete $3;
  }
| NUMBER COLON formula COLON COLON
  {
    formulae[$1] = *$3;
    delete $3;
  }
;

formula:
  LPAR formula RPAR
  {
    *$2 = "(" + (*$2 ) + ")";
    $$ = $2;
  }
| formula OP_AND formula
  {
    (*$1) += " * " + (*$3);
    delete $3;
    $$ = $1;
  }
| formula OP_OR formula
  {
    (*$1) += " + " + (*$3);
    delete $3;
    $$ = $1;
  }
| K_FINAL
  { $$ = new std::string("final"); }
| K_TRUE
  { $$ = new std::string("true"); }
| K_FALSE
  { $$ = new std::string("false"); }
| SEND IDENT
  {
    $$ = new std::string($2);
    delete $2;
  }
| RECEIVE IDENT
  {
    $$ = new std::string($2);
    delete $2;
  }
;

transitions:
  transition
| transitions COMMA transition
;

transition:
  NUMBER ARROW NUMBER COLON SEND IDENT
  {
    succ[$1][$6] = $3;
    free($6);
  }
| NUMBER ARROW NUMBER COLON RECEIVE IDENT
  {
    succ[$1][$6] = $3;
    free($6);
  }
;


%%


/*!
 * \brief map node IDs
 * 
 * \note only call, AFTER finding the final node
 */
unsigned int mapNode(unsigned int n)
{
  static unsigned int nextFree = 1;
  if(n == *emptyNode)
    return 0;

  if(IDMapping[n] == 0)
    IDMapping[n] = nextFree++;

  return IDMapping[n];
}

/*!
 * \brief finds the empty node
 */
void findEmpty()
{
  for(std::map<unsigned int, std::string>::iterator it = formulae.begin();
       it != formulae.end(); ++it)
  {
    if(it->second == "true")
    {
      bool abort = false;
      for(std::set<std::string>::iterator i = inputs.begin();
           i != inputs.end(); ++i)
      {
        if( (succ[it->first].find(*i) == succ[it->first].end()) || // no successor by this event
            (succ[it->first][*i] != it->first) ) // successor is not this node
        {
          abort = true; // this is not the empty node
          break;
        }
      }

      if(abort)
        continue;

      for(std::set<std::string>::iterator o = outputs.begin();
           o != outputs.end(); ++o)
      {
        if( (succ[it->first].find(*o) == succ[it->first].end()) || // no successor by this event
            (succ[it->first][*o] != it->first) ) // successor is not this node
        {
          abort = true; // this is not the empty node
          break;
        }
      }

      if(!abort)
      {
        emptyNode = new unsigned int(it->first);
        break;
      }
    }
  }

  if(emptyNode == NULL)
    createEmpty();
}

/*!
 * \brief create empty node if none present
 */
void createEmpty()
{
  for(unsigned int i = 0;;++i)
  {
    if(formulae.find(i) == formulae.end()) // find first unused "old" ID
    {
      emptyNode = new unsigned int(i); // set empty node
      formulae[i] = "true"; // set formula
      // set successors
      for(std::set<std::string>::iterator s = inputs.begin();
           s != inputs.end(); ++s)
      {
        succ[i][*s] = i;
      }
      for(std::set<std::string>::iterator s = outputs.begin();
           s != outputs.end(); ++s)
      {
        succ[i][*s] = i;
      }

      break; // quit loop
    }
  }

  // link other nodes to empty node
  for(std::map<unsigned int, std::string >::iterator it = formulae.begin();
       it != formulae.end(); ++it)
  {
    for(std::set<std::string>::iterator i = inputs.begin();
         i != inputs.end(); ++i)
    {
      if(succ[it->first].find(*i) == succ[it->first].end())
        succ[it->first][*i] = *emptyNode;
    }
  }

  message("empty-node created");
}

/*!
 * \brief print a node
 */
void printNode(unsigned int n)
{
  (*myOut) << "  " << mapNode(n) << " : " << formulae[n] << "\n";
  for(std::map<std::string, unsigned int>::iterator s = succ[n].begin();
       s != succ[n].end(); ++s)
  {
    (*myOut) << "    " << s->first << " -> " << mapNode(s->second) << "\n";
  }
}

