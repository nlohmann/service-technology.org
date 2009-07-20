/*****************************************************************************\

#warning "LICENSE"

\*****************************************************************************/

%{
// options for Bison
#define YYDEBUG 1
#define YYERROR_VERBOSE 0  // for verbose error messages


// to avoid the message "parser stack overflow"
#define YYMAXDEPTH 1000000
#define YYINITDEPTH 10000

#include <iostream>
#include <cstdlib>
#include <string>
#include <list>
#include "adapter.h"

// from flex
extern char* adapt_rules_yytext;
extern int adapt_rules_yylineno;
extern int adapt_rules_yylex();
extern int adapt_rules_yyerror(const char*);

RuleSet * workingSet = NULL;
RuleSet::AdapterRule::cfMode cfmodus = RuleSet::AdapterRule::AR_NORMAL;

int adapt_rules_yyerror(const char* msg)
{
  std::cerr << msg << std::endl;
  std::cerr << "error in line " << adapt_rules_yylineno << ": token last read: `" << adapt_rules_yytext << "'" << std::endl;
  exit(EXIT_FAILURE);
}

%}

// Bison options
%defines
%name-prefix="adapt_rules_yy"

%token RULE_HIDDEN RULE_OBSERVABLE RULE_CONTROLLABLE
%token NAME ARROW COMMA SEMICOLON

%token_table

%{
typedef std::list< unsigned int > uilist;
%}

%union {
  char *str;
  uilist * channelList;
}

/* the types of the non-terminal symbols */
%type <str> NAME
%type <channelList> channel_list
%type <channelList> opt_channel_list


%%

/* adapter rules consist of rules for partial communication flow (hidden,
** observable and controllable) and rules for total communication flow
** (called standard rules or total rules) */

adapt_rules:
    RULE_HIDDEN 
    { cfmodus = RuleSet::AdapterRule::AR_HIDDEN; }
    partial_rule
    SEMICOLON adapt_rules
|
    RULE_OBSERVABLE 
    { cfmodus = RuleSet::AdapterRule::AR_OBSERVABLE; }
    partial_rule
    SEMICOLON adapt_rules
|
    RULE_CONTROLLABLE 
    { cfmodus = RuleSet::AdapterRule::AR_CONTROLLABLE; }
    partial_rule
    SEMICOLON adapt_rules
|
    total_rule
    SEMICOLON adapt_rules
|
    /* empty */
;

/* partial rules cannot have optional channel lists */

partial_rule:
    channel_list 
    ARROW channel_list
    {
        std::pair< std::list<unsigned int>, std::list<unsigned int> > rulepair(*$1, *$3);
        RuleSet::AdapterRule * rule = new RuleSet::AdapterRule(rulepair, cfmodus);
        workingSet->_adapterRules.push_back(rule);
        delete $1;
        delete $3;
    }
;

/* total rules can have optional channel lists */

total_rule:
    opt_channel_list 
    ARROW opt_channel_list
    {
        std::pair< std::list<unsigned int>, std::list<unsigned int> > rulepair(*$1, *$3);
        RuleSet::AdapterRule * rule = new RuleSet::AdapterRule(rulepair, cfmodus);
        workingSet->_adapterRules.push_back(rule);
        delete $1;
        delete $3;
    }
;

opt_channel_list:
    channel_list
    {
        $$ = $1;
    }
|
    /* empty */
    {
        $$ = new std::list< unsigned int>();
    }
;

channel_list:
    NAME
    {
        $$ = new std::list< unsigned int>();
        $$->push_back(workingSet->getIdForMessage(std::string($1)));
    }
|
    NAME COMMA channel_list
    {
        $$ = $3;
        $$->push_back(workingSet->getIdForMessage(std::string($1)));
    }
;


