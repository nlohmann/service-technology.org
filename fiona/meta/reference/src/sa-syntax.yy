%{
extern char* sa_yytext;
extern int sa_yylex();
extern int sa_yyerror(char const *msg);
%}

%name-prefix="sa_yy"
%error-verbose
%token_table
%defines

%token KEY_NODES KEY_INITIAL KEY_FINAL
%token KEY_INTERFACE KEY_INPUT KEY_OUTPUT KEY_SYNCHRONOUS
%token COLON COMMA SEMICOLON IDENT ARROW NUMBER

%union {
    char *str;
    unsigned int value;
}

%type <value> NUMBER
%type <str>   IDENT

%start sa
%%


sa:
  KEY_INTERFACE input output synchronous KEY_NODES nodes
;


input:
  /* empty */
| KEY_INPUT identlist SEMICOLON
;


output:
  /* empty */
| KEY_OUTPUT identlist SEMICOLON
;


synchronous:
  /* empty */
| KEY_SYNCHRONOUS identlist SEMICOLON
;


identlist:
  /* empty */
| IDENT
| identlist COMMA IDENT
;


nodes:
  node
| nodes node
;


node:
  NUMBER annotation successors
;


annotation:
  /* empty */
| COLON KEY_INITIAL
| COLON KEY_FINAL
| COLON KEY_INITIAL COMMA KEY_FINAL
;


successors:
  /* empty */
| successors IDENT ARROW NUMBER
;


%%

int main() {
    return sa_yyparse();
}
