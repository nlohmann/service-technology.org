/*****************************************************************************\
 Sayo -- Service Automata Yielded from Operating guidelines

 Copyright (C) 2009  Christian Sura <christian.sura@uni-rostock.de>

 Sayo is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Sayo is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Sayo.  If not, see <http://www.gnu.org/licenses/>. 
\*****************************************************************************/


%{
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "config.h"

using std::map;
using std::string;
using std::vector;

/// output stream - set in main.cc
extern std::ostream* myOut;


/// mapping of node IDs (OG->SA)
map<unsigned int,unsigned int> nodes_;

/// mapping of input label to index of input labels and inputSockets
map<string, unsigned int> inputIndex;

/// vector of input labels
vector<string> inputLabels;

/// input sockets; stores successors on input events
unsigned int* inputSockets;

/// vector of output labels
vector<string> outputLabels;


/// from flex
extern char* og_yytext;
extern int og_yylex();
extern int og_yyerror(char const *msg);

/*!
 * \brief gets new node ID
 *
 * Each node has a unique ID. This function generates "fresh" IDs,
 * i.e. IDs, that are not yet used in the generated service automaton.
 * Node 0 is the Node, all transitions, that have to lead to a deadlock,
 * will lead to, with the only successor 1, which has no successors.
 * These nodes are generated at the beginning, so the first free
 * ID is 2 and after each call of newNodeID() 
 * the next unused ID will be i+1.
 */
unsigned int newNodeID()
{
  static unsigned int i=2; // initiate i with 2
  return i++; // return i and increment for the next call
}

/*!
 * \brief map OG node ID to SA node ID
 *
 * Each node in the operating guideline and the generated
 * service automaton has a unique ID, but both differs from
 * each other. If OG node n has no corresponding node yet,
 * a new ID will be requested. Otherwise the already mapped
 * ID will be returned.
 */
unsigned int mapNode(unsigned int n)
{
  if(nodes_[n] == 0)          // if node is not mapped, yet
    nodes_[n] = newNodeID();  // get new ID and map
  return nodes_[n];           // return corresponding ID
}

/// type of read ident
enum IdentType 
{
  T_INPUT, 
  T_OUTPUT
}; 
/// global flag, determining whether we are reading input or output labels
IdentType identType;

/// global flag, for writing the initial state
bool initialState;

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
    char flag;
}

%type <value> NUMBER
%type <str>   IDENT
%type <flag>  annotation

%left OP_OR
%left OP_AND
%left OP_NOT

%start og
%%


og:
  { 
    /* 
     * Since the mapping of input labels to indices of inputLabels
     * return 0 if the label is not found, labels have to begin
     * at index 1, so a dummy has to be pushed.
     */
    inputLabels.push_back("");

    // next read state will be initial
    initialState = true;
  }

  KEY_INTERFACE input output synchronous 
  KEY_NODES
 
  {
    // finished parsing interface

    // write interface
    (*myOut) << "INTERFACE\n  INPUT ";

    // write input
    for(int i=1; i<inputLabels.size()-1; ++i)
      (*myOut) << inputLabels[i] << ", ";
    if(inputLabels.size() > 1)
      (*myOut) << inputLabels[inputLabels.size()-1];
    (*myOut) << ";\n  OUTPUT ";

    // write output
    for(int i=0; i<outputLabels.size()-1; ++i)
      (*myOut) << outputLabels[i] << ", ";
    if(!outputLabels.empty())
      (*myOut) << outputLabels[outputLabels.size()-1];
    (*myOut) << ";\n\nNODES\n";

    // now the amount of input labels is known, so the array can be created
    inputSockets = new unsigned int[inputLabels.size()];

    // write dead-end nodes
    (*myOut) << "  0\n    TAU -> 1\n\n"
             << "  1\n\n";
  }
  
  nodes
  
  {
    // cleanup
    delete inputSockets;
  }
;


input:
  /* empty */
| KEY_INPUT { identType = T_INPUT; /* now reading input labels */ }
  identlist SEMICOLON
;


output:
  /* empty */
