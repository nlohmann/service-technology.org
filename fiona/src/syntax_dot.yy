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

int label_length(string* str) {
    int length;

    length = str->substr(str->find_first_of("{"), str->find_first_of("}")).length();
    
    return (int)(length * char_width_ratio);
}

%}

// Bison options
%name-prefix="dot_yy"

%union{
    int     	int_val;
    string*	str_val;
}

%start	graph 

%type <str_val>    KW_ID
%type <str_val>    stmt
%type <str_val>    attr_stmt
%type <str_val>    attr_list
%type <str_val>    a_list
%type <str_val>    node_stmt
%type <str_val>    node_id
%type <str_val>    node_attr_list
%type <str_val>    node_a_list
%type <str_val>    edge_stmt
%type <str_val>    edgeRHS
%type <str_val>    edge_attr_list
%type <str_val>    edge_a_list

%left KW_DIGRAPH
%left KW_GRAPH
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
%left KW_FOO
%left KW_EQUAL
%left KW_AT
%left KW_EDGEOP
%left KW_STRICT

%%

graph:  /* empty */
        KW_DIGRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE
            {
            }
;

stmt_list: /* empty */
        stmt KW_SEMICOLON
            { 
            }
        | stmt KW_SEMICOLON stmt_list 
            { 
            }
;

stmt:   /* empty */
        attr_stmt
            {
            }
        | node_stmt 
            { 
            }
        | edge_stmt 
            { 
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
;

a_list: /* empty */ 
        KW_ID KW_EQUAL KW_STRING
            {
            }
        | KW_ID KW_EQUAL KW_STRING KW_COMMA a_list
            {
            }
        | KW_ID KW_EQUAL KW_INT
            {
            }
        | KW_ID KW_EQUAL KW_ID KW_COMMA a_list
            {
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
        | node_id node_attr_list        
            {
                GasTexNode* node = new GasTexNode;
                node->id = dummyNode.id;
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
;

node_attr_list: /* empty */
        KW_LBRACKET KW_RBRACKET
            {
            }
        | KW_LBRACKET node_a_list KW_RBRACKET
            { 
            }
;

node_a_list: /* empty */ 
        KW_ID KW_EQUAL KW_STRING
            {
                if ((*$1) == "label") {
                    dummyNode.label = (*$3).substr(1, ($3)->length() - 2);
                } else if ((*$1) == "pos") {
                    dummyNode.posX = atoi((*$3).substr(1, ($3)->find_first_of(",")).c_str());
                    dummyNode.posY = atoi((*$3).substr(($3)->find_first_of(",") + 1, ($3)->length()).c_str());
                    dummyNode.posX = (int) (dummyNode.posX * scale_factor);
                    dummyNode.posY = (int) (dummyNode.posY * scale_factor);
                } else if ((*$1) == "width") {
                    dummyNode.width = atof((*$3).substr(1, ($3)->length() - 2).c_str());
                } else if ((*$1) == "height") {
                    dummyNode.height = atof((*$3).substr(1, ($3)->length() - 2).c_str());
                }
            }
        | KW_ID KW_EQUAL KW_STRING KW_COMMA node_a_list
            {
                if ((*$1) == "label") {
                    dummyNode.label = (*$3).substr(1, (*$3).length() - 2);
                } else if ((*$1) == "pos") {
                    dummyNode.posX = atoi((*$3).substr(1, ($3)->find_first_of(",")).c_str());
                    dummyNode.posY = atoi((*$3).substr(($3)->find_first_of(",") + 1, ($3)->length()).c_str());
                    dummyNode.posX = (int) (dummyNode.posX * scale_factor);
                    dummyNode.posY = (int) (dummyNode.posY * scale_factor);
                } else if ((*$1) == "width") {
                    dummyNode.width = atof((*$3).substr(1, (*$3).length() - 2).c_str());
                } else if ((*$1) == "height") {
                    dummyNode.height = atof((*$3).substr(1, (*$3).length() - 2).c_str());
                }
            }
        | KW_ID KW_EQUAL KW_ID
            {
            }
        | KW_ID KW_EQUAL KW_ID KW_COMMA node_a_list
            {
            }
        | KW_ID KW_EQUAL KW_INT
            {
                if (((*$1) == "peripheries")) {
		    dummyNode.isFinal = true;
                }
            }
        | KW_ID KW_EQUAL KW_INT KW_COMMA node_a_list
            {
                if (((*$1) == "peripheries")) {
		    dummyNode.isFinal = true;
                }
            } 
;

edge_stmt: /* empty */
        node_id edgeRHS edge_attr_list
            {
                GasTexEdge* edge = new GasTexEdge(gastexGraph->getNode(*$1), gastexGraph->getNode(*$2));
                edge->label = dummyEdge.label; 

                gastexGraph->addEdge(edge);
            }
;

edgeRHS: /* empty */
        KW_EDGEOP node_id
            {
                (*$$) = (*$2);
            }
;

edge_attr_list: /* empty */
        KW_LBRACKET KW_RBRACKET
            {
            }
        | KW_LBRACKET edge_a_list KW_RBRACKET
            { 
            }
;

edge_a_list: /* empty */ 
        KW_ID KW_EQUAL KW_STRING
            {
                if ((*$1) == "label") {
                    dummyEdge.label = (*$3).substr(1, ($3)->length() - 2);
                }
            }
        | KW_ID KW_EQUAL KW_STRING KW_COMMA edge_a_list
            {
                if ((*$1) == "label") {
                    dummyEdge.label = (*$3).substr(1, ($3)->length() - 2);
                }
            }
        | KW_ID KW_EQUAL KW_INT
            {
            }
        | KW_ID KW_EQUAL KW_INT KW_COMMA edge_a_list
            {
            }
        | KW_ID KW_EQUAL KW_ID
            {
            }
        | KW_ID KW_EQUAL KW_ID KW_COMMA edge_a_list
            {
            }
;
            
%%

int dot_yyerror(string s)
{
  extern int dot_yylineno;	// defined and maintained in lex.c
  extern char *dot_yytext;	// defined and maintained in lex.c
  
  cerr << "ERROR: " << s << " at symbol \"" << dot_yytext;
  cerr << "\" on line " << dot_yylineno << endl;
  exit(1);
}

int yyerror(char *s)
{
  return yyerror(string(s));
}


