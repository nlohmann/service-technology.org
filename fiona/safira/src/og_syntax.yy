/*****************************************************************************\
 Safira -- Implementing a Set Algebra for Service Behavior

 Copyright (c) 2010 Kathrin Kaschner

 Safira is free software: you can redistribute it and/or modify it under the
 terms of the GNU Affero General Public License as published by the Free
 Software Foundation, either version 3 of the License, or (at your option)
 any later version.

 Safira is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
 more details.

 You should have received a copy of the GNU Affero General Public License
 along with Safira.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

%{
#include <iostream>
#include <map>
#include <string>
#include <list>

#include "helpers.h"
#include "Node.h"
#include "Formula.h"
#include "Graph.h"


using namespace std;

template<typename TKey, typename TValue>
void printMap(map<TKey,TValue> *theMap);
void printNodes(map<int, Node*> *nodeMap);

extern char* og_yytext;
extern int og_yylex();
extern int og_yyerror(char const *msg);

Graph* graph;
extern map<string, int> label2id;
extern map<int, string> id2label;
extern map<int, char> inout;

string identTmp;
Node *currentNode;

// extern int currentIdPos;
// int firstLabelId; //all labels including tau
//int firstInputId; //input labels
//int firstOutputId;//output labels

//extern int lastLabelId;
//int lastInputId;
//int lastOutputId; 

bool initialNodesDecl = false;
bool expliciteTauLoops = false;

%}

%name-prefix="og_yy"
%error-verbose
%token_table
%defines

%token KEY_NODES 
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_INITIALNODES KEY_GLOBALFORMULA KEY_TAULOOPS
%token COMMA COLON SEMICOLON IDENT ARROW NUMBER
%token KEY_TRUE KEY_FALSE KEY_FINAL BIT_F BIT_S
%token LPAR RPAR

%union {
    char *str;
    int value;
    Formula *f;
//    Node *n;   
}

%type <value> NUMBER
%type <str>   IDENT
//%type <n>   node
%type <f>     formula
%type <f>     gformula
%type <f>	  annotation


%left OP_OR
%left OP_AND
%left OP_NOT

%start og
%%

 
og:
  opt_tauloops 
  KEY_INTERFACE 
  input {/*firstInputId = firstLabelId+1; lastInputId = currentIdPos;*/}
  output{/*firstOutputId = lastInputId + 1; lastOutputId = currentIdPos; lastLabelId = currentIdPos;*/} /*{printMap(&label2id);}*/  
  initialnodes 
  globalformula 
  KEY_NODES nodes //{printNodes(&nodes);}
;

opt_tauloops: /* empty */
             | KEY_TAULOOPS {expliciteTauLoops = true;}

input:
  /* empty */
| KEY_INPUT identlist_in SEMICOLON 
;


output:
  /* empty */
| KEY_OUTPUT identlist_out SEMICOLON
;

initialnodes:
  /* empty */ {initialNodesDecl = false;}
| KEY_INITIALNODES initialnodelist SEMICOLON {initialNodesDecl = true;}
;  

globalformula:
   /*empty*/  {graph->globalFormula = new FormulaTrue(); graph->globalFormulaAsString = graph->globalFormula->toString();}
| KEY_GLOBALFORMULA gformula SEMICOLON {graph->globalFormula = $2; graph->globalFormulaAsString = graph->globalFormula->toString(); }

initialnodelist:
  NUMBER {graph->addInitialNode($1);}
| initialnodelist COMMA NUMBER {graph->addInitialNode($3);}
		
identlist_in:
  /* empty */
| IDENT { 
		  addLabel(string($1), '?');
		  free($1);
		  } 
| identlist_in COMMA IDENT { 
          addLabel(string($3), '?');
		  free($3);
		  } 
;

identlist_out:
  /* empty */
| IDENT { 
		  addLabel(string($1), '!');
		  free($1);
		  } 
| identlist_out COMMA IDENT { 
          addLabel(string($3), '!');
		  free($3);
		  } 
;


nodes:
  node    
| nodes node    
;


node:
  NUMBER 
  annotation  
  	{ if (!initialNodesDecl) {
  	    graph->addInitialNode($1); 
  	    initialNodesDecl = true; }
  	  if(graph->nodes.find($1) == graph->nodes.end()){
  		currentNode = new Node($2,$1);
  		graph->nodes[$1] = currentNode; } 
  	  else {
  	  	currentNode = graph->nodes[$1];
  	  	currentNode->formula = $2; } 
    } 
  successors 
    { if (!expliciteTauLoops)  // if tau loops are implicite
         currentNode->addEdge(label2id["tau"], currentNode);
    } 
;  


annotation: COLON formula {$$=$2;}
;


gformula:
  LPAR gformula RPAR
  	{ $$ = $2; }
| gformula OP_AND gformula
	{ $$ = new FormulaAND($1, $3); }
| gformula OP_OR gformula
	{ $$ = new FormulaOR($1, $3); }
| OP_NOT gformula
	{ $$ = new FormulaNOT($2); }
| KEY_FINAL
	{ $$ = new FormulaFinal(); }
| KEY_TRUE
	{ $$ = new FormulaTrue();}
| KEY_FALSE
	{ $$ = new FormulaFalse(); }
| NUMBER
	{ $$ = new FormulaNUM($1);
	  graph->lits.insert($1); 
	}
;


formula:
  LPAR formula RPAR
  	{ $$ = $2; }
| formula OP_AND formula
	{ $$ = new FormulaAND($1, $3); }
| formula OP_OR formula
	{ $$ = new FormulaOR($1, $3); }
| OP_NOT formula
	{ $$ = new FormulaNOT($2); }
| KEY_FINAL
	{ $$ = new FormulaFinal(); }
| KEY_TRUE
	{ $$ = new FormulaTrue(); }
| KEY_FALSE
	{ $$ = new FormulaFalse(); }
| IDENT 
	{ if (label2id.find(string($1)) == label2id.end()){
	  	 og_yyerror("could not find the literal");}
	  $$ = new FormulaLit(label2id[string($1)]); 
	  free($1);}
;



successors:
  /* empty */
| successors IDENT 
  	{ identTmp = $2;
  	  if (label2id.find(string($2)) == label2id.end()) 
		og_yyerror("could not find label of edge"); 
	  } 
				
  ARROW NUMBER
  	{ int l = label2id[identTmp];
  	  if (graph->nodes.find($5) == graph->nodes.end()) {
  	  	graph->nodes[$5] = new Node(NULL,$5);
  	  }
  	  currentNode->addEdge(l, graph->nodes[$5]);
  	  //printf("Kante %d - %s -> %d added \n", currentNode->id, identTmp.c_str(), nodes[$5]->id);  
  	  free($2);
  	}
  	
;
%%

template<typename TKey, typename TValue>
void printMap(map<TKey,TValue> *theMap){
    for (typename map<TKey, TValue>::const_iterator n = theMap->begin(); n != theMap->end(); ++n){
		cout << "Key: " << n->first << "   Value: " << n->second << endl;
	}
}



