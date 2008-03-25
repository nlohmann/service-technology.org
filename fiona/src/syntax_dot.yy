/* dot.y */

%{
// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


#include <stdlib.h>
#include <stdio.h>
#include "dot2tex.h"
#include "GastexGraph.h"
int dot_yyerror(char *s);
int dot_yylex(void);

GasTexGraph* gastexGraph;

GasTexNode dummyNode;			// for temporary storage of node values
GasTexEdge dummyEdge(NULL, NULL);	// for temporary storage of edge values

int i;
bool first_node = true;

%}

// Bison options
%name-prefix="dot_yy"

%union{
    int     	int_val;
    string*	str_val;
}

%start	graph 

%type <str_val>    subgraph 
%type <str_val>    KW_ID
%type <str_val>    stmt
%type <str_val>    attr_stmt
%type <str_val>    attr_list
%type <str_val>    a_list
%type <str_val>    node_stmt
%type <str_val>    node_id
%type <str_val>    port 
%type <str_val>    compass_pt 
%type <str_val>    edge_stmt
%type <str_val>    edgeRHS

%left KW_DIGRAPH
%left KW_GRAPH
%left KW_SUBGRAPH
%left KW_NODE
%left KW_EDGE
%left KW_ID
%token <str_val> KW_STRING
%token <int_val> KW_INT
%left KW_LBRACE
%left KW_RBRACE
%left KW_LBRACKET
%left KW_RBRACKET
%left KW_LPARAN
%left KW_RPARAN
%left KW_COMMA
%left KW_SEMICOLON
%left KW_COLON
%left KW_EQUAL
%left KW_AT
%left KW_EDGEOP
%left KW_STRICT
%left KW_N
%left KW_NE
%left KW_E
%left KW_SE
%left KW_S
%left KW_SW
%left KW_W
%left KW_NW


%%

graph:  /* empty */
        KW_GRAPH KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_GRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_STRICT KW_GRAPH KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_STRICT KW_GRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_DIGRAPH KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_DIGRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
cout << "digraph id { stmt_list }" << endl; 
            }
        | KW_STRICT KW_DIGRAPH KW_LBRACE stmt_list KW_RBRACE
            {
            }
        | KW_STRICT KW_DIGRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
            }
;

stmt_list: /* empty */
        | stmt
            { 
cout << "stmt" << endl; 
            }
        | stmt KW_SEMICOLON
            { 
cout << "stmt;" << endl; 
            }
        | stmt stmt_list 
            { 
cout << "stmt stmt_list" << endl; 
            }
        | stmt KW_SEMICOLON stmt_list 
            { 
cout << "stmt stmt_list;" << endl; 
            }
;

stmt:   /* empty */
        attr_stmt
            {
cout << "attr_stmt" << endl; 
            }
        | node_stmt
            { 
cout << "node_stmt" << endl; 
            }
        | edge_stmt
            { 
cout << "edge_stmt" << endl; 
            }
        | KW_ID KW_EQUAL KW_ID
            { 
cout << "id = id" << endl; 
            }
        | subgraph 
            { 
cout << "subgraph" << endl; 
            }
;

subgraph: /* empty */
        KW_LBRACE stmt_list KW_RBRACE
            {
cout << "{ stmt_list }" << endl; 
            }
        | KW_SUBGRAPH KW_LBRACE stmt_list KW_RBRACE
            {
cout << "subgraph { stmt_list }" << endl; 
            }
        | KW_SUBGRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
cout << "subgraph id { stmt_list }" << endl; 
            }
;
attr_stmt: /* empty */ 
        KW_GRAPH attr_list 
            {
            }
        | KW_NODE attr_list
            { 
            }
        | KW_EDGE attr_list
            { 
            }
;

attr_list: /* empty */
        KW_LBRACKET KW_RBRACKET
            {
            }
        | KW_LBRACKET a_list KW_RBRACKET
            { 
            }
        | KW_LBRACKET KW_RBRACKET attr_list
            { 
            }
        | KW_LBRACKET a_list KW_RBRACKET attr_list
            { 
            }
;

