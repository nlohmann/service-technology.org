%{
// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000


#include <string>
#include <iostream>
#include <set>
#include <map>
using std::cerr;
using std::endl;
using std::string;
using std::set;
using std::map;

extern char *adapt_rules_yytext;
extern int adapt_rules_yylineno;

int adapt_rules_yyerror(const char* msg)
{
  cerr << msg << endl;
  cerr << "error in line " << adapt_rules_yylineno << ": token last read: `" << adapt_rules_yytext << "'" << endl;
  exit(1);
}

// from flex
extern char* adapt_rules_yytext;
extern int adapt_rules_yylex();
%}

// Bison options
%name-prefix="adapt_rules_yy"

%token NAME ARROW COMMA SEMICOLON

%token_table

%union {
  char *str;
}

/* the types of the non-terminal symbols */
%type <str> NAME


%%

/*
 *
 * adapt_rules: ( adapt_rule ; )*
 *
 */
adapt_rules:
    adapt_rule SEMICOLON adapt_rules
|
    /* empty */
;

adapt_rule:
    opt_channel_list ARROW opt_channel_list
;

opt_channel_list:
    channel_list
|
    /* empty */
;

channel_list:
    NAME
|
    NAME COMMA channel_list
;

