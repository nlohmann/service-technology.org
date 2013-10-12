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
extern char* rg_yytext;
extern int rg_yylex();
extern int rg_yyerror(char const *msg);

// the command line parameters (declared in main.cc)
extern gengetopt_args_info args_info;

//Output stream defined and used in main.cc
extern std::stringstream outStream;

unsigned int currentState = 0;
%}

%name-prefix="rg_yy"
%error-verbose
%token_table
%expect 0
%defines

%token KW_STATE KW_LOWLINK KW_SCC COLON COMMA ARROW NUMBER NAME

%union {
  unsigned int val;
  char* str;
}

%type <val> NUMBER
%type <str> NAME

%%


rg:
  {
	//Finished parsing, write output
   
        //Write keyword to stream
  	outStream << "digraph{\n\n";
  	//Use Helvetica
  	outStream << "edge [fontname=Helvetica fontsize=10]\n";
  	outStream << "node [fontname=Helvetica fontsize=10]\n";  

  }
  states
  {
  	outStream << "\n}";
  }
;

states:
  state
| states state
;

state:
  KW_STATE NUMBER
  {
    outStream << "  " << $2 << " [label=\"" << $2 << "\\n["; 
    currentState = $2;
  }
   lowlink scc markings_or_transitions
;


scc:
  /* empty */
| KW_SCC scc_members
;

scc_members:
  scc_member
| scc_members scc_member
;

scc_member:
  NUMBER
;

lowlink:
  KW_LOWLINK NUMBER
;

markings_or_transitions:
  /* empty */
  {
    outStream << "]\"]" << std::endl;
  }
| markings
  {
    outStream << "]\"]" << std::endl;
  }
| transitions
| markings 
  {
    outStream << "]\"]" << std::endl;
  }
  transitions
;

markings:
  marking
| markings COMMA { outStream << ", "; } marking
;

marking:
  NAME COLON NUMBER
  {
    if ( $3 > 1 )
    {
      outStream << $1 << ":" << $3;
    }
    else
    {
      outStream << $1;
    }
  }
;

transitions:
  transition
| transitions transition
;

transition:
  NAME ARROW NUMBER
  {
    outStream << "  " << currentState << " -> " << $3 << " [label=\"" << $1 << "\"]" << std::endl;
  }
;