a_list: /* empty */ 
        KW_ID
            {
            }
        | KW_ID KW_EQUAL KW_ID
            {
                if ((*$1) == "label") {
                    dummyNode.label = (*$3).substr(1, (*$3).length() - 2);
                    dummyEdge.label = (*$3).substr(1, (*$3).length() - 2);
                } else if ((*$1) == "pos") {
                    dummyNode.posX = atoi((*$3).substr(1, ($3)->find_first_of(",")).c_str());
                    dummyNode.posY = atoi((*$3).substr(($3)->find_first_of(",") + 1, ($3)->length()).c_str());
                    dummyNode.posX = (int) (dummyNode.posX * scale_factor);
                    dummyNode.posY = (int) (dummyNode.posY * scale_factor);
                } else if ((*$1) == "width") {
                    dummyNode.width = atof((*$3).substr(1, ($3)->length() - 2).c_str());
                } else if ((*$1) == "height") {
                    dummyNode.height = atof((*$3).substr(1, ($3)->length() - 2).c_str());
                } else if ((*$1) == "peripheries") {
		    dummyNode.isFinal = true;
                }
            }
        | KW_ID KW_COMMA 
            {
            }
        | KW_ID a_list
            {
            }
        | KW_ID KW_EQUAL KW_ID KW_COMMA
            {
                if ((*$1) == "label") {
                    dummyNode.label = (*$3).substr(1, (*$3).length() - 2);
                    dummyEdge.label = (*$3).substr(1, (*$3).length() - 2);
                } else if ((*$1) == "pos") {
                    dummyNode.posX = atoi((*$3).substr(1, ($3)->find_first_of(",")).c_str());
                    dummyNode.posY = atoi((*$3).substr(($3)->find_first_of(",") + 1, ($3)->length()).c_str());
                    dummyNode.posX = (int) (dummyNode.posX * scale_factor);
                    dummyNode.posY = (int) (dummyNode.posY * scale_factor);
                } else if ((*$1) == "width") {
                    dummyNode.width = atof((*$3).substr(1, (*$3).length() - 2).c_str());
                } else if ((*$1) == "height") {
                    dummyNode.height = atof((*$3).substr(1, (*$3).length() - 2).c_str());
                } else if ((*$1) == "peripheries") {
		    dummyNode.isFinal = true;
                }
            }
        | KW_ID KW_EQUAL KW_ID a_list 
            {
                if ((*$1) == "label") {
                    dummyNode.label = (*$3).substr(1, (*$3).length() - 2);
                    dummyEdge.label = (*$3).substr(1, (*$3).length() - 2);
                } else if ((*$1) == "pos") {
                    dummyNode.posX = atoi((*$3).substr(1, ($3)->find_first_of(",")).c_str());
                    dummyNode.posY = atoi((*$3).substr(($3)->find_first_of(",") + 1, ($3)->length()).c_str());
                    dummyNode.posX = (int) (dummyNode.posX * scale_factor);
                    dummyNode.posY = (int) (dummyNode.posY * scale_factor);
                } else if ((*$1) == "width") {
                    dummyNode.width = atof((*$3).substr(1, (*$3).length() - 2).c_str());
                } else if ((*$1) == "height") {
                    dummyNode.height = atof((*$3).substr(1, (*$3).length() - 2).c_str());
                } else if ((*$1) == "peripheries") {
		    dummyNode.isFinal = true;
                }
            }
        | KW_ID KW_COMMA a_list
            {
            }
        | KW_ID KW_EQUAL KW_ID KW_COMMA a_list
            {
                if ((*$1) == "label") {
                    dummyNode.label = (*$3).substr(1, (*$3).length() - 2);
                    dummyEdge.label = (*$3).substr(1, (*$3).length() - 2);
                } else if ((*$1) == "pos") {
                    dummyNode.posX = atoi((*$3).substr(1, ($3)->find_first_of(",")).c_str());
                    dummyNode.posY = atoi((*$3).substr(($3)->find_first_of(",") + 1, ($3)->length()).c_str());
                    dummyNode.posX = (int) (dummyNode.posX * scale_factor);
                    dummyNode.posY = (int) (dummyNode.posY * scale_factor);
                } else if ((*$1) == "width") {
                    dummyNode.width = atof((*$3).substr(1, (*$3).length() - 2).c_str());
                } else if ((*$1) == "height") {
                    dummyNode.height = atof((*$3).substr(1, (*$3).length() - 2).c_str());
                } else if ((*$1) == "peripheries") {
		    dummyNode.isFinal = true;
                }
            }
;

node_stmt: /* empty */
        node_id
            {
                GasTexNode* node = new GasTexNode;
                node->id = (*$1);
                if (first_node) {
                    node->isInitial = true;
                    first_node = false;
                }

                gastexGraph->addNode(node);
                dummyNode.isInitial = false;
                dummyNode.isFinal = false; 
            }
        | node_id attr_list        
            {
                GasTexNode* node = new GasTexNode;
                node->id = (*$1); 
                node->label = dummyNode.label;
		node->isFinal = dummyNode.isFinal;
    		node->posX = dummyNode.posX;
    		node->posY = dummyNode.posY;
    		node->width = dummyNode.width;
    		node->height = dummyNode.height;
                if (first_node) {
                    node->isInitial = true;
                    first_node = false;
                }
                
                gastexGraph->addNode(node); 
                dummyNode.isInitial = false;
                dummyNode.isFinal = false; 
            }
;

node_id: /* empty */
        KW_ID 
            {
                dummyNode.id = (*$1);
            }
        | KW_ID port 
            {
                dummyNode.id = (*$1);
            }
;

port: /* empty */
        KW_COLON KW_ID
            {
            }
        | KW_COLON KW_ID KW_COLON compass_pt
            {
            }
        | KW_COLON compass_pt
            {
            }
;

compass_pt: /* empty */
        KW_N {}
        | KW_NE {}
        | KW_E {}
        | KW_SE {}
        | KW_S {}
        | KW_SW {}
        | KW_W {}
        | KW_NW {}
;

edge_stmt: /* empty */
        node_id edgeRHS
            {
                GasTexEdge* edge = new GasTexEdge(gastexGraph->getNode(*$1), gastexGraph->getNode(*$2));
                edge->label = dummyEdge.label; 
                gastexGraph->addEdge(edge);
            }
        | node_id edgeRHS attr_list
            {
                GasTexEdge* edge = new GasTexEdge(gastexGraph->getNode(*$1), gastexGraph->getNode(*$2));
                edge->label = dummyEdge.label; 
                gastexGraph->addEdge(edge);
            }
        | subgraph edgeRHS
            {
            }
        | subgraph edgeRHS attr_list
            {
            }
;

edgeRHS: /* empty */
        KW_EDGEOP node_id
            {
                (*$$) = (*$2);
            }
        | KW_EDGEOP node_id edgeRHS
            {
                (*$$) = (*$2);
            }
        | KW_EDGEOP subgraph
            {
            }
        | KW_EDGEOP subgraph edgeRHS
            {
            }
;

%%

int dot_yyerror(string s) {
  extern int dot_yylineno;	// defined and maintained in lex.c
  extern char *dot_yytext;	// defined and maintained in lex.c
  
  cerr << "ERROR: " << s << " at symbol \"" << dot_yytext;
  cerr << "\" on line " << dot_yylineno << endl;
  exit(1);
}

int yyerror(char *s) {
  return yyerror(string(s));
}


