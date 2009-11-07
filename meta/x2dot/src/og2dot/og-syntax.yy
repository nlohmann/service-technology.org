%{
#include <iostream>
#include "config.h"
#include <map>
#include <vector>
#include <utility>
#include <string.h>
#include <sstream>

#include "cmdline.h"

//Result of lexical analyis
extern char* og_yytext;
extern int og_yylex();
extern int og_yyerror(char const *msg);

// the command line parameters (declared in main.cc)
extern gengetopt_args_info args_info;

//Output stream defined and used in main.cc
extern std::ostream* outStream;

//Helper variables
unsigned int currentNode;
char currentAnnotation;

//List of nodes

std::vector<unsigned int> nodes;

//Mapping of node ID to the node's annotation

std::map<unsigned int, std::string> nodeAnnotation;

//Mapping of node ID to a list of the successors of this node

std::map<unsigned int, std::vector<std::pair<char*, unsigned int> > > nodeSuccessors;

std::stringstream strStream;

//Mapping of labels to their prefixes

std::map<std::string, char> labelPrefix;
char currentSection = ' ';

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
	//Use Helvetica
	(*outStream) << "edge [fontname=Helvetica fontsize=10]\n";
	(*outStream) << "node [fontname=Helvetica fontsize=10]\n";
 	

  }

  KEY_INTERFACE input output synchronous KEY_NODES nodes

  {
	//Finished parsing, write output

	//Create invisible node (in order to mark the initial state)
	(*outStream) << "INIT" << nodes[0] <<  " [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"]\n";     

	//For all nodes...
	for(int i=0;i<nodes.size();++i){			
		//List nodes
		(*outStream) << nodes[i] << " [label=\"" << nodeAnnotation[nodes[i]] << "\"]\n";
		std::vector<std::pair<char*, unsigned int> > successors = nodeSuccessors[nodes[i]];
		//write the node's ID, node's annotation and all links to its successors to the stream...
		for(int j=0;j<successors.size();++j){
			(*outStream) << nodes[i];
			(*outStream) << " -> " << successors[j].second;
			if(args_info.noPrefix_given)
				(*outStream) << " [label= \"" << successors[j].first << "\"]\n";
			else
				(*outStream) << " [label= \"" << labelPrefix[successors[j].first] << successors[j].first << "\"]\n";	
		}
	

	}
	//Mark initial state
	(*outStream) << "INIT" << nodes[0] << " -> " << nodes[0] << " [minlen=\"0.5\"]" << "\n";

	//Finish writing output
	(*outStream) << "\n}";

  }
;



input:
  /* empty */
| KEY_INPUT {currentSection = 'i';}
  identlist SEMICOLON
;


output:
  /* empty */
| KEY_OUTPUT {currentSection = 'o';} 
  identlist SEMICOLON 
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS {currentSection = 's';}
  identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT 
	{
		switch(currentSection){
			case 'i': labelPrefix[$1] = '?'; break;
			case 'o': labelPrefix[$1] = '!'; break;
			case 's': labelPrefix[$1] = '#'; break;
			default: labelPrefix[$1] = ' ';
		}	
	
	}
| identlist COMMA IDENT
	{
		switch(currentSection){
			case 'i': labelPrefix[$3] = '?'; break;
			case 'o': labelPrefix[$3] = '!'; break;
			case 's': labelPrefix[$3] = '#'; break;
			default: labelPrefix[$3] = ' ';
		}	
	
	}	

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
	nodeAnnotation[$1] = strStream.str();
	strStream.str("");
  }

  successors
;


annotation:
  /* empty */
| COLON formula
| DOUBLECOLON BIT_S {strStream << "S";}
| DOUBLECOLON BIT_F {strStream << "F";}

;


formula:
  LPAR {strStream << "(";}
  formula
  RPAR {strStream << ")";}

| formula 
  OP_AND {strStream << " &and; ";} 
  formula 

| formula 
  OP_OR  {strStream << " &or; ";} 
  formula 

| OP_NOT  {strStream << " &not; ";} 
  formula 

| KEY_FINAL {strStream << " FINAL ";}

| KEY_TRUE {strStream << " TRUE ";}

| KEY_FALSE {strStream << " FALSE ";}

| IDENT 
	{
		if(args_info.noPrefix_given)
			strStream << $1;
		else 
			strStream << labelPrefix[$1] << $1;
	}
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
  
  }
;




