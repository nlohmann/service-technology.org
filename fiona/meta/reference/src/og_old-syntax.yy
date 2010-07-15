%{
extern char* og_old_yytext;
extern int og_old_yylex();
extern int og_old_yyerror(char const *msg);
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
}

%type <value> number
%type <str>   ident

%left op_or
%left op_and

%start og
%%


og:
 interface nodes initialnode transitions
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
| number colon formula
| number colon key_finalnode
| number colon formula colon color
| number colon formula colon key_finalnode
| number colon formula colon color colon key_finalnode
;


formula:
  lpar formula rpar
| formula op_and formula
| formula op_or formula
| key_final
| key_true
| key_false
| ident
;


color:
| key_blue
| key_red
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
;

%%

int main() {
    return og_old_yyparse();
}
