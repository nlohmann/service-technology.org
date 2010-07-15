%{
#include <cstring>
#include <map>
#include <set>
#include "types.h"
#include "Label.h"
#include "OperatingGuideline.h"

extern char* og_yytext;
extern int og_yylex();
extern int og_yyerror(char const *msg);

extern Label GlobalLabels;

// for current operating guideline
extern std::set<label_id_t> OGInterface;
extern std::map<og_service_index_t, OGMarking*> OGMarkings;
og_service_index_t CurrentMarkingIndex;
std::map<unsigned int, og_service_index_t> OGNumberConverter;

// for current node
og_service_index_t CurrentMarkingNumber;
unsigned SBit;
unsigned FBit;
unsigned TBit;
std::map<label_id_t, std::pair<label_id_t, og_service_index_t> > CurrentSuccessors;

// for current transition
label_id_t CurrentTransitionLabel;
og_service_index_t CurrentTransitionDestination;

// type of read ident
enum IdentType 
{
  T_INPUT, 
  T_OUTPUT
}; 
// global flag, determining whether we are reading input or output labels
IdentType identType;

inline og_service_index_t mapNodeNumber(unsigned int NodeNumber) {
	std::map<unsigned int, og_service_index_t>::iterator it;
	it = OGNumberConverter.find(NodeNumber);
	if (it == OGNumberConverter.end()) {
		// number is new
		OGNumberConverter.insert( std::make_pair(NodeNumber, CurrentMarkingIndex++) );
		return CurrentMarkingIndex - 1; 
	}
	else {
		// number is old
		return it->second;
	}
}

%}

%name-prefix="og_yy"
%error-verbose
%token_table
%defines

%token KEY_NODES
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COMMA COLON DOUBLECOLON SEMICOLON IDENT ARROW NUMBER
%token KEY_TRUE KEY_FALSE KEY_FINAL BIT_F BIT_S BIT_T
%token LPAR RPAR

%union {
    char *str;
    unsigned int value;
}

%type <value> NUMBER
%type <str>   IDENT

%left OP_OR
%left OP_AND
%left OP_NOT

%start og
%%


og:
	{ 
		CurrentMarkingIndex = 0; 
		OGNumberConverter.clear();
	}
  KEY_INTERFACE input output synchronous KEY_NODES nodes
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
	}
	identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
	{
    switch(identType) // what type of label is read
    {
      case T_INPUT: // reading input
      {
        string tmpStr("?"); tmpStr += $1;
				OGInterface.insert(GlobalLabels.mapLabel(tmpStr));
				//assert(OGInterface.find(tmpStr[0] = '!') == OGInterface.end());
				if (OGInterface.find(tmpStr[0] = '!') != OGInterface.end())
					og_yyerror("at least one label is used for input and output");
        break;
      }
      case T_OUTPUT: // reading output
      {
        string tmpStr("!"); tmpStr += $1;
				OGInterface.insert(GlobalLabels.mapLabel(tmpStr));
				//assert(OGInterface.find(tmpStr[0] = '?') == OGInterface.end());
				if (OGInterface.find(tmpStr[0] = '?') != OGInterface.end())
					og_yyerror("at least one label is used for input and output");        
				break;
      }
    }
  }
| identlist COMMA IDENT
	{
    switch(identType) // what type of label is read
    {
      case T_INPUT: // reading input
      {
        string tmpStr("?"); tmpStr += $3;
				OGInterface.insert(GlobalLabels.mapLabel(tmpStr));
				//assert(OGInterface.find(tmpStr[0] = '!') == OGInterface.end());
				if (OGInterface.find(tmpStr[0] = '!') != OGInterface.end())
					og_yyerror("at least one label is used for input and output");
        break;
      }
      case T_OUTPUT: // reading output
      {
        string tmpStr("!"); tmpStr += $3;
				OGInterface.insert(GlobalLabels.mapLabel(tmpStr));
				//assert(OGInterface.find(tmpStr[1] = '?') == OGInterface.end());
				if (OGInterface.find(tmpStr[0] = '?') != OGInterface.end())
					og_yyerror("at least one label is used for input and output");
        break;
      }
    }
  }
;


nodes:
  node
| nodes node
;


node:
  NUMBER 
	{ /*current node*/
		SBit = FBit = TBit = 0;
		CurrentSuccessors.clear();
		CurrentMarkingNumber = mapNodeNumber($1);
	} 
	annotation successors
	/*save current node*/
	{ 
		OGMarking *newMarking = new OGMarking(CurrentSuccessors, FBit, SBit, TBit);
		OGMarkings.insert ( std::make_pair(CurrentMarkingNumber, newMarking) );
	}
;


annotation:
  /* empty */
| COLON formula
	{
    // parsing 3-bit OGs there should be no formula
    og_yyerror("read a formula; only 3-bit annotations are supported");
  } 
| DOUBLECOLON BIT_S { SBit = 1; }
| DOUBLECOLON BIT_F { FBit = 1; }
| DOUBLECOLON BIT_T { TBit = 1; }
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
;


successors:
  /* empty */
| successors IDENT 
	{ 
		string tmpStr("?"); tmpStr += $2;
		if (!GlobalLabels.Name2ID(tmpStr, CurrentTransitionLabel)) {
			tmpStr[0] = '!';			
			assert(GlobalLabels.Name2ID(tmpStr, CurrentTransitionLabel));
		}
	}
	ARROW NUMBER 
	{ 
		CurrentTransitionDestination = mapNodeNumber($5);
		/*save current transition*/		
		CurrentSuccessors.insert( std::make_pair(CurrentTransitionLabel, 
																std::make_pair(CurrentTransitionLabel, CurrentTransitionDestination)) );
  }
;


%%