| KEY_OUTPUT { identType = T_OUTPUT; /* now reading output labels */}
  identlist SEMICOLON
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS
  {
    // don't know what to do with synchronous transitions
    og_yyerror("synchronous communication is not supported");
    return EXIT_FAILURE;
  }
  identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
  {
    switch(identType) // what type of label is read
    {
      case T_INPUT: // reading input, writing output
      {
        string tmpStr = $1;
        outputLabels.push_back(tmpStr);
        break;
      }
      case T_OUTPUT: // reading output, writing input
      {
        string tmpStr = $1;
        
        /* 
         * The recent size of inputLabels will be the index
         * of the next inserted label.
         */
        inputIndex[tmpStr] = inputLabels.size(); 
        inputLabels.push_back(tmpStr);
        break;
      }
    }

    // since the lexer uses strdup() to copy the ident, we have to free it
    free($1);
  }
| identlist COMMA IDENT
  {
    switch(identType) // what type of label is read
    {
      case T_INPUT: // reading input, writing output
      {
        string tmpStr = $3;
        outputLabels.push_back(tmpStr);
        break;
      }
      case T_OUTPUT: // reading output, writing input
      {
        string tmpStr = $3;
        
        /* 
         * The recent size of inputLabels will be the index
         * of the next inserted label.
         */
        inputIndex[tmpStr] = inputLabels.size();
        inputLabels.push_back(tmpStr);
        break;
      }
    }

    // since the lexer uses strdup() to copy the ident, we have to free it
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
    // read a node, write corresponding node
    if(initialState)
    {
      (*myOut) << "  " << mapNode($1) << ": INITIAL\n";
      initialState = false;
    }
    else
    {
      (*myOut) << "  " << mapNode($1) << "\n";
    }

    /*
     * Set node 0 as the successor of each input event.
     * If there exists a successor on an event in the OG,
     * the successor at this event will be overwritten with
     * the corresponding node when reading the successors.
     */
    for(int i=0; i<inputLabels.size(); ++i)
      inputSockets[i] = 0;
  }

  annotation successors
  
  {
    /*
     * Complete receive arcs, i.e. for each input event
     * still leading to node 0 write this transition
     * to the outpur stream.
     */
    for(int i=1; i<inputLabels.size(); ++i)
    {
      if(inputSockets[i] == 0)
      {
        (*myOut) << "    " << inputLabels[i] << " -> 0\n";
      }
    }

    /*
     * Evaluate the char returned by annotation:
     * If the s- or f-bit is set, copy this node and all successors
     * on each input event, and add a TAU transition to the copy. 
     * If the f bit is set, the copy has to be final.
     */
    switch($3)
    {
      case 's':
      case 'f':
      {
        // get new ID for the copy
        unsigned int newNode = newNodeID(); 
        
        // add TAU transition
        (*myOut) << "    TAU -> " << newNode << "\n\n" 

        // copy node
                 << "  " << newNode;

        // destine, whether the copy has to be final
        if($3 == 'f')
          (*myOut) << " : FINAL";
        
        (*myOut) << "\n";

        // copy input-successors
        for(int i=1; i<inputLabels.size(); ++i)
          (*myOut) << "    " << inputLabels[i] << " -> "
                   << inputSockets[i] << "\n";
      }
      default: /*do nothing*/ ;
    }
   
    (*myOut) << "\n";
  }
;


annotation:
  /* empty */    { $$ = '-'; }
| COLON formula
  {
    // parsing 2-bit OGs there should be no formula
    og_yyerror("read a formula; only 2-bit annotations are supported");
    return EXIT_FAILURE;
  } 
| DOUBLECOLON BIT_S    { $$ = 's'; }
| DOUBLECOLON BIT_F    { $$ = 'f'; }
;


formula:
  LPAR formula RPAR
| formula OP_AND formula
| formula OP_OR formula
| OP_NOT formula
| KEY_FINAL
| KEY_TRUE
| KEY_FALSE
| IDENT 
  { 
    // since lexer uses strdup() to copy the ident it has to be freed
    free($1); 
  }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    // read an OG successor on an event
    // write corresponting SA successor on the same event
    (*myOut) << "    " << $2 << " -> " << mapNode($4) << "\n";
    
    // store successor (i.e. overwrite link to node 0)
    inputSockets[inputIndex[$2]] = mapNode($4);
    
    // since lexer uses strdup() to copy the ident it has to be freed
    free($2);
  }
;
