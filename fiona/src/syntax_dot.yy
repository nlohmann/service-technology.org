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
int dot_yyerror(char *s);
int dot_yylex(void);

int i;

string tex_format(string str) {
    string result;
    string::size_type pos;
    
    if (strip_command_sequence) {
        for(pos = 0; pos < str.length(); ++pos) {
            switch (str[pos]) {
		case '\\':
                    str.replace(pos, 1, "\\textbackslash ");
                    pos += 1;
                    break;
                case '{':
                    str.replace(pos, 1, "\\{");
                    pos += 1;
                    break;
                case '}':
                    str.replace(pos, 1, "\\{");
                    pos += 1;
                    break;
                case '#':
                    str.replace(pos, 1, "\\#");
                    pos += 1;
                    break;
                default: ;
            }

        }
    }
    
    result = str;
    return result;
}

int label_length(string* str) {
    int length;

    length = str->substr(str->find_first_of("{"), str->find_first_of("}")).length();
    
    return (int)(length * char_width_ratio);
}

%}

// Bison options
%name-prefix="dot_yy"

%union{
    int     int_val;
    string*	str_val;
}

%start	graph 

%type <str_val>    KW_ID
%type <str_val>    stmt
%type <str_val>    attr_stmt
%type <str_val>    node_stmt
%type <str_val>    edge_stmt
%type <str_val>    edgeRHS
%type <str_val>    node_id
%type <str_val>    attr_list
%type <str_val>    node_attr_list
%type <str_val>    a_list
%type <str_val>    node_a_list

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
//                cout << "KW_DIGRAPH KW_ID KW_LBRACE stmt_list KW_RBRACE" << endl; 
            }
;

stmt_list: /* empty */
        stmt KW_SEMICOLON
            { 
//                cout << "stmt KW_SEMICOLON" << endl; 
            }
        | stmt KW_SEMICOLON stmt_list 
            { 
//                cout << "stmt KW_SEMICOLON stmt_list" << endl; 
            }
;

stmt:   /* empty */
        attr_stmt
            {
                (*$$) = (*$1);
//                cout << (*$$) << endl;
            }
        | node_stmt 
            { 
                (*$$) = (*$1);
                buffer[i++] = (*$$);
//                cout << (*$$) << endl;  
            }
        | edge_stmt 
            { 
                (*$$) = (*$1);
                buffer[i++] = (*$$);
//                cout << (*$$) << endl; 
            }
;

attr_stmt: /* empty */ 
        KW_GRAPH attr_list 
            {
                (*$$) = ""; 
//                cout << "KW_GRAPH attr_list" << endl; 
            }
        | KW_NODE attr_list
            { 
                (*$$) = "";
//                cout << "KW_NODE attr_list" << endl; 
            }
        | KW_EDGE attr_list
            { 
                (*$$) = "";
//                cout << "KW_EDGE attr_list" << endl; 
            }
;

attr_list: /* empty */
        KW_LBRACKET KW_RBRACKET
            {
//                cout << "KW_LBRACKET KW_RBRACKET" << endl; 
            }
        | KW_LBRACKET a_list KW_RBRACKET
            { 
    		  (*$$) = (*$2); 
//                cout << "KW_LBRACKET a_list KW_RBRACKET" << endl; 
            }
;

node_attr_list: /* empty */
        KW_LBRACKET KW_RBRACKET
            {
//                cout << "KW_LBRACKET KW_RBRACKET" << endl; 
            }
        | KW_LBRACKET node_a_list KW_RBRACKET
            { 
              (*$$) = (*$2); 
//                cout << "KW_LBRACKET a_list KW_RBRACKET" << endl; 
            }
;

a_list: /* empty */ 
        KW_ID KW_EQUAL KW_STRING
            {
                if ((*$1) == "label") {
                    (*$$) = a_list_label_str[0] + tex_format((*$3).substr(1, ($3)->length() - 2)) + a_list_label_str[1];
                } else if ((*$1) == "pos") { 
                    (*$$) = a_list_pos_str[0] + tex_format((*$3).substr(1, ($3)->length() - 2)) + a_list_pos_str[1];
                } else if ((*$1) == "bb") { 
                    (*$$) = a_list_bb_str[0] + tex_format((*$3).substr(1, ($3)->length() - 2)) + a_list_bb_str[1];
                } else { 
                    (*$$) = "";
                }
//                cout << "KW_ID KW_EQUAL KW_STRING" << endl; 
            }
        | KW_ID KW_EQUAL KW_STRING KW_COMMA a_list
            {
                if ((*$1) == "label") {
                    (*$$) = (*$5) + a_list_label_str[0] + tex_format((*$3).substr(1, ($3)->length() - 2)) + a_list_label_str[1];
                } else if ((*$1) == "pos") {
                    (*$$) = a_list_pos_str[0] + tex_format((*$3).substr(1, ($3)->length() - 2)) + a_list_pos_str[1] + (*$5);
                } else if ((*$1) == "bb") {
                    (*$$) = a_list_bb_str[0] + tex_format((*$3).substr(1, ($3)->length() - 2)) + a_list_bb_str[1] + (*$5);
                } else {
                    (*$$) = (*$5);
                }
//                cout << "KW_ID KW_EQUAL KW_STRING KW_COMMA a_list" << endl; 
            }
        | KW_ID KW_EQUAL KW_INT
            {
                (*$$) = ""; 
/*                if ((*$1) == "fontsize") {
                    char temp[4];
                    sprintf(temp, "%d", $3);
                    (*$$) = (*$1) + ": " + temp;
	        }*/
//                cout << "KW_ID KW_EQUAL KW_INT" << endl; 
            }
        | KW_ID KW_EQUAL KW_ID KW_COMMA a_list
            {
		(*$$) = (*$5); 
//                cout << "KW_ID KW_EQUAL KW_ID KW_COMMA a_list" << endl; 
            }
