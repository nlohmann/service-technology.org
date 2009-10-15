%{
#include <iostream>
#include "config.h"
#include <map>
#include <vector>
#include <utility>

//Result of lexical analyis
extern char* og_yytext;
extern int og_yylex();
extern int og_yyerror(char const *msg);

//Output stream defined and used in main.cc
extern std::ostream* outStream;

//Helper variables
unsigned int currentNode;
char currentAnnotation;

//List of nodes

std::vector<unsigned int> nodes;

//Remember whether a node has successors
std::map<unsigned int, bool> hasSuccessors;

//Mapping of node ID to the node's annotation

std::map<unsigned int, char> nodeAnnotation;

//Mapping of node ID to a list of the successors of this node

std::map<unsigned int, std::vector<std::pair<char*, unsigned int> > > nodeSuccessors;

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
    char* str;
    unsigned int value;
    char bit;
}

%type <value> NUMBER
%type <str>   IDENT
%type <bit>   annotation


%left OP_OR
%left OP_AND
%left OP_NOT

%start og
%%

/*Everything concerning the interface of the operating guideline can
safely be skipped as it is not relevant for creating
the dot output */

og:

  {
	//Write keyword to stream
	(*outStream) << "digraph{\n\n";
      
  }

  KEY_INTERFACE input output synchronous KEY_NODES nodes

  {
	//Finished parsing, write output

	//For all nodes...
	for(int i=0;i<nodes.size();++i){
		//...check whether the node has successors...
		if(hasSuccessors[nodes[i]]){
			std::vector<std::pair<char*, unsigned int> > successors = nodeSuccessors[nodes[i]];
			//... if yes, write the node's ID, node's annotation and all links to its successors to the stream...
			for(int j=0;j<successors.size();++j){
				(*outStream) << nodeAnnotation[nodes[i]] << nodes[i];
				(*outStream) << " -> " << nodeAnnotation[successors[j].second] << successors[j].second;
				(*outStream) << " [label= " << successors[j].first << "]\n";
			}
		}
		else{
			//... if not, just write the node's ID and its annotation to the stream
			(*outStream) << nodeAnnotation[nodes[i]] << nodes[i] <<"\n";
		}

	}
	//Finish writing output
	(*outStream) << "\n}";

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
| IDENT {free($1);}
| identlist COMMA IDENT {free($3);}
;



nodes:
  node
| nodes node
;


node:
  NUMBER annotation

  {
	//The empty node can be omitted
	if($1 == 0)
		break;
	//Store current node and map its ID to its annotation
	nodes.push_back($1);
	currentNode = $1;
	nodeAnnotation[$1] = $2;
	hasSuccessors[$1] = false;


  }

  successors
;


annotation:
  /* empty */ {$$ = ' ';}
| COLON formula {og_yyerror("read a formula; only 2-bit annotations are supported");}
| DOUBLECOLON BIT_S {$$ = 'S';}
| DOUBLECOLON BIT_F {$$ = 'F';}

;


formula:
  LPAR formula RPAR
| formula OP_AND formula
| formula OP_OR formula
| OP_NOT formula
| KEY_FINAL
| KEY_TRUE
| KEY_FALSE
| IDENT {free($1);}
;


successors:
  /* empty */ 
| successors IDENT ARROW NUMBER

  {	
	//All links to the empty node can be skipped
	if($4 == 0)
		break;
	//Store list of successors
	nodeSuccessors[currentNode].push_back(std::pair<char*, unsigned int>($2, $4)); 	
	hasSuccessors[currentNode] = true;
  
  }
;




