%{
#include <iostream>
#include <map>
#include <string>
#include <vector>

using std::cerr;
using std::endl;
using std::flush;
using std::map;
using std::string;
using std::vector;

/// output stream
extern std::ostream* myOut;


/// mapping of node IDs (OG->SA)
map<unsigned int,unsigned int> nodes_;

/// mapping of input label to vector index
map<string, unsigned int> inputIndex;

/// vector of input labels
vector<string> inputLabels;

/// input sockets; stores successors on input events
unsigned int* inputSockets;

/// vector of output labels
vector<string> outputLabels;


/// from flex
extern char* ognew_yytext;
extern int ognew_yylex();
extern int ognew_yyerror(char const *msg);

/// gets new node ID
unsigned int newNodeID()
{
  static unsigned int i=2;
  return i++;
}

/// map OG node ID to SA node ID
unsigned int mapNode(unsigned int n)
{
  if(nodes_[n] == 0)
    nodes_[n] = newNodeID();
  return nodes_[n];
}

/// type of read ident
enum IdentType 
{
  T_INPUT, 
  T_OUTPUT
}; 
IdentType identType;

%}

%name-prefix="ognew_yy"
%error-verbose
%token_table
%defines

%token KEY_NODES
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COMMA COLON SEMICOLON IDENT ARROW NUMBER
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
    inputLabels.push_back(""); // inputLabels[0] must be filled 
  }
  KEY_INTERFACE input output synchronous 
  KEY_NODES 
  {
    // write interface
    (*myOut) << "INTERFACE" << endl << "  INPUT ";

    // write input
    for(int i=1; i<inputLabels.size()-1; ++i)
      (*myOut) << inputLabels[i] << ",";
    if(inputLabels.size() > 1)
      (*myOut) << inputLabels[inputLabels.size()-1];
    (*myOut) << ";" << endl << "  OUTPUT ";

    // write output
    for(int i=0; i<outputLabels.size()-1; ++i)
      (*myOut) << outputLabels[i] << ",";
    if(!outputLabels.empty())
      (*myOut) << outputLabels[outputLabels.size()-1];
    (*myOut) << ";" << endl << endl << "NODES" << endl;

    inputSockets = new unsigned int[inputLabels.size()];

    // write dead-end nodes
    (*myOut) << "  0" << endl << "  TAU -> 1" << endl << endl
             << "  1" << endl << endl;
  }
  nodes
;


input:
  /* empty */
| KEY_INPUT { identType = T_INPUT; }
  identlist SEMICOLON
;


output:
  /* empty */
| KEY_OUTPUT { identType = T_OUTPUT; }
  identlist SEMICOLON
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS
  {
    cerr << "WARNING: OG uses synchronous communication. This is not supported." << endl << flush;
    return EXIT_FAILURE;
  }
  identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
  {
    switch(identType)
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
        inputIndex[tmpStr] = inputLabels.size();
        inputLabels.push_back(tmpStr);
        break;
      }
    }
    free($1);
  }
| identlist COMMA IDENT
  {
    switch(identType)
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
        inputIndex[tmpStr] = inputLabels.size();
        inputLabels.push_back(tmpStr);
        break;
      }
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
    (*myOut) << "  " << mapNode($1) << endl; 
    for(int i=0; i<inputLabels.size(); ++i)
      inputSockets[i] = 0;
  }
  annotation successors
  {
    // complete receive arcs
    for(int i=1; i<inputLabels.size(); ++i)
    {
      if(inputSockets[i] == 0)
      {
        (*myOut) << "    " << inputLabels[i] << " -> 0" << endl;
      }
    }

    // evaluate bits
    switch($3)
    {
      case 's':
      case 'f':
      {
        unsigned int newNode = newNodeID();
        (*myOut) << "    TAU -> " << newNode << endl << endl

        // copy node
                 << "  " << newNode;
        if($3 == 'f')
          (*myOut) << " : FINALNODE";
        
        (*myOut) << endl;
        for(int i=1; i<inputLabels.size(); ++i)
          (*myOut) << "    " << inputLabels[i] << " -> "
                   << inputSockets[i] << endl;
      }
      default: /*do nothing*/ ;
    }
   
    (*myOut) << endl;
  }
;


annotation:
  /* empty */    { $$ = '-'; }
| COLON formula  
  {
    cerr << "WARNING: Found a formula. Can only read 2-bit OGs." << endl << flush;
    return EXIT_FAILURE;
  } 
| COLON BIT_S    { $$ = 's'; }
| COLON BIT_F    { $$ = 'f'; }
;


formula:
  LPAR formula RPAR
| formula OP_AND formula
| formula OP_OR formula
| OP_NOT formula
| KEY_FINAL
| KEY_TRUE
| KEY_FALSE
| IDENT { free($1); }
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
  {
    (*myOut) << "    " << $2 << " -> " << mapNode($4) << endl;
    inputSockets[inputIndex[$2]] = mapNode($4);
    free($2);
  }
;


%%