;

node_a_list: /* empty */ 
        KW_ID KW_EQUAL KW_STRING
            {
                if ((*$1) == "label") {
                    (*$$) = a_list_label_str[0] + (*$3).substr(1, ($3)->length() - 2) + a_list_label_str[1];
                } else if ((*$1) == "pos") { 
                    (*$$) = a_list_pos_str[0] + (*$3).substr(1, ($3)->length() - 2) + a_list_pos_str[1];
                } else if ((*$1) == "bb") { 
                    (*$$) = a_list_bb_str[0] + (*$3).substr(1, ($3)->length() - 2) + a_list_bb_str[1];
                } else { 
                    (*$$) = "";
                }
//                cout << "KW_ID KW_EQUAL KW_STRING" << endl; 
            }
        | KW_ID KW_EQUAL KW_STRING KW_COMMA node_a_list
            {
                if ((*$1) == "label") {
                    (*$$) = (*$5) + a_list_label_str[0] + tex_format((*$3).substr(1, ($3)->length() - 2)) + a_list_label_str[1];
                } else if ((*$1) == "pos") {
                    int x, y;
                    x = atoi((*$3).substr(1, ($3)->find_first_of(",")).c_str());
                    x = (int)(x * scale_factor);
                    
                    y = atoi((*$3).substr(($3)->find_first_of(",") + 1, ($3)->length()).c_str());
                    y = (int)(y * scale_factor);

                    char temp[12];
                    sprintf(temp, "%i, %i", x, y);
                    
                    minx = (minx < x) ? minx : x;
                    miny = (miny < y) ? miny : y;
                    maxx = (maxx > x) ? maxx : x;
                    maxy = (maxy > y) ? maxy : y;
                     
                    (*$$) = a_list_pos_str[0] + temp + a_list_pos_str[1] + (*$5);
                } else if ((*$1) == "bb") {
                    (*$$) = a_list_bb_str[0] + (*$3).substr(1, ($3)->length() - 2) + a_list_bb_str[1] + (*$5);
                } else {
                    (*$$) = (*$5);
                }
//                cout << "KW_ID KW_EQUAL KW_STRING KW_COMMA node_a_list" << endl; 
            }
        | KW_ID KW_EQUAL KW_INT
            {
                (*$$) = ""; 
/*                if ((*$1) == "fontsize") {
                    char buffer[4];
                    sprintf(buffer, "%d", $3);
                    (*$$) = (*$1) + ": " + buffer;
            }*/
//                cout << "KW_ID KW_EQUAL KW_INT" << endl; 
            }
        | KW_ID KW_EQUAL KW_ID KW_COMMA node_a_list
            {
        (*$$) = (*$5); 
//                cout << "KW_ID KW_EQUAL KW_ID KW_COMMA node_a_list" << endl; 
            }
;

node_stmt: /* empty */
        node_id
            {
                (*$$) = node_stmt_str[0] + (*$1) + node_stmt_str[1];
//                cout << "node_id" << endl; 
            }
        | node_id node_attr_list        
            { 
                char buffer[128];
                sprintf(buffer, "%i", label_length($2));
                (*$$) = node_stmt2_str[0] + buffer + node_stmt2_str[1] + (*$1) + node_stmt2_str[2] + (*$2);
//               cout << "node_id node_attr_list" << endl; 
            }
;

node_id: /* empty */
        KW_ID 
            {
                (*$$) = (*$1);
//                cout << "KW_ID" << endl; 
            }
;

edge_stmt: /* empty */
        node_id edgeRHS attr_list
            {
                (*$$) = edge_stmt_str[0] + (*$1) + edge_stmt_str[1] + (*$2) + edge_stmt_str[2] + 
                    ($3)->substr(($3)->find_first_of("{"), ($3)->find_first_of("}") - ($3)->find_first_of("{") + 1);
//                cout << "node_id edgeRHS attr_list" << endl; 
            }
;

edgeRHS: /* empty */
        KW_EDGEOP node_id
            {
                (*$$) = (*$2);
//                cout << "KW_EDGEOP node_id" << endl; 
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


