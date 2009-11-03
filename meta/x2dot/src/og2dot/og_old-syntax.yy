%{
#include <iostream>
#include "config.h"
#include <map>
#include <vector>
#include <utility>
#include <string.h>
#include <sstream>

//Result of lexical analyis
extern char* og_old_yytext;
extern int og_old_yylex();
extern int og_old_yyerror(char const *msg);

//Output stream defined and used in main.cc
extern std::ostream* outStream;

//List of nodes

std::vector<unsigned int> nodesOld;

//Mapping of node ID to the node's annotation

std::map<unsigned int, std::string> nodeAnnotationOld;

//Mapping of node ID to it's color

std::map<unsigned int, char> nodeColor;


std::stringstream strStreamOld;

%}



%name-prefix="og_old_yy"
%error-verbose
%token_table
%defines

%token key_nodes key_initialnode key_finalnode key_transitions
%token key_interface key_input key_output
%token key_red key_blue
%token comma colon semicolon ident arrow number
%token key_true key_false key_final
%token lpar rpar

%union {
    char *str;
    unsigned int value;
    char ch;
}

%type <value> number
%type <str>   ident
%type <ch>    color;

%left op_or
%left op_and

%start og
%%


og:

  {
	//Write keyword to stream
	(*outStream) << "digraph{\n\n";
	//Use Helvetica
	(*outStream) << "edge [fontname=Helvetica fontsize=10]\n";
	(*outStream) << "node [fontname=Helvetica fontsize=10]\n";

  }	

 interface nodes

  {
	
	for(int i=0;i<nodesOld.size();++i){			
		//List nodes
		(*outStream) << nodesOld[i] << " [label=\"" << nodeAnnotationOld[nodesOld[i]] << "\"";
		if(nodeColor[nodesOld[i]]=='b' || nodeColor[nodesOld[i]]=='r'){
			(*outStream) << ",style= \"filled\"";
			if(nodeColor[nodesOld[i]]=='b')
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

	//Finish writing output
	(*outStream) << "\n}";
	
  }
;


interface:
  /* for backwards compatibility, the interface is optional */
| key_interface key_input places_list semicolon
  key_output places_list semicolon
;


places_list:
  /* empty */
| places_list comma ident
| ident
;


nodes:
  key_nodes nodes_list semicolon
;


nodes_list:
  /* empty */
| node 
| nodes_list comma node
;


node:
  number
	
	{
		//Store current node and map its ID to its annotation
		nodesOld.push_back($1);
	}
	
| number colon formula
	
	{
		nodesOld.push_back($1);
		nodeAnnotationOld[$1] = strStreamOld.str();
		strStreamOld.str("");
	}
	
| number colon key_finalnode
		
	{
		nodesOld.push_back($1);
		nodeAnnotationOld[$1] = strStreamOld.str();
		strStreamOld.str("");
	}
	
| number colon formula colon color
		
	{
		nodesOld.push_back($1);
		nodeAnnotationOld[$1] = strStreamOld.str();
		nodeColor[$1] = $5;
		strStreamOld.str("");
	}
	
| number colon formula colon key_finalnode
		
	{
		nodesOld.push_back($1);
		nodeAnnotationOld[$1] = strStreamOld.str();
		strStreamOld.str("");
	}

| number colon formula colon color colon key_finalnode
	
	{
		nodesOld.push_back($1);
		nodeAnnotationOld[$1] = strStreamOld.str();
		nodeColor[$1] = $5;
		strStreamOld.str("");
	}

;


formula:
  lpar {strStreamOld << "(";}
  formula 
  rpar {strStreamOld << ")";}
| formula 
  op_and {strStreamOld << " &and; ";}  
  formula
| formula 
  op_or {strStreamOld << " &or; ";} 
  formula
| key_final {strStreamOld << " FINAL ";}
| key_true {strStreamOld << " TRUE ";}
| key_false {strStreamOld << " FALSE ";}
| ident {strStreamOld << $1;}
;


color: {$$ = ' ';}
| key_blue {$$ = 'b';}
| key_red {$$ = 'r';}
;


initialnode:
  key_initialnode number semicolon
;


transitions:
  key_transitions transitions_list semicolon
;


transitions_list:
  /* empty */
| transition
| transitions_list comma transition
;


transition:
  number arrow number colon ident 
  	{
		*(outStream) << $1 << "->" << $3 << "[label=\"" << $5 << "\"]\n";
  	}
;


