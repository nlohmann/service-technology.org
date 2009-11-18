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

//Mapping of node ID to it's color

std::map<unsigned int, char> nodeColor;

//Initial node ID
int initialNode;

//Helper variables
std::stringstream strStream;
std::stringstream strStreamOld;

//Mapping of labels to their prefixes

std::map<std::string, char> labelPrefix;
char currentSection = ' ';

%}

%name-prefix="og_yy"
%error-verbose
%token_table
%defines

%token KEY_NODES KEY_TRANSITIONS KEY_INITIALNODE
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS KEY_RED KEY_BLUE KEY_FINALNODE KEY_INITIAL
%token COMMA COLON DOUBLECOLON SEMICOLON IDENT ARROW NUMBER
%token KEY_TRUE KEY_FALSE KEY_FINAL BIT_F BIT_S BIT_T
%token LPAR RPAR

%union {
    char* str;
    unsigned int value;
    char ch;
}

%type <value> NUMBER
%type <str>   IDENT 
%type <ch>    color

%left OP_OR
%left OP_AND
%left OP_NOT

%start og
%%

og:
   KEY_INTERFACE input output synchronous KEY_NODES nodes

  {
	//Finished parsing, write output

	//Create invisible node (in order to mark the initial state)
	(*outStream) << "INIT" << nodes[0] <<  " [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"]\n";     

	//For all nodes...
	for(int i=0;i<nodes.size();++i){			
		//List nodes
		if(nodeAnnotation[nodes[i]] == " INITIAL " || nodeAnnotation[nodes[i]] == " FINAL "){
			if(nodeAnnotation[nodes[i]] == " INITIAL "){
				(*outStream) << nodes[i] << " [label=\" \"]\n";
				//Create invisible node (in order to mark the initial state)
				(*outStream) << "INIT" << nodes[i] <<  " [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"]\n";   
  				//Mark initial state
				(*outStream) << "INIT" << nodes[i] << " -> " << nodes[i] << " [minlen=\"0.5\"]" << "\n";
			}				
			if(nodeAnnotation[nodes[i]] == " FINAL "){
				(*outStream) << nodes[i] << " [label=\" \" peripheries=2]\n";
			}
		}
		else
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

/*Syntax for old format*/
| KEY_INTERFACE input output synchronous nodesOld

  {
	for(int i=0;i<nodes.size();++i){			
		//List nodes		
		(*outStream) << nodes[i] << " [label=\"" << nodeAnnotation[nodes[i]] << "\"";
		if(nodeColor[nodes[i]]=='b' || nodeColor[nodes[i]]=='r'){
			(*outStream) << ",style= \"filled\"";
			if(nodeColor[nodes[i]]=='b')
				(*outStream) << ",color=\"blue\"]\n";
			else
				(*outStream) << ",color=\"red\"]\n";		
		}
		else
			(*outStream) << "]\n";
	}

  }
 
 initialnode transitions	

  {
	//Create invisible node (in order to mark the initial state)
	(*outStream) << "INIT" << initialNode <<  " [label=\"\" height=\"0.01\" width=\"0.01\" style=\"invis\"]\n";   
  	//Mark initial state
	(*outStream) << "INIT" << initialNode << " -> " << initialNode << " [minlen=\"0.5\"]" << "\n";
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
			case 's': labelPrefix[$1] = '#';
		}	
	
	}
| identlist COMMA IDENT
	{
		switch(currentSection){
			case 'i': labelPrefix[$3] = '?'; break;
			case 'o': labelPrefix[$3] = '!'; break;
			case 's': labelPrefix[$3] = '#';
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

nodesOld: 
  KEY_NODES nodes_list SEMICOLON
;


nodes_list:
  /* empty */
| nodeOld 
| nodes_list COMMA nodeOld
;


nodeOld:
  NUMBER
	
	{
		//Store current node and map its ID to its annotation
		nodes.push_back($1);
	}
	
| NUMBER COLON formula
	
	{
		nodes.push_back($1);
		nodeAnnotation[$1] = strStreamOld.str();
		strStreamOld.str("");
	}
	
| NUMBER COLON KEY_FINALNODE
		
	{
		nodes.push_back($1);
		nodeAnnotation[$1] = strStreamOld.str();
		strStreamOld.str("");
	}
	
| NUMBER COLON formula COLON color
		
	{
		nodes.push_back($1);
		nodeAnnotation[$1] = strStreamOld.str();
		nodeColor[$1] = $5;
		strStreamOld.str("");
	}
	
| NUMBER COLON formula COLON KEY_FINALNODE
		
	{
		nodes.push_back($1);
		nodeAnnotation[$1] = strStreamOld.str();
		strStreamOld.str("");
	}

| NUMBER COLON formula COLON color COLON KEY_FINALNODE
	
	{
		nodes.push_back($1);
		nodeAnnotation[$1] = strStreamOld.str();
		nodeColor[$1] = $5;
		strStreamOld.str("");
	}

;

color: 
	{$$ = ' ';}
| KEY_BLUE {$$ = 'b';}
| KEY_RED {$$ = 'r';}
;

initialnode:
  KEY_INITIALNODE NUMBER SEMICOLON {initialNode = $2;}
;


transitions:
  KEY_TRANSITIONS transitions_list SEMICOLON
;


transitions_list:
  /* empty */
| transition
| transitions_list COMMA transition
;


transition:
  NUMBER ARROW NUMBER COLON IDENT 
  	{
		*(outStream) << $1 << "->" << $3 << "[label=\"" << $5 << "\"]\n";
  	}
;

annotation:
  /* empty */
| COLON formula
| DOUBLECOLON BIT_S {strStream << "S";}
| DOUBLECOLON BIT_F {strStream << "F";}
| DOUBLECOLON BIT_T {strStream << "T";}

;


formula:
  LPAR {strStream << "("; strStreamOld << "(";}
  formula
  RPAR {strStream << ")"; strStreamOld << ")";}

| formula 
  OP_AND {strStream << " &and; "; strStreamOld << " &and; ";} 
  formula 

| formula 
  OP_OR  {strStream << " &or; "; strStreamOld <<  " &or; ";} 
  formula 

| OP_NOT  {strStream << " &not; "; strStreamOld << " &not; ";} 
  formula 

| KEY_FINAL {strStream << " FINAL "; strStreamOld << " FINAL ";}

| KEY_TRUE {strStream << " TRUE "; strStreamOld << " TRUE ";}

| KEY_FALSE {strStream << " FALSE "; strStreamOld << " FALSE ";}

| KEY_INITIAL {strStream << " INITIAL "; strStreamOld << " INITIAL ";}

| IDENT 
	{
		 if(args_info.noPrefix_given)
			strStream << $1;
		 else 
			strStream << labelPrefix[$1] << $1;
		 strStreamOld << $1;
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




